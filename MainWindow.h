#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractSlider>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QQueue>
#include <QScrollBar>
#include <QSqlQueryModel>
#include <QtConcurrentRun>
#include <QThread>

#include "BarBuffer.h"
#include "BarDataAdapter.h"
#include "BarDataList.h"
#include "FilterOptionFactory.h"
//#include "MySqlConnector.h"
#include "SearchResultWriter.h"
#include "SQLiteHandler.h"
#include "SRModel.h"
#include "XmlConfigHandler.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    const QString application_path = QCoreApplication::applicationDirPath();
    const QString config_path = application_path + "/config.xml";
    explicit MainWindow(const int &id, QWidget *parent = 0);
    ~MainWindow();
    void openSearchConditions(const QString &filename);
    void saveSearchConditions(const QString &filename);
    void refresh_threshold_component();
    QSqlQuery* local_getSqlQuery(); // experimental

    // Setter for threshold
    void set_macd_threshold_id(const int &id) { tid_macd = id; } // non-zero based
    void set_rsi_threshold_id(const int &id) { tid_rsi = id; } // non-zero based
    void set_slowk_threshold_id(const int &id) { tid_slowk = id; } // non-zero based
    void set_slowd_threshold_id(const int &id) { tid_slowd = id; } // non-zero based
    void set_cgf_threshold_id(const int &id) { tid_cgf = id; } // zero based
    void set_clf_threshold_id(const int &id) { tid_clf = id; } // zero based
    void set_cgs_threshold_id(const int &id) { tid_cgs = id; } // zero based
    void set_cls_threshold_id(const int &id) { tid_cls = id; } // zero based

    // Getter for threshold
    int get_macd_threshold() const { return tid_macd; }
    int get_rsi_threshold() const { return tid_rsi; }
    int get_slowk_threshold() const { return tid_slowk; }
    int get_slowd_threshold() const { return tid_slowd; }
    int get_cgf_threshold() const { return tid_cgf; }
    int get_clf_threshold() const { return tid_clf; }
    int get_cgs_threshold() const { return tid_cgs; }
    int get_cls_threshold() const { return tid_cls; }
    QString getSymbol() const;

  signals:
    void start_updating_database();
    void finish_updating_database();

  private slots:
    void start_updating_data() {
      mutex.lock();
      ++active_thread;
      mutex.unlock();
    }

    void finish_updating_data() {
      mutex.lock();
      --active_thread;
      mutex.unlock();
    }

    void on_btnClearResult_clicked();
    void on_btnExport_clicked();
    void on_btnRefresh_clicked();
    void on_btnSearch_clicked();
    void on_cbSymbol_currentIndexChanged(const int &index);
    void on_chkPlotMatcher_stateChanged(const int &value);
    void on_tabWidget_currentChanged(const int &index);
    void resizeEvent(QResizeEvent *e);

  private:
    friend class MasterWindow;
    Ui::MainWindow *ui;
    FilterOptionFactory *filter = NULL;
    QVector<QString> vec_label_string;
    int SBAR_ID; // app id for multi-instance handling
    int prevtab_idx; // index of previous tab widget

    SRModel *srmodel = NULL;
    SRModel *srmodel_monthly = NULL;
    SRModel *srmodel_weekly = NULL;
    SRModel *srmodel_daily = NULL;
    SRModel *srmodel_60min = NULL;
    SRModel *srmodel_5min = NULL;

    QSqlQuery *model_query = NULL;
    QSqlQuery *model_query_daily = NULL;
    QSqlQuery *model_query_weekly = NULL;
    QSqlQuery *model_query_monthly = NULL;
    QSqlQuery *model_query_60min = NULL;
    QSqlQuery *model_query_5min = NULL;

    QSqlDatabase model_database;

    // status information
    QLabel label_query_result;
    QLabel label_sync_result;
    QLabel label_tab_id;
    QLabel label_threshold_params;
    bool is_enable_components;

    // database hander
    SQLiteHandler sqlitedb;
    QMap<QString,int> database_last_rowid;
    QMap<QString,bool> database_update_flag;
    QStringList matcher_rowid;

    // config vars
    QDateTime last_update;
    QString database_dir;
    QString input_dir;
    QString result_dir;
    QStringList list_symbol;
    QMap<int,QString> list_interval_weight;
    QMap<int,int> list_interval_threshold; // sr intersection threshold
    QMap<QString,QVector<XmlConfigHandler::t_sr_threshold>> list_threshold; // sr refresh threshold
    QVector<XmlConfigHandler::t_threshold_1> t_macd;
    QVector<XmlConfigHandler::t_threshold_1> t_rsi;
    QVector<XmlConfigHandler::t_threshold_1> t_slowk;
    QVector<XmlConfigHandler::t_threshold_1> t_slowd;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cgf;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_clf;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cgs;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cls;

    bool enable_developer_mode;
    bool enable_parent_indexing;
    bool enable_update_support_resistance;
    bool enable_update_histogram;
    bool enable_sync_result;
    int tid_macd;
    int tid_rsi;
    int tid_slowk;
    int tid_slowd;
    int tid_cgf, tid_clf, tid_cgs, tid_cls;

    // thread vars
    QThreadPool *thread_pool = QThreadPool::globalInstance();
    QMutex mutex;
    int active_thread;

    // Future
    IntervalWeight *m_projection_weight;
    SRModel *m_srmodel;
    QTableView *m_table;
    QSqlDatabase *m_main_database;
    QStringList *m_databaseName;
    QStringList *m_databaseAlias;
    QString *m_sql_select;
    QStringList *m_projectionNames;
    bool *m_forwardOnly;
    int *m_prevbar_count;

    void clear_program_result(const QString &symbol); // clear previous tradestation sync result

    void clear_model(); // free resource after query execute

    void enable_components(const bool &flag);

    // write search result to file (to use in TradeStation)
    void export_result_to_file(const QString &symbol, const QString &interval, QSqlQuery *query, const QSqlDatabase &dbQuery, const bool &delete_flag);

    // read config.xml and refresh window component and variables from config.xml
    // (such as: combobox symbol, label from..to...)
    void initialize(bool update_interface);

    QString parse_filename(const QString &filename, QString *out_symbol, IntervalWeight *out_interval);

    void prepare_database_last_rowid(); // init database_last_rowid (for optimize updating index of new inserted records)

    void prepare_database_update_flag(); // optimization for refresh database

    void read_matcher_file_v2(const QString &filename,const QString &symbol,const QString &interval);

    void set_data_range_label(); // refresh data range information label

    void set_table_view(QTableView *table, const IntervalWeight &base_interval, const IntervalWeight &projection_interval); // init column width of QTableView

    void set_tab_widget_interval(const IntervalWeight &fromInterval); // handle tab widget

    void set_threshold_parameters(const QString &symbol); // refresh threshold option

    QString sql_string_builder(FilterOptionFactory *table, const QString &symbol,
      QStringList *databaseNames, QStringList *databaseAlias, QStringList *projection_name, const QStringList &matcherResult,
      const IntervalWeight &projection_interval, const bool &enable_projection, const int &id_threshold,
      const double &breakThreshold, const double &avgThreshold, const int &macd_id,
      const int &rsi_id, const int &slowk_id, const int &slowd_id, const int &cgf_id, const int &clf_id, const int &cgs_id, const int &cls_id);

