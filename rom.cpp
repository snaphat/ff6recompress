#include "compression.cpp"
#include "json.cpp"
#include "types.hpp"

template <>
struct std::hash<u8vec> {
    std::size_t operator() (const u8vec &v) const
    {
        return std::hash<std::string>() (std::string(v.cbegin(), v.cend()));
    }
};

inline auto conv_addr(unsigned addr) -> unsigned int {
    return (addr & 0x400000) ? (addr & 0x3FFFFF) : ((addr&0x8000) ? addr & 0x3FFFFF : 0x0);
}

class Rom {
    private:
        u8vec rom;
        JsonWrap json;

    /*
        Recompress rom relevant data segment.

        Used by other recompress() functions for recompression.
    */
    auto _recompress(auto offset) {

        auto [unpacked, old_size] = unpack_lzss(&rom[offset]);
        auto repacked             = pack_aplib(unpacked, 0x10000);

        if (repacked.size() > old_size)
            std::cout << "warning: " << repacked.size() << ">=" << old_size << std::endl;
        else {
            auto save = old_size - repacked.size();
            saved_bytes += save;
        }

        // Return repacked data (cannot add to rom until we check if it is a duplicate).
        return make_tuple(move(repacked), old_size); // <u8vec, uint*>
    }

    public:
    uint64 saved_bytes = 0;

    Rom(auto filename) {;
        // Open file and buffer into memory.
        auto fd = fopen(filename, "rb");
        if (fd) {
            fseek (fd, 0, SEEK_END);
            unsigned int length = ftell(fd);
            fseek (fd, 0, SEEK_SET);
            u8vec buffer(length);
            auto _ = fread (&buffer[0], 1, length, fd);
            fclose (fd);
            rom = move(buffer);
        }

        json = std::move(JsonWrap(_config));
    }

    auto size() { return rom.size(); }

    auto save(std::string filename) -> void {
        // Write json.
        json.save(filename);
        // Write rom.
        auto fd = fopen((filename+".sfc").c_str(), "wb");
        fwrite(&rom[0], 1, rom.size(), fd);
        fclose(fd);
    }

    /*
        Recompress single segment of data and copy into rom.
    */
    auto recompress(auto json_entry, uint32 bank_offset) -> void {
        auto offset           = conv_addr(bank_offset);
        auto [data, old_size] = _recompress(offset);
        json.add_freespace(bank_offset+data.size(), bank_offset+old_size);
        json.insert(json_entry, bank_offset, bank_offset+data.size());
        copy(data.begin(), data.end(), &rom[offset]);
    }

