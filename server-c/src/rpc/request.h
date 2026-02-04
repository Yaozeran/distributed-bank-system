/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao
 * 
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <cstdint>
#include <cstddef>
#include <array>

#include "../serdes.h"
#include "protocol.h"

class Request
{
 public:

  constexpr Request() = default;
  constexpr Request(int id, op_code op, uint8_t* in)
      : id_(id), op_code_(op), payload_{} {
    memcpy(payload_.data(), in, payload_size - 1);
    payload_[payload_size - 1] = '\0';
  }

  inline size_t Serialize(char* out) {
    return ser(out, id_, op_code_, payload_);
  }

  inline size_t Deserialize(const char* in) {
    return des(in, id_, op_code_, payload_);
  }

  inline int GetId() const { return id_; } 

  inline op_code GetOpCode() const { return op_code_; }

  inline const char* GetPayload() const { return payload_.data(); }

 private:

  /* the request id */
  int id_;
  /* the operation code sepecified in the request */
  op_code op_code_;
  /* the data needed */
  std::array<char, payload_size> payload_;

};

#endif /* REQUEST_H */