#ifndef FILTEROPTIONFACTORY_H
#define FILTEROPTIONFACTORY_H

#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QList>
#include <QPushButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimeEdit>
#include <QWidget>

#include "BarDataAdapter.h"
#include "SQLiteHandler.h"
#define FILTEROPTION_MAX_ROW 50

class FilterOptionFactory : public QObject {
    Q_OBJECT

  public:
    FilterOptionFactory(QTableWidget *t) {
      if (t != NULL) {
        parent = t->parentWidget();
        table = t;
        table->setColumnCount(7);
        table->setColumnWidth(0, 75); // interval
        table->setColumnWidth(1, 140); // column name
        table->setColumnWidth(2, 140); // first operator
        table->setColumnWidth(3, 170); // second operator
        table->setColumnWidth(4, 70); // search value
        table->setColumnWidth(5, 30); // plus
        table->setColumnWidth(6, 30); // minus
        table->setRowHeight(0, 25);
        table->verticalHeader()->hide();
        table->horizontalHeader()->hide();
        table->setShowGrid(false);
        table->setStyleSheet("QTableWidget::item {padding: 1px}");
      }

      signal_interval = new QSignalMapper(this);
      signal_columnname = new QSignalMapper(this);
      signal_first_operator = new QSignalMapper(this);
      signal_second_operator = new QSignalMapper(this);
      signal_addrow = new QSignalMapper(this);
      signal_removerow = new QSignalMapper(this);

      connect(signal_interval, SIGNAL(mapped(QWidget*)), this, SLOT(interval_onIndexChanged(QWidget*)));
      connect(signal_columnname, SIGNAL(mapped(QWidget*)), this, SLOT(columnName_onIndexChanged(QWidget*)));
      connect(signal_first_operator, SIGNAL(mapped(QWidget*)), this, SLOT(first_operator_onIndexChanged(QWidget*)));
      connect(signal_second_operator, SIGNAL(mapped(QWidget*)), this, SLOT(second_operator_onIndexChanged(QWidget*)));
      connect(signal_addrow, SIGNAL(mapped(QWidget*)), this, SLOT(addRowButton_onClick(QWidget*)));
      connect(signal_removerow, SIGNAL(mapped(QWidget*)), this, SLOT(removeRowButton_onClick(QWidget*)));

      // initialize first line
      insertAt(0);

      // define integer column
      integer_column.append(SQLiteHandler::COLUMN_NAME_FGS);
      integer_column.append(SQLiteHandler::COLUMN_NAME_FLS);
      integer_column.append(SQLiteHandler::COLUMN_NAME_CGF);
      integer_column.append(SQLiteHandler::COLUMN_NAME_CLF);
      integer_column.append(SQLiteHandler::COLUMN_NAME_CGS);
      integer_column.append(SQLiteHandler::COLUMN_NAME_CLS);
      integer_column.append(SQLiteHandler::COLUMN_NAME_VOLUME);
      integer_column.append(SQLiteHandler::COLUMN_NAME_RSI_G70);
      integer_column.append(SQLiteHandler::COLUMN_NAME_RSI_L30);
      integer_column.append(SQLiteHandler::COLUMN_NAME_MACD_G0);
      integer_column.append(SQLiteHandler::COLUMN_NAME_MACD_L0);
      integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWK_G80);
      integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWK_L20);
      integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWD_G80);
      integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWD_L20);
      integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY);
      integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY);
      integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_DAILY);
      integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_60MIN);
      integer_column.append(SQLiteHandler::COLUMN_NAME_DAILY_RLINE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_DAILY_SLINE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE);
      integer_column.append(SQLiteHandler::COLUMN_NAME_FCROSS);
      integer_column.append(SQLiteHandler::COLUMN_NAME_SCROSS);

      // generic threshold
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DISTRES, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DISTSUP, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DISTRES_ATR, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DISTSUP_ATR, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DISTRES_RANK, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DISTSUP_RANK, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DAILY_RLINE, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_DAILY_SLINE, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE, 1);
      srthreshold_column.insert(SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE, 1);
    }

    ~FilterOptionFactory() {
      // delete signal mapper
      delete signal_interval;
      delete signal_columnname;
      delete signal_first_operator;
      delete signal_second_operator;
      delete signal_addrow;
      delete signal_removerow;

      // delete rows
      for (int i = count-1; i > -1; --i) {
        delete cbInterval[i];
        delete cbColumnName[i];
        delete cbFirstOperator[i];
        delete cbSecondOperator[i];
        delete txtSearchValue[i];
        delete btnAddRow[i];
        delete btnRemoveRow[i];
      }

      // optional
      table->clear();
      cbInterval.clear();
      cbColumnName.clear();
      cbFirstOperator.clear();
      cbSecondOperator.clear();
      txtSearchValue.clear();
      btnAddRow.clear();
      btnRemoveRow.clear();
    }

    QComboBox* createInterval(QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem("1min", WEIGHT_1MIN);
      cb->addItem("5min", WEIGHT_5MIN);
      cb->addItem("60min", WEIGHT_60MIN);
      cb->addItem("Daily", WEIGHT_DAILY);
      cb->addItem("Weekly", WEIGHT_WEEKLY);
      cb->addItem("Monthly", WEIGHT_MONTHLY);
      cb->setCurrentText("Daily");
      return cb;
    }

    QComboBox* createColumnName(QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem("Date", SQLiteHandler::COLUMN_NAME_DATE);
      cb->addItem("Price", USERDATA_PRICE);
      cb->addItem("Open", SQLiteHandler::COLUMN_NAME_OPEN);
      cb->addItem("High", SQLiteHandler::COLUMN_NAME_HIGH);
      cb->addItem("Low", SQLiteHandler::COLUMN_NAME_LOW);
      cb->addItem("Close", SQLiteHandler::COLUMN_NAME_CLOSE);
      cb->addItem("Volume", SQLiteHandler::COLUMN_NAME_VOLUME);
      cb->addItem("MACD", SQLiteHandler::COLUMN_NAME_MACD);
      cb->addItem("MACD (R+)", USERDATA_MACD_RANK_POS);
      cb->addItem("MACD (R-)", USERDATA_MACD_RANK_NEG);
      cb->addItem("MACD > 0", SQLiteHandler::COLUMN_NAME_MACD_G0);
      cb->addItem("MACD < 0", SQLiteHandler::COLUMN_NAME_MACD_L0);
      cb->addItem("MACD > 0 (R)", SQLiteHandler::COLUMN_NAME_MACD_G0_RANK);
      cb->addItem("MACD < 0 (R)", SQLiteHandler::COLUMN_NAME_MACD_L0_RANK);
      cb->addItem("MACDAvg", SQLiteHandler::COLUMN_NAME_MACDAVG);
      cb->addItem("MACDDiff", SQLiteHandler::COLUMN_NAME_MACDDIFF);
      cb->addItem("RSI", SQLiteHandler::COLUMN_NAME_RSI);
      cb->addItem("RSI > 70", SQLiteHandler::COLUMN_NAME_RSI_G70);
      cb->addItem("RSI < 30", SQLiteHandler::COLUMN_NAME_RSI_L30);      
      cb->addItem("SlowK", SQLiteHandler::COLUMN_NAME_SLOWK);
      cb->addItem("SlowK > 80", SQLiteHandler::COLUMN_NAME_SLOWK_G80);
      cb->addItem("SlowK < 20", SQLiteHandler::COLUMN_NAME_SLOWK_L20);
      cb->addItem("SlowD", SQLiteHandler::COLUMN_NAME_SLOWD);
      cb->addItem("SlowD > 80", SQLiteHandler::COLUMN_NAME_SLOWD_G80);
      cb->addItem("SlowD < 20", SQLiteHandler::COLUMN_NAME_SLOWD_L20);
      cb->addItem("ATR", SQLiteHandler::COLUMN_NAME_ATR);
      cb->addItem("ATR (R)", SQLiteHandler::COLUMN_NAME_ATR_RANK);
      cb->addItem("Day-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
      cb->addItem("Day-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      cb->addItem("F-Slope", SQLiteHandler::COLUMN_NAME_FASTAVG_SLOPE);
      cb->addItem("S-Slope", SQLiteHandler::COLUMN_NAME_SLOWAVG_SLOPE);
      cb->addItem("Dist(O-F)", SQLiteHandler::COLUMN_NAME_DISTOF);
      cb->addItem("Dist(O-F) (R+)", USERDATA_DISTOF_RANK_POS);
      cb->addItem("Dist(O-F) (R-)", USERDATA_DISTOF_RANK_NEG);
      cb->addItem("Dist(O-S)", SQLiteHandler::COLUMN_NAME_DISTOS);
      cb->addItem("Dist(O-S) (R+)", USERDATA_DISTOS_RANK_POS);
      cb->addItem("Dist(O-S) (R-)", USERDATA_DISTOS_RANK_NEG);
      cb->addItem("Dist(H-F)", SQLiteHandler::COLUMN_NAME_DISTHF);
      cb->addItem("Dist(H-F) (R+)", USERDATA_DISTHF_RANK_POS);
      cb->addItem("Dist(H-F) (R-)", USERDATA_DISTHF_RANK_NEG);
      cb->addItem("Dist(H-S)", SQLiteHandler::COLUMN_NAME_DISTHS);
      cb->addItem("Dist(H-S) (R+)", USERDATA_DISTHS_RANK_POS);
      cb->addItem("Dist(H-S) (R-)", USERDATA_DISTHS_RANK_NEG);
      cb->addItem("Dist(L-F)", SQLiteHandler::COLUMN_NAME_DISTLF);
      cb->addItem("Dist(L-F) (R+)", USERDATA_DISTLF_RANK_POS);
      cb->addItem("Dist(L-F) (R-)", USERDATA_DISTLF_RANK_NEG);
      cb->addItem("Dist(L-S)", SQLiteHandler::COLUMN_NAME_DISTLS);
      cb->addItem("Dist(L-S) (R+)", USERDATA_DISTLS_RANK_POS);
      cb->addItem("Dist(L-S) (R-)", USERDATA_DISTLS_RANK_NEG);
      cb->addItem("Dist(C-F)", SQLiteHandler::COLUMN_NAME_DISTCF);
      cb->addItem("Dist(C-F) (R+)", USERDATA_DISTCF_RANK_POS);
      cb->addItem("Dist(C-F) (R-)", USERDATA_DISTCF_RANK_NEG);
      cb->addItem("Dist(C-S)", SQLiteHandler::COLUMN_NAME_DISTCS);
      cb->addItem("Dist(C-S) (R+)", USERDATA_DISTCS_RANK_POS);
      cb->addItem("Dist(C-S) (R-)", USERDATA_DISTCS_RANK_NEG);
      cb->addItem("Dist(F-S)", SQLiteHandler::COLUMN_NAME_DISTFS);
      cb->addItem("Dist(F-S) (R)", SQLiteHandler::COLUMN_NAME_DISTFS_RANK);
      cb->addItem("Dist(FS-Cross)", SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS);
      cb->addItem("Dist(FS-Cross) (R)", SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK);
      cb->addItem("F > S", SQLiteHandler::COLUMN_NAME_FGS);
      cb->addItem("F < S", SQLiteHandler::COLUMN_NAME_FLS);
      cb->addItem("F > S (R)", SQLiteHandler::COLUMN_NAME_FGS_RANK);
      cb->addItem("F < S (R)", SQLiteHandler::COLUMN_NAME_FLS_RANK);
      cb->addItem("C > F", SQLiteHandler::COLUMN_NAME_CGF);
      cb->addItem("C < F", SQLiteHandler::COLUMN_NAME_CLF);
      cb->addItem("C > F (R)", SQLiteHandler::COLUMN_NAME_CGF_RANK);
      cb->addItem("C < F (R)", SQLiteHandler::COLUMN_NAME_CLF_RANK);
      cb->addItem("C > S", SQLiteHandler::COLUMN_NAME_CGS);
      cb->addItem("C < S", SQLiteHandler::COLUMN_NAME_CLS);
      cb->addItem("C > S (R)", SQLiteHandler::COLUMN_NAME_CGS_RANK);
      cb->addItem("C < S (R)", SQLiteHandler::COLUMN_NAME_CLS_RANK);
      cb->addItem("Uptail", SQLiteHandler::COLUMN_NAME_N_UPTAIL);
      cb->addItem("Uptail (R)", SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL_RANK);
      cb->addItem("Downtail", SQLiteHandler::COLUMN_NAME_N_DOWNTAIL);
      cb->addItem("Downtail (R)", SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL_RANK);
      cb->addItem("Body", SQLiteHandler::COLUMN_NAME_N_BODY);
      cb->addItem("Body (R)", SQLiteHandler::COLUMN_NAME_CANDLE_BODY_RANK);
      cb->addItem("TotalLength", SQLiteHandler::COLUMN_NAME_N_TOTALLENGTH);
      cb->addItem("TotalLength (R)", SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH_RANK);
      cb->addItem("DistResistance", SQLiteHandler::COLUMN_NAME_DISTRES_ATR); // XX
      cb->addItem("DistResistance (R)", SQLiteHandler::COLUMN_NAME_DISTRES_RANK); // XX
      cb->addItem("DistSupport", SQLiteHandler::COLUMN_NAME_DISTSUP_ATR); //XX
      cb->addItem("DistSupport (R)", SQLiteHandler::COLUMN_NAME_DISTSUP_RANK); // XX
      cb->addItem("F-Cross", SQLiteHandler::COLUMN_NAME_FCROSS);
      cb->addItem("S-Cross", SQLiteHandler::COLUMN_NAME_SCROSS);
      cb->addItem("ZoneDaily", SQLiteHandler::COLUMN_NAME_ZONE);
      cb->addItem("ZoneWeekly", SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY);
      cb->addItem("ZoneMonthly", SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY);
      cb->addItem("DailyResistance (#)", SQLiteHandler::COLUMN_NAME_DAILY_RLINE); // XX
      cb->addItem("DailySupport (#)", SQLiteHandler::COLUMN_NAME_DAILY_SLINE); // XX
      cb->addItem("WeeklyResistance (#)", SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE); // XX
      cb->addItem("WeeklySupport (#)", SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE); // XX
      cb->addItem("MonthlyResistance (#)", SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE); // XX
      cb->addItem("MonthlySupport (#)", SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE); // XX
      cb->addItem("BarColor", USERDATA_BARCOLOR);
      cb->addItem("Time", SQLiteHandler::COLUMN_NAME_TIME);
      cb->addItem("PrevBarColor", SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR);
      return cb;
    }

    QComboBox* createNumericOperator(QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem("greater than or equal", ">=");
      cb->addItem("less than or equal", "<=");
      cb->addItem("equal", "=");
      return cb;
    }

    QComboBox* createStringOperator(QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem("contains", "%_%");
      cb->addItem("begins with", "_%");
      cb->addItem("ends with", "%_");
      cb->addItem("equal", "=");
      return cb;
    }

    QComboBox* createFirstOperatorPrice(QWidget *parent, const int &base_weight) {
      QComboBox *cb = new QComboBox(parent);
      if (base_weight <= WEIGHT_DAILY)
        cb->addItem("Intersect Daily", OPERATOR_INTERSECT_DAILY);
      if (base_weight <= WEIGHT_WEEKLY)
        cb->addItem("Intersect Weekly", OPERATOR_INTERSECT_WEEKLY);
      cb->addItem("Intersect Monthly", OPERATOR_INTERSECT_MONTHLY);
      return cb;
    }

    QComboBox* createSecondOperatorPrice(QWidget *parent, const IntervalWeight &interval) {
      QComboBox *cb = new QComboBox(parent);
      if (interval == WEIGHT_DAILY) {
        cb->addItem("Daily Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
        cb->addItem("Daily Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        cb->addItem("Daily Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        cb->addItem("Daily Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
        cb->addItem("Daily Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        cb->addItem("Daily Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        cb->addItem("Day-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("Day-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      else if (interval == WEIGHT_WEEKLY) {
        cb->addItem("Weekly Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
        cb->addItem("Weekly Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        cb->addItem("Weekly Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        cb->addItem("Weekly Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
        cb->addItem("Weekly Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        cb->addItem("Weekly Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        cb->addItem("Week-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("Week-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      else {
        cb->addItem("Monthly Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
        cb->addItem("Monthly Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        cb->addItem("Monthly Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        cb->addItem("Monthly Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
        cb->addItem("Monthly Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        cb->addItem("Monthly Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        cb->addItem("Month-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("Month-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      return cb;
    }

    QComboBox* createBarColorOption(QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem("Green", BARCOLOR_GREEN);
      cb->addItem("Red", BARCOLOR_RED);
      cb->addItem("Doji", BARCOLOR_DOJI);
      return cb;
    }

    static QDoubleSpinBox* createDoubleSpinbox(QWidget *parent, const double &value, const int precision) {
      QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
      sb->setButtonSymbols(QDoubleSpinBox::NoButtons);
      sb->setRange(-9999999999.999, std::numeric_limits<double>::max());
      sb->setDecimals(precision);
      sb->setValue(value);
      return sb;
    }

    static QSpinBox* createIntegerSpinbox(QWidget *parent, const int &value) {
      QSpinBox *sb = new QSpinBox(parent);
      sb->setButtonSymbols(QSpinBox::NoButtons);
      sb->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
      sb->setValue(value);
      return sb;
    }

    static QDateTimeEdit* createDateTimeEdit(QWidget* parent = NULL) {
      QDateTimeEdit *dt = new QDateTimeEdit(parent);
      dt->setDisplayFormat("MM/dd/yyyy hh:mm");
      return dt;
    }

    static QDateEdit* createDateEdit(QWidget* parent = NULL) {
      QDateEdit *dt = new QDateEdit(parent);
      dt->setDisplayFormat("MM/dd/yyyy");
      return dt;
    }

    static QTimeEdit* createTimeEdit(QWidget *parent = NULL) {
      QTimeEdit *dt = new QTimeEdit(parent);
      dt->setDisplayFormat("hh:mm");
      return dt;
    }

    static QTextEdit* createTextEdit(QWidget *parent = NULL) {
      QTextEdit *w = new QTextEdit(parent);
      return w;
    }

    QDoubleSpinBox* clone_qdoublespinbox(const QDoubleSpinBox &src) {
      QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
      sb->setValue(src.value());
      return sb;
    }

    QComboBox* clone_qcombobox(const QComboBox &src) {
      QComboBox *cb = new QComboBox(parent);
      copy_qcombobox(src, cb);
      return cb;
    }

    void copy_qcombobox(const QComboBox &src, QComboBox *dest) {
      if (dest == NULL) dest = new QComboBox(parent);
      dest->clear();
      int current_index = src.currentIndex();
      int size = src.count();
      for (int i = 0; i < size; ++i) {
        dest->addItem(src.itemText(i), src.itemData(i));
      }
      dest->setCurrentIndex(current_index);
    }

    QWidget* clone_qwidget(QWidget *src) {
      QWidget *cloneWidget = NULL;

      if (qobject_cast<QDateEdit*>(src) != NULL) {
        QDateEdit *w = createDateEdit(parent);
        w->setDate(qobject_cast<QDateEdit*>(src)->date());
        cloneWidget = w;
        qDebug() << "date value" << w->date().toString();
      }
      else if (qobject_cast<QTimeEdit*>(src) != NULL) {
        QTimeEdit *w = createTimeEdit(parent);
        w->setTime(qobject_cast<QTimeEdit*>(src)->time());
        cloneWidget = w;
        qDebug() << "time value" << w->time().toString();
      }
      else if (qobject_cast<QDateTimeEdit*>(src) != NULL) {
        QDateTimeEdit *w = createDateTimeEdit(parent);
        w->setDateTime(qobject_cast<QDateTimeEdit*>(src)->dateTime());
        cloneWidget = w;
        qDebug() << "datetime value" << w->dateTime().toString();
      }
      else if (qobject_cast<QSpinBox*>(src) != NULL) {
        int value = qobject_cast<QSpinBox*>(src)->value();
        cloneWidget = createIntegerSpinbox(parent, value);
        qDebug() << "int value" << value;
      }
      else if (qobject_cast<QDoubleSpinBox*>(src) != NULL) {
        double value = qobject_cast<QDoubleSpinBox*>(src)->value();
        cloneWidget = createDoubleSpinbox(parent, value, 4);
        qDebug() << "double value" << value;
      }
      else if (qobject_cast<QComboBox*>(src) != NULL) {
        cloneWidget = clone_qcombobox(*(qobject_cast<QComboBox*>(src)));
        qDebug() << "combo value" << qobject_cast<QComboBox*>(cloneWidget)->currentText();
      }

      return cloneWidget;
    }

    void insertAt(const int &index) {
      if (count >= FILTEROPTION_MAX_ROW || index < 0 || index > count)
        return;

      IntervalWeight base_interval;
      if (index > 0) {
        base_interval = getIntersectWeight(index-1);
      } else {
        base_interval = WEIGHT_DAILY;
      }

      QComboBox *wInterval = createInterval(parent);
      QComboBox *wColumnName = createColumnName(parent);
      QComboBox *wFirstOperator = createNumericOperator(parent);
      QComboBox *wSecondOperator = NULL;
      QPushButton *wAddRow = new QPushButton("+", parent);
      QPushButton *wRemoveRow = new QPushButton("-", parent);
      QWidget *wSearchValue = NULL;

      // copy previous row value
      if (index > 0) {
        wInterval->setCurrentIndex(cbInterval[index-1]->currentIndex()); // interval
        copy_qcombobox(*(cbColumnName[index-1]), wColumnName); // column name
        copy_qcombobox(*(cbFirstOperator[index-1]), wFirstOperator); // first operator
//        wColumnName = clone_qcombobox(*(cbColumnName[index-1]), parent); // column name
//        wFirstOperator = clone_qcombobox(*(cbFirstOperator[index-1]), parent); // first operator

        if (cbSecondOperator[index-1] != NULL) {
          wSecondOperator = clone_qcombobox(*(cbSecondOperator[index-1])); // second operator
        }

        if (txtSearchValue[index-1] != NULL) {
//          wSearchValue = clone_qwidget(txtSearchValue[index-1], parent); // value
//          qDebug() << "copy search value. clone widget:" << (wSearchValue != NULL);

            if (qobject_cast<QDateEdit*>(txtSearchValue[index-1]) != NULL) {
              QDateEdit *w = createDateEdit(parent);
              w->setDate(qobject_cast<QDateEdit*>(txtSearchValue[index-1])->date());
              wSearchValue = w;
            }
            else if (qobject_cast<QTimeEdit*>(txtSearchValue[index-1]) != NULL) {
              QTimeEdit *w = createTimeEdit(parent);
              w->setTime(qobject_cast<QTimeEdit*>(txtSearchValue[index-1])->time());
              wSearchValue = w;
            }
            else if (qobject_cast<QDateTimeEdit*>(txtSearchValue[index-1]) != NULL) {
              QDateTimeEdit *w = createDateTimeEdit(parent);
              w->setDateTime(qobject_cast<QDateTimeEdit*>(txtSearchValue[index-1])->dateTime());
              wSearchValue = w;
            }
            else if (qobject_cast<QSpinBox*>(txtSearchValue[index-1]) != NULL) {
              int value = qobject_cast<QSpinBox*>(txtSearchValue[index-1])->value();
              wSearchValue = createIntegerSpinbox(parent, value);
            }
            else if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[index-1]) != NULL) {
              double value = qobject_cast<QDoubleSpinBox*>(txtSearchValue[index-1])->value();
              QString column_name = wColumnName->currentData().toString();
              int precision = 4;
              if (column_name == SQLiteHandler::COLUMN_NAME_MACD ||
                  column_name == SQLiteHandler::COLUMN_NAME_MACDAVG ||
                  column_name == SQLiteHandler::COLUMN_NAME_MACDDIFF) precision = 6;
              wSearchValue = createDoubleSpinbox(parent, value, precision);
            }
            else if (qobject_cast<QComboBox*>(txtSearchValue[index-1]) != NULL) {
              wSearchValue = clone_qcombobox(*(qobject_cast<QComboBox*>(txtSearchValue[index-1])));
            }
            else if (qobject_cast<QTextEdit*>(txtSearchValue[index-1]) != NULL) {
              QTextEdit *w = createTextEdit(parent);
              w->setText(qobject_cast<QTextEdit*>(txtSearchValue[index-1])->toPlainText());
              wSearchValue = w;
            } else {
              qDebug() << "WTF insert row";
            }

        }
      }
      // set default value
      else {
        wSearchValue = createDateEdit(parent);
      }

      if (table != 0 ) {
//        qDebug() << "begin table bind::index" << index;
        table->insertRow(index);
        table->setRowHeight(index, 25);
        table->setCellWidget(index, 0, wInterval);
        table->setCellWidget(index, 1, wColumnName);
        table->setCellWidget(index, 2, wFirstOperator);

        // for duration or number of react
        if (wSecondOperator != NULL && wSearchValue != NULL) {
          table->setCellWidget(index, 3, wSecondOperator);
          table->setCellWidget(index, 4, wSearchValue);
        }
        // normal case
        else {
          table->setSpan(index, 3, 1, 2); // set columnSpan = 2
          if (wSecondOperator != NULL) {
            table->setCellWidget(index, 3, wSecondOperator);
          } else {
            table->setCellWidget(index, 3, wSearchValue);
          }
        }

        table->setCellWidget(index, 5, wAddRow);
        table->setCellWidget(index, 6, wRemoveRow);
      }

      signal_interval->setMapping(wInterval, wInterval);
      signal_columnname->setMapping(wColumnName, wColumnName);
      signal_first_operator->setMapping(wFirstOperator, wFirstOperator);
      signal_addrow->setMapping(wAddRow, wAddRow);
      signal_removerow->setMapping(wRemoveRow, wRemoveRow);

      connect(wInterval, SIGNAL(currentIndexChanged(int)), signal_interval, SLOT(map()));
      connect(wColumnName, SIGNAL(currentIndexChanged(int)), signal_columnname, SLOT(map()));
      connect(wFirstOperator, SIGNAL(currentIndexChanged(int)), signal_first_operator, SLOT(map()));
      connect(wAddRow, SIGNAL(released()), signal_addrow, SLOT(map()));
      connect(wRemoveRow, SIGNAL(released()), signal_removerow, SLOT(map()));

      cbInterval.insert(index, wInterval);
      cbColumnName.insert(index, wColumnName);
      cbFirstOperator.insert(index, wFirstOperator);
      cbSecondOperator.insert(index, wSecondOperator);
      txtSearchValue.insert(index, wSearchValue);
      btnAddRow.insert(index, wAddRow);
      btnRemoveRow.insert(index, wRemoveRow);
      ++count;

      if (wSecondOperator != NULL) {
        signal_second_operator->setMapping(wSecondOperator, wSecondOperator);
        connect(wSecondOperator, SIGNAL(currentIndexChanged(int)), signal_second_operator, SLOT(map()));
      }
    }

    void removeAt(const int &index) {
      if (count < 0 && index < -1 && index > count)
        return;

      // remove signal mapping
      signal_interval->removeMappings(cbInterval[index]);
      signal_columnname->removeMappings(cbColumnName[index]);
      signal_first_operator->removeMappings(cbFirstOperator[index]);
      signal_addrow->removeMappings(btnAddRow[index]);
      signal_removerow->removeMappings(btnRemoveRow[index]);

      // handle second operator
      if (cbSecondOperator[index] != NULL) {
        signal_second_operator->removeMappings(cbSecondOperator[index]);
        delete cbSecondOperator[index];
      }

      // delete pointer
      delete cbInterval[index];
      delete cbColumnName[index];
      delete cbFirstOperator[index];
      delete txtSearchValue[index];
      delete btnAddRow[index];
      delete btnRemoveRow[index];

      // remove table widget row
      if (table != 0) table->removeRow(index);

      // remove from vector
      cbInterval.removeAt(index);
      cbColumnName.removeAt(index);
      cbFirstOperator.removeAt(index);
      cbSecondOperator.removeAt(index);
      txtSearchValue.removeAt(index);
      btnAddRow.removeAt(index);
      btnRemoveRow.removeAt(index);
      --count;
    }

    void reset() {
      int current_count = count;
      for (int i = 0; i < current_count; ++i) {
        removeAt(0);
      }
      insertAt(0);
    }

    int length() const {
      return count;
    }

    QString getInterval(const int &index) const {
      return cbInterval[index]->currentText();
    }

    int getIntervalWeight(const int &index) const {
      return cbInterval[index]->currentData().toInt();
    }

    IntervalWeight getIntervalWeight_v2(const int &index) const {
      int weight = cbInterval[index]->currentData().toInt();
      switch(weight) {
        case WEIGHT_MONTHLY: return WEIGHT_MONTHLY;
        case WEIGHT_WEEKLY: return WEIGHT_WEEKLY;
        case WEIGHT_DAILY: return WEIGHT_DAILY;
        case WEIGHT_60MIN: return WEIGHT_60MIN;
        case WEIGHT_5MIN: return WEIGHT_5MIN;
        case WEIGHT_1MIN: return WEIGHT_1MIN;
        default: return WEIGHT_INVALID;
      }
      return WEIGHT_INVALID;
    }

    QString getColumnName(const int &index) const {
      return cbColumnName[index]->currentData().toString();
    }

    QString getFirstOperator(const int &index) const {
      return (cbFirstOperator[index] != NULL)?cbFirstOperator[index]->currentData().toString():"";
    }

    QString getSecondOperator(const int &index) const {
      return (cbSecondOperator[index] != NULL)?cbSecondOperator[index]->currentData().toString():"";
    }

    QString getSearchValue(const int &index) const {
      QString currData = cbColumnName[index]->currentData().toString();
      if (currData == SQLiteHandler::COLUMN_NAME_DATE) {
        return qobject_cast<QDateEdit*>(txtSearchValue[index])->date().toString("yyyy-MM-dd");
      }
      else if (currData == SQLiteHandler::COLUMN_NAME_TIME) {
        return qobject_cast<QTimeEdit*>(txtSearchValue[index])->time().toString("HH:mm");
      }
      else if (currData == USERDATA_DATETIME) {
        return qobject_cast<QDateTimeEdit*>(txtSearchValue[index])->dateTime().toString("yyyy-MM-dd hh:mm");
      }
      else if (qobject_cast<QComboBox*>(txtSearchValue[index]) != NULL) {
        return qobject_cast<QComboBox*>(txtSearchValue[index])->currentText();
      }
      else if (qobject_cast<QSpinBox*>(txtSearchValue[index]) != NULL) {
        return QString::number(qobject_cast<QSpinBox*>(txtSearchValue[index])->value());
      }
      else if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[index]) != NULL) {
        return QString::number(qobject_cast<QDoubleSpinBox*>(txtSearchValue[index])->value());
      }
      else if (qobject_cast<QTextEdit*>(txtSearchValue[index]) != NULL) {
        return qobject_cast<QTextEdit*>(txtSearchValue[index])->toPlainText();
      }
      return "";
    }

    QString getSqlString(const int &index, const int &id_threshold, QString tableName = "") const {
      QString value = getSearchValue(index);

      // return default value when no value given
      if (value.isEmpty()) value = "0";

      QString column = cbColumnName[index]->currentData().toString();
      QString colDate = SQLiteHandler::COLUMN_NAME_DATE;
      QString colTime = SQLiteHandler::COLUMN_NAME_TIME;
      QString search_operator = cbFirstOperator[index]->currentData().toString();

      if (!tableName.isEmpty()) tableName += ".";

      if (column == colDate || column == colTime) {
        return tableName + column + search_operator + "'" + value + "'";
      }
      else if (column == USERDATA_DATETIME) {
        QStringList s = value.split(" ");
        QString dateValue = "'" + s[0] + "'";
        QString timeValue = "'" + s[1] + "'";
        // ==  (date_ = yyyymmdd and time_ >  hhmm)
        // >  ((date_ = yyyymmdd and time_ >  hhmm) OR date > yyyymmdd)"
        // >= ((date_ = yyyymmdd and time_ >= hhmm) OR date > yyyymmdd)"
        if (search_operator == "=") {
          return "(" + tableName + colTime + "=" + timeValue + " and " +
                       tableName + colDate + "=" + dateValue + ")";
        } else {
          return "(" + tableName + colDate + search_operator[0] + dateValue + " or (" +
                       tableName + colDate + "=" + dateValue + " and " +
                       tableName + colTime + search_operator + timeValue + "))";
        }
      }
      //
      // Close
      //
      else if (column == USERDATA_DISTCF_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTCF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTCF + ">0)";
      }
      else if (column == USERDATA_DISTCF_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTCF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTCF + "<0)";
      }
      else if (column == USERDATA_DISTCS_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTCS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTCS + ">0)";
      }
      else if (column == USERDATA_DISTCS_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTCS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTCS + "<0)";
      }
      //
      // Open
      //
      else if (column == USERDATA_DISTOF_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTOF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTOF + ">0)";
      }
      else if (column == USERDATA_DISTOF_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTOF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTOF + "<0)";
      }
      else if (column == USERDATA_DISTOS_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTOS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTOS + ">0)";
      }
      else if (column == USERDATA_DISTOS_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTOS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTOS + "<0)";
      }
      //
      // High
      //
      else if (column == USERDATA_DISTHF_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTHF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTHF + ">0)";
      }
      else if (column == USERDATA_DISTHF_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTHF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTHF + "<0)";
      }
      else if (column == USERDATA_DISTHS_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTHS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTHS + ">0)";
      }
      else if (column == USERDATA_DISTHS_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTHS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTHS + "<0)";
      }
      //
      // Low
      //
      else if (column == USERDATA_DISTLF_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTLF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTLF + ">0)";
      }
      else if (column == USERDATA_DISTLF_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTLF_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTLF + "<0)";
      }
      else if (column == USERDATA_DISTLS_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTLS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTLS + ">0)";
      }
      else if (column == USERDATA_DISTLS_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTLS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTLS + "<0)";
      }
      else if (column == USERDATA_MACD_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_MACD_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_MACD + ">0)";
      }
      else if (column == USERDATA_MACD_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_MACD_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_MACD + "<0)";
      }
      else if (srthreshold_column.contains(column)) {
        column += "_" + QString::number(id_threshold+1);
      }
      else if (column == SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR) {
        QStringList _value = value.toUpper().split("");
        _value.removeFirst();
        _value.removeLast();
        if (search_operator == "=") {
          return tableName + column + search_operator + "'" + _value.join(",") + "'";
        } else {
          return tableName + column + " LIKE '" + search_operator.replace("_", _value.join(",")) + "'";
        }
      }
      else if (column == USERDATA_BARCOLOR) {
        if (value == "Green")
          return "(" + tableName + SQLiteHandler::COLUMN_NAME_OPEN + "<" + tableName+SQLiteHandler::COLUMN_NAME_CLOSE + ")";

        if (value == "Red")
          return "(" + tableName + SQLiteHandler::COLUMN_NAME_OPEN + ">" + tableName+SQLiteHandler::COLUMN_NAME_CLOSE + ")";

        if (value == "Doji")
          return "(" + tableName + SQLiteHandler::COLUMN_NAME_OPEN + "=" + tableName+SQLiteHandler::COLUMN_NAME_CLOSE + ")";
      }

      return tableName + column + search_operator + value;
    }

    QString getSqlPriceIntersect(const int &index, const QString &main_alias, const QString &intersect_alias, const double &avg_threshold) const {
      QString column = cbColumnName[index]->currentData().toString();

      if (column == USERDATA_PRICE) {
        QString _main = main_alias;
        QString _intersect = intersect_alias;
        if (!_main.isEmpty()) _main += ".";
        if (!_intersect.isEmpty()) _intersect += ".";
        QString intersect_column = _intersect + qobject_cast<QComboBox*>(cbSecondOperator[index])->currentData().toString();
        QString main_pricelow = "(" + _main + SQLiteHandler::COLUMN_NAME_LOW + "-" + QString::number(avg_threshold) + ")";
        QString main_pricehigh = "(" + _main + SQLiteHandler::COLUMN_NAME_HIGH + "+" + QString::number(avg_threshold) + ")";
        return "(" + intersect_column + ">=" + main_pricelow + " and " +
                     intersect_column + "<=" + main_pricehigh + ")";
      }
      return "";
    }

    // backup
    /*QString getSqlResistanceIntersect_XX(const int &index, const QString &main_alias, const QString &intersect_alias, const double &threshold, const int &threshold_index) const {
      QString column = cbColumnName[index]->currentData().toString();

      if (column == USERDATA_PRICE) {
        QString resistance_table = SQLiteHandler::TABLE_NAME_RESISTANCE;
        QString r = QString::number(threshold);
        QString type=this->get_intersect_type(index);
        QString value = getSearchValue(index);
        QString intersect_resistance;
        QString id_threshold;
        QString resistance_count;
        QString resistance_duration;

        if (intersect_alias == NULL) {
          intersect_resistance = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          id_threshold = "RT.id_threshold=" + QString::number(threshold_index);
          if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
            resistance_count = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT;
          }
          else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
            resistance_duration = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION;
          }
        } else {
          intersect_resistance = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          id_threshold = intersect_alias.split(".")[0] + "." + resistance_table + ".id_threshold=" + QString::number(threshold_index);
          if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
            resistance_count = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT;
          }
          else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
            resistance_duration = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION;
          }
        }

        QString _main = main_alias;
        QString _intersect = intersect_alias;
        if (!_main.isEmpty()) _main += ".";
        if (!_intersect.isEmpty()) _intersect += ".";

        QString main_pricehigh = _main + SQLiteHandler::COLUMN_NAME_HIGH;
        QString main_priceopen = _main + SQLiteHandler::COLUMN_NAME_OPEN;
        QString main_priceclose = _main + SQLiteHandler::COLUMN_NAME_CLOSE;
        QString query_intersect_resistance = "(" +
          id_threshold + " and " +
          intersect_resistance + "-" + r + "<=" + main_pricehigh + " and " +
          intersect_resistance + "+" + r + ">=" + main_priceopen + " and "+
          intersect_resistance + "+" + r + ">=" + main_priceclose;

        if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
          query_intersect_resistance = query_intersect_resistance + " and " + resistance_count + ">=" + value + ")";
        }
        else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
          query_intersect_resistance = query_intersect_resistance + " and " + resistance_duration + ">=" + value + ")";
        }
        else {
          query_intersect_resistance = query_intersect_resistance + ")";
        }

        return query_intersect_resistance;
      }
      return "";
    }*/

    /*QString getSqlSupportIntersect_XX(const int &index, const QString &main_alias, const QString &intersect_alias, const double &threshold, const int &threshold_index) const {
      QString column = cbColumnName[index]->currentData().toString();

      if (column == USERDATA_PRICE) {
        QString support_table = SQLiteHandler::TABLE_NAME_SUPPORT;
        QString r = QString::number(threshold);
        QString type = this->get_intersect_type(index);
        QString value = getSearchValue(index);
        QString intersect_support;
        QString id_threshold;
        QString support_count;
        QString support_duration;

        if (intersect_alias == NULL) {
          intersect_support = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          id_threshold = "ST.id_threshold=" + QString::number(threshold_index);

          if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
            support_count = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT;
          }
          else if(type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
            support_duration = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION;
          }
        } else {
          intersect_support = intersect_alias.split(".")[0] + "." + support_table + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          id_threshold = intersect_alias.split(".")[0] + "." + support_table + ".id_threshold=" + QString::number(threshold_index);

          if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
            support_count = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT;
          }
          else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
            support_duration = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION;
          }
        }

        QString _main = main_alias;
        QString _intersect = intersect_alias;
        if (!_main.isEmpty()) _main += ".";
        if (!_intersect.isEmpty()) _intersect += ".";

        QString main_pricelow = _main + SQLiteHandler::COLUMN_NAME_LOW;
        QString main_priceopen = _main + SQLiteHandler::COLUMN_NAME_OPEN;
        QString main_priceclose = _main + SQLiteHandler::COLUMN_NAME_CLOSE;
        QString query_intersect_support = "(" +
          id_threshold + " and " +
          intersect_support + "+" + r + ">=" + main_pricelow + " and " +
          intersect_support + "-" + r + "<=" + main_priceopen + " and " +
          intersect_support + "-" + r + "<=" + main_priceclose;

        if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
          query_intersect_support = query_intersect_support + " and " + support_count + ">=" + value + ")";
        }
        else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
          query_intersect_support = query_intersect_support + " and " + support_duration + ">=" + value + ")";
        }
        else {
          query_intersect_support = query_intersect_support + ")";
        }

        return query_intersect_support;
      }
      return "";
    }*/

    // current
    QString getSqlResistanceIntersect(const int &index, const QString &main_alias, const QString &intersect_alias, const double &threshold, const int &threshold_index) const {
      QString column = cbColumnName[index]->currentData().toString();

      if (column != USERDATA_PRICE) return "";

      QString resistance_table = SQLiteHandler::TABLE_NAME_RESISTANCE;
      QString r = QString::number(threshold);
      QString type=this->get_intersect_type(index);
      QString value = getSearchValue(index);
      QString intersect_resistance;
      QString id_threshold;
      QString resistance_count;
      QString resistance_duration;

      if (intersect_alias.isEmpty()) {
        intersect_resistance = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
        id_threshold = "RT.id_threshold=" + QString::number(threshold_index);
        if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
          resistance_count = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
          resistance_duration = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION;
        }
      }
      else {
        intersect_resistance = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
        id_threshold = intersect_alias.split(".")[0] + "." + resistance_table + ".id_threshold=" + QString::number(threshold_index);
        if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
          resistance_count = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
          resistance_duration = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION;
        }
      }

      QString _main = main_alias;
      QString _intersect = intersect_alias;
      if (!_main.isEmpty()) _main += ".";
      if (!_intersect.isEmpty()) _intersect += ".";

      QString main_pricehigh = _main + SQLiteHandler::COLUMN_NAME_HIGH;
      QString main_priceopen = _main + SQLiteHandler::COLUMN_NAME_OPEN;
      QString main_priceclose = _main + SQLiteHandler::COLUMN_NAME_CLOSE;
      QString query_intersect_resistance = "(" +
          id_threshold + " and " +
          intersect_resistance + "-" + r + "<=" + main_pricehigh + " and (" +
          intersect_resistance + "+" + r + ">=" + main_priceopen + " or "+
          intersect_resistance + "+" + r + ">=" + main_priceclose+ " )";
      //        QString query_intersect_resistance = "(" +
      //            id_threshold + " and " +
      //            intersect_resistance + "-" + r + "<=" + main_pricehigh + " and " +
      //            intersect_resistance + "+" + r + ">=" + main_priceopen + " and "+
      //            main_priceopen + "<" + main_priceclose;

      if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
        query_intersect_resistance = query_intersect_resistance + " and " + resistance_count + ">=" + value + ")";
      }
      else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
        query_intersect_resistance = query_intersect_resistance + " and " + resistance_duration + ">=" + value + ")";
      }
      else {
        query_intersect_resistance = query_intersect_resistance + ")";
      }

      return query_intersect_resistance;
    }

    QString getSqlSupportIntersect(const int &index, const QString &main_alias, const QString &intersect_alias, const double &threshold, const int &threshold_index) const {
      QString column = cbColumnName[index]->currentData().toString();

      if (column != USERDATA_PRICE) return "";

      QString support_table = SQLiteHandler::TABLE_NAME_SUPPORT;
      QString r = QString::number(threshold);
      QString type = this->get_intersect_type(index);
      QString value = getSearchValue(index);
      QString intersect_support;
      QString id_threshold;
      QString support_count;
      QString support_duration;

      if (intersect_alias.isEmpty()) {
        intersect_support = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
        id_threshold = "ST.id_threshold=" + QString::number(threshold_index);

        if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
          support_count = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
          support_duration = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION;
        }
      }
      else {
        intersect_support = intersect_alias.split(".")[0] + "." + support_table + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
        id_threshold = intersect_alias.split(".")[0] + "." + support_table + ".id_threshold=" + QString::number(threshold_index);

        if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
          support_count = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
          support_duration = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION;
        }
      }

      QString _main = main_alias;
      QString _intersect = intersect_alias;
      if (!_main.isEmpty()) _main += ".";
      if (!_intersect.isEmpty()) _intersect += ".";

      QString main_pricelow = _main + SQLiteHandler::COLUMN_NAME_LOW;
      QString main_priceopen = _main + SQLiteHandler::COLUMN_NAME_OPEN;
      QString main_priceclose = _main + SQLiteHandler::COLUMN_NAME_CLOSE;
      QString query_intersect_support = "(" +
          id_threshold + " and " +
          intersect_support + "+" + r + ">=" + main_pricelow + " and (" +
          intersect_support + "-" + r + "<=" + main_priceopen + " or " +
          intersect_support + "-" + r + "<=" + main_priceclose + " )";
      //        QString query_intersect_support = "(" +
      //          id_threshold + " and " +
      //          intersect_support + "+" + r + ">=" + main_pricelow + " and " +
      //          intersect_support + "-" + r + "<=" + main_priceopen + " and " +
      //          main_priceopen + ">" + main_priceclose;

      if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
        query_intersect_support = query_intersect_support + " and " + support_count + ">=" + value + ")";
      }
      else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
        query_intersect_support = query_intersect_support + " and " + support_duration + ">=" + value + ")";
      }
      else {
        query_intersect_support = query_intersect_support + ")";
      }

      return query_intersect_support;
    }


    ///////////////////////////////////////////////

//    QString getSqlResistanceIntersect_V2(const int &index, const QString &main_alias, const IntervalWeight &projection_interval, const int &id_threshold) const {
//      QString _main = main_alias;
//      if (!main_alias.isEmpty()) _main += ".";
//      return _main + SQLiteHandler::get_interval_name(projection_interval) + "_RLine_" + QString::number(id_threshold+1) + ">=1";
//    }

//    QString getSqlSupportIntersect_V2(const int &index, const QString &main_alias, const IntervalWeight &projection_interval, const int &id_threshold) const {
//      QString _main = main_alias;
//      if (!main_alias.isEmpty()) _main += ".";
//      return _main + SQLiteHandler::get_interval_name(projection_interval) + "_SLine_" + QString::number(id_threshold+1) + ">=1";
//    }

    ///////////////////////////////////////////////


    QString get_smallest_interval() const {
      int skey = get_smallest_interval_weight();
      QString res;
      switch (skey) {
        case WEIGHT_MONTHLY: res = "Monthly"; break;
        case WEIGHT_WEEKLY: res = "Weekly"; break;
        case WEIGHT_DAILY: res = "Daily"; break;
        case WEIGHT_60MIN: res = "60min"; break;
        case WEIGHT_5MIN: res = "5min"; break;
        case WEIGHT_1MIN: res = "1min"; break;
        default: res = ""; break;
      }
      return res;
    }

    int get_smallest_interval_weight() const {
      int smallest = WEIGHT_LARGEST;
      int w;
      for (int i = 0; i < count; ++i) {
        w = getIntervalWeight(i);
        if (w < smallest) smallest = w;
      }
      return smallest;
    }

    IntervalWeight get_smallest_interval_weight_v2() const {
      int skey = get_smallest_interval_weight();
      IntervalWeight res;
      switch (skey) {
        case WEIGHT_MONTHLY: res = WEIGHT_MONTHLY; break;
        case WEIGHT_WEEKLY: res = WEIGHT_WEEKLY; break;
        case WEIGHT_DAILY: res = WEIGHT_DAILY; break;
        case WEIGHT_60MIN: res = WEIGHT_60MIN; break;
        case WEIGHT_5MIN: res = WEIGHT_5MIN; break;
        case WEIGHT_1MIN: res = WEIGHT_1MIN; break;
        default: res = WEIGHT_INVALID; break;
      }
      return res;
    }

    IntervalWeight getIntersectWeight(const int &index) const {
      if (!is_intersect_operator(index))
        return WEIGHT_INVALID;
      QString search_operator = cbFirstOperator[index]->currentData().toString();
      if (search_operator == OPERATOR_INTERSECT_DAILY)
        return WEIGHT_DAILY;
      if (search_operator == OPERATOR_INTERSECT_WEEKLY)
        return WEIGHT_WEEKLY;
      if (search_operator == OPERATOR_INTERSECT_MONTHLY)
        return WEIGHT_MONTHLY;
      return WEIGHT_INVALID;
    }

    // Return intersect column name:
    //  Resistance, Support, Duration, ReactCount, FastAvg, SlowAvg
    QString get_intersect_type(const int &index) const {
      if (cbSecondOperator[index] != NULL && is_intersect_operator(index)) {
        return qobject_cast<QComboBox*>(cbSecondOperator[index])->currentData().toString();
      }
      return "";
    }

    QString getConditionString(const int &index) {
      if (index < 0 || index > count) return "";

      QString currData = cbColumnName[index]->currentData().toString();
      QString condition_string =
        cbInterval[index]->currentText() + "," +
        cbColumnName[index]->currentText() + "," +
        cbFirstOperator[index]->currentText() + ",";

      if (cbSecondOperator[index] != NULL) {
        condition_string += cbSecondOperator[index]->currentText();
        if (txtSearchValue[index] != NULL)
          condition_string += ",";
      }

      if (currData == SQLiteHandler::COLUMN_NAME_DATE) {
        condition_string +=  qobject_cast<QDateEdit*>(txtSearchValue[index])->date().toString("MM/dd/yyyy");
      }
      else if (currData == SQLiteHandler::COLUMN_NAME_TIME) {
        condition_string +=  qobject_cast<QTimeEdit*>(txtSearchValue[index])->time().toString("HH:mm");
      }
      else if (currData == USERDATA_DATETIME) {
        condition_string +=  qobject_cast<QDateTimeEdit*>(txtSearchValue[index])->dateTime().toString("MM/dd/yyyy HH:mm");
      }
      else {
        condition_string += getSearchValue(index);
      }

      return condition_string;
    }

    void setConditionString(const int &index, const QStringList &condition) {
      // the array of string should contain column text from CONDITION
      // for example:
      // [0] = Daily
      // [1] = Date
      // [2] = greater than or equal
      // [3] = 01/01/2015

//      qDebug() << condition[0] << condition[1] << condition[2] << condition[3];

      if (condition.size() < 4) return;

      cbInterval[index]->setCurrentText(condition[0]);
      cbColumnName[index]->setCurrentText(condition[1]);
      cbFirstOperator[index]->setCurrentText(condition[2]);

      if (is_intersect_operator(index)) {
        cbSecondOperator[index]->setCurrentText(condition[3]);
      }

      if (txtSearchValue[index] != NULL) {
        int IDX = is_intersect_operator(index)? 4 : 3;

        if (condition[1] == "Date") {
          qobject_cast<QDateEdit*>(txtSearchValue[index])->setDate(QDate::fromString(condition[IDX], "MM/dd/yyyy"));
        }
        else if (condition[1] == "Time") {
          qobject_cast<QTimeEdit*>(txtSearchValue[index])->setTime(QTime::fromString(condition[IDX],"HH:mm"));
        }
        else if (condition[1] == "Datetime") {
          qobject_cast<QDateTimeEdit*>(txtSearchValue[index])->setDateTime(QDateTime::fromString(condition[IDX], "MM/dd/yyyy HH:mm"));
        }
        else if (qobject_cast<QComboBox*>(txtSearchValue[index]) != NULL) {
          qobject_cast<QComboBox*>(txtSearchValue[index])->setCurrentText(condition[IDX]);
        }
        else if (qobject_cast<QSpinBox*>(txtSearchValue[index]) != NULL) {
          qobject_cast<QSpinBox*>(txtSearchValue[index])->setValue(condition[IDX].toInt());
        }
        else if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[index]) != NULL) {
          qobject_cast<QDoubleSpinBox*>(txtSearchValue[index])->setValue(condition[IDX].toDouble());
        }
      }
    }

    void set_developer_mode(const bool &b) {
      enable_developer_mode = b;
    }

    bool is_intersect_operator(const int &index) const {
      QString search_operator = cbFirstOperator[index]->currentData().toString();
      return (search_operator == OPERATOR_INTERSECT_DAILY ||
              search_operator == OPERATOR_INTERSECT_WEEKLY ||
              search_operator == OPERATOR_INTERSECT_MONTHLY);
    }

    bool is_single_interval() const {
      QMap<int,QString> temp;
      for (int i = 0; i < count; ++i) {
        temp.insert(getIntervalWeight(i),getInterval(i));
        if (temp.count() > 1) return false;
      }
      return true;
    }

  private slots:
    void interval_onIndexChanged(QWidget *w) {
      const int idx_rowitem = cbInterval.indexOf(static_cast<QComboBox*>(w));
      const int curr_interval_weight = getIntervalWeight(idx_rowitem);
      QString currData = cbColumnName[idx_rowitem]->currentData().toString();
      QString current_text = cbColumnName[idx_rowitem]->currentText();
      QString item_data = cbColumnName[idx_rowitem]->itemData(0).toString();
      QString cdata;
      QDate date_;
      QDateTime datetime_;
      IntervalWeight interval_w = getIntervalWeight_v2(idx_rowitem);
      int columnName_count = cbColumnName[idx_rowitem]->count();

      cbColumnName[idx_rowitem]->blockSignals(true);

      //
      // Rename FastAvg and SlowAvg by interval
      //
      for (int i = 0; i < columnName_count; ++i) {
        cdata = cbColumnName[idx_rowitem]->itemData(i).toString();
        if (cdata == SQLiteHandler::COLUMN_NAME_FASTAVG) {
          cbColumnName[idx_rowitem]->setItemText(i, BarDataAdapter::avg_name_by_interval(interval_w, FASTAVG_LENGTH));
        } else if (cdata == SQLiteHandler::COLUMN_NAME_SLOWAVG) {
          cbColumnName[idx_rowitem]->setItemText(i, BarDataAdapter::avg_name_by_interval(interval_w, SLOWAVG_LENGTH));
          break;
        }
      }

      //
      // DateRange
      //
      if (curr_interval_weight < WEIGHT_DAILY) {
        bool is_exists = false;

        for (int i = 0; i < columnName_count; ++i) {
          if (cbColumnName[idx_rowitem]->itemText(i) == "DateRange") {
            is_exists = true;
            break;
          }
        }

        if (!is_exists) {
          cbColumnName[idx_rowitem]->addItem("DateRange", SQLiteHandler::COLUMN_NAME_DATERANGE);
        }
      } else {
        cbColumnName[idx_rowitem]->blockSignals(false);

        for (int i = 0; i < columnName_count; ++i) {
          if (cbColumnName[idx_rowitem]->itemText(i) == "DateRange") {
            cbColumnName[idx_rowitem]->removeItem(i);
            break;
          }
        }
      }

      //
      // Zone & Resistance/Support (#)
      //
      setZoneColumnName(cbColumnName[idx_rowitem], interval_w);
      setSRLineColumn(cbColumnName[idx_rowitem], interval_w);

      cbColumnName[idx_rowitem]->blockSignals(false);

      for (int i = 0; i < cbColumnName[idx_rowitem]->count(); ++i) {
        if (cbColumnName[idx_rowitem]->itemText(i) == current_text) {
          cbColumnName[idx_rowitem]->setCurrentIndex(i);
          break;
        }
      }

      // (2) handle intersect operator, show only intersect that greater than current resolution
      if (currData == USERDATA_PRICE) {
        QVariant curr_data = cbFirstOperator[idx_rowitem]->currentData();
        int base_weight = getIntervalWeight(idx_rowitem);

        // (a) first operator
        setFirstOperatorPrice(cbFirstOperator[idx_rowitem], base_weight);

        // preserve previous value
        int idx = cbFirstOperator[idx_rowitem]->findData(curr_data);
        if (idx > -1) {
          cbFirstOperator[idx_rowitem]->blockSignals(true);
          cbFirstOperator[idx_rowitem]->setCurrentIndex(idx);
          cbFirstOperator[idx_rowitem]->blockSignals(false);
        }

        // (b) second operator
        setSecondOperatorPrice(cbSecondOperator[idx_rowitem], getIntersectWeight(idx_rowitem),
                               cbSecondOperator[idx_rowitem]->currentIndex());
      }

      // (3) swap Date or DateTime
      if (curr_interval_weight < WEIGHT_DAILY && item_data == SQLiteHandler::COLUMN_NAME_DATE) {

        if (cbColumnName[idx_rowitem]->currentIndex() == 0 &&
            qobject_cast<QDateEdit*>(txtSearchValue[idx_rowitem]) != NULL) {
          date_ = qobject_cast<QDateEdit*>(txtSearchValue[idx_rowitem])->date();
        }

        cbColumnName[idx_rowitem]->blockSignals(true);
        cbColumnName[idx_rowitem]->setItemText(0, "Datetime");
        cbColumnName[idx_rowitem]->setItemData(0, USERDATA_DATETIME);
        cbColumnName[idx_rowitem]->blockSignals(false);

        if (!date_.isNull()) {
//          delete txtSearchValue[idx_rowitem];
          QDateTimeEdit *dt = createDateTimeEdit(parent);
          dt->setDate(date_);
          txtSearchValue[idx_rowitem] = dt;
          table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
        }
      }
      else if (curr_interval_weight >= WEIGHT_DAILY && item_data == USERDATA_DATETIME) {

        if (cbColumnName[idx_rowitem]->currentIndex() == 0 &&
            qobject_cast<QDateTimeEdit*>(txtSearchValue[idx_rowitem]) != NULL) {
          datetime_ = qobject_cast<QDateTimeEdit*>(txtSearchValue[idx_rowitem])->dateTime();
        }

        cbColumnName[idx_rowitem]->blockSignals(true);
        cbColumnName[idx_rowitem]->setItemText(0, "Date");
        cbColumnName[idx_rowitem]->setItemData(0, SQLiteHandler::COLUMN_NAME_DATE);
        cbColumnName[idx_rowitem]->blockSignals(false);

        if (!datetime_.isNull()) {
//          delete txtSearchValue[idx_rowitem];
          QDateEdit *dt = createDateEdit(parent);
          dt->setDate(datetime_.date());
          txtSearchValue[idx_rowitem] = dt;
          table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
        }
      }
    }

    void columnName_onIndexChanged(QWidget *w) {
      QString current_data = qobject_cast<QComboBox*>(w)->currentData().toString();
      int idx_rowitem = cbColumnName.indexOf(qobject_cast<QComboBox*>(w));

      // (1) handle search operator
      if (current_data == USERDATA_PRICE) {
        // create first operator (intersect)
        setFirstOperatorPrice(cbFirstOperator[idx_rowitem], getIntervalWeight(idx_rowitem));

//        delete txtSearchValue[idx_rowitem];
        txtSearchValue[idx_rowitem] = NULL;

        // create second operator
        cbSecondOperator[idx_rowitem] = createSecondOperatorPrice(parent, getIntersectWeight(idx_rowitem));
        signal_second_operator->setMapping(cbSecondOperator[idx_rowitem], cbSecondOperator[idx_rowitem]);
        connect(cbSecondOperator[idx_rowitem], SIGNAL(currentIndexChanged(int)), signal_second_operator, SLOT(map()));

        table->setSpan(idx_rowitem, 3, 1, 2);
        table->setCellWidget(idx_rowitem, 3, cbSecondOperator[idx_rowitem]);
      }
      else {
        // remove (previously) second operator
        if (cbSecondOperator[idx_rowitem] != NULL) {
          signal_second_operator->removeMappings(cbSecondOperator[idx_rowitem]);
          table->removeCellWidget(idx_rowitem, 3);
          cbSecondOperator[idx_rowitem] = NULL;

          if (txtSearchValue[idx_rowitem] != NULL) {
            QWidget *temp = clone_qwidget(txtSearchValue[idx_rowitem]); //XXX
            table->removeCellWidget(idx_rowitem, 4);
            table->setSpan(idx_rowitem, 3, 1, 2); // set columnSpan = 2
            txtSearchValue[idx_rowitem] = temp;
          }
        }

        if (current_data == SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR) {
          // (b) create string operator
          setStringOperator(cbFirstOperator[idx_rowitem], cbFirstOperator[idx_rowitem]->currentIndex());

        } else {
          // (b) create numeric operator
          setNumericOperator(cbFirstOperator[idx_rowitem], cbFirstOperator[idx_rowitem]->currentIndex());

          // leave only "equal to" condition in search operator
          if (current_data == USERDATA_BARCOLOR || current_data.startsWith("Zone")) {
            cbFirstOperator[idx_rowitem]->blockSignals(true);
            cbFirstOperator[idx_rowitem]->removeItem(0);
            cbFirstOperator[idx_rowitem]->removeItem(0);
            cbFirstOperator[idx_rowitem]->blockSignals(false);
          }
        }
      }

      // (2) handle search value
      if (current_data == USERDATA_DATETIME) {
        txtSearchValue[idx_rowitem] = createDateTimeEdit(parent);
        table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
      }
      else if (current_data == SQLiteHandler::COLUMN_NAME_DATE) {
        txtSearchValue[idx_rowitem] = createDateEdit(parent);
        table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
      }
      else if (current_data == SQLiteHandler::COLUMN_NAME_TIME) {
        txtSearchValue[idx_rowitem] = createTimeEdit(parent);
        table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
      }
      else if (current_data == SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR) {
        txtSearchValue[idx_rowitem] = createTextEdit(parent);
        table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
      }
      else if (integer_column.contains(current_data)) {
        int value = 0;

        // try to cast previous value
        if (txtSearchValue[idx_rowitem] != NULL) {
          if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[idx_rowitem]) != NULL) {
            value = static_cast<int>(qobject_cast<QDoubleSpinBox*> (txtSearchValue[idx_rowitem])->value());
          }
          else if (qobject_cast<QSpinBox*>(txtSearchValue[idx_rowitem]) != NULL) {
            value = qobject_cast<QSpinBox*>(txtSearchValue[idx_rowitem])->value();
          }
        }

        txtSearchValue[idx_rowitem] = createIntegerSpinbox(parent, value);
        table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
      }
      else if (current_data == USERDATA_BARCOLOR) {
        txtSearchValue[idx_rowitem] = createBarColorOption(parent);
        table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
      }
      else if (current_data != USERDATA_PRICE) {
        table->setSpan(idx_rowitem, 3, 1, 2); // set columnSpan = 1
        int precision = 4;
        if (current_data == SQLiteHandler::COLUMN_NAME_MACD || current_data == SQLiteHandler::COLUMN_NAME_MACDAVG ||
            current_data == SQLiteHandler::COLUMN_NAME_MACDDIFF) precision = 6;

        // init search value
        if (txtSearchValue[idx_rowitem] == NULL) {
          txtSearchValue[idx_rowitem] = createDoubleSpinbox(parent, 0, precision);
          table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
        }
        else {
          // try to cast previous value
          double value = 0;
          if (qobject_cast<QSpinBox*>(txtSearchValue[idx_rowitem]) != NULL) {
            value = static_cast<double>(qobject_cast<QSpinBox*>(txtSearchValue[idx_rowitem])->value());
          }
          else if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[idx_rowitem]) != NULL) {
            value = qobject_cast<QDoubleSpinBox*>(txtSearchValue[idx_rowitem])->value();
          }

//          delete txtSearchValue[idx_rowitem];
          txtSearchValue[idx_rowitem] = createDoubleSpinbox(parent, value, precision);
          table->setCellWidget(idx_rowitem, 3, txtSearchValue[idx_rowitem]);
        }
      }

      repaint_table();
    }

    void first_operator_onIndexChanged(QWidget *w) {
      const int idx_rowitem = cbFirstOperator.indexOf(static_cast<QComboBox*>(w));

      // (1) intersect operator: rename second operator
      if (is_intersect_operator(idx_rowitem)) {
        IntervalWeight weight = getIntersectWeight(idx_rowitem);
        setSecondOperatorPrice(cbSecondOperator[idx_rowitem], weight, cbSecondOperator[idx_rowitem]->currentIndex());
      }
    }

    void second_operator_onIndexChanged(QWidget *w) {
      const int idx_rowitem = cbSecondOperator.indexOf(static_cast<QComboBox*>(w));
      QString current_data = cbSecondOperator[idx_rowitem]->currentData().toString();

      // (1) for Duration or React Count: set up search value cell
      if (current_data == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION ||
          current_data == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT ||
          current_data == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION ||
          current_data == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
        if (txtSearchValue[idx_rowitem] == NULL) {
          txtSearchValue[idx_rowitem] = createIntegerSpinbox(parent, 0);
          table->setSpan(idx_rowitem, 3, 1, 1); // set columnSpan = 1
          table->setCellWidget(idx_rowitem, 4, txtSearchValue[idx_rowitem]);
        }
      }
      // (2) else remove search value cell
      else if (txtSearchValue[idx_rowitem] != NULL) {
        table->removeCellWidget(idx_rowitem, 4);
        table->setSpan(idx_rowitem, 3, 1, 2); // set columnSpan = 2
//        delete txtSearchValue[idx_rowitem];
        txtSearchValue[idx_rowitem] = NULL;
      }

      repaint_table();
    }

    void addRowButton_onClick(QWidget *w) {
      int idx = btnAddRow.indexOf(static_cast<QPushButton*>(w));
      insertAt(idx < 0? 0 : idx+1);
    }

    void removeRowButton_onClick(QWidget *w) {
      if (count > 1)
        removeAt(btnRemoveRow.indexOf(static_cast<QPushButton*>(w)));
    }

  private:
    QTableWidget *table = NULL;
    QWidget *parent = NULL;
    QStringList integer_column;
    QHash<QString, int> srthreshold_column;

    // Enum intersect operator
    const QString OPERATOR_INTERSECT_1MIN = "INTERSECT_" + QString::number(WEIGHT_1MIN);
    const QString OPERATOR_INTERSECT_5MIN = "INTERSECT_" + QString::number(WEIGHT_5MIN);
    const QString OPERATOR_INTERSECT_60MIN = "INTERSECT_" + QString::number(WEIGHT_60MIN);
    const QString OPERATOR_INTERSECT_DAILY = "INTERSECT_" + QString::number(WEIGHT_DAILY);
    const QString OPERATOR_INTERSECT_WEEKLY = "INTERSECT_" + QString::number(WEIGHT_WEEKLY);
    const QString OPERATOR_INTERSECT_MONTHLY = "INTERSECT_" + QString::number(WEIGHT_MONTHLY);

    // Enum special defined column name
    const QString USERDATA_BARCOLOR = "_BARCOLOR_";
    const QString USERDATA_DATETIME = "_DATETIME_";
    const QString USERDATA_PRICE = "_PRICE_";
    const QString USERDATA_MACD_RANK_POS = SQLiteHandler::COLUMN_NAME_MACD_RANK + "_POS";
    const QString USERDATA_MACD_RANK_NEG = SQLiteHandler::COLUMN_NAME_MACD_RANK + "_NEG";
    const QString USERDATA_DISTCF_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTCF_RANK + "_POS";
    const QString USERDATA_DISTCF_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTCF_RANK + "_NEG";
    const QString USERDATA_DISTCS_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTCS_RANK + "_POS";
    const QString USERDATA_DISTCS_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTCS_RANK + "_NEG";
    const QString USERDATA_DISTOF_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTOF_RANK + "_POS";
    const QString USERDATA_DISTOF_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTOF_RANK + "_NEG";
    const QString USERDATA_DISTOS_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTOS_RANK + "_POS";
    const QString USERDATA_DISTOS_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTOS_RANK + "_NEG";
    const QString USERDATA_DISTHF_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTHF_RANK + "_POS";
    const QString USERDATA_DISTHF_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTHF_RANK + "_NEG";
    const QString USERDATA_DISTHS_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTHS_RANK + "_POS";
    const QString USERDATA_DISTHS_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTHS_RANK + "_NEG";
    const QString USERDATA_DISTLF_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTLF_RANK + "_POS";
    const QString USERDATA_DISTLF_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTLF_RANK + "_NEG";
    const QString USERDATA_DISTLS_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTLS_RANK + "_POS";
    const QString USERDATA_DISTLS_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTLS_RANK + "_NEG";

    // Widget
    QList<QComboBox *> cbInterval;
    QList<QComboBox *> cbColumnName;
    QList<QComboBox *> cbFirstOperator; // 1st operator
    QList<QComboBox *> cbSecondOperator; // 2nd operator
    QList<QWidget *> txtSearchValue;
    QList<QPushButton *> btnAddRow;
    QList<QPushButton *> btnRemoveRow;

    // Signal Mapper
    QSignalMapper *signal_interval = NULL;
    QSignalMapper *signal_columnname = NULL;
    QSignalMapper *signal_first_operator = NULL;
    QSignalMapper *signal_second_operator = NULL;
    QSignalMapper *signal_addrow = NULL;
    QSignalMapper *signal_removerow = NULL;

    bool enable_developer_mode = false;
    int count = 0;
    int rsi_lb, rsi_ub;
    int macd_lb, macd_ub;
    int slowk_lb, slowk_ub;
    int slowd_lb, slowd_ub;

    void setNumericOperator(QComboBox *cb, const int &index) {
      if (cb != NULL) {
        cb->blockSignals(true);
        cb->clear();
        cb->addItem("greater than or equal", ">=");
        cb->addItem("less than or equal", "<=");
        cb->addItem("equal", "=");
        cb->setCurrentIndex(index);
        cb->blockSignals(false);
      }
    }

    void setStringOperator(QComboBox *cb, const int &index) {
      if (cb != NULL) {
        cb->blockSignals(true);
        cb->clear();
        cb->addItem("contains", "%_%");
        cb->addItem("begins with", "_%");
        cb->addItem("ends with", "%_");
        cb->addItem("equal", "=");
        cb->setCurrentIndex(index);
        cb->blockSignals(false);
      }
    }

    void setFirstOperatorPrice(QComboBox *cb, const int &base_weight) {
      if (cb != NULL) {
        cb->blockSignals(true);
        cb->clear();
        if (base_weight <= WEIGHT_DAILY)
          cb->addItem("Intersect Daily", OPERATOR_INTERSECT_DAILY);
        if (base_weight <= WEIGHT_WEEKLY)
          cb->addItem("Intersect Weekly", OPERATOR_INTERSECT_WEEKLY);
        cb->addItem("Intersect Monthly", OPERATOR_INTERSECT_MONTHLY);
        cb->blockSignals(false);
      }
    }

    void setSecondOperatorPrice(QComboBox *cb, const IntervalWeight &interval, const int &index) {
      if (cb != NULL) {
        cb->blockSignals(true);
        cb->clear();
        if (interval == WEIGHT_DAILY) {
          cb->addItem("Daily Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
          cb->addItem("Daily Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
          cb->addItem("Daily Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
          cb->addItem("Daily Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
          cb->addItem("Daily Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
          cb->addItem("Daily Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
          cb->addItem("Day-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
          cb->addItem("Day-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
        }
        else if (interval == WEIGHT_WEEKLY) {
          cb->addItem("Weekly Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
          cb->addItem("Weekly Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
          cb->addItem("Weekly Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
          cb->addItem("Weekly Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
          cb->addItem("Weekly Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
          cb->addItem("Weekly Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
          cb->addItem("Week-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
          cb->addItem("Week-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
        }
        else {
          cb->addItem("Monthly Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
          cb->addItem("Monthly Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
          cb->addItem("Monthly Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
          cb->addItem("Monthly Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
          cb->addItem("Monthly Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
          cb->addItem("Monthly Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
          cb->addItem("Month-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
          cb->addItem("Month-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
        }
        cb->setCurrentIndex(index);
        cb->blockSignals(false);
      }
    }

    void setZoneColumnName(QComboBox *cb, const IntervalWeight &interval) {
      cb->blockSignals(true);
      IntervalWeight w = interval;
      int idx = -1;

      for (int i = 0; i < cb->count(); ) {
        if (cb->itemText(i).startsWith("Zone")) {
          if (idx < 0) idx = i;
          cb->removeItem(i);
          continue;
        }
        ++i;
      }

      while (w != WEIGHT_INVALID) {
        if (w == interval) {
          cb->insertItem(idx, "Zone" + SQLiteHandler::get_interval_name(w), SQLiteHandler::COLUMN_NAME_ZONE);
        } else {
          switch (w) {
            case WEIGHT_MONTHLY: cb->insertItem(idx, "ZoneMonthly", SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY); break;
            case WEIGHT_WEEKLY: cb->insertItem(idx, "ZoneWeekly", SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY); break;
            case WEIGHT_DAILY: cb->insertItem(idx, "ZoneDaily", SQLiteHandler::COLUMN_NAME_ZONE_DAILY); break;
            case WEIGHT_60MIN: cb->insertItem(idx, "Zone60min", SQLiteHandler::COLUMN_NAME_ZONE_60MIN); break;
          }
        }
        ++idx;
        w = SQLiteHandler::get_parent_interval(w);
      }

      cb->blockSignals(false);
    }

    void setSRLineColumn(QComboBox *cb, const IntervalWeight &interval) {
      cb->blockSignals(true);
      IntervalWeight w = interval;
      QString text;
      int idx = -1;

      for (int i = 0; i < cb->count();) {
        text = cb->itemText(i);
        if (text.endsWith("Resistance (#)") || text.endsWith("Support (#)")) {
          if (idx < 0) idx = i;
          cb->removeItem(i);
          continue;
        }
        ++i;
      }

      while (w != WEIGHT_INVALID) {
        switch (w) {
          case WEIGHT_MONTHLY:
            cb->insertItem(idx++, "MonthlyResistance (#)", SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE);
            cb->insertItem(idx++, "MonthlySupport (#)", SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE);
            break;

          case WEIGHT_WEEKLY:
            cb->insertItem(idx++, "WeeklyResistance (#)", SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE);
            cb->insertItem(idx++, "WeeklySupport (#)", SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE);
            break;

          case WEIGHT_DAILY:
            cb->insertItem(idx++, "DailyResistance (#)", SQLiteHandler::COLUMN_NAME_DAILY_RLINE);
            cb->insertItem(idx++, "DailySupport (#)", SQLiteHandler::COLUMN_NAME_DAILY_SLINE);
            break;

          default: break;
        }
        w = SQLiteHandler::get_parent_interval(w);
      }

      cb->blockSignals(false);
    }

    void repaint_table() {
      if (table != NULL) {
        table->setStyleSheet("QTableWidget::item { padding: 1px; }");
        table->repaint();
      }
    }
};

#endif // FILTEROPTIONFACTORY_H
