/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao */

#include "server.h"

static inline void SetResponse(Response& response, int id, status_code s, const std::string& msg) {
  response.SetId(id);
  response.SetStatusCode(s);
  response.SetPayload(msg);
}

void Server::BindSocket(int port) {
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = INADDR_ANY;
  addr_.sin_port = htons(port);
  if (bind(sockfd_, (sockaddr*)(&addr_), sizeof(addr_)) < 0) {
    perror("bind");
    close(sockfd_);
    exit(1);
  }
}

void Server::ResetIOStreams() {
  memset(in_.data(), 0, in_buf_len);
  memset(out_.data(), 0, out_buf_len);
}

int Server::GenRandomValue(int min, int max) {
  std::uniform_int_distribution<> distr(min, max);
  return distr(gen_);
};

void Server::StartListening(int port)  {
  BindSocket(port);
  while (running_) {
    sockaddr_in client_addr{};
    socklen_t client_addr_len = sizeof(client_addr);

    ssize_t n = recvfrom(sockfd_, in_.data(), sizeof(in_), 0, (sockaddr*)(&client_addr), &client_addr_len);
    if (n < 0) {
      perror("recvfrom");
      continue;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 100);
    

    int recv_seed = GenRandomValue(1, 100);
    if (recv_seed < intv_start_ || recv_seed > intv_end_) {
      controller_.WriteToConsole("experimental simulation: package lost during receiving request");
      memset(in_.data(), 0, in_buf_len);
      continue;
    }

    Request* request = new Request();
    Response* response = new Response();

    request->Deserialize(in_.data());
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    controller_.ReceiveRpcRequest(std::string(client_ip), *request);
    
    Filter(request, response, client_addr, client_addr_len); // after this, response should be serialized to out

    int send_seed = GenRandomValue(1, 100);
    if (send_seed < intv_start_ || send_seed > intv_end_) {
      controller_.WriteToConsole("experimental simulation: package lost during posting response");
      ResetIOStreams();
      continue;
    }

    ssize_t sent = sendto(sockfd_, out_.data(), sizeof(out_), 0, (sockaddr*)(&client_addr), client_addr_len);
    if (sent < 0) {
      perror("sendto");
    }
    
    ResetIOStreams();
  }
}

void Server::ChangeMode(mode m) { 
  mode_ = m; 
  controller_.WriteToConsole("mode changed to " + mode_to_str(m));
}

void Server::ChangeLostRate(int i) {
  intv_start_ = i;
}

void Server::Filter(Request* request, Response* response, const sockaddr_in& client_addr, socklen_t len) {
  switch (mode_) {
    case mode::at_least_once: {
      // perform request again, but do not record them in history
      Dispatch(request, response, client_addr, len);
      response->Serialize(out_.data());
      delete request;
      delete response;
      request = nullptr;
      response = nullptr;
      return;
    }
    case mode::at_most_once: {
      auto iter = requests_.find(request->GetId());
      if (iter != requests_.end()) { // duplicated request
        // return previous response outcome to the client
        auto iter_resp = responses_.find(request->GetId());
        response = iter_resp->second;
        response->Serialize(out_.data());
        response = nullptr;
        delete request;
        request = nullptr;
      } else {
        Dispatch(request, response, client_addr, len);
        requests_[request->GetId()] = request;
        responses_[request->GetId()] = response;
        response->Serialize(out_.data());
      }
      return;
    } // ignore
    default: return;
  }
}

void Server::Dispatch(Request* request, Response* response, const sockaddr_in& client_addr, socklen_t len) {
  switch (request->GetOpCode()) {
    case op_code::open: {
      HandleCreateAccount(*request, *response);
      break;
    }
    case op_code::close: {
      HandleDeleteAccount(*request, *response);
      break;
    } 
    case op_code::check_balance: {
      HandleCheckBalance(*request, *response);
      break;
    }
    case op_code::deposit: {
      HandleDeposit(*request, *response);
      break;
    }
    case op_code::withdraw: {
      HandleWithdraw(*request, *response);
      break;
    }
    case op_code::transfer: {
      HandleTransfer(*request, *response);
      break;
    }
    case op_code::exchange: {
      HandleExchange(*request, *response);
      break; 
    }
    case op_code::monitor: {
      HandleMonitor(*request, *response, client_addr, len);
      break;
    }
    default: return;
  }
}