//    QString sql_string_builder_WTF(FilterOptionFactory *table, const QString &symbol,
//      QStringList *databaseNames, QStringList *databaseAlias, QString *sql_count, QStringList *projection_name,
//      const int &id_threshold, const double &sr_threshold, const double &avg_threshold);
};

class FileLoader : public QThread {
  Q_OBJECT
  public:
    FileLoader(const QString &filename, const QString &database_path,
               const QSqlDatabase &database, const QString &symbol, const IntervalWeight &interval) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->filename = filename;
      this->database = database;
      this->symbol = symbol;
      this->interval = interval;
      this->database_path = database_path;
    }

  private:
    QSqlDatabase database;
    QString filename;
    QString symbol;
    QString database_path;
    IntervalWeight interval;

    void run() {
      BarDataAdapter da;
      long rows = da.loadFile(database, database_path, filename, symbol, interval);
      qDebug() << filename << "-> rows:" << rows;

      if (rows > 0) {
        da.update_candle(database);
        da.update_zone(database, database_path, symbol, interval);
      }

//      database.close();
//      database = QSqlDatabase();
      SQLiteHandler::removeDatabase(&database);
      exit(0);
    }
};

class ParentIndexThread : public QThread {
  Q_OBJECT

  public:
    ParentIndexThread(const QSqlDatabase &child, const QString &database_path, const QString &symbol, const IntervalWeight &parent_weight) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->base_database = child;
      this->symbol = symbol;
      this->parent_weight = parent_weight;
      this->parent_database_name = SQLiteHandler::getDatabaseName(symbol, parent_weight);
      this->base_weight = SQLiteHandler::get_child_interval(parent_weight);
      this->database_path = database_path;
    }

  private:
    QSqlDatabase base_database;
    QString parent_database_name;
    QString symbol;
    QString database_path;
    IntervalWeight parent_weight;
    IntervalWeight base_weight;

    void run() {
      qDebug() << "Indexing:" << base_database.databaseName();
      BarDataAdapter da;

//      if (parent_weight == WEIGHT_60MIN) {
//        da.update_index_parent(base_database, symbol, parent_database_name, parent_weight);
//      } else {
//        da.update_index_parent_v2(base_database, symbol, parent_weight);
//      }

      da.update_index_parent_prev(base_database, database_path, symbol, parent_weight);
      da.update_parent_monthly_prev(base_database, database_path, symbol);

      if (base_weight < WEIGHT_DAILY) {
        qDebug() << "Update daterange and displacement FS" << symbol << base_weight;
        da.update_daterange(base_database, database_path, symbol, base_weight);

        qDebug() << "Update day-10 and day-50";
        da.update_intraday_avg(base_database, database_path, symbol, base_weight);
      }

      QString connection = base_database.connectionName();
      base_database.close();
      base_database = QSqlDatabase();
      QSqlDatabase::removeDatabase(connection);
      exit(0);
    }
};

