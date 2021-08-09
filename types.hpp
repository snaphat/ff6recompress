#pragma once

#include <iomanip>
#include <iostream>
#include <list>
#include <tuple>
#include <sstream>
#include <vector>
#include <unordered_map>
typedef unsigned long long uint64;
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef std::vector<uint8> u8vec;
typedef std::tuple<uint32, uint32> u32pair;
typedef std::optional<std::tuple<u32pair, uint32, uint32, uint32>> optPtrTbl;
typedef std::optional<std::tuple<std::string, u32pair, optPtrTbl>> optEntry;

// Convert std::string to u32.
inline auto to_u32(std::string n) -> uint32 {
    return std::stoul(n, nullptr, 16);
}

// Convert std::string to u32 tuple.
inline auto to_u32_pair(std::string r) -> u32pair {
    auto pos = r.find_first_of('-');
    auto beg = to_u32(r.substr(0, pos));
    auto end = to_u32(r.substr(pos+1));
    return std::make_tuple(beg, end);
}

// Convert some type to u32 tuple.
inline auto to_u32_pair(auto r) -> u32pair {
    return to_u32_pair(r.template get<std::string>());
}
