/* Copyright 2026 (c), Yao Zeran, Zhang Chenzhi, Zhang Senyao
 *
 * The <rpcpanel.h> file defines qt rpc panel widget which shows rpc calls */

#ifndef GUI_RPCPANEL_H
#define GUI_RPCPANEL_H

#include <QWidget>
#include <QTableView>
#include <QPlainTextEdit>
#include <QStandardItemModel>

#include "../rpc/request.h"
#include "../server/controller.h"


class RpcPanel: public QWidget, public RpcViewInterface
{

  Q_OBJECT 

 public: 

  RpcPanel(QWidget* parent);

  void AddRpcRequest(const std::string& t, const std::string& ip, const Request& req) override;

  void AddRpcResponse(const std::string& t, const std::string& ip, const Response& resp) override;


 private:

  QTableView* table_view_;

  QStandardItemModel* rpc_log_; 

  void AddRpcEntry(const QString& timestamp, const QString& rpc_type, 
    const QString& ip, const QString& id, const QString& op_code);

};

class RpcConsole : public QWidget, public ConsoleViewInterface
{
  Q_OBJECT

 public:
  
  RpcConsole(QWidget* parent);

  void WriteToConsole(const std::string& str) override;

 private:
  
  QPlainTextEdit* consoleOutput;
  
};

#endif /* GUI_RPCPANEL_H */