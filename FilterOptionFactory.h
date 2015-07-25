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
#include "XmlConfigHandler.h"
#define FILTEROPTION_MAX_ROW  50
#define MAX_THRESHOLD_COUNT   5

class FilterOptionFactory : public QObject {
    Q_OBJECT

  public:
    FilterOptionFactory(QTableWidget *t) {
      if ( t != NULL ) {
        parent = t->parentWidget();
        table = t;
        table->setColumnCount(10);
        table->setColumnWidth(0, 70); // interval
        table->setColumnWidth(1, 140); // column name
        table->setColumnWidth(2, 140); // first operator
        table->setColumnWidth(3, 170); // second operator
        table->setColumnWidth(4, 70); // search value
        table->setColumnWidth(5, 50); // condition (row)
        table->setColumnWidth(6, 30); // plus
        table->setColumnWidth(7, 30); // minus
        table->setColumnWidth(8, 30); // open block
        table->setColumnWidth(9, 30); // close block
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
      signal_beginblock = new QSignalMapper(this);
      signal_endblock = new QSignalMapper(this);

      connect(signal_interval, SIGNAL(mapped(QWidget*)), this, SLOT(interval_onIndexChanged(QWidget*)));
      connect(signal_columnname, SIGNAL(mapped(QWidget*)), this, SLOT(columnName_onIndexChanged(QWidget*)));
      connect(signal_first_operator, SIGNAL(mapped(QWidget*)), this, SLOT(first_operator_onIndexChanged(QWidget*)));
      connect(signal_second_operator, SIGNAL(mapped(QWidget*)), this, SLOT(second_operator_onIndexChanged(QWidget*)));
      connect(signal_addrow, SIGNAL(mapped(QWidget*)), this, SLOT(addRowButton_onClick(QWidget*)));
      connect(signal_removerow, SIGNAL(mapped(QWidget*)), this, SLOT(removeRowButton_onClick(QWidget*)));
      connect(signal_beginblock, SIGNAL(mapped(QWidget*)), this, SLOT(beginBlockButton_onClick(QWidget*)));
      connect(signal_endblock, SIGNAL(mapped(QWidget*)), this, SLOT(endBlockButton_onClick(QWidget*)));

      // read config
      XmlConfigHandler *config = XmlConfigHandler::get_instance();
      t_macd = config->get_macd_threshold();
      t_rsi = config->get_rsi_threshold();
      t_slowk = config->get_slowk_threshold();
      t_slowd = config->get_slowd_threshold();
      t_cgf = config->get_cgf_threshold();
      t_clf = config->get_clf_threshold();
      t_cgs = config->get_cgs_threshold();
      t_cls = config->get_cls_threshold();
      idx_macd = 1;
      idx_rsi = 1;
      idx_slowk = 1;
      idx_slowd = 1;

      // initialize first line
      insertAt(0);

      // listing integer column
      {
        integer_column.append(SQLiteHandler::COLUMN_NAME_FGS);
        integer_column.append(SQLiteHandler::COLUMN_NAME_FLS);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CGF);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLF);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CGS);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLS);
        integer_column.append(SQLiteHandler::COLUMN_NAME_VOLUME);

        QString _id;
        for (int i = 0; i < MAX_THRESHOLD_COUNT; ++i) {
          _id = "_" + QString::number( i );
          integer_column.append(SQLiteHandler::COLUMN_NAME_MACD_VALUE1 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_MACD_VALUE2 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_RSI_VALUE1 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_RSI_VALUE2 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWK_VALUE1 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWK_VALUE2 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWD_VALUE1 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_SLOWD_VALUE2 + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_CGF + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_CLF + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_CGS + _id);
          integer_column.append(SQLiteHandler::COLUMN_NAME_CLS + _id);
        }

        integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_DAILY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_ZONE_60MIN);
        integer_column.append(SQLiteHandler::COLUMN_NAME_OPEN_ZONE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_OPEN_ZONE_MONTHLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_OPEN_ZONE_WEEKLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_OPEN_ZONE_DAILY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_OPEN_ZONE_60MIN);
        integer_column.append(SQLiteHandler::COLUMN_NAME_HIGH_ZONE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_HIGH_ZONE_MONTHLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_HIGH_ZONE_WEEKLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_HIGH_ZONE_DAILY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_HIGH_ZONE_60MIN);
        integer_column.append(SQLiteHandler::COLUMN_NAME_LOW_ZONE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_LOW_ZONE_MONTHLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_LOW_ZONE_WEEKLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_LOW_ZONE_DAILY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_LOW_ZONE_60MIN);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLOSE_ZONE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_MONTHLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_WEEKLY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_DAILY);
        integer_column.append(SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_60MIN);
        integer_column.append(SQLiteHandler::COLUMN_NAME_DAILY_RLINE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_DAILY_SLINE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE);
        integer_column.append(SQLiteHandler::COLUMN_NAME_FCROSS);
        integer_column.append(SQLiteHandler::COLUMN_NAME_SCROSS);
      }

      // generic threshold
      {
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

      // intersect extra column
      {
        sr_intersect_integer_column.append(SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        sr_intersect_integer_column.append(SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        sr_intersect_integer_column.append(SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        sr_intersect_integer_column.append(SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        sr_intersect_double_column.append(USERDATA_RESISTANCE_DIST_POINT);
        sr_intersect_double_column.append(USERDATA_RESISTANCE_DIST_ATR);
        sr_intersect_double_column.append(USERDATA_SUPPORT_DIST_POINT);
        sr_intersect_double_column.append(USERDATA_SUPPORT_DIST_ATR);
      }      
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
      QString _id;
      QString column_v1;
      QString column_v2;
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

      // MACD
      _id = "_" + QString::number(idx_macd);
      column_v1 = "MACD " + t_macd[idx_macd-1].operator1 + " " + QString::number(t_macd[idx_macd-1].value1);
      column_v2 = "MACD " + t_macd[idx_macd-1].operator2 + " " + QString::number(t_macd[idx_macd-1].value2);
      cb->addItem(column_v1, SQLiteHandler::COLUMN_NAME_MACD_VALUE1 + _id);
      cb->addItem(column_v1 + " (R)", SQLiteHandler::COLUMN_NAME_MACD_RANK1 + _id);
      cb->addItem(column_v2, SQLiteHandler::COLUMN_NAME_MACD_VALUE2 + _id);
      cb->addItem(column_v2 + " (R)", SQLiteHandler::COLUMN_NAME_MACD_RANK2 + _id);
//      cb->addItem("MACD > 0", SQLiteHandler::COLUMN_NAME_MACD_G0);
//      cb->addItem("MACD < 0", SQLiteHandler::COLUMN_NAME_MACD_L0);
//      cb->addItem("MACD > 0 (R)", SQLiteHandler::COLUMN_NAME_MACD_G0_RANK);
//      cb->addItem("MACD < 0 (R)", SQLiteHandler::COLUMN_NAME_MACD_L0_RANK);

      cb->addItem("MACDAvg", SQLiteHandler::COLUMN_NAME_MACDAVG);
      cb->addItem("MACDDiff", SQLiteHandler::COLUMN_NAME_MACDDIFF);

      // RSI
      _id = "_" + QString::number(idx_rsi);
      column_v1 = "RSI " + t_rsi[idx_rsi-1].operator1 + " " + QString::number(t_rsi[idx_rsi-1].value1);
      column_v2 = "RSI " + t_rsi[idx_rsi-1].operator2 + " " + QString::number(t_rsi[idx_rsi-1].value2);
      cb->addItem("RSI", SQLiteHandler::COLUMN_NAME_RSI);
      cb->addItem(column_v1, SQLiteHandler::COLUMN_NAME_RSI_VALUE1 + _id);
      cb->addItem(column_v1 + " (R)", SQLiteHandler::COLUMN_NAME_RSI_RANK1 + _id);
      cb->addItem(column_v2, SQLiteHandler::COLUMN_NAME_RSI_VALUE2 + _id);
      cb->addItem(column_v2 + " (R)", SQLiteHandler::COLUMN_NAME_RSI_RANK2 + _id);
//      cb->addItem("RSI > 70", SQLiteHandler::COLUMN_NAME_RSI_G70);
//      cb->addItem("RSI < 30", SQLiteHandler::COLUMN_NAME_RSI_L30);

      // SlowK
      _id = "_" + QString::number(idx_slowk);
      column_v1 = "SlowK " + t_slowk[idx_slowk-1].operator1 + " " + QString::number(t_slowk[idx_slowk-1].value1);
      column_v2 = "SlowK " + t_slowk[idx_slowk-1].operator2 + " " + QString::number(t_slowk[idx_slowk-1].value2);
      cb->addItem("SlowK", SQLiteHandler::COLUMN_NAME_SLOWK);
      cb->addItem(column_v1, SQLiteHandler::COLUMN_NAME_SLOWK_VALUE1 + _id);
      cb->addItem(column_v1 + " (R)", SQLiteHandler::COLUMN_NAME_SLOWK_RANK1 + _id);
      cb->addItem(column_v2, SQLiteHandler::COLUMN_NAME_SLOWK_VALUE2 + _id);
      cb->addItem(column_v2 + " (R)", SQLiteHandler::COLUMN_NAME_SLOWK_RANK2 + _id);
//      cb->addItem("SlowK > 80", SQLiteHandler::COLUMN_NAME_SLOWK_G80);
//      cb->addItem("SlowK < 20", SQLiteHandler::COLUMN_NAME_SLOWK_L20);

      // SlowD
      _id = "_" + QString::number(idx_slowd);
      column_v1 = "SlowD " + t_slowd[idx_slowd-1].operator1 + " " + QString::number(t_slowd[idx_slowd-1].value1);
      column_v2 = "SlowD " + t_slowd[idx_slowd-1].operator2 + " " + QString::number(t_slowd[idx_slowd-1].value2);
      cb->addItem("SlowD", SQLiteHandler::COLUMN_NAME_SLOWD);
      cb->addItem(column_v1, SQLiteHandler::COLUMN_NAME_SLOWD_VALUE1 + _id);
      cb->addItem(column_v1 + " (R)", SQLiteHandler::COLUMN_NAME_SLOWD_RANK1 + _id);
      cb->addItem(column_v2, SQLiteHandler::COLUMN_NAME_SLOWD_VALUE2 + _id);
      cb->addItem(column_v2 + " (R)", SQLiteHandler::COLUMN_NAME_SLOWD_RANK2 + _id);
//      cb->addItem("SlowD > 80", SQLiteHandler::COLUMN_NAME_SLOWD_G80);
//      cb->addItem("SlowD < 20", SQLiteHandler::COLUMN_NAME_SLOWD_L20);

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
      cb->addItem("Dist(FS-Cross) (ATR)", SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_ATR);
      cb->addItem("Dist(FS-Cross) (R+)", USERDATA_DISTCC_FSCROSS_RANK_POS);
      cb->addItem("Dist(FS-Cross) (R-)", USERDATA_DISTCC_FSCROSS_RANK_NEG);
      cb->addItem("F < S", SQLiteHandler::COLUMN_NAME_FLS);
      cb->addItem("F < S (R)", SQLiteHandler::COLUMN_NAME_FLS_RANK);
      cb->addItem("F > S", SQLiteHandler::COLUMN_NAME_FGS);
      cb->addItem("F > S (R)", SQLiteHandler::COLUMN_NAME_FGS_RANK);
      cb->addItem("C < F", SQLiteHandler::COLUMN_NAME_CLF);
      cb->addItem("C < F (R)", SQLiteHandler::COLUMN_NAME_CLF_RANK);
      cb->addItem("C > F", SQLiteHandler::COLUMN_NAME_CGF);
      cb->addItem("C > F (R)", SQLiteHandler::COLUMN_NAME_CGF_RANK);
      cb->addItem("C < S", SQLiteHandler::COLUMN_NAME_CLS);
      cb->addItem("C < S (R)", SQLiteHandler::COLUMN_NAME_CLS_RANK);
      cb->addItem("C > S", SQLiteHandler::COLUMN_NAME_CGS);
      cb->addItem("C > S (R)", SQLiteHandler::COLUMN_NAME_CGS_RANK);
      cb->addItem("Uptail", SQLiteHandler::COLUMN_NAME_N_UPTAIL);
      cb->addItem("Uptail (R)", SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL_RANK);
      cb->addItem("Downtail", SQLiteHandler::COLUMN_NAME_N_DOWNTAIL);
      cb->addItem("Downtail (R)", SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL_RANK);
      cb->addItem("Body", SQLiteHandler::COLUMN_NAME_N_BODY);
      cb->addItem("Body (R)", SQLiteHandler::COLUMN_NAME_CANDLE_BODY_RANK);
      cb->addItem("TotalLength", SQLiteHandler::COLUMN_NAME_N_TOTALLENGTH);
      cb->addItem("TotalLength (R)", SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH_RANK);
      cb->addItem("DistResistance", SQLiteHandler::COLUMN_NAME_DISTRES);
      cb->addItem("DistResistance (ATR)", SQLiteHandler::COLUMN_NAME_DISTRES_ATR);
      cb->addItem("DistResistance (R)", SQLiteHandler::COLUMN_NAME_DISTRES_RANK);
      cb->addItem("DistSupport", SQLiteHandler::COLUMN_NAME_DISTSUP);
      cb->addItem("DistSupport (ATR)", SQLiteHandler::COLUMN_NAME_DISTSUP_ATR);
      cb->addItem("DistSupport (R)", SQLiteHandler::COLUMN_NAME_DISTSUP_RANK);
      cb->addItem("F-Cross", SQLiteHandler::COLUMN_NAME_FCROSS);
      cb->addItem("S-Cross", SQLiteHandler::COLUMN_NAME_SCROSS);
      cb->addItem("O-ZoneDaily", SQLiteHandler::COLUMN_NAME_OPEN_ZONE);
      cb->addItem("H-ZoneDaily", SQLiteHandler::COLUMN_NAME_HIGH_ZONE);
      cb->addItem("L-ZoneDaily", SQLiteHandler::COLUMN_NAME_LOW_ZONE);
      cb->addItem("C-ZoneDaily", SQLiteHandler::COLUMN_NAME_ZONE);
      cb->addItem("O-ZoneWeekly", SQLiteHandler::COLUMN_NAME_OPEN_ZONE_WEEKLY);
      cb->addItem("H-ZoneWeekly", SQLiteHandler::COLUMN_NAME_HIGH_ZONE_WEEKLY);
      cb->addItem("L-ZoneWeekly", SQLiteHandler::COLUMN_NAME_LOW_ZONE_WEEKLY);
      cb->addItem("C-ZoneWeekly", SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY);
      cb->addItem("O-ZoneMonthly", SQLiteHandler::COLUMN_NAME_OPEN_ZONE_MONTHLY);
      cb->addItem("H-ZoneMonthly", SQLiteHandler::COLUMN_NAME_HIGH_ZONE_MONTHLY);
      cb->addItem("L-ZoneMonthly", SQLiteHandler::COLUMN_NAME_LOW_ZONE_MONTHLY);
      cb->addItem("C-ZoneMonthly", SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY);
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

      if (base_weight <= WEIGHT_5MIN) {
        cb->addItem("Intersect 5min", OPERATOR_INTERSECT_5MIN);
      }

      if (base_weight <= WEIGHT_60MIN) {
        cb->addItem("Intersect 60min", OPERATOR_INTERSECT_60MIN);
      }

      if (base_weight <= WEIGHT_DAILY) {
        cb->addItem("Intersect Daily", OPERATOR_INTERSECT_DAILY);
      }

      if (base_weight <= WEIGHT_WEEKLY) {
        cb->addItem("Intersect Weekly", OPERATOR_INTERSECT_WEEKLY);
      }

      cb->addItem("Intersect Monthly", OPERATOR_INTERSECT_MONTHLY);
      return cb;
    }

    QComboBox* createSecondOperatorPrice(QWidget *parent, const IntervalWeight &interval) {
      QComboBox *cb = new QComboBox(parent);

      if (interval == WEIGHT_60MIN) {
        cb->addItem("60-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("60-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      else if (interval == WEIGHT_5MIN) {
        cb->addItem("5-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("5-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      else if (interval == WEIGHT_DAILY) {
        cb->addItem("Daily Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
        cb->addItem("Daily Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        cb->addItem("Daily Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        cb->addItem("Daily Resistance DistPoint", USERDATA_RESISTANCE_DIST_POINT); // XX
        cb->addItem("Daily Resistance DistATR", USERDATA_RESISTANCE_DIST_ATR); // XX
        cb->addItem("Daily Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
        cb->addItem("Daily Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        cb->addItem("Daily Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        cb->addItem("Daily Support DistPoint", USERDATA_SUPPORT_DIST_POINT); // XX
        cb->addItem("Daily Support DistATR", USERDATA_SUPPORT_DIST_ATR); // XX
        cb->addItem("Day-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("Day-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      else if (interval == WEIGHT_WEEKLY) {
        cb->addItem("Weekly Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
        cb->addItem("Weekly Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        cb->addItem("Weekly Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        cb->addItem("Weekly Resistance DistPoint", USERDATA_RESISTANCE_DIST_POINT); // XX
        cb->addItem("Weekly Resistance DistATR", USERDATA_RESISTANCE_DIST_ATR); // XX
        cb->addItem("Weekly Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
        cb->addItem("Weekly Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        cb->addItem("Weekly Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        cb->addItem("Weekly Support DistPoint", USERDATA_SUPPORT_DIST_POINT); // XX
        cb->addItem("Weekly Support DistATR", USERDATA_SUPPORT_DIST_ATR); // XX
        cb->addItem("Week-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("Week-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }
      else if (interval == WEIGHT_MONTHLY) {
        cb->addItem("Monthly Resistance", SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE);
        cb->addItem("Monthly Resistance # React", SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT);
        cb->addItem("Monthly Resistance Duration", SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION);
        cb->addItem("Monthly Resistance DistPoint", USERDATA_RESISTANCE_DIST_POINT); // XX
        cb->addItem("Monthly Resistance DistATR", USERDATA_RESISTANCE_DIST_ATR); // XX
        cb->addItem("Monthly Support", SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE);
        cb->addItem("Monthly Support # React", SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT);
        cb->addItem("Monthly Support Duration", SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION);
        cb->addItem("Monthly Support DistPoint", USERDATA_SUPPORT_DIST_POINT); // XX
        cb->addItem("Monthly Support DistATR", USERDATA_SUPPORT_DIST_ATR); // XX
        cb->addItem("Month-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem("Month-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
      }

      return cb;
    }

    QComboBox* createSecondOperatorAvg(QWidget *parent, const IntervalWeight &interval) {
      QComboBox *cb = new QComboBox(parent);
      IntervalWeight w = interval;

      cb->addItem("Itself", "");

      while (w != WEIGHT_INVALID) {
        cb->addItem(BarDataAdapter::avg_name_by_interval(w, FASTAVG_LENGTH), SQLiteHandler::COLUMN_NAME_FASTAVG);
        cb->addItem(BarDataAdapter::avg_name_by_interval(w, SLOWAVG_LENGTH), SQLiteHandler::COLUMN_NAME_SLOWAVG);
        w = SQLiteHandler::get_parent_interval(w);
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

    QComboBox* createConditionRow(QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem("AND", " AND ");
      cb->addItem("OR", " OR ");
      return cb;
    }

    QComboBox* createConditionBlock(const QString &text, const QString &data, QWidget *parent = NULL) {
      QComboBox *cb = new QComboBox(parent);
      cb->addItem(text, data);
      cb->setEnabled(false);
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
//        qDebug() << "date value" << w->date().toString();
      }
      else if (qobject_cast<QTimeEdit*>(src) != NULL) {
        QTimeEdit *w = createTimeEdit(parent);
        w->setTime(qobject_cast<QTimeEdit*>(src)->time());
        cloneWidget = w;
//        qDebug() << "time value" << w->time().toString();
      }
      else if (qobject_cast<QDateTimeEdit*>(src) != NULL) {
        QDateTimeEdit *w = createDateTimeEdit(parent);
        w->setDateTime(qobject_cast<QDateTimeEdit*>(src)->dateTime());
        cloneWidget = w;
//        qDebug() << "datetime value" << w->dateTime().toString();
      }
      else if (qobject_cast<QSpinBox*>(src) != NULL) {
        int value = qobject_cast<QSpinBox*>(src)->value();
        cloneWidget = createIntegerSpinbox(parent, value);
//        qDebug() << "int value" << value;
      }
      else if (qobject_cast<QDoubleSpinBox*>(src) != NULL) {
        double value = qobject_cast<QDoubleSpinBox*>(src)->value();
        cloneWidget = createDoubleSpinbox(parent, value, 4);
//        qDebug() << "double value" << value;
      }
      else if (qobject_cast<QComboBox*>(src) != NULL) {
        cloneWidget = clone_qcombobox(*(qobject_cast<QComboBox*>(src)));
//        qDebug() << "combo value" << qobject_cast<QComboBox*>(cloneWidget)->currentText();
      }

      return cloneWidget;
    }

    void insertAt(const int &index) {
      if (count >= FILTEROPTION_MAX_ROW || index < 0 || index > count)
        return;

      bool isPrevRowBlock = (index > 0 && cbBlock[index-1] != NULL);

      QComboBox *wInterval = createInterval(parent);
      QComboBox *wColumnName = createColumnName(parent);
      QComboBox *wFirstOperator = createNumericOperator(parent);
      QComboBox *wSecondOperator = NULL;
      QPushButton *wAddRow = new QPushButton("+", parent);
      QPushButton *wRemoveRow = new QPushButton("-", parent);
      QComboBox *wConditionRow = createConditionRow(parent);
      QComboBox *wConditionBlock = NULL;
      QPushButton *wBeginBlock = new QPushButton("(", parent);
      QPushButton *wEndBlock = new QPushButton(")", parent);
      QWidget *wSearchValue = NULL;

      // copy previous row value
      if ( index > 0 && !isPrevRowBlock ) {
        wInterval->setCurrentIndex(cbInterval[index-1]->currentIndex()); // interval
        copy_qcombobox(*(cbColumnName[index-1]), wColumnName); // column name
        copy_qcombobox(*(cbFirstOperator[index-1]), wFirstOperator); // first operator
        copy_qcombobox(*(cbConditionRow[index-1]), wConditionRow); // condition (row)
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

      if ( table != NULL ) {
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

        table->setCellWidget(index, 5, wConditionRow);
        table->setCellWidget(index, 6, wBeginBlock);
        table->setCellWidget(index, 7, wEndBlock);
        table->setCellWidget(index, 8, wAddRow);
        table->setCellWidget(index, 9, wRemoveRow);
      }

      signal_interval->setMapping(wInterval, wInterval);
      signal_columnname->setMapping(wColumnName, wColumnName);
      signal_first_operator->setMapping(wFirstOperator, wFirstOperator);
      signal_addrow->setMapping(wAddRow, wAddRow);
      signal_removerow->setMapping(wRemoveRow, wRemoveRow);
      signal_beginblock->setMapping(wBeginBlock, wBeginBlock);
      signal_endblock->setMapping(wEndBlock, wEndBlock);

      connect(wInterval, SIGNAL(currentIndexChanged(int)), signal_interval, SLOT(map()));
      connect(wColumnName, SIGNAL(currentIndexChanged(int)), signal_columnname, SLOT(map()));
      connect(wFirstOperator, SIGNAL(currentIndexChanged(int)), signal_first_operator, SLOT(map()));
      connect(wAddRow, SIGNAL(released()), signal_addrow, SLOT(map()));
      connect(wRemoveRow, SIGNAL(released()), signal_removerow, SLOT(map()));
      connect(wBeginBlock, SIGNAL(released()), signal_beginblock, SLOT(map()));
      connect(wEndBlock, SIGNAL(released()), signal_endblock, SLOT(map()));

      cbInterval.insert(index, wInterval);
      cbColumnName.insert(index, wColumnName);
      cbFirstOperator.insert(index, wFirstOperator);
      cbSecondOperator.insert(index, wSecondOperator);
      txtSearchValue.insert(index, wSearchValue);
      btnAddRow.insert(index, wAddRow);
      btnRemoveRow.insert(index, wRemoveRow);
      cbBlock.insert(index, NULL);
      cbConditionRow.insert(index, wConditionRow);
      cbConditionBlock.insert(index, wConditionBlock);
      btnBeginBlock.insert(index, wBeginBlock);
      btnEndBlock.insert(index, wEndBlock);
      ++count;

      if ( wSecondOperator != NULL ) {
        signal_second_operator->setMapping(wSecondOperator, wSecondOperator);
        connect(wSecondOperator, SIGNAL(currentIndexChanged(int)), signal_second_operator, SLOT(map()));
      }
    }

    void insertBeginBlock(const int &index) {
      if (count >= FILTEROPTION_MAX_ROW || index < 0 || index > count)
        return;

      QComboBox *wBlock = createConditionBlock("BEGIN BLOCK", "(", parent);
      QPushButton *wAddRow = new QPushButton("+", parent);
      QPushButton *wRemoveRow = new QPushButton("-", parent);

      if ( table != NULL ) {
        table->insertRow(index);
        table->setRowHeight(index, 25);
        table->setCellWidget(index, 0, wBlock);
        table->setSpan(index, 0, 1, 8);
        table->setCellWidget(index, 8, wAddRow);
        table->setCellWidget(index, 9, wRemoveRow);
      }

      signal_addrow->setMapping(wAddRow, wAddRow);
      signal_removerow->setMapping(wRemoveRow, wRemoveRow);

      connect(wAddRow, SIGNAL(released()), signal_addrow, SLOT(map()));
      connect(wRemoveRow, SIGNAL(released()), signal_removerow, SLOT(map()));

      cbInterval.insert(index, NULL);
      cbColumnName.insert(index, NULL);
      cbFirstOperator.insert(index, NULL);
      cbSecondOperator.insert(index, NULL);
      txtSearchValue.insert(index, NULL);
      btnAddRow.insert(index, wAddRow);
      btnRemoveRow.insert(index, wRemoveRow);
      cbBlock.insert(index, wBlock);
      cbConditionRow.insert(index, NULL);
      cbConditionBlock.insert(index, NULL);
      btnBeginBlock.insert(index, NULL);
      btnEndBlock.insert(index, NULL);
      ++count;
    }

    void insertEndBlock(const int &index) {
      if (count >= FILTEROPTION_MAX_ROW || index < 0 || index > count)
        return;

      QComboBox *wBlock = createConditionBlock("END BLOCK", ")", parent);
      QComboBox *wConditionBlock = createConditionRow(parent);
      QPushButton *wAddRow = new QPushButton("+", parent);
      QPushButton *wRemoveRow = new QPushButton("-", parent);

      if ( table != NULL ) {
        table->insertRow(index);
        table->setRowHeight(index, 25);
        table->setCellWidget(index, 0, wBlock);
        table->setSpan(index, 0, 1, 6);
        table->setCellWidget(index, 6, wConditionBlock);
        table->setSpan(index, 6, 1, 2);
        table->setCellWidget(index, 8, wAddRow);
        table->setCellWidget(index, 9, wRemoveRow);
      }

      signal_addrow->setMapping(wAddRow, wAddRow);
      signal_removerow->setMapping(wRemoveRow, wRemoveRow);

      connect(wAddRow, SIGNAL(released()), signal_addrow, SLOT(map()));
      connect(wRemoveRow, SIGNAL(released()), signal_removerow, SLOT(map()));

      cbInterval.insert(index, NULL);
      cbColumnName.insert(index, NULL);
      cbFirstOperator.insert(index, NULL);
      cbSecondOperator.insert(index, NULL);
      txtSearchValue.insert(index, NULL);
      btnAddRow.insert(index, wAddRow);
      btnRemoveRow.insert(index, wRemoveRow);
      cbBlock.insert(index, wBlock);
      cbConditionRow.insert(index, NULL);
      cbConditionBlock.insert(index, wConditionBlock);
      btnBeginBlock.insert(index, NULL);
      btnEndBlock.insert(index, NULL);
      ++count;
    }

    void removeAt(const int &index) {
      if (count < 0 && index < -1 && index > count)
        return;

      // remove signal mapping
      signal_interval->removeMappings( cbInterval[index] );
      signal_columnname->removeMappings( cbColumnName[index] );
      signal_first_operator->removeMappings( cbFirstOperator[index] );
      signal_addrow->removeMappings( btnAddRow[index] );
      signal_removerow->removeMappings( btnRemoveRow[index] );
      signal_beginblock->removeMappings( btnBeginBlock[index] );
      signal_endblock->removeMappings( btnEndBlock[index] );

      // handle second operator
      if ( cbSecondOperator[index] != NULL ) {
        signal_second_operator->removeMappings(cbSecondOperator[index]);
        delete cbSecondOperator[index];
      }

      // delete pointer
      delete cbInterval[index];
      delete cbColumnName[index];
      delete cbFirstOperator[index];
      delete cbSecondOperator[index];
      delete txtSearchValue[index];
      delete btnAddRow[index];
      delete btnRemoveRow[index];
      delete cbBlock[index];
      delete cbConditionRow[index];
      delete cbConditionBlock[index];
      delete btnBeginBlock[index];
      delete btnEndBlock[index];

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
      cbBlock.removeAt(index);
      cbConditionRow.removeAt(index);
      cbConditionBlock.removeAt(index);
      btnBeginBlock.removeAt(index);
      btnEndBlock.removeAt(index);
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
      if ( cbInterval[index] != NULL ) {
        return cbInterval[index]->currentText();
      }
      return "";
    }

    int getIntervalWeight(const int &index) const {
      if ( cbInterval[index] != NULL ) {
        return cbInterval[index]->currentData().toInt();
      }
      return WEIGHT_INVALID;
    }

    IntervalWeight getIntervalWeight_v2(const int &index) const {
      if ( index < 0 || index > count )
        return WEIGHT_INVALID;

      if ( cbInterval[index] != NULL ) {
        int weight = cbInterval[index]->currentData().toInt();

        switch( weight ) {
          case WEIGHT_MONTHLY: return WEIGHT_MONTHLY;
          case WEIGHT_WEEKLY: return WEIGHT_WEEKLY;
          case WEIGHT_DAILY: return WEIGHT_DAILY;
          case WEIGHT_60MIN: return WEIGHT_60MIN;
          case WEIGHT_5MIN: return WEIGHT_5MIN;
          case WEIGHT_1MIN: return WEIGHT_1MIN;
          default: return WEIGHT_INVALID;
        }
      }
      return WEIGHT_INVALID;
    }

    QString getColumnName(const int &index) const {
      return cbColumnName[index] != NULL ?
             cbColumnName[index]->currentData().toString() : "";
    }

    QString getFirstOperator(const int &index) const {
      return cbFirstOperator[index] != NULL ?
             cbFirstOperator[index]->currentData().toString() : "";
    }

    QString getSecondOperator(const int &index) const {
      return cbSecondOperator[index] != NULL ?
             cbSecondOperator[index]->currentData().toString() : "";
    }

    QString getSecondOperatorText(const int &index) const {
      return cbSecondOperator[index] != NULL ?
             cbSecondOperator[index]->currentText() : "";
    }

    QString getSearchValue(const int &index) const {
      if (cbColumnName[index] == NULL) return "";

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

    QString getSqlConditionRow(const int &index) const {
      // ignore out of index and last row
      if ( index < 0 || index >= count-1 ) return "";

      bool isNextRowEndBlock = (cbBlock[index+1] != NULL &&
          cbBlock[index+1]->currentData().toString() == ")");

      if ( !isNextRowEndBlock ) {
        if ( cbConditionRow[index] != NULL ) {
          return cbConditionRow[index]->currentData().toString();
        }
        else if ( cbConditionBlock[index] != NULL ) {
          return cbConditionBlock[index]->currentData().toString();
        }
      }

      return "";
    }

    QString getSqlString(const int &index, const int &id_threshold, QString tableName = "", QString intersectTableAlias = "") const {
      if ( index < 0 || index > count ) return "";

      if ( cbBlock[index] != NULL ) {
        return cbBlock[index]->currentData().toString();
      }

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
      else if (column == SQLiteHandler::COLUMN_NAME_OPEN ||
               column == SQLiteHandler::COLUMN_NAME_HIGH ||
               column == SQLiteHandler::COLUMN_NAME_LOW ||
               column == SQLiteHandler::COLUMN_NAME_CLOSE) {
        QString second_operator = getSecondOperator(index);

        if (!second_operator.isEmpty()) {
          second_operator = intersectTableAlias + "." + second_operator;

          bool isPositive = false;
          bool isZero = (value.toDouble() == 0);

          if (value.toDouble() >= 0) {
            value = "+" + value;
            isPositive = true;
          }

          if (search_operator != "=") {
            return "(" + tableName + column + search_operator + second_operator + value + ")";
          } else {

            if (isZero) {
              return tableName + column + "=" + second_operator;
            }
            else if (isPositive) {
              return "((" + tableName + column + ">=" + second_operator + ") AND "
                      "(" + tableName + column + "<=" + second_operator + value + "))";
            } else {
              return "((" + tableName + column + "<=" + second_operator + ") AND "
                      "(" + tableName + column + ">=" + second_operator + value + "))";
            }
          }
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
      else if (column == USERDATA_DISTCC_FSCROSS_RANK_POS) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS + ">0)";
      }
      else if (column == USERDATA_DISTCC_FSCROSS_RANK_NEG) {
        return "(" + tableName + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK + search_operator + value + " AND " +
                     tableName + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS + "<0)";
      }
      else if ( srthreshold_column.contains(column) ) {
        column += "_" + QString::number(id_threshold);
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
        return "(" + intersect_column + ">=" + main_pricelow + " AND " +
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
      QString resistance_value_column;
      QString resistance_intersect_column;
      QString id_threshold;

      if (intersect_alias.isEmpty()) {
        resistance_value_column = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
        id_threshold = "RT.id_threshold=" + QString::number(threshold_index);

        if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
          resistance_intersect_column = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
          resistance_intersect_column = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION;
        }
        else if (type == USERDATA_RESISTANCE_DIST_POINT) {
          resistance_intersect_column = "RT." + SQLiteHandler::COLUMN_NAME_DIST_POINT;
        }
        else if (type == USERDATA_RESISTANCE_DIST_ATR) {
          resistance_intersect_column = "RT." + SQLiteHandler::COLUMN_NAME_DIST_ATR;
        }
      }
      else {
        resistance_value_column = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
        id_threshold = intersect_alias.split(".")[0] + "." + resistance_table + ".id_threshold=" + QString::number(threshold_index);

        if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT) {
          resistance_intersect_column = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION) {
          resistance_intersect_column = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION;
        }
        else if (type == USERDATA_RESISTANCE_DIST_POINT) {
          resistance_intersect_column = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_DIST_POINT;
        }
        else if (type == USERDATA_RESISTANCE_DIST_ATR) {
          resistance_intersect_column = intersect_alias.split(".")[0] + "." + resistance_table + "." + SQLiteHandler::COLUMN_NAME_DIST_ATR;
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
          resistance_value_column + "-" + r + "<=" + main_pricehigh + " and (" +
          resistance_value_column + "+" + r + ">=" + main_priceopen + " or "+
          resistance_value_column + "+" + r + ">=" + main_priceclose + " )";
//        QString query_intersect_resistance = "(" +
//            id_threshold + " and " +
//            intersect_resistance + "-" + r + "<=" + main_pricehigh + " and " +
//            intersect_resistance + "+" + r + ">=" + main_priceopen + " and "+
//            main_priceopen + "<" + main_priceclose;

      if (type == SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT ||
          type == SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION ||
          type == USERDATA_RESISTANCE_DIST_POINT ||
          type == USERDATA_RESISTANCE_DIST_ATR) {
        query_intersect_resistance += " and " + resistance_intersect_column + ">=" + value + ")";
      }
      else {
        query_intersect_resistance += ")";
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
      QString support_value_column;
      QString support_intersect_column;
      QString id_threshold;

      if (intersect_alias.isEmpty()) {
        support_value_column = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
        id_threshold = "ST.id_threshold=" + QString::number(threshold_index);

        if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
          support_intersect_column = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
          support_intersect_column = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION;
        }
        else if (type == USERDATA_SUPPORT_DIST_POINT) {
          support_intersect_column = "ST." + SQLiteHandler::COLUMN_NAME_DIST_POINT;
        }
        else if (type == USERDATA_SUPPORT_DIST_ATR) {
          support_intersect_column = "ST." + SQLiteHandler::COLUMN_NAME_DIST_ATR;
        }
      }
      else {
        support_value_column = intersect_alias.split(".")[0] + "." + support_table + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
        id_threshold = intersect_alias.split(".")[0] + "." + support_table + ".id_threshold=" + QString::number(threshold_index);

        if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT) {
          support_intersect_column = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT;
        }
        else if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION) {
          support_intersect_column = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION;
        }
        else if (type == USERDATA_SUPPORT_DIST_POINT) {
          support_intersect_column = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_DIST_POINT;
        }
        else if (type == USERDATA_SUPPORT_DIST_ATR) {
          support_intersect_column = intersect_alias.split(".")[0] + "." + support_table + "." +SQLiteHandler::COLUMN_NAME_DIST_ATR;
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
          support_value_column + "+" + r + ">=" + main_pricelow + " and (" +
          support_value_column + "-" + r + "<=" + main_priceopen + " or " +
          support_value_column + "-" + r + "<=" + main_priceclose + " )";
      //        QString query_intersect_support = "(" +
      //          id_threshold + " and " +
      //          intersect_support + "+" + r + ">=" + main_pricelow + " and " +
      //          intersect_support + "-" + r + "<=" + main_priceopen + " and " +
      //          main_priceopen + ">" + main_priceclose;

      if (type == SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT ||
          type == SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION ||
          type == USERDATA_SUPPORT_DIST_POINT ||
          type == USERDATA_SUPPORT_DIST_ATR) {
        query_intersect_support += " and " + support_intersect_column + ">=" + value + ")";
      }
      else {
        query_intersect_support += ")";
      }

      return query_intersect_support;
    }

//    QString getSqlResistanceIntersect_V2(const int &index, const QString &main_alias, const IntervalWeight &projection_interval, const int &id_threshold) const {
//      QString _main = main_alias;
//      if (!main_alias.isEmpty()) _main += ".";
//      return _main + SQLiteHandler::get_interval_name(projection_interval) + "_RLine_" + QString::number(id_threshold) + ">=1";
//    }

//    QString getSqlSupportIntersect_V2(const int &index, const QString &main_alias, const IntervalWeight &projection_interval, const int &id_threshold) const {
//      QString _main = main_alias;
//      if (!main_alias.isEmpty()) _main += ".";
//      return _main + SQLiteHandler::get_interval_name(projection_interval) + "_SLine_" + QString::number(id_threshold) + ">=1";
//    }

    QString get_smallest_interval() const {
      int skey = get_smallest_interval_weight();
      QString res;
      switch ( skey ) {
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
        if ( w != WEIGHT_INVALID && w < smallest ) smallest = w;
      }
      return smallest;
    }

    IntervalWeight get_smallest_interval_weight_v2() const {
      int skey = get_smallest_interval_weight();
      IntervalWeight res;
      switch ( skey ) {
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
      if (!is_intersect_operator(index)) return WEIGHT_INVALID;
      QString search_operator = cbFirstOperator[index]->currentData().toString();
      if (search_operator == OPERATOR_INTERSECT_1MIN) return WEIGHT_1MIN;
      if (search_operator == OPERATOR_INTERSECT_5MIN) return WEIGHT_5MIN;
      if (search_operator == OPERATOR_INTERSECT_60MIN) return WEIGHT_60MIN;
      if (search_operator == OPERATOR_INTERSECT_DAILY) return WEIGHT_DAILY;
      if (search_operator == OPERATOR_INTERSECT_WEEKLY) return WEIGHT_WEEKLY;
      if (search_operator == OPERATOR_INTERSECT_MONTHLY) return WEIGHT_MONTHLY;
      return WEIGHT_INVALID;
    }

    // Return intersect column name:
    //  Resistance, Support, Duration, ReactCount,
    //  FastAvg, SlowAvg
    QString get_intersect_type(const int &index) const {
      if (cbSecondOperator[index] != NULL && is_intersect_operator(index)) {
        return qobject_cast<QComboBox*>(cbSecondOperator[index])->currentData().toString();
      }
      return "";
    }


    //
    // For Open/Save search condition into text file
    //
    QString getConditionString(const int &index) {
      if (index < 0 || index > count) return "";

      QString condition_string;

      if ( cbColumnName[index] != NULL ) {

        QString currData = cbColumnName[index]->currentData().toString();

        condition_string =
          cbInterval[index]->currentText() + "," +
          cbColumnName[index]->currentText() + "," +
          cbFirstOperator[index]->currentText() + ",";

        if ( cbSecondOperator[index] != NULL ) {
          condition_string += cbSecondOperator[index]->currentText();

          if ( txtSearchValue[index] != NULL )
            condition_string += ",";
        }

        if ( currData == SQLiteHandler::COLUMN_NAME_DATE ) {
          condition_string += qobject_cast<QDateEdit*>(txtSearchValue[index])->date().toString("MM/dd/yyyy");
        }
        else if ( currData == SQLiteHandler::COLUMN_NAME_TIME ) {
          condition_string += qobject_cast<QTimeEdit*>(txtSearchValue[index])->time().toString("HH:mm");
        }
        else if ( currData == USERDATA_DATETIME ) {
          condition_string += qobject_cast<QDateTimeEdit*>(txtSearchValue[index])->dateTime().toString("MM/dd/yyyy HH:mm");
        }
        else {
          condition_string += getSearchValue(index);
        }

        condition_string += "," + cbConditionRow[index]->currentText();

      } else {
        condition_string += cbBlock[index]->currentData().toString();

        if ( cbConditionBlock[index] != NULL ) {
          condition_string += "," + cbConditionBlock[index]->currentText();
        }
      }

      return condition_string;
    }

    void setConditionString(const int &index, const QStringList &condition) {
      // the array of string should contain column text from CONDITION
      // for example:
      // standard condition:
      // [0] = Daily
      // [1] = Date
      // [2] = greater than or equal
      // [3] = 01/01/2015
      // [4] = Condition (Row)
      // with SecondOperator:
      // [0] = Daily
      // [1] = Price
      // [2] = Intersect Daily
      // [3] = Daily Resistant #React
      // [4] = 3
      // [5] = Condition (Row)

      if ( condition[0] == "(" ) {
        insertBeginBlock( index );
      }
      else if ( condition[0] == ")" ) {
        insertEndBlock( index );
        cbConditionBlock[index]->setCurrentText( condition[1] );
      }
      else {
        if ( condition.size() < 4 ) return; // just in case (validation purpose)

        cbInterval[index]->setCurrentText( condition[0] );
        cbColumnName[index]->setCurrentText( condition[1] );
        cbFirstOperator[index]->setCurrentText( condition[2] );

        if ( cbSecondOperator[index] != NULL ) {
          cbSecondOperator[index]->setCurrentText( condition[3] );
        }

        if ( txtSearchValue[index] != NULL ) {
          int c_idx = condition.size() - 2;

          if (condition[1] == "Date") {
            qobject_cast<QDateEdit*>( txtSearchValue[index] )
                ->setDate(QDate::fromString( condition[c_idx] , "MM/dd/yyyy" ));
          }
          else if (condition[1] == "Time") {
            qobject_cast<QTimeEdit*>(txtSearchValue[index])
                ->setTime(QTime::fromString( condition[c_idx] , "HH:mm" ));
          }
          else if (condition[1] == "Datetime") {
            qobject_cast<QDateTimeEdit*>(txtSearchValue[index])
                ->setDateTime(QDateTime::fromString( condition[c_idx] , "MM/dd/yyyy HH:mm" ));
          }
          else if (qobject_cast<QComboBox*>(txtSearchValue[index]) != NULL) {
            qobject_cast<QComboBox*>(txtSearchValue[index])
                ->setCurrentText( condition[c_idx] );
          }
          else if (qobject_cast<QSpinBox*>(txtSearchValue[index]) != NULL) {
            qobject_cast<QSpinBox*>(txtSearchValue[index])
                ->setValue( condition[c_idx].toInt() );
          }
          else if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[index]) != NULL) {
            qobject_cast<QDoubleSpinBox*>(txtSearchValue[index])
                ->setValue( condition[c_idx].toDouble() );
          }

          cbConditionRow[index]->setCurrentText( condition[ condition.size() - 1 ] );
        }
      }
    }

    //
    // Parameters
    //
    void set_developer_mode(const bool &b) {
      enable_developer_mode = b; //currently not useful
    }

    bool is_intersect_operator(const int &index) const {
      if (cbFirstOperator[index] != NULL) {
        QString search_operator = cbFirstOperator[index]->currentData().toString();

        return (search_operator == OPERATOR_INTERSECT_5MIN ||
                search_operator == OPERATOR_INTERSECT_60MIN ||
                search_operator == OPERATOR_INTERSECT_DAILY ||
                search_operator == OPERATOR_INTERSECT_WEEKLY ||
                search_operator == OPERATOR_INTERSECT_MONTHLY);
      }

      return false;
    }

    bool is_single_interval() const {
      QMap<int,QString> temp;

      for (int i = 0; i < count; ++i) {
        if ( cbBlock[i] == NULL ) {
          temp.insert( getIntervalWeight(i) , getInterval(i) );
          if ( temp.count() > 1 ) return false;
        }
      }
      return true;
    }

    void set_macd_threshold(const int &index) {
      idx_macd = index;
    }

    void set_rsi_threshold(const int &index) {
      idx_rsi = index;
    }

    void set_slowk_threshold(const int &index) {
      idx_slowk = index;
    }

    void set_slowd_threshold(const int &index) {
      idx_slowd = index;
    }

    void set_cgf_threshold(const int &index) {
      idx_cgf = index;
    }

    void set_clf_threshold(const int &index) {
      idx_clf = index;
    }

    void set_cgs_threshold(const int &index) {
      idx_cgs = index;
    }

    void set_cls_threshold(const int &index) {
      idx_cls = index;
    }

    // TODO:
    void refresh_column_name(const QString &symbol) {
      QString _id, value1, value2;
      QString item_data;
      QString item_text;
      int N;

      for (int i = 0; i < count; ++i) {
        N = cbColumnName[i]->count();

        for (int j = 0; j < N; ++j) {
          item_text = cbColumnName[i]->itemText(j);
          item_data = cbColumnName[i]->itemData(j).toString();

          if (item_data == SQLiteHandler::COLUMN_NAME_MACD) {
            j += 3; // Skip MACD, MACD (R+), MACD (R-)
            _id = "_" + QString::number(idx_macd);
            value1 = "MACD " + t_macd[idx_macd-1].operator1 + " " + QString::number(t_macd[idx_macd-1].value1);
            value2 = "MACD " + t_macd[idx_macd-1].operator2 + " " + QString::number(t_macd[idx_macd-1].value2);

            cbColumnName[i]->setItemText(j  , value1);
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_MACD_VALUE1 + _id);
            cbColumnName[i]->setItemText(j  , value2);
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_MACD_VALUE2 + _id);
            cbColumnName[i]->setItemText(j  , value1 + " (R)");
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_MACD_RANK1 + _id);
            cbColumnName[i]->setItemText(j  , value2 + " (R)");
            cbColumnName[i]->setItemData(j  , SQLiteHandler::COLUMN_NAME_MACD_RANK2 + _id);
          }
          else if (item_data == SQLiteHandler::COLUMN_NAME_RSI) {
            ++j; // Skip RSI
            _id = "_" + QString::number(idx_rsi);
            value1 = "RSI " + t_rsi[idx_rsi-1].operator1 + " " + QString::number(t_rsi[idx_rsi-1].value1);
            value2 = "RSI " + t_rsi[idx_rsi-1].operator2 + " " + QString::number(t_rsi[idx_rsi-1].value2);
            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_RSI_VALUE1 + _id);
            ++j; // goto (R)
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_RSI_RANK1 + _id);
            ++j; // Skip (R)
            cbColumnName[i]->setItemText(j, value2);
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_RSI_VALUE2 + _id);
            ++j; // goto (R)
            cbColumnName[i]->setItemText(j, value2 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_RSI_RANK2 + _id);
          }
          else if (item_data == SQLiteHandler::COLUMN_NAME_SLOWK) {
            ++j; // Skip SlowK
            _id = "_" + QString::number(idx_slowk);
            value1 = "SlowK " + t_slowk[idx_slowk-1].operator1 + " " + QString::number(t_slowk[idx_slowk-1].value1);
            value2 = "SlowK " + t_slowk[idx_slowk-1].operator2 + " " + QString::number(t_slowk[idx_slowk-1].value2);
            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWK_VALUE1 + _id);
            ++j; // goto (R)
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWK_RANK1 + _id);
            ++j; // skip (R)
            cbColumnName[i]->setItemText(j, value2);
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWK_VALUE2 + _id);
            ++j; // goto (R)
            cbColumnName[i]->setItemText(j, value2 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWK_RANK2 + _id);
          }
          else if (item_data == SQLiteHandler::COLUMN_NAME_SLOWD) {
            ++j; // Skip SlowD
            _id = "_" + QString::number(idx_slowd);
            value1 = "SlowD " + t_slowd[idx_slowd-1].operator1 + " " + QString::number(t_slowd[idx_slowd-1].value1);
            value2 = "SlowD " + t_slowd[idx_slowd-1].operator2 + " " + QString::number(t_slowd[idx_slowd-1].value2);
            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWD_VALUE1 + _id);
            ++j; // goto (R)
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWD_RANK1 + _id);
            ++j; // skip (R)
            cbColumnName[i]->setItemText(j, value2);
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWD_VALUE2 + _id);
            ++j; // goto (R)
            cbColumnName[i]->setItemText(j, value2 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_SLOWD_RANK2 + _id);
          }
          else if (item_data.startsWith(SQLiteHandler::COLUMN_NAME_CGF)) {
            _id = "_" + QString::number(idx_cgf); // zero based
            value1 = t_cgf[symbol][ idx_cgf ].value;

            if ( value1.toDouble() != 0 ) {
              value1 = "C > F-" + BarDataAdapter::remove_trailing_zero( value1 );
            } else {
              value1 = "C > F";
            }

            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_CGF + _id);
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_CGF_RANK + _id);
          }
          else if (item_data.startsWith(SQLiteHandler::COLUMN_NAME_CLF)) {
            _id = "_" + QString::number(idx_clf); // zero based
            value1 = t_clf[symbol][ idx_clf ].value;

            if ( value1.toDouble() != 0 ) {
              value1 = "C < F+" + BarDataAdapter::remove_trailing_zero( value1 );
            } else {
              value1 = "C < F";
            }

            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_CLF + _id);
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_CLF_RANK + _id);
          }
          else if (item_data.startsWith(SQLiteHandler::COLUMN_NAME_CGS)) {
            _id = "_" + QString::number(idx_cgs); // zero based
            value1 = t_cgs[symbol][ idx_cgs ].value;

            if ( value1.toDouble() != 0 ) {
              value1 = "C > S-" + BarDataAdapter::remove_trailing_zero( value1 );
            } else {
              value1 = "C > S";
            }

            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_CGS + _id);
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_CGS_RANK + _id);
          }
          else if (item_data.startsWith(SQLiteHandler::COLUMN_NAME_CLS)) {
            _id = "_" + QString::number(idx_cls); // zero based
            value1 = t_cls[symbol][ idx_cls ].value;

            if ( value1.toDouble() != 0 ) {
              value1 = "C < S+" + BarDataAdapter::remove_trailing_zero( value1 );
            } else {
              value1 = "C < S";
            }

            cbColumnName[i]->setItemText(j, value1);
            cbColumnName[i]->setItemData(j++, SQLiteHandler::COLUMN_NAME_CLS + _id);
            cbColumnName[i]->setItemText(j, value1 + " (R)");
            cbColumnName[i]->setItemData(j, SQLiteHandler::COLUMN_NAME_CLS_RANK + _id);
          }
        }
      }
    }

    bool validate_block() {
      QStack<int> m_stack;
      QString sign;

      for (int i = 0; i < count; ++i) {
        if ( cbBlock[i] != NULL ) {
          sign = cbBlock[i]->currentData().toString();
          if ( sign == "(" ) {
            m_stack.push( 1 );
          } else {
            if ( m_stack.isEmpty() ) return false;
            m_stack.pop();
          }
        }
      }

      return m_stack.isEmpty();
    }

  private slots:
    void interval_onIndexChanged(QWidget *w) {
      const int idx_rowitem = cbInterval.indexOf(static_cast<QComboBox*>(w));
      const int curr_interval_weight = getIntervalWeight(idx_rowitem);
      QString current_data = cbColumnName[idx_rowitem]->currentData().toString();
      QString current_text = cbColumnName[idx_rowitem]->currentText();
      QString first_item_data = cbColumnName[idx_rowitem]->itemData(0).toString();
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

      //
      // (2A) handle Price's Intersect Operator, show only intersect that greater than current resolution
      //
      if (current_data == USERDATA_PRICE) {
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
        setSecondOperatorPrice(cbSecondOperator[idx_rowitem],
                               getIntersectWeight(idx_rowitem),
                               cbSecondOperator[idx_rowitem]->currentIndex());
      }

      //
      // (2B) handle OHLC's second operator option
      //
      else if (current_data == SQLiteHandler::COLUMN_NAME_OPEN ||
               current_data == SQLiteHandler::COLUMN_NAME_HIGH ||
               current_data == SQLiteHandler::COLUMN_NAME_LOW ||
               current_data == SQLiteHandler::COLUMN_NAME_CLOSE) {

        setSecondOperatorAvg(cbSecondOperator[idx_rowitem],
                             getIntervalWeight_v2(idx_rowitem),
                             cbSecondOperator[idx_rowitem]->currentIndex());
      }

      // (3) swap Date or DateTime
      if (curr_interval_weight < WEIGHT_DAILY && first_item_data == SQLiteHandler::COLUMN_NAME_DATE) {

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
      else if (curr_interval_weight >= WEIGHT_DAILY && first_item_data == USERDATA_DATETIME) {

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
        //
        // Create First operator
        //
        setFirstOperatorPrice(cbFirstOperator[idx_rowitem], getIntervalWeight(idx_rowitem));

//        delete txtSearchValue[idx_rowitem];
        txtSearchValue[idx_rowitem] = NULL;

        //
        // Create Second operator
        //
        cbSecondOperator[idx_rowitem] = createSecondOperatorPrice(parent, getIntersectWeight(idx_rowitem));
        signal_second_operator->setMapping(cbSecondOperator[idx_rowitem], cbSecondOperator[idx_rowitem]);
        connect(cbSecondOperator[idx_rowitem], SIGNAL(currentIndexChanged(int)), signal_second_operator, SLOT(map()));

        table->setSpan(idx_rowitem, 3, 1, 2);
        table->setCellWidget(idx_rowitem, 3, cbSecondOperator[idx_rowitem]);
      }
      else if (current_data == SQLiteHandler::COLUMN_NAME_OPEN ||
               current_data == SQLiteHandler::COLUMN_NAME_HIGH ||
               current_data == SQLiteHandler::COLUMN_NAME_LOW ||
               current_data == SQLiteHandler::COLUMN_NAME_CLOSE) {
        //
        // Create First operator
        //
        setNumericOperator(cbFirstOperator[idx_rowitem], 0);

//        delete txtSearchValue[idx_rowitem];
        txtSearchValue[idx_rowitem] = NULL;

        //
        // Create Second operator
        //
        cbSecondOperator[idx_rowitem] = createSecondOperatorAvg(parent, getIntervalWeight_v2(idx_rowitem));
        signal_second_operator->setMapping(cbSecondOperator[idx_rowitem], cbSecondOperator[idx_rowitem]);
        connect(cbSecondOperator[idx_rowitem], SIGNAL(currentIndexChanged(int)), signal_second_operator, SLOT(map()));

        if (table->columnSpan(idx_rowitem, 3) > 1) {
          table->setSpan(idx_rowitem, 3, 1, 1);
        }

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
          if (current_data == USERDATA_BARCOLOR || current_data.contains("Zone")) {
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
      else if (current_data == SQLiteHandler::COLUMN_NAME_OPEN ||
               current_data == SQLiteHandler::COLUMN_NAME_HIGH ||
               current_data == SQLiteHandler::COLUMN_NAME_LOW ||
               current_data == SQLiteHandler::COLUMN_NAME_CLOSE) {
        double value = 0;

        // try to cast previous value
        if (txtSearchValue[idx_rowitem] != NULL) {
          if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[idx_rowitem]) != NULL) {
            value = qobject_cast<QDoubleSpinBox*> (txtSearchValue[idx_rowitem])->value();
          }
          else if (qobject_cast<QSpinBox*>(txtSearchValue[idx_rowitem]) != NULL) {
            value = qobject_cast<QSpinBox*>(txtSearchValue[idx_rowitem])->value();
          }
        }

        txtSearchValue[idx_rowitem] = createDoubleSpinbox(parent, value, 4);
        qobject_cast<QDoubleSpinBox*>(txtSearchValue[idx_rowitem])->setRange(-9999999, 9999999);
        table->setCellWidget(idx_rowitem, 4, txtSearchValue[idx_rowitem]);
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

      //
      // intersect operator: rename second operator
      //
      if (is_intersect_operator(idx_rowitem)) {
        IntervalWeight weight = getIntersectWeight(idx_rowitem);
        setSecondOperatorPrice(cbSecondOperator[idx_rowitem], weight, cbSecondOperator[idx_rowitem]->currentIndex());
      }
    }

    void second_operator_onIndexChanged(QWidget *w) {
      const int idx_rowitem = cbSecondOperator.indexOf(static_cast<QComboBox*>(w));
      QString current_data = cbSecondOperator[idx_rowitem]->currentData().toString();

      //
      // Price intersect
      //
      if (is_intersect_operator(idx_rowitem)) {
        // (1) for Duration or React Count: set up search value cell
        if (sr_intersect_integer_column.contains(current_data)) {
          if (qobject_cast<QSpinBox*> (txtSearchValue[idx_rowitem]) == NULL) {
            if (txtSearchValue[idx_rowitem] == NULL) {
              table->setSpan(idx_rowitem, 3, 1, 1);
            }
            txtSearchValue[idx_rowitem] = createIntegerSpinbox(parent, 0);
            table->setCellWidget(idx_rowitem, 4, txtSearchValue[idx_rowitem]);
          }
        }
        else if (sr_intersect_double_column.contains(current_data)) {
          if (qobject_cast<QDoubleSpinBox*>(txtSearchValue[idx_rowitem]) == NULL) {
            if (txtSearchValue[idx_rowitem] == NULL) {
              table->setSpan(idx_rowitem, 3, 1, 1);
            }
            txtSearchValue[idx_rowitem] = createDoubleSpinbox(parent, 0, 4);
            table->setCellWidget(idx_rowitem, 4, txtSearchValue[idx_rowitem]);
          }
        }
        // (2) else remove search value cell
        else if (txtSearchValue[idx_rowitem] != NULL) {
          table->removeCellWidget(idx_rowitem, 4);
          table->setSpan(idx_rowitem, 3, 1, 2); // set columnSpan = 2
          txtSearchValue[idx_rowitem] = NULL;
        }
      }

      repaint_table();
    }

    void addRowButton_onClick(QWidget *w) {
      int idx = btnAddRow.indexOf(static_cast<QPushButton*>(w));
      insertAt(idx < 0 ? 0 : idx + 1);
    }

    void removeRowButton_onClick(QWidget *w) {
      if ( count > 1 ) {
        removeAt( btnRemoveRow.indexOf( static_cast<QPushButton*>(w) ) );

        // scan for empty data in parentheses block
        QString curr_sign;
        QString next_sign;

        for (int i = 0; i < count; ++i) {
          if ( cbBlock[i] != NULL ) {
            curr_sign = cbBlock[i]->currentData().toString();
            next_sign =
              i + 1 < count && cbBlock[i+1] != NULL ?
              cbBlock[i+1]->currentData().toString() : "";

            if ( curr_sign == "(" && next_sign == ")" ) {
              removeAt(i);
              removeAt(i);
            }
          }
        }

        if ( count == 0 ) {
          insertAt(0);
        }
      }
    }

    void beginBlockButton_onClick(QWidget *w) {
      int index = btnBeginBlock.indexOf(static_cast<QPushButton*>(w));

      //
      // insert StartBlock BEFORE current row
      //
      insertBeginBlock( index > -1 ? index : 0 );
    }

    void endBlockButton_onClick(QWidget *w) {
      int index = btnEndBlock.indexOf(static_cast<QPushButton*>(w));

      // simple check for pair of BeginBlock
      QStack<int> m_stack;
      QString sign;

      for (int i = index; i > -1; --i) {
        if ( cbBlock[i] != NULL ) {
          sign = cbBlock[i]->currentData().toString();
          if ( sign == "(" ) {
            m_stack.push_back(0);
          }
          else if ( !m_stack.isEmpty() ) {
            m_stack.pop();
          }
        }
      }

      // if unpaired BeginBlock is exists then
      if ( !m_stack.isEmpty() ) {

        // insert EndBlock AFTER current row
        insertEndBlock( index + 1 );
      }
    }

  private:
    QTableWidget *table = NULL;
    QWidget *parent = NULL;
    QStringList integer_column; // for list of column that searchvalue represent in integer
    QStringList sr_intersect_integer_column; // for duration, react count
    QStringList sr_intersect_double_column; // for DistPoint, DistATR
    QHash<QString, int> srthreshold_column; // for list of column name that depend on sr_threshold index

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
    const QString USERDATA_DISTCC_FSCROSS_RANK_POS = SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK + "_POS";
    const QString USERDATA_DISTCC_FSCROSS_RANK_NEG = SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK + "_NEG";
    const QString USERDATA_RESISTANCE_DIST_POINT = "resistance_" + SQLiteHandler::COLUMN_NAME_DIST_POINT;
    const QString USERDATA_RESISTANCE_DIST_ATR = "resistance_" + SQLiteHandler::COLUMN_NAME_DIST_ATR;
    const QString USERDATA_SUPPORT_DIST_POINT = "support_" + SQLiteHandler::COLUMN_NAME_DIST_POINT;
    const QString USERDATA_SUPPORT_DIST_ATR = "support_" + SQLiteHandler::COLUMN_NAME_DIST_ATR;

    // Widget on QTableView
    QList<QComboBox *> cbInterval;
    QList<QComboBox *> cbColumnName;
    QList<QComboBox *> cbFirstOperator; // 1st operator
    QList<QComboBox *> cbSecondOperator; // 2nd operator
    QList<QWidget *> txtSearchValue;
    QList<QPushButton *> btnAddRow;
    QList<QPushButton *> btnRemoveRow;
    QList<QComboBox *> cbBlock;
    QList<QComboBox *> cbConditionRow; // condition (row level)
    QList<QComboBox *> cbConditionBlock; // condition (block level)
    QList<QPushButton *> btnBeginBlock; // Begin block
    QList<QPushButton *> btnEndBlock; // End block

    // Signal Mapper
    QSignalMapper *signal_interval = NULL;
    QSignalMapper *signal_columnname = NULL;
    QSignalMapper *signal_first_operator = NULL;
    QSignalMapper *signal_second_operator = NULL;
    QSignalMapper *signal_addrow = NULL;
    QSignalMapper *signal_removerow = NULL;
    QSignalMapper *signal_beginblock = NULL;
    QSignalMapper *signal_endblock = NULL;

    // Threshold
    QVector<XmlConfigHandler::t_threshold_1> t_macd;
    QVector<XmlConfigHandler::t_threshold_1> t_rsi;
    QVector<XmlConfigHandler::t_threshold_1> t_slowk;
    QVector<XmlConfigHandler::t_threshold_1> t_slowd;
    QMap< QString , QVector<XmlConfigHandler::t_threshold_2> > t_cgf;
    QMap< QString , QVector<XmlConfigHandler::t_threshold_2> > t_clf;
    QMap< QString , QVector<XmlConfigHandler::t_threshold_2> > t_cgs;
    QMap< QString , QVector<XmlConfigHandler::t_threshold_2> > t_cls;

    // current not used
    bool enable_developer_mode = false;

    // number of rows of condition in tables
    int count = 0;

    // index of threshold used in this class
    int idx_macd;
    int idx_rsi;
    int idx_slowk;
    int idx_slowd;
    int idx_cgf;
    int idx_clf;
    int idx_cgs;
    int idx_cls;

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
        if (base_weight <= WEIGHT_5MIN) { cb->addItem("Intersect 5min", OPERATOR_INTERSECT_5MIN); }
        if (base_weight <= WEIGHT_60MIN) { cb->addItem("Intersect 60min", OPERATOR_INTERSECT_60MIN); }
        if (base_weight <= WEIGHT_DAILY) { cb->addItem("Intersect Daily", OPERATOR_INTERSECT_DAILY); }
        if (base_weight <= WEIGHT_WEEKLY) { cb->addItem("Intersect Weekly", OPERATOR_INTERSECT_WEEKLY); }
        cb->addItem("Intersect Monthly", OPERATOR_INTERSECT_MONTHLY);
        cb->blockSignals(false);
      }
    }

    void setSecondOperatorPrice(QComboBox *cb, const IntervalWeight &interval, const int &index) {
      if (cb != NULL) {
        cb->blockSignals(true);
        cb->clear();
        if (interval == WEIGHT_5MIN) {
          cb->addItem("5-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
          cb->addItem("5-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
        }
        else if (interval == WEIGHT_60MIN) {
          cb->addItem("60-10", SQLiteHandler::COLUMN_NAME_FASTAVG);
          cb->addItem("60-50", SQLiteHandler::COLUMN_NAME_SLOWAVG);
        }
        else if (interval == WEIGHT_DAILY) {
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
        else if (interval == WEIGHT_MONTHLY) {
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

    void setSecondOperatorAvg(QComboBox *cb, const IntervalWeight &interval, const int &index) {
      if (cb != NULL) {
        cb->blockSignals(true);
        cb->clear();

        IntervalWeight w = interval;
        cb->addItem("Itself", 0);

        while (w != WEIGHT_INVALID) {
          cb->addItem(BarDataAdapter::avg_name_by_interval(w, FASTAVG_LENGTH), SQLiteHandler::COLUMN_NAME_FASTAVG);
          cb->addItem(BarDataAdapter::avg_name_by_interval(w, SLOWAVG_LENGTH), SQLiteHandler::COLUMN_NAME_SLOWAVG);
          w = SQLiteHandler::get_parent_interval(w);
        }

        if (index > cb->count()) {
          cb->setCurrentIndex(cb->count() - 1);
        } else {
          cb->setCurrentIndex(index);
        }

        cb->blockSignals(false);
      }
    }

    void setZoneColumnName(QComboBox *cb, const IntervalWeight &interval) {
      cb->blockSignals(true);
      IntervalWeight w = interval;
      int idx = -1;

      for (int i = 0; i < cb->count(); ) {
        if (cb->itemText(i).contains("Zone")) {
          if (idx < 0) idx = i;
          cb->removeItem(i);
          continue;
        }
        ++i;
      }

      while (w != WEIGHT_INVALID) {
        if (w == interval) {
          cb->insertItem(idx++, "O-Zone" + SQLiteHandler::get_interval_name(w), SQLiteHandler::COLUMN_NAME_OPEN_ZONE);
          cb->insertItem(idx++, "H-Zone" + SQLiteHandler::get_interval_name(w), SQLiteHandler::COLUMN_NAME_HIGH_ZONE);
          cb->insertItem(idx++, "L-Zone" + SQLiteHandler::get_interval_name(w), SQLiteHandler::COLUMN_NAME_LOW_ZONE);
          cb->insertItem(idx++, "C-Zone" + SQLiteHandler::get_interval_name(w), SQLiteHandler::COLUMN_NAME_ZONE);
        } else {
          switch (w) {
            case WEIGHT_MONTHLY:
              cb->insertItem(idx++, "O-ZoneMonthly", SQLiteHandler::COLUMN_NAME_OPEN_ZONE_MONTHLY);
              cb->insertItem(idx++, "H-ZoneMonthly", SQLiteHandler::COLUMN_NAME_HIGH_ZONE_MONTHLY);
              cb->insertItem(idx++, "L-ZoneMonthly", SQLiteHandler::COLUMN_NAME_LOW_ZONE_MONTHLY);
              cb->insertItem(idx++, "C-ZoneMonthly", SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY);
              break;

            case WEIGHT_WEEKLY:
              cb->insertItem(idx++, "O-ZoneWeekly", SQLiteHandler::COLUMN_NAME_OPEN_ZONE_WEEKLY);
              cb->insertItem(idx++, "H-ZoneWeekly", SQLiteHandler::COLUMN_NAME_HIGH_ZONE_WEEKLY);
              cb->insertItem(idx++, "L-ZoneWeekly", SQLiteHandler::COLUMN_NAME_LOW_ZONE_WEEKLY);
              cb->insertItem(idx++, "C-ZoneWeekly", SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY);
              break;

            case WEIGHT_DAILY:
              cb->insertItem(idx++, "O-ZoneDaily", SQLiteHandler::COLUMN_NAME_OPEN_ZONE_DAILY);
              cb->insertItem(idx++, "H-ZoneDaily", SQLiteHandler::COLUMN_NAME_HIGH_ZONE_DAILY);
              cb->insertItem(idx++, "L-ZoneDaily", SQLiteHandler::COLUMN_NAME_LOW_ZONE_DAILY);
              cb->insertItem(idx++, "C-ZoneDaily", SQLiteHandler::COLUMN_NAME_ZONE_DAILY);
              break;

            case WEIGHT_60MIN:
              cb->insertItem(idx++, "O-Zone60min", SQLiteHandler::COLUMN_NAME_OPEN_ZONE_60MIN);
              cb->insertItem(idx++, "H-Zone60min", SQLiteHandler::COLUMN_NAME_HIGH_ZONE_60MIN);
              cb->insertItem(idx++, "L-Zone60min", SQLiteHandler::COLUMN_NAME_LOW_ZONE_60MIN);
              cb->insertItem(idx++, "C-Zone60min", SQLiteHandler::COLUMN_NAME_ZONE_60MIN);
              break;
          }
        }
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
