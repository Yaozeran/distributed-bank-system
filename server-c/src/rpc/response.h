/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao
 * 
 * The <response.h> file defines the rpc response class. */

#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <array>
#include <cstddef>
#include <cstdint>

#include "../serdes.h"
#include "protocol.h"

class Response
{
 public:

  Response() = default;
  constexpr Response(status_code status_code, std::string str)
      : status_code_(status_code), payload_{} {
    size_t len = str.size();
    memcpy(payload_.data(), str.data(), len);
    payload_[len] = '\0';
  }

  inline size_t Serialize(char* out) {
    return ser(out, id_, status_code_, payload_);
  }

  inline size_t Deserialize(const char* in) {
    return des(in, id_, status_code_, payload_);
  }

  inline int GetId() const { return id_; }
  inline void SetId(int id) { id_ = id; }

  inline status_code GetStatusCode() const { return status_code_; }
  inline void SetStatusCode(status_code c) { status_code_ = c; }

  inline char* GetPayload() { return payload_.data(); }
  inline void SetPayload(const std::string& data) { 
    if (data.size() > payload_size) {}
    size_t len = data.size();
    std::memcpy(payload_.data(), data.data(), len);
    payload_[len] = '\0';
  }

 private: 

  int id_;
  status_code status_code_;
  std::array<char, payload_size> payload_;

};



#endif /* RESPONSE_H */