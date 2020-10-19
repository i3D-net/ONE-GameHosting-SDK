#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

#include <one/arcus/allocator.h>

#define ONE_UNORDERED_MAP(key_type, value_type)                   \
    std::unordered_map<key_type, value_type, std::hash<key_type>, \
                       std::equal_to<key_type>, StandardAllocator<char>>

namespace i3d {
namespace one {

// All std dynamic types in the one namespace must use the following types.
typedef std::basic_string<char, std::char_traits<char>, StandardAllocator<char>> String;
typedef std::basic_ostringstream<char, std::char_traits<char>, StandardAllocator<char>>
    OStringStream;

std::string to_std_string(const String &);
String to_one_string(const std::string &);

}  // namespace one
}  // namespace i3d
