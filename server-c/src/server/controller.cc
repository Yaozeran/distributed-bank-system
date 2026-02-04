#include "controller.h"

#include <iostream>
#include <utility>
#include <chrono>

#include "../core/accounts.h"

static constexpr std::string GetCurrentTimeStamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_t = std::chrono::system_clock::to_time_t(now);
  std::string time_str = std::ctime(&now_t);
  if (!time_str.empty() && time_str.back() == '\n') {
    time_str.pop_back();
  }
  return time_str;
}

/* Binders */

void Controller::BindHeaderViewModel(HeaderViewInterface* view) {
  view->AddController(this);
}

void Controller::BindRpcViewModel(RpcViewInterface* view) {
  rpc_view_ = view;
}

void Controller::BindConsoleViewModel(ConsoleViewInterface* view) {
  console_view_ = view;
}

void Controller::BindAccountViewModel(AccountViewInterface* view) {
  account_view_ = view;
}

void Controller::BindCallbackViewModel(CallbackViewInterface* view) {
  callback_view_ = view;
}

void Controller::BindChangeModeCallback(std::function<void(mode)> cb) {
  change_mode_cb = cb;
}

void Controller::BindChangeLostRateCallback(std::function<void(int)> cb) {
  change_lost_rate_cb = cb;
}

/* Change server mode */

void Controller::ChangeMode(mode m) {
  change_mode_cb(m);
}

void Controller::ChangeLostRate(int r) {
  change_lost_rate_cb(r);
}

/* Rpc view */

void Controller::ReceiveRpcRequest(const std::string& ip, const Request& req) {
  std::string time_stamp = GetCurrentTimeStamp();
  rpc_view_->AddRpcRequest(time_stamp, ip, req);
}

void Controller::PostRpcResponse(const std::string& ip, const Response& resp) {
  std::string time_stamp = GetCurrentTimeStamp();
  rpc_view_->AddRpcResponse(time_stamp, ip, resp);
}

/* Console view */

void Controller::WriteToConsole(const std::string& msg) {
  console_view_->WriteToConsole(msg);
}

/* Account view */

void Controller::CreateAccount(const Account& account) {
  account_view_->CreateAccount(account);
  std::cout << account.ToString() << " created. " << std::endl;
}

void Controller::DeleteAccount(const Account& account) {
  account_view_->DeleteAccount(account);
  std::cout << account.ToString() << " deleted. " << std::endl; 
}

void Controller::Deposit(const Account& account) {
  account_view_->HandleDeposit(account);
}

void Controller::Withdraw(const Account& account) {
  account_view_->HandleWithdraw(account);
}

void Controller::Transfer(const Account& recv_account, const Account& send_account) {
  account_view_->HandleTransfer(recv_account, send_account);
}

void Controller::Exchange(const Account& account) {
  account_view_->HandleExchange(account);
}

/* Callback view */

void Controller::CreateCallback(const CallbackData& cb) {
  callback_view_->CreateCallback(cb);
}

void Controller::DeleteCallback(const CallbackData& cb) {
  callback_view_->DeleteCallback(cb);
}