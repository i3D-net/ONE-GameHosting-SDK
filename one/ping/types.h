#pragma once

#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <one/ping/allocator.h>

namespace i3d {
namespace ping {

// See: https://en.cppreference.com/w/cpp/language/type_alias
template <class T>
using Vector = std::vector<T, StandardAllocator<T>>;

// See: https://en.cppreference.com/w/cpp/language/type_alias
template <class T>
using List = std::list<T, StandardAllocator<T>>;

// All std dynamic types in the one namespace must use the following types.
typedef std::basic_string<char, std::char_traits<char>, StandardAllocator<char>> String;
typedef std::basic_ostringstream<char, std::char_traits<char>, StandardAllocator<char>>
    OStringStream;

std::string to_std_string(const String &);
String to_one_string(const std::string &);

}  // namespace ping
}  // namespace i3d
