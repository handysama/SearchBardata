#ifndef SRMODEL_H
#define SRMODEL_H

#include <QColor>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlTableModel>

#define FASTAVG_LENGTH 10
#define SLOWAVG_LENGTH 50
#define DATE_COLUMN  0
#define TIME_COLUMN  1
#define OPEN_COLUMN  2
#define HIGH_COLUMN  3
#define LOW_COLUMN   4
#define CLOSE_COLUMN 5
#define MACD_COLUMN  8

class SRModel : public QSqlTableModel {
    Q_OBJECT

  public:
    SRModel(const double &threshold) {
      this->threshold = threshold;
      this->SRstart = -1;
      this->disable_text_color = false;
    }

    void setQuery(const QSqlQuery &query) {
      QSqlTableModel::setQuery(query);

      QStringList s;
      QString column_name;
      const int column_count = this->columnCount();

      for (int i = 0; i < column_count; ++i) {
        column_name = headerData(i, Qt::Horizontal).toString();

        if (column_name.endsWith(QString::number(FASTAVG_LENGTH))) { fastavg_column.append(i); }
        else if (column_name.endsWith(QString::number(SLOWAVG_LENGTH))) { slowavg_column.append(i); }
        else {
          // Resistance & Support
          if (SRstart < 0) {
            s = column_name.split(" ");
            if (s.size() > 1 && s[0] != "#" && (s[1] == "Resistance" || s[1] == "Support")) {
              SRstart = i;
            }
          }
        }
      }
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
      if (!index.isValid()) return QVariant();

      if (!disable_text_color && role == Qt::TextColorRole && index.column() >= CLOSE_COLUMN) {

        int column = index.column();
        QString column_name = headerData(column, Qt::Horizontal).toString();

        open = record(index.row()).value(OPEN_COLUMN).toDouble();
        high = record(index.row()).value(HIGH_COLUMN).toDouble();
        low = record(index.row()).value(LOW_COLUMN).toDouble();
        close = record(index.row()).value(CLOSE_COLUMN).toDouble();
        fastavg = record(index.row()).value( fastavg_column.first() ).toDouble();
        slowavg = record(index.row()).value( slowavg_column.first() ).toDouble();

        if (column == CLOSE_COLUMN) {
          if (open < close) return QColor(54,186,0);
          if (open > close) return QColor(Qt::red);
          return QSqlQueryModel::data(index, role);
        }
        else if (column_name.startsWith("MACD (R)")) {
          if (record(index.row()).value(MACD_COLUMN).toDouble() < 0) return QColor(Qt::blue);
        }
        else if (column_name.endsWith("-10") || column_name.endsWith("-50")) {
          fastavg_slowavg = record(index.row()).value(column).toDouble();
          if (fastavg_slowavg >= low && fastavg_slowavg <= high)
            return QColor(Qt::red);
        }
        else if (column_name.startsWith("Dist")) {
          if (column_name.startsWith("Dist(C-F)") && close - fastavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(C-S)") && close - slowavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(O-F)") && open - fastavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(O-S)") && open - slowavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(H-F)") && high - fastavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(H-S)") && high - slowavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(L-F)") && low - fastavg < 0) return QColor(Qt::blue);
          else if (column_name.startsWith("Dist(L-S)") && low - slowavg < 0) return QColor(Qt::blue);
        }
        else if (SRstart > -1 && column >= SRstart) {
          return QColor(Qt::red);

//          QStringList split_str = this->headerData(column,Qt::Horizontal).toString().split(" ");

//          if (split_str[1] == "Resistance") {
////            set_values(index.row(), column);
//  //          if ((high >= sr - threshold) && (open <= sr + threshold) && (close <= sr + threshold))
////            if (high >= sr_minus_threshold && open <= sr_plus_threshold && close > open)
//              return QColor(Qt::red);
//          }
//          else if (split_str[1] == "Support") {
////            set_values(index.row(), column);
//  //          if ((low <= sr + threshold) && (open >= sr - threshold) && (close >= sr - threshold))
////            if (low <= sr_plus_threshold && open >= sr_minus_threshold && close < open)
//              return QColor(Qt::red);
//          }
        }
      }

      return QSqlQueryModel::data(index, role);
    }

  private:
    mutable double open;
    mutable double high;
    mutable double low;
    mutable double close;
    mutable double fastavg;
    mutable double slowavg;
    mutable double sr;
    mutable double sr_minus_threshold;
    mutable double sr_plus_threshold;
    mutable double fastavg_slowavg;
    double threshold;
    int SRstart;
    bool disable_text_color;
    QVector<int> fastavg_column;
    QVector<int> slowavg_column;
};

#endif // SRMODEL_H
