/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao */

#ifndef SERDES_H
#define SERDES_H

#include <cstddef>
#include <type_traits>
#include <cstring>
#include <memory>
#include <array>
#include <unordered_map>
#include <string>
#include <optional>

#include "core/currency.h"
#include "rpc/protocol.h"

/* Impl for arithmetic types */

template<typename T>
inline typename std::enable_if<std::is_arithmetic<T>::value, size_t>::type
serialize(char* out, const T& obj) {
  std::memcpy(out, std::addressof(obj), sizeof(obj));
  return sizeof(obj);
}

template<typename T>
inline typename std::enable_if<std::is_arithmetic<T>::value, size_t>::type
deserialize(const char* in, T& obj) {
  std::memcpy(std::addressof(obj), in, sizeof(obj));
  return sizeof(obj);
}

/* Impl for std::string */

inline size_t serialize(char* out, const std::string& str) {
  size_t s = str.size();
  serialize(out, s);
  std::memcpy(out + sizeof(s), str.data(), str.size());
  return sizeof(s) + s;
}

inline size_t deserialize(const char* in, std::string& str) {
  size_t s;
  deserialize(in, s); // size tag of the str
  str.assign(in + sizeof(s), s);
  return sizeof(s) + s;
}

/* Impl for operation code */

inline size_t serialize(char* out, const op_code& c) {
  int v = static_cast<int>(c);
  serialize(out, v);
  return sizeof(v);
}

inline size_t deserialize(const char* in, op_code& c) {
  int v;
  deserialize(in, v);
  std::optional<op_code> r = int_to_op_code(v);
  if (!r) {
    throw std::runtime_error("invalid serialized data: operation code");
  } else {
    c = *r;
  }
  return sizeof(v);
}

/* Impl for status code */

inline size_t serialize(char* out, const status_code& c) {
  int v = static_cast<int>(c);
  serialize(out, v);
  return sizeof(v);
}

inline size_t deserialize(const char* in, status_code& c) {
  int v;
  deserialize(in, v);
  std::optional<status_code> r = int_to_status_code(v);
  if (!r) {
    throw std::runtime_error("invalid serialized data: status code");
  } else {
    c = *r;
  }
  return sizeof(v);
}

/* Impl for currency */

inline size_t serialize(char* out, const currency& c) {
  size_t i = 0;
  std::string str = currency_to_str(c);
  i += serialize(out, str);
  return i;
}

inline size_t deserialize(const char* in, currency& c) {
  size_t i = 0;
  std::string str;
  i += deserialize(in, str);
  std::optional<currency> r = str_to_currency(str);
  if (!r) {
    throw std::runtime_error("invalid serialized data: currency");
  } else {
    c = *r;
  }
  return i;
}

/* Impl for std::array */

template<typename T, size_t N>
inline size_t serialize(char* out, const std::array<T, N>& arr) {
  size_t i = 0;
  for (const auto& elem : arr) {
    i += serialize(out + i, elem);
  }
  return i;
}

template<typename T, std::size_t N>
inline size_t deserialize(const char* in, std::array<T, N>& arr) {
  size_t i = 0;
  for (auto& elem : arr) {
    i += deserialize(in + i, elem);
  }
  return i;
}

/* Impl for unordered map */

template<typename K, typename V>
inline size_t serialize(char* out, const std::unordered_map<K, V>& map) {
  size_t i = 0, size = map.size();
  i += serialize(out + i, size);
  for (const auto& [key, value] : map) {
    i += serialize(out + i, key);
    i += serialize(out + i, value);
  }
  return i;
}

template<typename K, typename V>
inline size_t deserialize(const char* in, std::unordered_map<K, V>& map) {
  size_t i = 0, size = 0;
  i += deserialize(in + i, size);
  map.clear();
  map.reserve(size);
  for (size_t n = 0; n < size; ++n) {
    K key;
    V value;
    i += deserialize(in + i, key);
    i += deserialize(in + i, value);
    map.emplace(std::move(key), std::move(value));
  }
  return i;
}

/* Variadic helper */

template<typename T>
inline size_t ser(char* out, T&& obj) {
  return serialize(out, obj);
}

template<typename T>
inline size_t des(const char* in, T&& obj) {
  return deserialize(in, obj);
}

template<typename T, typename... Types>
inline size_t ser(char* out, T&& first, Types&&... types) {
  size_t i = 0;
  i += ser(out + i, std::forward<T>(first));
  i += ser(out + i, std::forward<Types>(types)...);
  return i;
}

template<typename T, typename... Types>
inline size_t des(const char* in, T&& first, Types&&... types) {
  size_t i = 0;
  i += des(in + i, std::forward<T>(first));
  i += des(in + i, std::forward<Types>(types)...);
  return i;
}

#endif /* SERDES_H */