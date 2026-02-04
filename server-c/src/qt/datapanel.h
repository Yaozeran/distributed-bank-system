/* Copyright 2026 (c), Yao Zeran, Zhang Chenzhi, Zhang Senyao
 *
 * The <datapanel.h> file defines qt panel widget which shows demo data */

#ifndef DATAPANEL_H
#define DATAPANEL_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>

#include "../core/accounts.h"
#include "../server/controller.h"

class AccountPanel : public QWidget, public AccountViewInterface
{
  Q_OBJECT

 public:

  AccountPanel(QWidget* parent);

  void CreateAccount(const Account& account) override;

  void DeleteAccount(const Account& account) override;

  void HandleDeposit(const Account& account) override;

  void HandleWithdraw(const Account& account) override;

  void HandleTransfer(const Account& recv_account, const Account& send_account) override;

  void HandleExchange(const Account& account) override;
 
 private: 

  QTableView* table_;

  QStandardItemModel* accounts_;

  void CreateAccount(const QString& id, const QString& user_name, 
    const QString& password, const QString& balance_);

  void HandleAccountBalanceUpdate(const Account& account);

};

class CallbackPanel : public QWidget, public CallbackViewInterface
{
  Q_OBJECT

 public:

  CallbackPanel(QWidget* parent);

  void CreateCallback(const CallbackData& callback) override;

  void DeleteCallback(const CallbackData& callback) override;

 private:

  QTableView* table_;

  QStandardItemModel* callbacks_;
  
};

#endif /* DATAPANEL_H */