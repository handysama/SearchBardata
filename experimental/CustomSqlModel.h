#ifndef CUSTOMSQLMODEL_HPP
#define CUSTOMSQLMODEL_HPP

#include <QAbstractTableModel>
#include <QDebug>
#include <QSqlQueryModel>
#include <QtWidgets>

class CustomSqlModel : public QSqlQueryModel {
  Q_OBJECT

  public:
    CustomSqlModel(QObject *parent = 0):QSqlQueryModel(parent) {

    }

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE {
      QVariant value = QSqlQueryModel::data(index, role);
      if (value.isValid() && role == Qt::DisplayRole) {
        if (index.column() == 0)
          return value.toString().prepend("#");
        else if (index.column() == 2)
          return value.toString().toUpper();
      }
      if (role == Qt::TextColorRole && index.column() == 1)
        return QVariant::fromValue(QColor(Qt::blue));
      return value;
    }
};

#endif // CUSTOMSQLMODEL_HPP
