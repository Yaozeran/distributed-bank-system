/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao
 * 
 * The <compat.h> file implements across platform compatibility related codes. */

#ifndef COMPAT_H
#define COMPAT_H

#include <cstdint>
#include <bit>

/* Byte swap helper functions
 *
 * These functions shift these bytes of an uint in reverse order:
 * suppose uint32_t input x is 0xabcdef01, this results in 0x01efcdab */

inline uint16_t bswap16(uint16_t x) {
  return (x >> 8) | (x << 8);
}

inline uint32_t bswap32(uint32_t x) {
  return ( 
    ((x & 0xff000000U) >> 24) | ((x & 0x00ff0000U) >>  8) | 
    ((x & 0x0000ff00U) <<  8) | ((x & 0x000000ffU) << 24)
  );
}

inline uint64_t bswap64(uint64_t x) {
  return (
    ((x & 0xff00000000000000ull) >> 56) | ((x & 0x00ff000000000000ull) >> 40) | 
    ((x & 0x0000ff0000000000ull) >> 24) | ((x & 0x000000ff00000000ull) >>  8) | 
    ((x & 0x00000000ff000000ull) <<  8) | ((x & 0x0000000000ff0000ull) << 24) |
    ((x & 0x000000000000ff00ull) << 40) | ((x & 0x00000000000000ffull) << 56)
  );
}

/* Endian conversion functions
 * 
 * - Usages:
 *   1. The web socket use big endian format to transfer serialized byte objects. */

inline uint16_t htole16(uint16_t x) {
  if constexpr (std::endian::native == std::endian::little) return x;
  return bswap16(x);
}

inline uint16_t htobe16(uint16_t x) {
  if constexpr (std::endian::native == std::endian::big) return x;
  return bswap16(x);
}

inline uint16_t betoh16(uint16_t x) {
  if constexpr (std::endian::native == std::endian::big) return x;
  return bswap16(x);
}

inline uint16_t letoh16(uint16_t x) {
  if constexpr (std::endian::native == std::endian::little) return x;
  return bswap16(x);
}

inline uint32_t htole32(uint32_t x) {
  if constexpr (std::endian::native == std::endian::little) return x;
  return bswap32(x);
}

inline uint32_t htobe32(uint32_t x) {
  if constexpr (std::endian::native == std::endian::big) return x;
  return bswap32(x);
}

inline uint32_t betoh32(uint32_t x) {
  if constexpr (std::endian::native == std::endian::big) return x;
  return bswap32(x);
}

inline uint32_t letoh32(uint32_t x) {
  if constexpr (std::endian::native == std::endian::little) return x;
  return bswap32(x);
}

#endif /* COMPAT_H */