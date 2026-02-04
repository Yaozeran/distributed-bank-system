/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao 
 *
 * The main entry point of server. */

#include "include.h"

#include <memory>
#include <thread>

#include <QApplication>

int main(int argc, char* argv[]) {

  QApplication app(argc, argv);
  MainWindow main_window;

  std::unique_ptr<Server> server = std::make_unique<Server>(8080);
  server->BindHeaderViewModel(main_window.GetHeader());
  server->BindRpcViewModel(main_window.GetRpcPanel());
  server->BindConsoleViewModel(main_window.GetRpcConsole());
  server->BindAccountViewModel(main_window.GetAccountPanel());
  server->BindCallbackViewModel(main_window.GetCallbackPanel());
  
  main_window.show();
  return app.exec();
}