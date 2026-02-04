/* Copyright 2026 (c), Yao Zeran, Zhang Chenzhi, Zhang Senyao */

#include "rpcpanel.h"

#include <QVBoxLayout>
#include <QHeaderView>

#include "../rpc/protocol.h"
#include "delegate.h"

#include <iostream>

RpcPanel::RpcPanel(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* rpc_layout = new QVBoxLayout(this);
  table_view_ = new QTableView(this);
  rpc_log_ = new QStandardItemModel(0, 5, this);
  rpc_log_->setHorizontalHeaderLabels({"Timestamp", "Type", "Ip", "Id", "Code"});
  table_view_->setModel(rpc_log_);
  table_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_view_->setItemDelegate(new PaddingDelegate(this));
  table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  table_view_->verticalHeader()->setVisible(false);
  rpc_layout->addWidget(table_view_);
  setLayout(rpc_layout);
}

void RpcPanel::AddRpcRequest(const std::string& t, const std::string& ip, const Request& req) {
  QString t_ = QString::fromStdString(t);
  QString ip_ = QString::fromStdString(ip);
  QString id_ = QString::number(req.GetId());
  QString op_code_ = QString::fromStdString(op_code_to_str(req.GetOpCode()));
  AddRpcEntry(t_, "RECV", ip_, id_, op_code_);
}

void RpcPanel::AddRpcResponse(const std::string& t, const std::string& ip, const Response& response) {
  QString t_ = QString::fromStdString(t);
  QString ip_ = QString::fromStdString(ip);
  QString id_ = QString::number(response.GetId());
  QString status_code_ = QString::fromStdString(status_code_to_str(response.GetStatusCode()));
  AddRpcEntry(t_, "POST", ip_, id_, status_code_); 
}

void RpcPanel::AddRpcEntry(const QString& timestamp, const QString& rpc_type, 
    const QString& ip, const QString& id, const QString& op_code) {
  QMetaObject::invokeMethod(this, [this, timestamp, rpc_type, ip, id, op_code]() {
    auto MakeItem = [](const QString& text) {
      QStandardItem* item = new QStandardItem(text);
      item->setTextAlignment(Qt::AlignCenter);
      return item;
    };
    QList<QStandardItem*> row;
    row << MakeItem(timestamp);
    QStandardItem* rpc_item = MakeItem(rpc_type);
    rpc_item->setForeground(rpc_type == "RECV" ? QBrush(Qt::blue) : QBrush(Qt::magenta));
    row << (rpc_item);
    row << MakeItem(ip);
    row << MakeItem(id);
    row << MakeItem(op_code);
    rpc_log_->appendRow(row);
    table_view_->scrollToBottom();
  }, Qt::QueuedConnection);
}


RpcConsole::RpcConsole(QWidget* parent) : QWidget(parent) {
    // 1. Create the layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    // 2. Initialize the text edit
    consoleOutput = new QPlainTextEdit(this);
    consoleOutput->setReadOnly(true); // Prevent user from typing directly
    // 3. Optional: Make it look like a real console
    // consoleOutput->setStyleSheet(
    //     "background-color: black; "
    //     "color: #00FF00; "
    //     "font-family: 'Courier New', monospace;"
    // );
    layout->addWidget(consoleOutput);
    setLayout(layout);
}

void RpcConsole::WriteToConsole(const std::string& str) {
  QMetaObject::invokeMethod(this, [this, str]() {
    // Convert std::string to QString and append
    // appendPlainText adds a newline automatically
    consoleOutput->appendPlainText(QString::fromStdString(str));
    // 4. Auto-scroll to the bottom
    consoleOutput->ensureCursorVisible();
  }, Qt::QueuedConnection); 
}