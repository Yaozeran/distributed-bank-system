#ifndef HEADER_H
#define HEADER_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSlider>

#include <functional>

#include "../rpc/protocol.h"
#include "../server/controller.h"

class HeaderWidget : public QWidget, public HeaderViewInterface
{
  Q_OBJECT

 public:

  explicit HeaderWidget(QWidget* parent);

  void AddController(Controller* controller) override { controller_ = controller; }

 private:

  mode m_;
  QPushButton* mode_btn;
  
  int rand_intv_;
  QSlider* slider_;

  Controller* controller_;

};

#endif /* HEADER_H */