    /*
        Recompress arrayed data and copy into rom.

        Arrayed data begins with a pointer table that is modified
        with the new locations of each segment after recompression.
    */
    auto recompress(auto json_entry, auto bank_ptr_tbl_start, uint32 ptr_entry_size,
                    auto bank_data_start, auto bank_data_end, auto entry_count) -> void {
        // Set data pointer table to start.
        auto dp_offset  = conv_addr(bank_ptr_tbl_start);

        // Extract data pointer logic.
        auto extract_ptr = [=](auto s) -> uint32 {
            uint32 t = s[0];
            for (auto i=1; i<ptr_entry_size; i++) { t += s[i] << (8*i); }
            return t;
        };

        // Insert data pointer logic.
        auto insert_ptr = [=](auto s, auto d) {
            auto t = reinterpret_cast<uint8*>(&s);
            for (auto i=0; i<ptr_entry_size; i++) { d[i] = t[i]; }
        };

        // Insert data logic.
        auto insert_data = [=](auto s, auto d) {
            copy(s.begin(), s.end(), d);
        };

        // extract table pointer for next entry & convert to little endian.
        auto init_data_ptr = extract_ptr(&rom[dp_offset]);
        auto old_data_ptr  = init_data_ptr;
        auto new_data_ptr  = init_data_ptr;              // set initial new ptr.

        std::hash<u8vec> hash;                          // Hash function for hashing data.
        std::unordered_map<uint64, uint32> lookup_tbl;  // lookup table for checking for duplicates.
        u8vec data;                                     // Reference to data.
        uint32 old_size;
        for(auto i=0; i<entry_count; i++) {
            // Compute locations of data.
            auto old_data_loc = bank_data_start + old_data_ptr;
            auto new_data_loc = bank_data_start + new_data_ptr;

            // Compress data.
            if (old_data_ptr < init_data_ptr)                    // ensure old dp is after the initial.
                data = u8vec(0,0);                               // invalid pointer.
            else
                tie(data, old_size) = _recompress(conv_addr(old_data_loc));   // valid pointer.

            // Check if compressed data is a duplicate.
            auto data_hash = hash(data);
            if (lookup_tbl.contains(data_hash)) {
                // Insert duplicate data pointer.
                insert_ptr(lookup_tbl[data_hash], &rom[dp_offset]); // Insert from lookup table.
            } else {
                // Insert new data pointer entry & data.
                lookup_tbl.insert({data_hash, new_data_ptr});       // Insert hash into lookup table.
                insert_ptr(new_data_ptr, &rom[dp_offset]);          // Insert ptr into rom.
                insert_data(data, &rom[conv_addr(new_data_loc)]);   // Insert data into rom.

                // Update bank-relative data save location after the inserted data.
                new_data_ptr += data.size();
            }

            // extract table pointer for next entry & convert to little endian.
            dp_offset   += ptr_entry_size;
            old_data_ptr = extract_ptr(&rom[dp_offset]);
        }
        json.add_freespace(bank_data_start+new_data_ptr, bank_data_end);
        json.insert(json_entry, bank_data_start+init_data_ptr, bank_data_start+new_data_ptr);
    }

    auto process() -> void {
        auto entries = {"battleBackgroundGraphics", "battleBackgroundLayout",
        "cinematicProgram", "creditsGraphics",
        "endingGraphics", "floatingIslandCinematic",
        "mapAnimationGraphicsLayer3",
        "mapGraphicsLayer3",
        "mapLayouts",
        "mapOverlayProperties",
        "mapTileProperties",
        "mapTilesets",
        "worldGraphics3",
        "worldLayout3",
        "worldPalette3",
        "titleIntroGraphics",
        "vectorApproachGraphics",
        "vectorApproachLayout",
        "worldCloudsGraphics",
        "worldCloudsLayout",
        "worldGraphics1",
        "worldLayout1",
        "worldOfRuinCinematic",
        "worldGraphics2",
        "worldLayout2"
        };

        std::cout << "\e[33mFile Size (bytes)\e[36m: \e[32m" << rom.size() << "\e[39m" << std::endl << std::endl;
        std::cout << "\e[33mRecompressing\e[36m:\e[39m" << std::endl;

        for ( auto entry : entries ) {
            auto maybe_entry = json.extract(entry);
            if (maybe_entry) {
                auto [name, range, maybe_tbl] = *maybe_entry;
                auto [data_beg, data_end] = range;
                std::cout << " \e[33m-\e[36m " << name << "\e[33m...\e[39m" << std::flush;
                if(maybe_tbl) {
                    auto [tbl_range, tbl_offset, tbl_ptr_length, array_length] = *maybe_tbl;
                    auto [tbl_beg, tbl_end] = tbl_range;
                    // std:: cout << " tbl" << std::hex << "[" << tbl_beg << "-" << tbl_end << "] off:"
                    // << tbl_offset << ", ps:" << std::dec << tbl_ptr_length << ", as:" << array_length
                    // << std::hex << "arr:[" << data_beg << "," << data_end << "]";
                    this->recompress(entry, tbl_beg, tbl_ptr_length, tbl_offset, data_end, array_length);
                } else {
                    this->recompress(entry, data_beg);
                    //std:: cout << std::hex << data_beg;
                }
                std::cout << std::setw(55-name.length()) << "\e[31mdone\e[39m" << std::endl;
            }
        }

        std::cout << std::endl << "\e[33mTotal savings (bytes)\e[36m: \e[32m" << std::dec << this->saved_bytes << "\e[39m" << std::endl;
    }
};
