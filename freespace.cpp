class FreeSpace {
    private:
        enum dir {right, left};
        std::list<std::tuple<uint64,dir>> range;
    public:
        auto add(uint32 beg, uint32 end) -> void {
            range.push_back(std::make_tuple(beg, dir::right));
            range.push_back(std::make_tuple(end, dir::left));
        }

        auto sort() { range.sort(); }

        auto trim() -> void {
            this->sort();
            auto prev_dir = dir::left;
            for (auto it = range.begin(); it != range.end();) {
                auto cur_dir = get<dir>(*it);
                // if direction match &
                // direction is right delete current element.
                // direction is left delete previous element.
                it = cur_dir == prev_dir ? (it = cur_dir == dir::right ? it = range.erase(it) : ++range.erase(--it)) : (++it);
                prev_dir = cur_dir;
            }
        }

        auto emit() {
            this->trim();
            std::stringstream ss;
            std::vector<std::string> out;
            ss << std::uppercase << std::hex;
            for (auto [val, dir] : range) {
                if (dir == dir::right) {
                    ss.str(std::string());
                    ss << "0x" << val << "-";
                } else {
                    ss << "0x" << val;
                    out.push_back(ss.str());
                }
            }
            return out;
        }
};
