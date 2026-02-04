/* Copyright (c) 2026, Yaozeran, Zhangchenzhi, Zhangsenyao
 * 
 * This file implements currency related business logic.
 */

#ifndef CURRENCY_H
#define CURRENCY_H

#include <string>
#include <optional>
#include <array>

enum class currency {
  usd = 0, rmb, sgd, jpy, bpd, count
};

inline std::optional<currency> str_to_currency(std::string str) {
  if (str == "USD") return currency::usd;
  if (str == "RMB") return currency::rmb; 
  if (str == "SGD") return currency::sgd;
  if (str == "JPY") return currency::jpy;
  if (str == "BPD") return currency::bpd;
  return std::nullopt;
}

inline std::string currency_to_str(currency c) {
  switch (c) {
    case currency::usd: return "USD";
    case currency::rmb: return "RMB";
    case currency::sgd: return "SGD";
    case currency::jpy: return "JPY";
    case currency::bpd: return "BPD";
    default: return "ERR";
  }
}

inline constexpr float exchange_table[(int)currency::count][(int)currency::count] = {
    {1.0000,  7.2300,  1.3400,  150.50,  0.7900},
    {0.1383,  1.0000,  0.1853,  20.810,  0.1093},
    {0.7463,  5.3960,  1.0000,  112.31,  0.5896},
    {0.0066,  0.0480,  0.0089,  1.0000,  0.0052},
    {1.2658,  9.1491,  1.6960,  192.30,  1.0000}
};

inline float convert(float amount_to_exchange, currency from, currency to) {
    return amount_to_exchange / exchange_table[(int)from][int(to)];
}

#endif /* CURRENCY_H */