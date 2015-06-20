#ifndef BACKGROUNDCOLORDELEGATE
#define BACKGROUNDCOLORDELEGATE

#include <QColor>
#include <QStyledItemDelegate>

class BackgroundColorDelegate : public QStyledItemDelegate {
  public:
    BackgroundColorDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {
      color = Qt::yellow;
    }

    QColor calculateColorForRow(int row) const {
      return color;
    }

    void setColor(QColor color) {
      this->color = color;
    }

    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const {
      QStyledItemDelegate::initStyleOption(option,index);
      QStyleOptionViewItemV4 *optionV4 = qstyleoption_cast<QStyleOptionViewItemV4*>(option);
      optionV4->backgroundBrush = QBrush(calculateColorForRow(index.row()));
    }

  private:
    QColor color;
};

#endif // BACKGROUNDCOLORDELEGATE
