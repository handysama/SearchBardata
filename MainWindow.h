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
//    void local_getSqlQuery(QTableView **table, SRModel **model);
    QSqlQuery* local_getSqlQuery();
    void openSearchConditions(const QString &filename);
    void saveSearchConditions(const QString &filename);
    ~MainWindow();

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
    QMap<int,int> list_interval_threshold; // intersection threshold
    QMap<QString,QVector<XmlConfigHandler::t_threshold>> list_threshold; // support/resistance threshold
    bool enable_developer_mode;
    bool enable_parent_indexing;
    bool enable_test_point_update;
    bool enable_distf_dists_rank;
    bool enable_distfs_rank;
    bool enable_sync_result;

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

    void set_table_view(QTableView *table); // init column width of QTableView

    void set_tab_widget_interval(const IntervalWeight &fromInterval); // handle tab widget

    void set_threshold_parameters(const QString &symbol); // refresh threshold option

    QString sql_string_builder(FilterOptionFactory *table, const QString &symbol,
      QStringList *databaseNames, QStringList *databaseAlias, QStringList *projection_name, const QStringList &matcherResult,
      const IntervalWeight &projection_interval, const bool &enable_projection, const int &id_threshold,
      const double &sr_threshold, const double &avg_threshold);

//    QString sql_string_builder_WTF(FilterOptionFactory *table, const QString &symbol,
//      QStringList *databaseNames, QStringList *databaseAlias, QString *sql_count, QStringList *projection_name,
//      const int &id_threshold, const double &sr_threshold, const double &avg_threshold);

};

class FileLoader : public QThread {
  Q_OBJECT
  public:
    FileLoader(const QString &filename, const QSqlDatabase &database, const QString &symbol, const IntervalWeight &interval) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->filename = filename;
      this->database = database;
      this->symbol = symbol;
      this->interval = interval;
    }

  private:
    QSqlDatabase database;
    QString filename;
    QString symbol;
    IntervalWeight interval;

    void run() {
      BarDataAdapter da;
      long rows = da.loadFile(database, filename, symbol, interval);
      qDebug() << filename << "-> rows:" << rows;

      if (rows > 0) {
        da.update_candle(database);
        da.update_zone(database, symbol, interval);
      }

      database.close();
      database = QSqlDatabase();
      exit(0);
    }
};

class ParentIndexThread : public QThread {
  Q_OBJECT

  public:
    ParentIndexThread(const QSqlDatabase &child, const QString &symbol, const IntervalWeight &parent_weight) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->base_database = child;
      this->symbol = symbol;
      this->parent_weight = parent_weight;
      this->parent_database_name = SQLiteHandler::getDatabaseName(symbol, parent_weight);
      this->base_weight = SQLiteHandler::get_child_interval(parent_weight);
    }

  private:
    QSqlDatabase base_database;
    QString parent_database_name;
    QString symbol;
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

      da.update_index_parent_prev(base_database, symbol, parent_weight);
      da.update_parent_monthly_prev(base_database, symbol);

      if (base_weight < WEIGHT_DAILY) {
        qDebug() << "Update daterange and displacement FS" << symbol << base_weight;
        da.update_daterange(base_database, symbol, base_weight);
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
      BarDataAdapter::update_dist_resistance(database, symbol, base_interval, id_threshold);
      BarDataAdapter::update_dist_support(database, symbol, base_interval, id_threshold);
      BarDataAdapter::update_sline_rline_count(database, b, symbol, base_interval, id_threshold);

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
    HistogramUpdater(const QSqlDatabase &database, const int &year) {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      this->database = database;
      this->rank_from_year = year;
    }

  private:
    QSqlDatabase database;
    int rank_from_year;

    void run() {
      qDebug() << "Update FGS/FLS Rank" << database.databaseName();
      BarDataAdapter::update_rank_fgs_fls(database);

      qDebug() << "Update DistFS/DistCC(FS-Cross)/ATR Rank" << database.databaseName();
      BarDataAdapter::update_rank_distfs_distcc_atr(database);

      qDebug() << "Update CGF/CLF/CGS/CLS Rank" << database.databaseName();
      BarDataAdapter::update_rank_cgf_clf_cgs_cls(database);

      qDebug() << "Update MACD Rank" << database.databaseName();
      BarDataAdapter::update_rank_macd(database);

      qDebug() << "Update Candle Rank" << database.databaseName();
      BarDataAdapter::update_rank_candle(database);

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
