#pragma once
#include <cstdint>
#include <memory>

typedef std::uint8_t byte_t; // byte
typedef uint16_t addr_t; // 16-bit address
typedef uint16_t instruction_t; // 16-bit instruction
typedef uint8_t pixel_t; // ON (255) or OFF (0) pixel

template <typename T>
using uptr = std::unique_ptr<T>; // abbreviation for std::unique_ptr
template <typename T>
using sptr = std::shared_ptr<T>; // abbreviation for std::shared_ptr