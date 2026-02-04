/* Copyright 2026 (c), Yao Zeran, Zhang Chenzhi, Zhang Senyao */

#include "datapanel.h"

#include <arpa/inet.h>

#include <QVBoxLayout>
#include <QHeaderView>

#include "delegate.h"

AccountPanel::AccountPanel(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* acnts_layout = new QVBoxLayout(this);
  table_ = new QTableView(this);
  accounts_ = new QStandardItemModel(0, 4, this);
  // set up 
  accounts_->setHorizontalHeaderLabels({"Account number", "User name", "Password", "Balance"});
  table_->setWordWrap(true);
  table_->setModel(accounts_);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setItemDelegate(new PaddingDelegate(this));
  table_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  table_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  table_->verticalHeader()->setVisible(false);
  // config widget layout 
  acnts_layout->addWidget(table_);
  setLayout(acnts_layout);
  
  CreateAccount("-1", "zeran", "12345678", "10.00 RMB");
  CreateAccount("-1", "chenzhi", "12345678", "10.00 SGD\n20.00 RMB");
  CreateAccount("-1", "senyao", "12345678", "5.00 SGD\n80.00 RMB");
}

void AccountPanel::CreateAccount(const Account& account) {
  QString id = QString::number(account.GetId());
  QString user_name = QString::fromStdString(account.GetUserName());
  QString password = QString::fromStdString(account.GetPassword());
  QString balance;
  for (const auto& [cur, amount] : account.GetBalance()) {
    balance += QString::number(amount, 'f', 2) + QString::fromStdString(currency_to_str(cur)) + "\n";
  }
  CreateAccount(id, user_name, password, balance);
}

void AccountPanel::CreateAccount(const QString& id, const QString& user_name, 
    const QString& password, const QString& balance) {
  QMetaObject::invokeMethod(this, [this, id, user_name, password, balance]() {
    QList<QStandardItem*> row;
    row << new QStandardItem(id);
    row << new QStandardItem(user_name);
    row << new QStandardItem(password);
    row << new QStandardItem(balance);
    for (QStandardItem* item : row) {
      item->setTextAlignment(Qt::AlignCenter | Qt::AlignTop);
    }
    accounts_->appendRow(row);
    table_->scrollToBottom();
  }, Qt::QueuedConnection);
}

void AccountPanel::DeleteAccount(const Account& account) {
  QString id = QString::number(account.GetId());
  // Iterate backwards so deleting a row doesn't mess up the index of subsequent rows
  for (int i = accounts_->rowCount() - 1; i >= 0; --i) {
    QStandardItem* item = accounts_->item(i, 0); // Column 0 is "Account number"
    if (item && item->text() == id) {
      accounts_->removeRow(i);
      return;
    }
  }
}

void AccountPanel::HandleAccountBalanceUpdate(const Account& account) {
  QMetaObject::invokeMethod(this, [this, account]() {
    QString id = QString::number(account.GetId());
    QList<QStandardItem*> matches = accounts_->findItems(id, Qt::MatchExactly, 0);
    if (matches.isEmpty()) {
      return;
    }
    int row_idx = matches.first()->row();
    QString balance;
    for (const auto& [cur, amount] : account.GetBalance()) {
      balance += QString::number(amount, 'f', 2) + " " + QString::fromStdString(currency_to_str(cur)) + "\n";
    }
    accounts_->item(row_idx, 3)->setText(balance);
  }, Qt::QueuedConnection);
}

void AccountPanel::HandleDeposit(const Account& account) {
  HandleAccountBalanceUpdate(account);
}

void AccountPanel::HandleWithdraw(const Account& account) {
  HandleAccountBalanceUpdate(account);
}

void AccountPanel::HandleTransfer(const Account& recv_account, const Account& send_account) {
  HandleAccountBalanceUpdate(recv_account);
  HandleAccountBalanceUpdate(send_account);
}

void AccountPanel::HandleExchange(const Account& account) {
  HandleAccountBalanceUpdate(account);
}

CallbackPanel::CallbackPanel(QWidget* parent) : QWidget(parent) {
  QVBoxLayout* layout = new QVBoxLayout(this);
  table_ = new QTableView(this);
  
  // 3 Columns: Client IP:Port, Start Time, Duration (ms)
  callbacks_ = new QStandardItemModel(0, 3, this); 
  callbacks_->setHorizontalHeaderLabels({"Client Address", "Start Time", "Duration"});

  table_->setModel(callbacks_);
  table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table_->verticalHeader()->setVisible(false);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

  layout->addWidget(table_);
  setLayout(layout);
}

void CallbackPanel::CreateCallback(const CallbackData& callback) {
  QMetaObject::invokeMethod(this, [this, callback]() {
    QString ip = QString(inet_ntoa(callback.GetClientAddr().sin_addr));
    QString port = QString::number(ntohs(callback.GetClientAddr().sin_port));
    QString address = ip + ":" + port;
    auto now_system = std::chrono::system_clock::now();
    time_t t_c = std::chrono::system_clock::to_time_t(now_system);
    QString startTime = QString::fromStdString(std::ctime(&t_c)).trimmed();
    QString duration = QString::number(callback.GetDuration().count()) + " ms";
    QList<QStandardItem*> row;
    row << new QStandardItem(address);
    row << new QStandardItem(startTime);
    row << new QStandardItem(duration);
    for (auto item : row) {
        item->setTextAlignment(Qt::AlignCenter);
    }
    callbacks_->appendRow(row);
  }, Qt::QueuedConnection);
}

void CallbackPanel::DeleteCallback(const CallbackData& callback) {
  QMetaObject::invokeMethod(this, [this, callback]() {
    QString ip = QString(inet_ntoa(callback.GetClientAddr().sin_addr));
    QString port = QString::number(ntohs(callback.GetClientAddr().sin_port));
    QString address = ip + ":" + port;

    for (int i = callbacks_->rowCount() - 1; i >= 0; --i) {
        if (callbacks_->item(i, 0)->text() == address) {
            callbacks_->removeRow(i);
        }
    }
  }, Qt::QueuedConnection); 
}
