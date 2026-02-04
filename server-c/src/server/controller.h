
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <functional>

#include "../core/accounts.h"
#include "../rpc/include.h"
#include "../server/callback.h"

class Controller;

class HeaderViewInterface
{
 public:
  virtual void AddController(Controller* controller) = 0;
};

class RpcViewInterface
{
 public:
  virtual void AddRpcRequest(const std::string& t, const std::string& ip, 
    const Request& req) = 0;
  virtual void AddRpcResponse(const std::string& t, const std::string& ip, 
    const Response& resp) = 0;
};

class AccountViewInterface
{
 public:
  virtual void CreateAccount(const Account& account) = 0;
  virtual void DeleteAccount(const Account& account) = 0;
  virtual void HandleDeposit(const Account& account) = 0;
  virtual void HandleWithdraw(const Account& account) = 0;
  virtual void HandleTransfer(const Account& recv_account, const Account& send_account) = 0;
  virtual void HandleExchange(const Account& account) = 0;
};

class ConsoleViewInterface
{
 public:
  virtual void WriteToConsole(const std::string& str) = 0;
};

class CallbackViewInterface
{
 public:
  virtual void CreateCallback(const CallbackData& callback) = 0;
  virtual void DeleteCallback(const CallbackData& callback) = 0;
};

class Controller
{
 public:

  Controller() = default;

  ~Controller() {};

  /* Bind view models */

  void BindHeaderViewModel(HeaderViewInterface* view);

  void BindRpcViewModel(RpcViewInterface* rpc_view);

  void BindConsoleViewModel(ConsoleViewInterface* console_view);

  void BindAccountViewModel(AccountViewInterface* account_view);

  void BindCallbackViewModel(CallbackViewInterface* callback_view);

  /* Update view models */

  void ReceiveRpcRequest(const std::string& ip, const Request& req);

  void PostRpcResponse(const std::string& ip, const Response& resp);

  void WriteToConsole(const std::string& str);

  void CreateAccount(const Account& account);

  void DeleteAccount(const Account& account);

  void Deposit(const Account& account);

  void Withdraw(const Account& account);

  void Transfer(const Account& recv_account, const Account& send_account);

  void Exchange(const Account& account);

  void CreateCallback(const CallbackData& callback);

  void DeleteCallback(const CallbackData& callback);

  /* Update server mode */

  void BindChangeModeCallback(std::function<void(mode)> callback);

  void ChangeMode(mode m);

  void BindChangeLostRateCallback(std::function<void(int)> callback);

  void ChangeLostRate(int r);

 private:

  RpcViewInterface* rpc_view_;

  ConsoleViewInterface* console_view_;

  AccountViewInterface* account_view_;

  CallbackViewInterface* callback_view_;

  /* server callbacks */

  std::function<void(mode)> change_mode_cb;

  std::function<void(int)> change_lost_rate_cb;

};

#endif /* CONTROLLER_H */