/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao
 * 
 * The <protocol.h> file defines udp and rpc protocols */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <optional>

/* udp payload size */
constexpr int payload_size = 1200;

/* rpc operation code */
enum class op_code {
  open = 1, close, check_balance, deposit, withdraw, transfer, exchange, monitor
};

inline std::optional<op_code> int_to_op_code(int i) {
  switch (i) {
    case 1: return op_code::open;
    case 2: return op_code::close;
    case 3: return op_code::check_balance;
    case 4: return op_code::deposit;
    case 5: return op_code::withdraw;
    case 6: return op_code::transfer;
    case 7: return op_code::exchange;
    case 8: return op_code::monitor;
    default: return std::nullopt;
  }
}

inline int op_code_to_int(op_code c) {
  switch (c) {
    case op_code::open: return 1;
    case op_code::close: return 2;
    case op_code::check_balance: return 3;
    case op_code::deposit: return 4;
    case op_code::withdraw: return 5;
    case op_code::transfer: return 6;
    case op_code::exchange: return 7;
    case op_code::monitor: return 8;
    default: return -1;
  }
}

inline std::string op_code_to_str(op_code c) {
  switch (c) {
    case op_code::open: return "open";
    case op_code::close: return "close";
    case op_code::check_balance: return "check";
    case op_code::deposit: return "deposit";
    case op_code::withdraw: return "withdraw";
    case op_code::transfer: return "transfer";
    case op_code::exchange: return "exchange";
    case op_code::monitor: return "monitor";
    default: return "error";
  }
}

enum class status_code {
  success = 1, fail = 2, error = 3, callback = 4
};

inline std::optional<status_code> int_to_status_code(int i) {
  switch (i) {
    case 1: return status_code::success;
    case 2: return status_code::fail;
    case 3: return status_code::error;
    case 4: return status_code::callback;
    default: return std::nullopt;
  }
}

inline int status_code_to_int(status_code c) {
  switch (c){
    case status_code::success: return 1;
    case status_code::fail: return 2;
    case status_code::error: return 3;
    case status_code::callback: return 4;
    default: return -1;
  }
}

inline std::string status_code_to_str(status_code c) {
  switch (c){
    case status_code::success: return "success";
    case status_code::fail: return "fail";
    case status_code::error: return "error";
    case status_code::callback: return "callback";
    default: return "error";
  }
}

enum class mode {
  at_least_once = 1, at_most_once = 2, maybe = 3
};

inline std::string mode_to_str(mode m) {
  switch (m){
    case mode::at_least_once: return "at least once semantic";
    case mode::at_most_once: return "at most once semantic";
    case mode::maybe: return "maybe semantic";
    default: return "error";
  }
}

#endif /* PROTOCOL_H */