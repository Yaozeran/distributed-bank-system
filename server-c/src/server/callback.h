#ifndef CALLBACK_H
#define CALLBACK_H

#include <chrono>

#include <netinet/in.h>

class CallbackData
{
 public:
  
  CallbackData() = default;
  CallbackData(const sockaddr_in& client_addr, socklen_t len, 
    const std::chrono::steady_clock::time_point& s, const std::chrono::milliseconds d)
      : client_addr_(client_addr), client_addr_len_(len), start_(s), dur_(d) {}

  ~CallbackData() {}

  bool IsActive() {
    auto now = std::chrono::steady_clock::now();
    if (now < start_ + dur_) { return true; }
    return false;
  }

  const sockaddr_in& GetClientAddr() const { return client_addr_; }

  socklen_t GetClientAddrLen() const { return client_addr_len_; }

  const std::chrono::steady_clock::time_point& GetStart() const { return start_; }
  
  const std::chrono::milliseconds& GetDuration() const { return dur_; }

 private:
  
  sockaddr_in client_addr_;
  socklen_t client_addr_len_;
  
  std::chrono::steady_clock::time_point start_;
  std::chrono::milliseconds dur_;

};

#endif /* CALLBACK_H */