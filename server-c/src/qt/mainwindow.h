#pragma once

#include <QMainWindow>

#include "header.h"
#include "rpcpanel.h"
#include "datapanel.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT
 
 public:

  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow();

  HeaderWidget* GetHeader() {return header_; }

  RpcPanel* GetRpcPanel() { return rpc_panel_; }

  RpcConsole* GetRpcConsole() { return rpc_console_; }

  AccountPanel* GetAccountPanel() { return acnt_panel_; }

  CallbackPanel* GetCallbackPanel() { return callback_panel_; }

 private:

  HeaderWidget* header_;

  RpcPanel* rpc_panel_; 

  RpcConsole* rpc_console_;

  AccountPanel* acnt_panel_;

  CallbackPanel* callback_panel_;

};