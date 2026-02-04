#include "header.h"

HeaderWidget::HeaderWidget(QWidget* parent) : QWidget(parent) {

  m_ = mode::at_most_once;
  rand_intv_ = 0;

  QHBoxLayout* main_layout = new QHBoxLayout(this);

  QWidget* title_cont = new QWidget();

  // title label
  QVBoxLayout* title_layout = new QVBoxLayout(title_cont);
  title_layout->setContentsMargins(0, 0, 0, 0);
  title_layout->setSpacing(2);
  QLabel* main_title = new QLabel("Distributed Bank", this);
  main_title->setStyleSheet(
    "font-size: 18px; font-weight: bold; color: #2c3e50;"
  );
  QLabel* sub_title = new QLabel("using udp and rpc protocol", this);
  sub_title->setStyleSheet(
    "font-style: italic; font-size: 12px; color: #7f8c8d;"
  );
  title_layout->addWidget(main_title);
  title_layout->addWidget(sub_title);

  // dashboard
  mode_btn = new QPushButton("at-most-once", this);
  mode_btn->setCheckable(true);
  mode_btn->setFixedWidth(150);
  mode_btn->setStyleSheet(
    "QPushButton { padding: 8px; border-radius: 4px; background-color: #ecf0f1; border: 1px solid #bdc3c7; }"
    "QPushButton:checked { background-color: #3498db; color: white; border: 1px solid #2980b9; }"
  );
  connect(mode_btn, &QPushButton::toggled, this, [this](bool) {
    switch (m_) {
      case mode::at_least_once: {
        m_ = mode::at_most_once;
        mode_btn->setText("at-most-once");
        controller_->ChangeMode(mode::at_most_once);
        break;
      }
      case mode::at_most_once: {
        m_ = mode::at_least_once;
        mode_btn->setText("at-least-once");
        controller_->ChangeMode(mode::at_least_once);
        break;
      }
      default: break;
    }
  });

  slider_ = new QSlider(Qt::Horizontal, this);
  QLabel* value_label = new QLabel("Value: 0", this);
  slider_->setRange(0, 100); 
  slider_->setValue(0);
  connect(slider_, &QSlider::valueChanged, this, [this, value_label](int newValue) {
    value_label->setText(QString("Value: %1").arg(newValue));
    // 2. Send the data to the controller
    // Assuming your view has a pointer to the controller
    controller_->ChangeLostRate(newValue); 
  });

  // --- Assemble ---
  main_layout->addWidget(title_cont);
  main_layout->addStretch(); // Pushes the button to the far right
  main_layout->addWidget(mode_btn);
  main_layout->addWidget(value_label);
  main_layout->addWidget(slider_);
  
  // Optional: Add a bottom border for visual separation
  this->setStyleSheet("HeaderWidget { border-bottom: 2px solid #ddd; background-color: white; }");
}