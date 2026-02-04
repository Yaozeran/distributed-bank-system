/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao
 *
 * The <server.h> file implements a simple udp server. */

#ifndef SERVER_H
#define SERVER_H

#include <cstddef>
#include <memory>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <utility>
#include <chrono>
#include <random>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../core/accounts.h"
#include "../rpc/include.h"
#include "../serdes.h"
#include "callback.h"
#include "controller.h"

constexpr size_t in_buf_len = 200 + payload_size;
constexpr size_t out_buf_len = 200 + payload_size;

class Server
{
 public:

  Server(int port) : controller_{}, 
      running_(true), mode_(mode::at_most_once), rd_{}, gen_(rd_()),
      in_{}, out_{}, callback_out_{},
      requests_{}, responses_{},
      account_id_ctr_(0), accounts_{}, callbacks_{} {
    controller_.BindChangeModeCallback([this](mode m)->void {
      this->ChangeMode(m);
    });
    controller_.BindChangeLostRateCallback([this](int i)->void {
      this->ChangeLostRate(i);
    });
    thread_ptr_ = std::make_unique<std::thread>(&Server::StartListening, this, port);
  }

  ~Server() {
    running_ = false;
    close(sockfd_);
    thread_ptr_->join();
    for (auto& [_, req] : requests_) { delete req; }
    for (auto& [_, resp] : responses_) { delete resp; }
    for (auto& [_, acc] : accounts_) { delete acc; }
  };

  void BindHeaderViewModel(HeaderViewInterface* view) { controller_.BindHeaderViewModel(view); }

  void BindRpcViewModel(RpcViewInterface* view) { controller_.BindRpcViewModel(view); }

  void BindConsoleViewModel(ConsoleViewInterface* view) { controller_.BindConsoleViewModel(view); }

  void BindAccountViewModel(AccountViewInterface* view) { controller_.BindAccountViewModel(view); }

  void BindCallbackViewModel(CallbackViewInterface* view) { controller_.BindCallbackViewModel(view); }
  
 private:

  /* the controller to which gui is bounded
   *   used to update gui view model */
  Controller controller_;

  /* atomic bool indicator of whether the server is running */
  std::atomic<bool> running_;
  /* pointer to the main thread for server to listen requests 
   *   on socket with sockfd_, addr_, and port */
  std::unique_ptr<std::thread> thread_ptr_;

  /* socket descriptor */
  int sockfd_;
  /* address of the socket */
  sockaddr_in addr_;

  /* input stream buffer of client request datagram */
  std::array<char, in_buf_len> in_;
  /* output stream buffer of server response datagram */
  std::array<char, out_buf_len> out_;
  /* output stream buffer of callback response datagram */
  std::array<char, out_buf_len> callback_out_;

  /* history of requests received from client 
   *   key: request id, value: pointer to request object on heap */
  std::unordered_map<int, Request*> requests_;
  /* history of attempted response the server posted to client's request
   *   key: request id, value: pointer to response object on heap */
  std::unordered_map<int, Response*> responses_;

  /* the account id counter */
  int account_id_ctr_;
  /* database: all accounts registered by clients */
  std::unordered_map<int, Account*> accounts_;
  /* database: all callbacks that client send to server 
   *   in order to receive update on their account balance */
  std::vector<CallbackData> callbacks_;

  std::random_device rd_;
  std::mt19937 gen_;
  int intv_start_ = 0, intv_end_ = 100;

  /* mode specifying the udp semantic
   * 
   *   at least once: when client sends duplicated request, 
   *     operate all regardless of idempotency
   * 
   *   at most once: when client sends duplicated request, do not perform 
   *     operation, post previous response again */
  mode mode_;



  /* The main thread function, consistently listen for client's requests 
   *   on a given port number */
  void StartListening(int port);

  /* Helpers */

  void BindSocket(int port);

  void ResetIOStreams();

  void ChangeMode(mode m);

  void ChangeLostRate(int i);

  void Filter(Request* request, Response* response, const sockaddr_in& client_addr, socklen_t len);

  int GenRandomValue(int min, int max);

  /* Dispatch the tasks by the request's operation code */
  void Dispatch(Request* request, Response* response, const sockaddr_in& client_addr, socklen_t len);

  /* Send message to client with active monitor window to inform updates on all accounts */
  void InvokeCallback(const std::string& msg);

  /* Handler helper functions */

  void HandleCreateAccount(const Request& request, Response& response);

  void HandleDeleteAccount(const Request& request, Response& repsonse);

  void HandleCheckBalance(const Request& request, Response& response);

  void HandleDeposit(const Request& request, Response& response);

  void HandleWithdraw(const Request& request, Response& response);

  void HandleTransfer(const Request& request, Response& response);

  void HandleExchange(const Request& request, Response& response);

  void HandleMonitor(const Request& request, Response& response, const sockaddr_in& client_addr, socklen_t len);

};

#endif /* SERVER_H */