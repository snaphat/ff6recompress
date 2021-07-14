#include <expand.h>
#include <shrink.h>
#include "types.hpp"

auto pack_aplib(u8vec& input, auto window_size) -> u8vec {
    /* Allocate max compressed size */
    auto csize = apultra_get_max_compressed_size(input.size());
    u8vec output(csize);
    //output[0] = 0xFF;
    //output[1] = 0xFF;
    auto size = apultra_compress(&input[0], &output[0], input.size(), csize, 0, window_size, 0, NULL, NULL);
    output.resize(size);
    return output;
}

auto unpack_aplib(u8vec& input) -> u8vec {
    auto dsize = apultra_get_max_decompressed_size(&input[0], input.size(), 0);
    u8vec output(dsize);
    auto size = apultra_decompress(&input[0], &output[0], input.size(), dsize, 0, 0);
    output.resize(size);
    return output;
}

auto unpack_lzss(auto input) {
    // Original decode:
    auto src = input;
    u8vec dest;
    uint64 s = 0; // source pointer
    uint64 d = 0; // destination pointer
    uint8 buffer[0x800] = {0};
    auto b = 0x07DE;
    uint8 line[34];
    uint32 length = src[s++];
    length |= (src[s++] << 8);
    if (length == 0)
        return make_tuple(move(dest), (uint64)0);
    while (s < length) {
        // read header
        auto header = src[s++];

        for (auto pass = 0; pass < 8; pass++, header >>= 1) {
            auto l = 0;
            if (header & 1) {
                // single byte (uncompressed)
                auto c = src[s++];
                line[l++] = c;
                buffer[b++] = c;
                b &= 0x07FF;
            } else {
                // 2-bytes (compressed)
                uint16 w = src[s++];
                w |= (src[s++] << 8);
                auto r = (w >> 11) + 3;
                w &= 0x07FF;

                for (auto i = 0; i < r; i++) {
                    auto c = buffer[(w + i) & 0x07FF];
                    line[l++] = c;
                    buffer[b++] = c;
                    b &= 0x07FF;
                }
            }
            // copy this pass to the destination buffer
            dest.insert(dest.end(), line, line+l);
            d += l;

            // reached end of compressed data
            if (s >= length) break;
        }
    }

    return make_tuple(move(dest), (uint64)s);
}
