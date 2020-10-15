#pragma once

#include <string>

#include <one/arcus/allocator.h>

namespace i3d {
namespace one {

// All strings in the one namespace should use the following type.
typedef std::basic_string<char, std::char_traits<char>, StandardAllocator<char>> String;

}  // namespace one
}  // namespace i3d
