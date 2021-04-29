#pragma once

#include <sstream>
#include <string>

#include <one/ping/allocator.h>

namespace i3d {
namespace ping {

// All std dynamic types in the one namespace must use the following types.
typedef std::basic_string<char, std::char_traits<char>, StandardAllocator<char>> String;
typedef std::basic_ostringstream<char, std::char_traits<char>, StandardAllocator<char>>
    OStringStream;

std::string to_std_string(const String &);
String to_one_string(const std::string &);

}  // namespace ping
}  // namespace i3d
