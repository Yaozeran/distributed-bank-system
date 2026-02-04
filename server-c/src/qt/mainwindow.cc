#include "mainwindow.h"

#include <QGuiApplication>
#include <QScreen>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {

  setWindowTitle("Bank Server");
  QScreen* screen = QGuiApplication::primaryScreen();
  QRect available = screen->availableGeometry();
  resize(available.width() * 0.7, available.height() * 0.6);
  move(available.topLeft());
  
  QWidget* main_widget = new QWidget();
  setCentralWidget(main_widget);

  QVBoxLayout* main_layout = new QVBoxLayout(main_widget);

  QHBoxLayout* header_layout = new QHBoxLayout();
  header_ = new HeaderWidget(main_widget);
  header_layout->addWidget(header_);

  QHBoxLayout* body_layout = new QHBoxLayout();

  QVBoxLayout* net_layout = new QVBoxLayout();
  rpc_panel_ = new RpcPanel(main_widget);
  rpc_console_ = new RpcConsole(main_widget);
  net_layout->addWidget(rpc_panel_, 7);
  net_layout->addWidget(rpc_console_, 3);

  QVBoxLayout* data_layout = new QVBoxLayout();
  acnt_panel_ = new AccountPanel(main_widget);
  callback_panel_ = new CallbackPanel(main_widget);
  data_layout->addWidget(acnt_panel_, 8);
  data_layout->addWidget(callback_panel_, 2);

  body_layout->addLayout(net_layout, 6);
  body_layout->addLayout(data_layout, 4);

  main_layout->addLayout(header_layout, 1);
  main_layout->addLayout(body_layout, 9);
}

MainWindow::~MainWindow() {
  // delete header_;
  // delete rpc_panel_;
  // delete rpc_console_;
  // delete acnt_panel_;
}