void Server::HandleCreateAccount(const Request& request, Response& response) {
  size_t i = 0;
  int id = account_id_ctr_++;
  std::string user_name;
  std::string password;
  float balance;
  currency currency;

  des(request.GetPayload(), user_name, password, balance, currency);
  Account* account = new Account(id, user_name, password, currency, balance);
  accounts_[id] = account;

  controller_.CreateAccount(*account);

  SetResponse(response, request.GetId(), status_code::success, "account created: " + account->ToString());

  InvokeCallback("account created: " + account->ToString()); 
}

void Server::HandleDeleteAccount(const Request& request, Response& response) {
  size_t i = 0;
  int id;
  std::string user_name;
  std::string password;
  i += des(request.GetPayload(), id, user_name, password);

  auto iter = accounts_.find(id);
  if (iter == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(id));
  } else if (user_name != iter->second->GetUserName()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: username not correct");
  } else if (password != iter->second->GetPassword()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: password not correct");
  } else { // delete
    delete iter->second;
    accounts_.erase(iter);
    std::unique_ptr<Account> account = std::make_unique<Account>();
    account->SetId(id);
    controller_.DeleteAccount(*account);
    SetResponse(response, request.GetId(),
      status_code::success, "successfully remove the account with id: " + std::to_string(id));
    InvokeCallback("account with id: " + std::to_string(id) + "deleted");
  }  
}

void Server::HandleCheckBalance(const Request& request, Response& response) {
  size_t i = 0;
  int id;
  std::string user_name;
  std::string password;
  currency cur_unit;
  i += des(request.GetPayload(), id, user_name, password, cur_unit);

  auto iter = accounts_.find(id);
  if (iter == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(id));
  } else if (user_name != iter->second->GetUserName()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: username not correct");
  } else if (password != iter->second->GetPassword()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: password not correct");
  } else {
    float bal = iter->second->GetBalance(cur_unit);
    SetResponse(response, request.GetId(), 
      status_code::success, "your current account balance is: " + std::to_string(bal));
  }
}

void Server::HandleDeposit(const Request& request, Response& response) {
  int id;
  std::string user_name;
  std::string password;
  currency cur_unit;
  float amount;
  des(request.GetPayload(), id, user_name, password, cur_unit, amount);

  auto iter = accounts_.find(id);
  if (iter == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(id));
  } else if (user_name != iter->second->GetUserName()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: username not correct");
  } else if (password != iter->second->GetPassword()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: password not correct");
  } else {
    float orig_bal = iter->second->GetBalance(cur_unit);
    iter->second->Deposit(cur_unit, amount);
    float curr_bal = iter->second->GetBalance(cur_unit);
    controller_.Deposit(*iter->second);
    controller_.WriteToConsole("deposit success: " + iter->second->ToString());
    SetResponse(response, request.GetId(), status_code::success, 
      "deposit success, current balance of " + currency_to_str(cur_unit) + " is: " + std::to_string(curr_bal));
    InvokeCallback(
      "successful deposit " + std::to_string(amount) + currency_to_str(cur_unit) + " to account with id: " + std::to_string(id));
  }
}

void Server::HandleWithdraw(const Request& request, Response& response) {
  int id;
  std::string user_name;
  std::string password;
  currency cur_unit;
  float amount;
  des(request.GetPayload(), id, user_name, password, cur_unit, amount);
  auto iter = accounts_.find(id);
  if (iter == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(id));
  } else if (user_name != iter->second->GetUserName()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: username not correct");
  } else if (password != iter->second->GetPassword()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: password not correct");
  } else {
    float orig_bal = iter->second->GetBalance(cur_unit);
    if (orig_bal < amount) {
      SetResponse(response, request.GetId(), 
        status_code::fail, "withdraw fails: insufficient fund");
    } else {
      iter->second->Withdraw(cur_unit, amount);
      float curr_bal = iter->second->GetBalance(cur_unit);
      controller_.Withdraw(*iter->second);
      controller_.WriteToConsole("withdraw success: " + iter->second->ToString());
      SetResponse(response, request.GetId(), status_code::success, 
        "withdraw success, current balance of " + currency_to_str(cur_unit) + " is: " + std::to_string(curr_bal));
      InvokeCallback(
        "successful withdraw " + std::to_string(amount) + currency_to_str(cur_unit) + " from account with id: " + std::to_string(id));
    }
  }
}

