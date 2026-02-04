/* Copyright (c) 2026, Yaozeran, Zhangchenzhi, Zhangsenyao
 *
 * The <accounts.h> file implements bank accounts related business logic. */

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <string>
#include <unordered_map>

#include "../serdes.h"
#include "currency.h"

class Account 
{
 public:

  Account() = default;
  Account(int id, std::string name, std::string pass, currency cur, float bal) 
      : id_(id), user_name_(name), password_(pass), balance_{} {
    SetBalance(cur, bal);
  }
  
  ~Account() = default;

  inline size_t Serialize(char* out) {  
    return ser(out, id_, user_name_, password_, balance_);
  }

  inline size_t Deserialize(const char* in) {

    return des(in, id_, user_name_, password_, balance_);
  }

  std::string ToString() const {
    std::string result = "Account { ";
    result += "id: " + std::to_string(id_);
    result += ", holder_name: " + user_name_;
    result += ", password: " + password_;
    result += ", balance: { ";
    bool first = true;
    for (const auto& [cur, amount] : balance_) {
      if (!first) result += ", ";
      first = false;

      result += currency_to_str(cur);
      result += ": ";
      result += std::to_string(amount);
    }
    result += " } }";
    return result;
  }

  inline float GetBalance(currency c) const {
    auto iter = balance_.find(c);
    if (iter == balance_.end()) { return 0.0; }
    return iter->second;
  }

  inline void Deposit(currency c, float amount) {
    auto iter = balance_.find(c);
    if (iter == balance_.end()) { 
      balance_[c] = 0.0; 
      iter = balance_.find(c);
    }
    iter->second += amount;
  }

  inline void Withdraw(currency c, float amount) {
    auto iter = balance_.find(c);
    if (iter == balance_.end()) { }
    if (iter->second < amount) { }
    iter->second -= amount;
  }

  /* Getters and Setters */

  inline int GetId() const { return id_; }

  inline void SetId(int id) { id_ = id; }

  inline std::string GetUserName() const { return user_name_; }

  inline void SetUserName(const std::string& str) { user_name_ = str; }

  inline std::string GetPassword() const { return password_; }

  inline void SetPassword(const std::string& str) { password_ = str; }

  inline const std::unordered_map<currency, float>& GetBalance() const { return balance_; }

  inline void SetBalance(currency cur, float amount) { balance_[cur] = amount; }
  
 private: 
 
  /* the account number */
  int id_;

  /* the user name */
  std::string user_name_;

  /* the password */
  std::string password_;

  /* the balance in terms of each type of currency */
  std::unordered_map<currency, float> balance_;

};

#endif /* ACCOUNTS_H */