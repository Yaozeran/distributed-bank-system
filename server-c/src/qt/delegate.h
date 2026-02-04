/* Copyright (c) 2026, Yao Zeran, Zhang Chenzhi, Zhang Senyao */

#include <QStyledItemDelegate>

class PaddingDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    // This adds the "padding" to the calculation of column widths
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize s = QStyledItemDelegate::sizeHint(option, index);
        return QSize(s.width() + 20, s.height() + 10); // +20px horizontal padding
    }
};