class ResistanceSupportUpdater : public QThread {
  Q_OBJECT
  public:
    ResistanceSupportUpdater(const QSqlDatabase &database,
                             const QString &database_path,
                             const QString &symbol,
                             const IntervalWeight &base_interval,
                             const double &b_value,
                             const double &threshold,
                             const int &id_threshold,
                             const int &start_rowid_from) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->database = database;
      this->symbol = symbol;
      this->base_interval = base_interval;
      this->id_threshold = id_threshold;
      this->threshold = threshold;
      this->b = b_value;
      this->r = b_value;
      this->start_rowid_from = start_rowid_from - 250;
      this->threshold = threshold;
      this->database_path = database_path;
      recalculate_data = false;
    }

    void set_recalculate(const bool &value) {
      recalculate_data = value;
    }

  private:
    QSqlDatabase database;
    double b;
    double r;
    int id_threshold;
    int threshold; // 2
    int start_rowid_from;
    bool recalculate_data;
    QString symbol;
    QString database_path;
    IntervalWeight base_interval;

    void run() {
      BarBuffer buffer;
      qDebug() << database.databaseName() << b <<  r << threshold << start_rowid_from << id_threshold << "recalculate:" << recalculate_data;

      if (base_interval >= WEIGHT_DAILY) {
        if (recalculate_data) {
          buffer.DBResistanceSupportGen_Recalculate(database, b, r, threshold, id_threshold);
        } else {
          buffer.DBResistanceSupportGen(database, b, r, threshold, start_rowid_from, id_threshold);
        }
        qDebug() << "Update Support/Resistance: DistPoint and DistATR" << symbol << base_interval;
        BarDataAdapter::update_dist_SRlines(database, id_threshold);
      }

      qDebug() << "Update counting Support/Resistance Intersect Line" << symbol << base_interval;
      BarDataAdapter::update_dist_resistance(database, database_path, symbol, base_interval, id_threshold);
      BarDataAdapter::update_dist_support(database, database_path, symbol, base_interval, id_threshold);
      BarDataAdapter::update_sline_rline_count(database, database_path, b, symbol, base_interval, id_threshold);

      QString connection = database.connectionName();
      database.close();
      database = QSqlDatabase();
      QSqlDatabase::removeDatabase(connection);
      exit(0);
    }
};

class HistogramUpdater : public QThread {
    Q_OBJECT

