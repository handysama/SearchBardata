/** Experimental */
#ifndef COMBOBOXITEMDELEGATE_H
#define COMBOBOXITEMDELEGATE_H

#include <QComboBox>
#include <QPainter>
#include <QStyledItemDelegate>

class ComboBoxItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    ComboBoxItemDelegate(QObject *parent = 0): QStyledItemDelegate(parent) {
    }

    ~ComboBoxItemDelegate() {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const {
//      if (index.data().canConvert<StarRating>()) {
//        StarRating starRating = qvariant_cast<StarRating>(index.data());
//        if (option.state & QStyle::State_Selected)
//          painter->fillRect(option.rect, option.palette.highlight());
//        starRating.paint(painter, option.rect, option.palette,
//                         StarRating::ReadOnly);
//      } else {
//        QStyledItemDelegate::paint(painter, option, index);
//      }

      QStyledItemDelegate::paint(painter, option, index);
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const {
//      if (index.data().canConvert<StarRating>()) {
//        StarRating starRating = qvariant_cast<StarRating>(index.data());
//        return starRating.sizeHint();
//      } else {
        return QStyledItemDelegate::sizeHint(option, index);
//      }
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
      // ComboBox ony in column 2
//      if (index.column() != 1)
//        return QStyledItemDelegate::createEditor(parent, option, index);

      // Create the combobox and populate it
      QComboBox *cb = new QComboBox(parent);
      int row = index.row();
      cb->addItem(QString("one in row %1").arg(row));
      cb->addItem(QString("two in row %1").arg(row));
      cb->addItem(QString("three in row %1").arg(row));
      return cb;
    }

    void setEditorData (QWidget *editor, const QModelIndex &index) const {
      if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        // get the index of the text in the combobox that matches the current value of the itenm
        QString currentText = index.data(Qt::EditRole).toString();
        int cbIndex = cb->findText(currentText);
        // if it is valid, adjust the combobox
        if (cbIndex >= 0)
          cb->setCurrentIndex(cbIndex);
      } else {
        QStyledItemDelegate::setEditorData(editor, index);
      }
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
      if (QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        // save the current text of the combo box as the current value of the item
        model->setData(index, cb->currentText(), Qt::EditRole);
      } else {
        QStyledItemDelegate::setModelData(editor, model, index);
      }
    }
};

#endif // COMBOBOXITEMDELEGATE_H