void Server::HandleTransfer(const Request& request, Response& response) {
  int sender_id;
  std::string user_name;
  std::string password;
  currency cur_unit;
  float amount;
  int receiver_id;
  des(request.GetPayload(), sender_id, user_name, password, cur_unit, amount, receiver_id);
  auto iter = accounts_.find(sender_id);
  auto iter_receiver = accounts_.find(receiver_id);
  if (iter == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(sender_id));
  } else if (user_name != iter->second->GetUserName()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: username not correct");
  } else if (password != iter->second->GetPassword()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: password not correct");
  } else if (iter_receiver == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(receiver_id));
  } else {
    if (iter->second->GetBalance(cur_unit) < amount) {
      SetResponse(response, request.GetId(), 
        status_code::fail, "withdraw fails: insufficient fund");
    } else {
      iter->second->Withdraw(cur_unit, amount);
      iter_receiver->second->Deposit(cur_unit, amount);
      controller_.Transfer(*iter_receiver->second, *iter->second);
      controller_.WriteToConsole(
        "transferred " + std::to_string(amount) + " " + currency_to_str(cur_unit) + 
        " to account with id: " + std::to_string(receiver_id));
      SetResponse(response, request.GetId(), 
        status_code::success, 
        "transferred " + std::to_string(amount) + " " + currency_to_str(cur_unit) + 
        " to account with id: " + std::to_string(receiver_id));
      InvokeCallback(
        "transferred " + std::to_string(amount) + " " + currency_to_str(cur_unit) + 
        " to account with id: " + std::to_string(receiver_id));
    }
  };
}

void Server::HandleExchange(const Request& request, Response& response) {
  int id;
  std::string user_name;
  std::string password;
  currency from_cur_unit;
  currency to_cur_unit;
  float amount_to_exchange;
  des(request.GetPayload(), id, user_name, password, from_cur_unit, to_cur_unit, amount_to_exchange);
  auto iter = accounts_.find(id);
  if (iter == accounts_.end()) {
    SetResponse(response, request.GetId(), 
      status_code::error, "account not found with id: " + std::to_string(id));
  } else if (user_name != iter->second->GetUserName()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: username not correct");
  } else if (password != iter->second->GetPassword()) {
    SetResponse(response, request.GetId(), 
      status_code::fail, "authentication fails: password not correct");
  } else {
    float amount_needed = convert(amount_to_exchange, from_cur_unit, to_cur_unit);
    if (iter->second->GetBalance(from_cur_unit) < amount_needed) {
      SetResponse(response, request.GetId(), 
        status_code::fail, "withdraw fails: insufficient fund");
    } else {
      iter->second->Withdraw(from_cur_unit, amount_needed);
      iter->second->Deposit(to_cur_unit, amount_to_exchange);
      controller_.Exchange(*iter->second);
      controller_.WriteToConsole("exchange successfully: " + iter->second->ToString());
      SetResponse(response, request.GetId(), 
        status_code::success, 
        "exchange successfully: " + iter->second->ToString());
      InvokeCallback("exchange successfully: " + iter->second->ToString());
    }
  }
}

void Server::HandleMonitor(const Request& request, Response& response, const sockaddr_in& client_addr, socklen_t len) {
  int64_t d;
  des(request.GetPayload(), d);
  bool flag = false;
  // assume no monitor request sent when one with same ip is active

  auto iter = callbacks_.begin();
  while (iter != callbacks_.end()) {
    if (
      iter->GetClientAddr().sin_addr.s_addr == client_addr.sin_addr.s_addr &&
      iter->GetClientAddr().sin_port == client_addr.sin_port
    ) {
      if (iter->IsActive()) {
        flag = true;
      } else {
        controller_.DeleteCallback(*iter);
        iter = callbacks_.erase(iter);
        continue;
      }
    }
    iter++;
  }
  if (flag) {
    SetResponse(response, request.GetId(), status_code::fail, "monitor window already exists");
  } else {
    CallbackData cb{client_addr, len, std::chrono::steady_clock::now(), std::chrono::milliseconds(d)};
    callbacks_.push_back(cb);
    controller_.WriteToConsole("new callback created");
    controller_.CreateCallback(cb);
    SetResponse(response, request.GetId(), status_code::success, "new monitor window created");
  }
}

/* Helper: send callback result to the client */
void Server::InvokeCallback(const std::string& msg) {
  size_t len = msg.length();
  for (auto& cb : callbacks_) {
    if (cb.IsActive()) {
      std::copy(msg.begin(), msg.begin() + len, callback_out_.data());
      ssize_t sent = sendto(
        sockfd_, callback_out_.data(), sizeof(callback_out_), 0, (sockaddr*)(&cb.GetClientAddr()), cb.GetClientAddrLen()
      );
      if (sent < 0) { perror("sendto"); }
      controller_.WriteToConsole("monitor callback send: " + msg);
      memset(callback_out_.data(), 0, out_buf_len);
    }
  } 
}