  public:
    HistogramUpdater(const QSqlDatabase &database, const int &year, const QString &symbol) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->database = database;
      this->rank_from_year = year;
      this->symbol = symbol;
      recalculate_threshold = false;
      init_config();
    }

    void set_recalculate_threshold(const bool &b) {
      recalculate_threshold = b;
    }

    void init_config() {
      XmlConfigHandler *config = XmlConfigHandler::get_instance();
      t_macd = config->get_macd_threshold();
      t_rsi = config->get_rsi_threshold();
      t_slowk = config->get_slowk_threshold();
      t_slowd = config->get_slowk_threshold();
      t_cgf = config->get_cgf_threshold(symbol);
      t_clf = config->get_clf_threshold(symbol);
      t_cgs = config->get_cgs_threshold(symbol);
      t_cls = config->get_cls_threshold(symbol);
    }

  private:
    QSqlDatabase database;
    int rank_from_year;
    bool recalculate_threshold;
    QString symbol;
    QVector<XmlConfigHandler::t_threshold_1> t_macd;
    QVector<XmlConfigHandler::t_threshold_1> t_rsi;
    QVector<XmlConfigHandler::t_threshold_1> t_slowk;
    QVector<XmlConfigHandler::t_threshold_1> t_slowd;
    QVector<XmlConfigHandler::t_threshold_2> t_cgf;
    QVector<XmlConfigHandler::t_threshold_2> t_clf;
    QVector<XmlConfigHandler::t_threshold_2> t_cgs;
    QVector<XmlConfigHandler::t_threshold_2> t_cls;


    void run() {
      if (!recalculate_threshold) {
        qDebug() << "Update FGS/FLS Rank" << database.databaseName();
        BarDataAdapter::update_fgs_fls_rank(database);

        qDebug() << "Update DistFS/DistCC(FS-Cross)/ATR Rank" << database.databaseName();
        BarDataAdapter::update_rank_distfs_distcc_atr(database);

        qDebug() << "Update CGF/CLF/CGS/CLS Rank" << database.databaseName();
        BarDataAdapter::update_rank_cgf_clf_cgs_cls(database);

        qDebug() << "Update MACD Rank" << database.databaseName();
        BarDataAdapter::update_macd_rank(database);

        qDebug() << "Update Candle Rank" << database.databaseName();
        BarDataAdapter::update_candle_rank(database);

        qDebug() << "Update DistOHLC x Fast/Slow" << database.databaseName();
        // Open
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTOF, SQLiteHandler::COLUMN_NAME_N_DISTOF);
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTOS, SQLiteHandler::COLUMN_NAME_N_DISTOS);
        // High
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTHF, SQLiteHandler::COLUMN_NAME_N_DISTHF);
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTHS, SQLiteHandler::COLUMN_NAME_N_DISTHS);
        // Low
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTLF, SQLiteHandler::COLUMN_NAME_N_DISTLF);
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTLS, SQLiteHandler::COLUMN_NAME_N_DISTLS);
        // Close
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTCF, SQLiteHandler::COLUMN_NAME_N_DISTCF);
        BarDataAdapter::update_distOHLC_v3(database, rank_from_year, SQLiteHandler::COLUMN_NAME_DISTCS, SQLiteHandler::COLUMN_NAME_N_DISTCS);
      }

      for (int i = 0; i < t_macd.size(); ++i) {
        if (!t_macd[i].operator1.isEmpty() && !t_macd[i].operator2.isEmpty()) {
          BarDataAdapter::update_column_threshold_1(database, SQLiteHandler::COLUMN_NAME_MACD, t_macd[i]);
        }
      }

      for (int i = 0; i < t_rsi.size(); ++i) {
        if (!t_rsi[i].operator1.isEmpty() && !t_rsi[i].operator2.isEmpty()) {
          BarDataAdapter::update_column_threshold_1(database, SQLiteHandler::COLUMN_NAME_RSI, t_rsi[i]);
        }
      }

      for (int i = 0; i < t_slowk.size(); ++i) {
        if (!t_slowk[i].operator1.isEmpty() && !t_slowk[i].operator2.isEmpty()) {
          BarDataAdapter::update_column_threshold_1(database, SQLiteHandler::COLUMN_NAME_SLOWK, t_slowk[i]);
        }
      }

      for (int i = 0; i < t_slowd.size(); ++i) {
        if (!t_slowd[i].operator1.isEmpty() && !t_slowd[i].operator2.isEmpty()) {
          BarDataAdapter::update_column_threshold_1(database, SQLiteHandler::COLUMN_NAME_SLOWD, t_slowd[i]);
        }
      }

      for (int i = 0; i < t_cgf.size(); ++i) {
        if ( !t_cgf[i].value.isEmpty() ) {
          BarDataAdapter::update_close_threshold(database,
            SQLiteHandler::COLUMN_NAME_CGF, t_cgf[i].t_id, t_cgf[i].value.toDouble());
          BarDataAdapter::update_close_threshold_rank(database, t_cgf[i].t_id);
        }
      }

      for (int i = 0; i < t_clf.size(); ++i) {
        if ( !t_clf[i].value.isEmpty() ) {
          BarDataAdapter::update_close_threshold(database,
            SQLiteHandler::COLUMN_NAME_CLF, t_clf[i].t_id, t_clf[i].value.toDouble());
          BarDataAdapter::update_close_threshold_rank(database, t_clf[i].t_id);
        }
      }

      for (int i = 0; i < t_cgs.size(); ++i) {
        if ( !t_cgs[i].value.isEmpty() ) {
          BarDataAdapter::update_close_threshold(database,
            SQLiteHandler::COLUMN_NAME_CGS, t_cgs[i].t_id, t_cgs[i].value.toDouble());
          BarDataAdapter::update_close_threshold_rank(database, t_cgs[i].t_id);
        }
      }

      for (int i = 0; i < t_cls.size(); ++i) {
        if ( !t_cls[i].value.isEmpty() ) {
          BarDataAdapter::update_close_threshold(database,
            SQLiteHandler::COLUMN_NAME_CLS, t_cls[i].t_id, t_cls[i].value.toDouble());
          BarDataAdapter::update_close_threshold_rank(database, t_cls[i].t_id);
        }
      }

      // experimental
//      BarDataAdapter::initialize_distfs_bin(database);
//      BarDataAdapter::update_distfs_bin_id(database);

      QString connection = database.connectionName();
      database.close();
      database = QSqlDatabase();
      QSqlDatabase::removeDatabase(connection);
      exit(0);
    }
};

#endif // MAINWINDOW_H
