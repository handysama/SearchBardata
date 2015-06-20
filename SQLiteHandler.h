#ifndef SQLITEHANDLER_H
#define SQLITEHANDLER_H

#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStringBuilder>

#define MAX_HISTOGRAM_BIN 2000
#define FASTAVG_LENGTH 10
#define SLOWAVG_LENGTH 50

enum IntervalWeight {
  WEIGHT_1MIN = 1,
  WEIGHT_5MIN = 5,
  WEIGHT_60MIN = 60,
  WEIGHT_DAILY = 1440,
  WEIGHT_WEEKLY = 10080,
  WEIGHT_MONTHLY = 43200,
  WEIGHT_INVALID = -1,
  WEIGHT_LARGEST = WEIGHT_MONTHLY,
  WEIGHT_SMALLEST = WEIGHT_1MIN
};

enum BarColor {
  BARCOLOR_GREEN = 0,
  BARCOLOR_RED = 1,
  BARCOLOR_DOJI = 3
};

class SQLiteHandler {
  public:
    /** TABLE NAME */
    static const QString TABLE_NAME_BARDATA;
    static const QString TABLE_NAME_BARDATA_VIEW;
    static const QString TABLE_NAME_RESISTANCE;
    static const QString TABLE_NAME_RESISTANCE_DATE;
    static const QString TABLE_NAME_SUPPORT;
    static const QString TABLE_NAME_SUPPORT_DATE;
//    static const QString TABLE_NAME_HISTOGRAM_BIN;
//    static const QString TABLE_NAME_RESISTANCE_TAB;
//    static const QString TABLE_NAME_SUPPORT_TAB;

    /** COLUMN NAME */
    static const QString COLUMN_NAME_ROWID;
    static const QString COLUMN_NAME_IDPARENT;
    static const QString COLUMN_NAME_IDPARENT_WEEKLY;
    static const QString COLUMN_NAME_IDPARENT_MONTHLY;
    static const QString COLUMN_NAME_IDPARENT_PREV;
    static const QString COLUMN_NAME_IDPARENT_PREV_WEEKLY;
    static const QString COLUMN_NAME_IDPARENT_PREV_MONTHLY;
    static const QString COLUMN_NAME_IDTHRESHOLD;
    static const QString COLUMN_NAME_DATE;
    static const QString COLUMN_NAME_TIME;
    static const QString COLUMN_NAME_OPEN;
    static const QString COLUMN_NAME_HIGH;
    static const QString COLUMN_NAME_LOW;
    static const QString COLUMN_NAME_CLOSE;
    static const QString COLUMN_NAME_VOLUME;
    static const QString COLUMN_NAME_MACD;
    static const QString COLUMN_NAME_MACDAVG;
    static const QString COLUMN_NAME_MACDDIFF;
    static const QString COLUMN_NAME_RSI;
    static const QString COLUMN_NAME_SLOWK;
    static const QString COLUMN_NAME_SLOWD;
    static const QString COLUMN_NAME_FASTAVG;
    static const QString COLUMN_NAME_SLOWAVG;
    static const QString COLUMN_NAME_FASTAVG_SLOPE;
    static const QString COLUMN_NAME_SLOWAVG_SLOPE;
    static const QString COLUMN_NAME_FCROSS;
    static const QString COLUMN_NAME_SCROSS;
    static const QString COLUMN_NAME_FGS;
    static const QString COLUMN_NAME_FGS_RANK;
    static const QString COLUMN_NAME_FLS;
    static const QString COLUMN_NAME_FLS_RANK;
    static const QString COLUMN_NAME_DISTF;
    static const QString COLUMN_NAME_DISTS;
    static const QString COLUMN_NAME_DISTCF;
    static const QString COLUMN_NAME_DISTCF_RANK;
    static const QString COLUMN_NAME_N_DISTCF;
    static const QString COLUMN_NAME_DISTCS;
    static const QString COLUMN_NAME_DISTCS_RANK;
    static const QString COLUMN_NAME_N_DISTCS;
    static const QString COLUMN_NAME_DISTFS;
    static const QString COLUMN_NAME_DISTFS_RANK;
    static const QString COLUMN_NAME_N_DISTFS;
    static const QString COLUMN_NAME_DISTCC_FSCROSS;
    static const QString COLUMN_NAME_DISTCC_FSCROSS_RANK;
    static const QString COLUMN_NAME_ATR;
    static const QString COLUMN_NAME_ATR_RANK;
    static const QString COLUMN_NAME_OPENBAR;
    static const QString COLUMN_NAME_PREV_DATE;
    static const QString COLUMN_NAME_PREV_TIME;
    static const QString COLUMN_NAME_PREV_BARCOLOR;
    static const QString COLUMN_NAME_MACD_G0;
    static const QString COLUMN_NAME_MACD_L0;
    static const QString COLUMN_NAME_RSI_G70;
    static const QString COLUMN_NAME_RSI_L30;
    static const QString COLUMN_NAME_SLOWK_G80;
    static const QString COLUMN_NAME_SLOWK_L20;
    static const QString COLUMN_NAME_SLOWD_G80;
    static const QString COLUMN_NAME_SLOWD_L20;
    static const QString COLUMN_NAME_MACD_RANK;
    static const QString COLUMN_NAME_MACD_G0_RANK;
    static const QString COLUMN_NAME_MACD_L0_RANK;
//    static const QString COLUMN_NAME_RSI_G70_RANK;
//    static const QString COLUMN_NAME_RSI_L30_RANK;
//    static const QString COLUMN_NAME_SLOWK_G80_RANK;
//    static const QString COLUMN_NAME_SLOWK_L20_RANK;
//    static const QString COLUMN_NAME_SLOWD_G80_RANK;
//    static const QString COLUMN_NAME_SLOWD_L20_RANK;
    static const QString COLUMN_NAME_CANDLE_UPTAIL;
    static const QString COLUMN_NAME_CANDLE_DOWNTAIL;
    static const QString COLUMN_NAME_CANDLE_BODY;
    static const QString COLUMN_NAME_CANDLE_TOTALLENGTH;
    static const QString COLUMN_NAME_N_UPTAIL;
    static const QString COLUMN_NAME_N_DOWNTAIL;
    static const QString COLUMN_NAME_N_BODY;
    static const QString COLUMN_NAME_N_TOTALLENGTH;
    static const QString COLUMN_NAME_CANDLE_UPTAIL_RANK;
    static const QString COLUMN_NAME_CANDLE_DOWNTAIL_RANK;
    static const QString COLUMN_NAME_CANDLE_BODY_RANK;
    static const QString COLUMN_NAME_CANDLE_TOTALLENGTH_RANK;
    static const QString COLUMN_NAME_DATERANGE;
    static const QString COLUMN_NAME_DATETIME_UB;
    static const QString COLUMN_NAME_DATETIME_LB;
    static const QString COLUMN_NAME_CGF;
    static const QString COLUMN_NAME_CLF;
    static const QString COLUMN_NAME_CGF_RANK;
    static const QString COLUMN_NAME_CLF_RANK;
    static const QString COLUMN_NAME_CGS;
    static const QString COLUMN_NAME_CLS;
    static const QString COLUMN_NAME_CGS_RANK;
    static const QString COLUMN_NAME_CLS_RANK;
    static const QString COLUMN_NAME_ZONE;
    static const QString COLUMN_NAME_ZONE_60MIN;
    static const QString COLUMN_NAME_ZONE_DAILY;
    static const QString COLUMN_NAME_ZONE_WEEKLY;
    static const QString COLUMN_NAME_ZONE_MONTHLY;
    static const QString COLUMN_NAME_DISTOF;
    static const QString COLUMN_NAME_DISTOS;
    static const QString COLUMN_NAME_N_DISTOF;
    static const QString COLUMN_NAME_N_DISTOS;
    static const QString COLUMN_NAME_DISTOF_RANK;
    static const QString COLUMN_NAME_DISTOS_RANK;
    static const QString COLUMN_NAME_DISTHF;
    static const QString COLUMN_NAME_DISTHS;
    static const QString COLUMN_NAME_N_DISTHF;
    static const QString COLUMN_NAME_N_DISTHS;
    static const QString COLUMN_NAME_DISTHF_RANK;
    static const QString COLUMN_NAME_DISTHS_RANK;
    static const QString COLUMN_NAME_DISTLF;
    static const QString COLUMN_NAME_DISTLS;
    static const QString COLUMN_NAME_N_DISTLF;
    static const QString COLUMN_NAME_N_DISTLS;
    static const QString COLUMN_NAME_DISTLF_RANK;
    static const QString COLUMN_NAME_DISTLS_RANK;
    // Generic column
    static const QString COLUMN_NAME_RES;
    static const QString COLUMN_NAME_DISTRES;
    static const QString COLUMN_NAME_DISTRES_ATR;
    static const QString COLUMN_NAME_DISTRES_RANK;
    static const QString COLUMN_NAME_SUP;
    static const QString COLUMN_NAME_DISTSUP;
    static const QString COLUMN_NAME_DISTSUP_ATR;
    static const QString COLUMN_NAME_DISTSUP_RANK;
    // Generic column
    static const QString COLUMN_NAME_DAILY_RLINE;
    static const QString COLUMN_NAME_DAILY_RLINE_F;
    static const QString COLUMN_NAME_DAILY_RLINE_S;
    static const QString COLUMN_NAME_DAILY_SLINE;
    static const QString COLUMN_NAME_DAILY_SLINE_F;
    static const QString COLUMN_NAME_DAILY_SLINE_S;
    // Generic column
    static const QString COLUMN_NAME_WEEKLY_RLINE;
    static const QString COLUMN_NAME_WEEKLY_RLINE_F;
    static const QString COLUMN_NAME_WEEKLY_RLINE_S;
    static const QString COLUMN_NAME_WEEKLY_SLINE;
    static const QString COLUMN_NAME_WEEKLY_SLINE_F;
    static const QString COLUMN_NAME_WEEKLY_SLINE_S;
    // Generic column
    static const QString COLUMN_NAME_MONTHLY_RLINE;
    static const QString COLUMN_NAME_MONTHLY_RLINE_F;
    static const QString COLUMN_NAME_MONTHLY_RLINE_S;
    static const QString COLUMN_NAME_MONTHLY_SLINE;
    static const QString COLUMN_NAME_MONTHLY_SLINE_F;
    static const QString COLUMN_NAME_MONTHLY_SLINE_S;
    // Resistance/Support
    static const QString COLUMN_NAME_DIST_POINT;
    static const QString COLUMN_NAME_DIST_ATR;
    static const QString COLUMN_NAME_RESISTANCE_COUNT;
    static const QString COLUMN_NAME_RESISTANCE_DURATION;
    static const QString COLUMN_NAME_RESISTANCE_VALUE;
    static const QString COLUMN_NAME_SUPPORT_COUNT;
    static const QString COLUMN_NAME_SUPPORT_DURATION;
    static const QString COLUMN_NAME_SUPPORT_VALUE;

    /** BARDATA */
    static const QString SQL_SELECT_BARDATA_DESC;
    static const QString SQL_SELECT_BARDATA_MAX_ROWID;
    static const QString SQL_INSERT_BARDATA_V2;
    static const QString SQL_UPDATE_BARDATA_V2;
//    static const QString SQL_SELECT_BARDATA_ROWID;
//    static const QString SQL_VIEW1_BARDATA_V2; // for QTableView

    /** RESISTANCE & SUPPORT */
    static const QString SQL_INSERT_RESISTANCE_V1;
    static const QString SQL_INSERT_SUPPORT_V1;

    /** SQL STRING BUILDER */
    static QString SQL_SELECT_BARDATA_MAXDATETIME(QString whereArgs);
    static QString SQL_SELECT_BARDATA_MINDATETIME(QString whereArgs);
    static QString SQL_SELECT_BARDATA_LIMIT(const QDate &d, const QTime &t, const int &limit);
    static QString SQL_SELECT_BARDATA_BUILDER(const QStringList &projection, const QString &where_args);
    static QString SQL_SELECT_BARDATA_BUILDER_JOIN(const QStringList &projection,
                                                   const QStringList &databaseAlias,
                                                   const QString &where_args,
                                                   const QStringList &SRjoinAlias,
                                                   const QVector<bool> &SRjoin,
                                                   const QString &target_alias,
                                                   const bool &count_query);
    static QStringList SQL_UPDATE_BARDATA_PARENT_INDEX_V2(const QString &parent,const int &weight,const int &start_index);
    static QString SQL_SELECT_DATETIME(const QDate &d, const QTime &t);
    static QString SQL_SELECT_EXISTS(const QString &tableName, const QString &whereArgs);
    static QString SQL_LAST_FROM_ROWID(const int &start_from_rowid);
    static QString SQL_ROWCOUNT_WHERE(const QString &tableName, const QString &whereArgs);
//    static QString SQL_SELECT_COUNT_FROM_YEAR(int year);
//    static QString SQL_SELECT_DISTF_ASC_FROM_YEAR(const int &year);
//    static QString SQL_SELECT_DISTS_ASC_FROM_YEAR(const int &year);
//    static QString SQL_SELECT_RESISTANCE_BY_DATE(const int &index, const QDate &date);
//    static QString SQL_SELECT_SUPPORT_BY_DATE(const int &index, const QDate &date);
//    static QString SQL_SELECT_PID_BY_DATETIME(const QDate &date, const QTime &time);
//    static QString SQL_SELECT_DATETIME_BY_RID(const int &pid);

    SQLiteHandler(int id = 0, QString database_path = "") {
      if (database_path.isEmpty()) {
        database_path = application_path;
      }
      this->application_id = id;
      this->database_path = database_path;
    }

    ~SQLiteHandler() {
      QSqlDatabase *db;
      QString connection;
      QHash<QString,QSqlDatabase*>::const_iterator it = databasePool.constBegin();

      while (it != databasePool.constEnd()) {
        db = it.value();
        if (db != NULL) {
          connection = db->connectionName();
          db->close();
          delete db;
          db = NULL;
          QSqlDatabase::removeDatabase(connection);
          qDebug() << "rem:" << connection;
        }
        ++it;
      }

      databasePool.clear();
      removeTempDatabase();
    }

    void set_application_id(const int &id) {
      application_id = id;
    }

    void set_database_path(const QString &path) {
      database_path = path;
    }

    void execVacuum(const QString &databaseName) {
      QSqlQuery query(getDatabase(databaseName));
      query.exec("VACUUM;");
    }

    bool checkIntegrity(const QString &databaseName) {
      QSqlQuery query(getDatabase(databaseName));
      query.exec("PRAGMA integrity_check;");
      return (query.next() && query.value(0).toString() == "ok");
    }

    QSqlQuery* execQuery(const QString &databaseName, const QString &sql) {
      QSqlQuery *query = new QSqlQuery(getDatabase(databaseName));
      query->setForwardOnly(true);
      if (!query->exec(sql)) {
        qDebug() << query->lastError();
        return NULL;
      }
      return query;
    }

    bool execNonQuery(const QString &databaseName, const QString &sql) {
      QSqlQuery query(getDatabase(databaseName));
      if (!query.exec(sql)) {
        qDebug() << query.lastError();
        return false;
      }
      return true;
    }


    //
    // static IntervalWeight util
    //
    static IntervalWeight get_parent_interval(const IntervalWeight &weight) {
      IntervalWeight res;
      switch (weight) {
        case WEIGHT_MONTHLY: res = WEIGHT_INVALID; break;
        case WEIGHT_WEEKLY: res = WEIGHT_MONTHLY; break;
        case WEIGHT_DAILY: res = WEIGHT_WEEKLY; break;
        case WEIGHT_60MIN: res = WEIGHT_DAILY; break;
        case WEIGHT_5MIN: res = WEIGHT_60MIN; break;
        case WEIGHT_1MIN: res = WEIGHT_5MIN; break;
        default: res = WEIGHT_INVALID;
      }
      return res;
    }

    static IntervalWeight get_child_interval(const IntervalWeight &weight) {
      IntervalWeight res;
      switch (weight) {
        case WEIGHT_MONTHLY: res = WEIGHT_WEEKLY; break;
        case WEIGHT_WEEKLY: res = WEIGHT_DAILY; break;
        case WEIGHT_DAILY: res = WEIGHT_60MIN; break;
        case WEIGHT_60MIN: res = WEIGHT_5MIN; break;
        case WEIGHT_5MIN: res = WEIGHT_1MIN; break;
        case WEIGHT_1MIN: res = WEIGHT_INVALID; break;
        default: res = WEIGHT_INVALID;
      }
      return res;
    }

    static QString get_interval_name(const IntervalWeight &weight) {
      QString res;
      switch (weight) {
        case WEIGHT_MONTHLY: res = "Monthly"; break;
        case WEIGHT_WEEKLY: res = "Weekly"; break;
        case WEIGHT_DAILY: res = "Daily"; break;
        case WEIGHT_60MIN: res = "60min"; break;
        case WEIGHT_5MIN: res = "5min"; break;
        case WEIGHT_1MIN: res = "1min"; break;
        default: res = "";
      }
      return res;
    }

    static IntervalWeight get_interval_weight(const QString &interval_name) {
      QString _interval = interval_name.toLower();
      if (_interval == "monthly") return WEIGHT_MONTHLY;
      if (_interval == "weekly") return WEIGHT_WEEKLY;
      if (_interval == "daily") return WEIGHT_DAILY;
      if (_interval == "60min") return WEIGHT_60MIN;
      if (_interval == "5min") return WEIGHT_5MIN;
      if (_interval == "1min") return WEIGHT_1MIN;
      return WEIGHT_INVALID;
    }


    //
    // static Clone Database
    //
    static QSqlDatabase clone_database(const QSqlDatabase &other) {
      static int _id = 1;
      QString connection = other.connectionName() + QString::number(++_id);
      QSqlDatabase database = QSqlDatabase::cloneDatabase(other,connection);
      database.open();
      return database;
    }

    // TODO : will be removed in next version
    static void removeCloneDatabase(QSqlDatabase *database) {
      SQLiteHandler::removeDatabase(database);
    }

    static void removeDatabase(QSqlDatabase *database) {
      if (database != NULL) {
        QString connection = database->connectionName();
        database->close();
        *database = QSqlDatabase();
        QSqlDatabase::removeDatabase(connection);
//        qDebug() << "Assert removeDatabase:" << (!QSqlDatabase::contains(connection));
      }
    }


    static QString getDatabaseName(const QString &symbol, const IntervalWeight &interval) {
      return symbol + "_" + get_interval_name(interval) + ".db";
    }

    /*static QString getDatabaseName(const QString &symbol, const QString &interval) {
      return symbol + "_" + interval + ".db";
    }*/

    QString getDatabasePath(const QString &symbol, const IntervalWeight &interval) {
      return database_path + "/" + symbol + "_" + get_interval_name(interval) + ".db";
    }

    const QSqlDatabase getTempResultDatabase(const IntervalWeight &interval, const int &id) const {
      QString database_path = this->database_path + "/" + getTempResultDatabaseName(interval, id);
      QString connectionString = create_tempdb_connection_string(interval, id);

//      if (tempDatabasePool.contains(connectionString)) {
//        tempDatabasePool[connectionString]->close();
//        delete tempDatabasePool[connectionString];
//        tempDatabasePool.remove(connectionString);
//        QSqlDatabase::removeDatabase(connectionString);
//      }

//      QSqlDatabase *db = new QSqlDatabase();
//      *db = QSqlDatabase::addDatabase("QSQLITE", connectionString);
//      db->setDatabaseName(database_path);
//      db->open();

//      tempDatabasePool.insert(connectionString, db);
//      return *db;

      if (QSqlDatabase::contains(connectionString)) {
        QSqlDatabase::removeDatabase(connectionString);
      }

      if (!tempdatabase_connection.contains(connectionString)) {
        tempdatabase_connection.push_back( connectionString );
      }

      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionString);
      db.setDatabaseName(database_path);
      db.open();
      return db;
    }

    const QString getTempResultDatabaseName(const IntervalWeight &interval, const int &id) const {
      QString appid = QString::number(application_id) + QString::number(id);
      return tempDatabaseName + appid + "." + get_interval_name(interval);
    }


    //
    // Internal Clone Database
    //
    /*QSqlDatabase getCloneDatabase(const QString &symbol, const QString &interval) {
      return getCloneDatabase(symbol + "_" + interval + ".db");
    }*/

    QSqlDatabase getCloneDatabase(const QString &symbol, const IntervalWeight &interval) {
      return getCloneDatabase(symbol + "_" + get_interval_name(interval) + ".db");
    }

    QSqlDatabase getCloneDatabase(const QString &databaseName) {
      QSqlDatabase other = getDatabase(databaseName);
      QString appid = QString::number(application_id) + QString::number(++clone_id);
      QString connection = other.connectionName() + appid + QTime::currentTime().toString("zzz");
      QSqlDatabase database = QSqlDatabase::cloneDatabase(other,connection);
      database.open();
      return database;
    }


    //
    // GetDatabase
    //
    const QSqlDatabase& getDatabase(const QString &symbol, const QString &interval) {
      return getDatabase(symbol + "_" + interval + ".db");
    }

    const QSqlDatabase& getDatabase(const QString &databaseName) {
      if (!databasePool.contains(databaseName) || databasePool[databaseName]->isOpenError()) {
        // Create new connection name
        QString connection = connectionName + databaseName + QString::number(application_id);
        QString database_path = this->database_path + "/" + databaseName;

        // Create new database object
        QSqlDatabase *db = new QSqlDatabase();
        *db = QSqlDatabase::addDatabase("QSQLITE", connection);
        db->setDatabaseName(database_path);
        db->open();

        // Create table & index
        QSqlQuery query(*db);
        QString page_size;
        if (databaseName.contains("Monthly")) {
          page_size = "4096";
        } else if (databaseName.contains("Weekly") || databaseName.contains("Daily")) {
          page_size = "16384";
        } else {
          page_size = "65536";
        }
        query.exec("PRAGMA journal_mode = OFF;");
        query.exec("PRAGMA page_size = " + page_size + ";");
        query.exec("PRAGMA cache_size = 10000;");
        query.exec(SQL_CREATE_TABLE_BARDATA_V2);
        query.exec(SQL_CREATE_TABLE_RESISTANCE_V1);
        query.exec(SQL_CREATE_TABLE_SUPPORT_V1);
        query.exec(SQL_CREATE_TABLE_RESISTANCE_DATE_V1);
        query.exec(SQL_CREATE_TABLE_SUPPORT_DATE_V1);

        // Create index
        query.exec(SQL_CREATE_INDEX_BARDATA_PARENT);
        query.exec(SQL_CREATE_INDEX_BARDATA_PARENT_PREV);
        query.exec(SQL_CREATE_INDEX_BARDATA_PARENT_MONTHLY);
        query.exec(SQL_CREATE_INDEX_BARDATA_PREVDATETIME);

        // alter script
//        initialize_histogram(*db);
//        query.exec("alter table bardata add column DistFS REAL;");
//        query.exec("alter table bardata add column DistFS_rank REAL;");

        // Store opened database instance
        databasePool.insert(databaseName, db);
        return *db;
      }

      // Always try reopen database
      QSqlDatabase *db = databasePool.value(databaseName);
      if (!db->isOpen()) db->open();
      return *db;
    }

    void removeDatabase(const QString &databaseName) {
      if (databasePool.contains(databaseName)) {
        QSqlDatabase *db = databasePool.take(databaseName);
        if (db != NULL) {
          QSqlDatabase *temp = db;
          QString connection = db->connectionName();
          db->close();
          *db = QSqlDatabase();
          delete temp;
          QSqlDatabase::removeDatabase(connection);
        }
      }
    }



    //
    // Temporary Result helper
    //
    /*QSqlQuery* getSqlQuery_v2(const QSqlDatabase &main_database,
                              const IntervalWeight &base_interval,
                              const IntervalWeight &projection_interval,
                              const int &id_threshold,
                              const double &threshold,
                              const QStringList &databaseName,
                              const QStringList &databaseAlias,
                              const QString &sql_select,
                              const int &prevbar_count,
                              const int &result_id) {
      QSqlQuery *query = new QSqlQuery(main_database);
      query->setForwardOnly(true);
      query->exec("PRAGMA cache_size = 50000;");
      query->exec("PRAGMA page_size = 65536;");

      for (int i = 1; i < databaseName.length(); ++i) {
        query->exec("ATTACH DATABASE '" + databaseName[i] + "' AS " + databaseAlias[i]);
      }

//      int column_count = projectionNames.length();
//      QStringList reprojection = projectionNames;
      QStringList intersectProjection;
      QString tempDatabasePath = database_path + "/" + SQLiteHandler::getTempResultDatabaseName(projection_interval, result_id);
      QString tempDatabaseAlias = "tempResult" + get_interval_name(projection_interval);
      QString tempTableName = "BardataView";
      QString sql_attach_temp = "ATTACH DATABASE '" + tempDatabasePath + "' AS " + tempDatabaseAlias + ";";
      QString sql_create_view = "CREATE TABLE IF NOT EXISTS " + tempDatabaseAlias + "." + tempTableName + "(";

//      for (int i = 0; i < column_count; ++i) {
//        if (i > 0) sql_create_view += ",";
//        sql_create_view += "'" + projectionNames[i] + "' TEXT";
//        if (projectionNames[i].startsWith("Min_") || projectionNames[i].startsWith("Max_")) {
//          intersectProjection.append(projectionNames[i]);
//        }
//      }

      sql_create_view += ",PRIMARY KEY(date_ ASC, time_ ASC));";

      query->exec(sql_attach_temp);
      query->exec("DROP TABLE IF EXISTS " + tempDatabaseAlias + "." + tempTableName + ";");
      query->exec(sql_create_view);
      if (query->lastError().isValid()) qDebug() << query->lastError();

      query->exec("PRAGMA journal_mode = OFF;");
      query->exec("PRAGMA synchronous = OFF;");
      query->exec("BEGIN TRANSACTION;");
      query->exec("INSERT INTO " + tempDatabaseAlias + "." + tempTableName + " " + sql_select);
      if (query->lastError().isValid()) qDebug() << query->lastError();
      query->exec("COMMIT;");
      query->exec("ALTER TABLE " + tempDatabaseAlias + "." + tempTableName + " add column delete_flag INTEGER default 0;");

      //
      // Bypass checking if zero
      //
      if (prevbar_count > 0) {
        if (projection_interval == base_interval) {
          support_resistance_check_previous_bar_v2(query, intersectProjection, base_interval, prevbar_count);
        } else {
          QString baseDatabasePath = database_path + "/" + SQLiteHandler::getTempResultDatabaseName(base_interval, result_id);
          QString baseDatabaseAlias = "baseResult" + get_interval_name(base_interval);
          query->exec("ATTACH DATABASE '" + baseDatabasePath + "' AS " + baseDatabaseAlias + ";");
          query->exec("PRAGMA journal_mode = OFF;");
          query->exec("PRAGMA synchronous = OFF;");
          query->exec("BEGIN TRANSACTION;");
          query->exec("update " + tempDatabaseAlias + "." + tempTableName +
                      " set delete_flag=1 where rowid in(select rowid from "+ baseDatabaseAlias + ".bardataview where delete_flag=1)");
          query->exec("COMMIT;");
          query->exec("DETACH DATABASE " + baseDatabaseAlias);
        }
      }


//      for (int i = 0; i < column_count; ++i) {
//        reprojection[i] = "a.'" + reprojection[i] + "'";
//      }

//      extend_view_resistance_support(query, &reprojection, id_threshold, threshold, base_interval, projection_interval, projection_interval);

//      for (int i = 0; i < intersectProjection.length(); ++i) {
//        reprojection.removeAt(reprojection.indexOf("a.'" + intersectProjection[i] + "'"));
//      }

      query->clear();
      delete query;

//      query = new QSqlQuery(getTempResultDatabase(projection_interval, result_id));
//      query->setForwardOnly(forwardOnly);
//      query->exec("select " + reprojection.join(",") + " from " + tempTableName + " a where delete_flag=0 order by a.date_ asc, a.time_ asc");

      if (query->lastError().isValid()) {
        qDebug() << "getSqlQuery v2" << query->lastError();
        delete query;
        return NULL;
      }

      return query;
    }*/

    QSqlQuery* getSqlQuery(const QSqlDatabase &main_database,
                           const IntervalWeight &base_weight,
                           const IntervalWeight &projection_weight,
                           const int &id_threshold,
                           const double &threshold,
                           const QStringList &databaseName,
                           const QStringList &databaseAlias,
                           const QString &sql_select,
                           const QStringList &projectionNames,
                           const bool &forwardOnly,
                           const int &prevbar_count,
                           const int &result_id) {
      QSqlQuery *query = NULL;
      QSqlQuery query_1(main_database);
      query_1.setForwardOnly(true);

      for (int i = 1; i < databaseName.length(); ++i) {
        query_1.exec("ATTACH DATABASE '" + databaseName[i] + "' AS " + databaseAlias[i]);
      }

      query_1.exec("PRAGMA journal_mode = OFF;");
      query_1.exec("PRAGMA synchronous = OFF;");
      query_1.exec("PRAGMA cache_size = 50000;");
      query_1.exec("PRAGMA page_size = 65536;");

      if (projectionNames.length() > 0) {
        int column_count = projectionNames.length();
        QStringList intersectProjection;
        QString tempDatabasePath = database_path + "/" + SQLiteHandler::getTempResultDatabaseName(projection_weight, result_id);
        QString tempDatabaseAlias = "tempResult" + get_interval_name(projection_weight);
        QString tempTableName = "BardataView";
        QString sql_attach_temp = "ATTACH DATABASE '" + tempDatabasePath + "' AS " + tempDatabaseAlias + ";";
        QString sql_create_view = "CREATE TABLE IF NOT EXISTS " + tempDatabaseAlias + "." + tempTableName + "(";

        for (int i = 0; i < column_count; ++i) {
          if (i > 0) sql_create_view += ",";
          sql_create_view += "'" + projectionNames[i] + "' TEXT";
          if (projectionNames[i].startsWith("Min_") ||
              projectionNames[i].startsWith("Max_")) {
            intersectProjection.append(projectionNames[i]);
          }
        }

        sql_create_view += ",PRIMARY KEY(date_ ASC, time_ ASC));";

        query_1.exec(sql_attach_temp);
        query_1.exec("DROP TABLE if exists " + tempDatabaseAlias + "." + tempTableName + ";");
        query_1.exec(sql_create_view);
        if (query_1.lastError().isValid()) qDebug() << query_1.lastError();
        query_1.exec("BEGIN TRANSACTION;");
        query_1.exec("INSERT INTO " + tempDatabaseAlias + "." + tempTableName + " " + sql_select);
        if (query_1.lastError().isValid()) qDebug() << query_1.lastError();

        query_1.exec("COMMIT;");
        query_1.exec("alter table " + tempDatabaseAlias + "." + tempTableName + " add column delete_flag INTEGER default 0;");

        //
        // Bypass intersect deletion if zero
        //
        if (prevbar_count > 0) {
          if (projection_weight == base_weight) {
            support_resistance_check_previous_bar_v2(&query_1, intersectProjection, base_weight, prevbar_count);
          } else {
            QString baseDatabasePath = database_path + "/" + SQLiteHandler::getTempResultDatabaseName(base_weight, result_id);
            QString baseDatabaseAlias = "baseResult" + get_interval_name(base_weight);
            query_1.exec("ATTACH DATABASE '" + baseDatabasePath + "' AS " + baseDatabaseAlias + ";");
            query_1.exec("PRAGMA journal_mode = OFF;");
            query_1.exec("PRAGMA synchronous = OFF;");
            query_1.exec("BEGIN TRANSACTION;");
            query_1.exec("update " + tempDatabaseAlias + "." + tempTableName +
                        " set delete_flag=1 where rowid in(select rowid from "+ baseDatabaseAlias + ".bardataview where delete_flag=1)");
            query_1.exec("COMMIT;");
            query_1.exec("DETACH DATABASE " + baseDatabaseAlias);
          }
        }

        QStringList reprojection = projectionNames;
        reprojection[1] = "strftime('%m/%d/%Y',date_) as Date";
        reprojection[2] = "time_ as Time";
//        reprojection[3] = "open_ as Open";
//        reprojection[4] = "high_ as High";
//        reprojection[5] = "low_ as Low";
//        reprojection[6] = "close_ as Close";
        for (int i = 3; i < column_count; ++i) {
          reprojection[i] = "a.'" + reprojection[i] + "'";
        }

        // v0.8.12
//        IntervalWeight parent_weight = WEIGHT_LARGEST;
//        while (parent_weight >= projection_weight) {
//          qDebug() << base_weight << projection_weight << parent_weight ;
//          extend_view_resistance_support(query, &reprojection, id_threshold, threshold, base_weight, parent_weight, projection_weight);
//          parent_weight = get_child_interval(parent_weight);
//        }

        // v0.8.11
//        while (parent_weight != base_weight) {
//          qDebug() << base_weight << projection_weight << parent_weight ;
//          extend_view_resistance_support(query, &reprojection, id_threshold, threshold, base_weight, parent_weight, projection_weight);
//          parent_weight = get_child_interval(parent_weight);
//        }
//        qDebug() << base_weight << projection_weight << parent_weight;
//        extend_view_resistance_support(query, &reprojection, id_threshold, threshold, base_weight, parent_weight, projection_weight);

        if (projection_weight >= WEIGHT_DAILY) {
          extend_view_resistance_support(&query_1, &reprojection, id_threshold, threshold, base_weight, projection_weight, projection_weight);
        }

        // BardataView (remove unnecessary columns)
        reprojection.removeAt(reprojection.indexOf("rowid"));
        reprojection.removeAt(reprojection.indexOf("a.'open_'"));
        reprojection.removeAt(reprojection.indexOf("a.'high_'"));
        reprojection.removeAt(reprojection.indexOf("a.'low_'"));
        reprojection.removeAt(reprojection.indexOf("a.'close_'"));
        reprojection.removeAt(reprojection.indexOf("a.'_parent'"));
        reprojection.removeAt(reprojection.indexOf("a.'_parent_monthly'"));
        reprojection.removeAt(reprojection.indexOf("a.'_parent_prev'"));
        reprojection.removeAt(reprojection.indexOf("a.'_parent_prev_monthly'"));
        reprojection.removeAt(reprojection.indexOf("a.'PrevDate'"));
        reprojection.removeAt(reprojection.indexOf("a.'PrevTime'"));
        reprojection.removeAt(reprojection.indexOf("a.'Rowid60min'"));
        reprojection.removeAt(reprojection.indexOf("a.'RowidDaily'"));
        reprojection.removeAt(reprojection.indexOf("a.'RowidWeekly'"));
        reprojection.removeAt(reprojection.indexOf("a.'RowidMonthly'"));

        for (int i = 0; i < intersectProjection.length(); ++i) {
          reprojection.removeAt(reprojection.indexOf("a.'" + intersectProjection[i] + "'"));
        }

        for (int i = 1; i < databaseName.length(); ++i) {
          query_1.exec("DETACH DATABASE " + databaseAlias[i]);
        }
        query_1.exec("DETACH DATABASE " + tempDatabaseAlias);
        query_1.clear();

        QSqlDatabase tempdb = getTempResultDatabase(projection_weight, result_id);
        query = new QSqlQuery(tempdb);
        query->setForwardOnly(forwardOnly);
        query->exec(sql_attach_temp);
        query->exec("select " + reprojection.join(",") + " from " + tempTableName + " a "
                    " where delete_flag=0 order by a.date_ asc, a.time_ asc");
      }
      else {
        QString new_sql = sql_select;
        query = new QSqlQuery(main_database);
        query->exec(new_sql);
      }

      if (query != NULL && query->lastError().isValid()) {
        qDebug() << "getSqlQuery" << query->lastError();
        delete query;
        return NULL;
      }

      return query;
    }

    // w/o realtime deletion
    void support_resistance_check_previous_bar(QSqlQuery *query,
                                               const QStringList &intersectProjection,
                                               const IntervalWeight &base_interval,
                                               const int &PrevBarCount) {
      QString sql_update;
      QString base_interval_name = get_interval_name(base_interval);
      QString database_alias;
      QString interval_name;
      QString column_name;
      QString rowid_column;
      QString having_clause;
      QStringList split_str;
      int N = intersectProjection.length();

      query->exec("BEGIN TRANSACTION;");
      for (int i = 0; i < N; ++i) {
        split_str = intersectProjection[i].split("_");
        interval_name = split_str[1];
        column_name = split_str[2];

        if (base_interval_name != interval_name) {
          database_alias = "db" + interval_name + ".";
          rowid_column = "Rowid" + interval_name;
        } else {
          database_alias = "";
          rowid_column = "Rowid";
        }

        if (column_name == "Resistance") {
          having_clause = "HAVING A."+intersectProjection[i]+"<=MAX(T."+intersectProjection[i]+"))";
        } else {
          having_clause = "HAVING A."+intersectProjection[i]+">=MIN(T."+intersectProjection[i]+"))";
        }

        sql_update =
          "UPDATE bardataview SET delete_flag=1 WHERE rowid IN("
          "SELECT A.rowid FROM bardataview A"
          " JOIN bardataview T ON T.rowid>=(A.rowid-" + QString::number(PrevBarCount) + ") AND T.rowid<A.rowid "
          " GROUP BY A.rowid " + having_clause;

        qDebug() << sql_update;
        query->exec(sql_update);
        if (query->lastError().isValid()) qDebug() << query->lastError();
      }
      query->exec("COMMIT;");
    }

    // with realtime deletion
    void support_resistance_check_previous_bar_v2(QSqlQuery *query,
                                                  const QStringList &intersectProjection,
                                                  const IntervalWeight &base_interval,
                                                  const int &PrevBarCount) const {
      QString sql_update;
      QString sql_select;
      QString base_interval_name = get_interval_name(base_interval);
      QString database_alias;
      QString interval_name;
      QString column_name;
      QString rowid_column;
      QString having_clause;
      QStringList split_str;
      int N = intersectProjection.length();

      for (int i = 0; i < N; ++i) {
        split_str = intersectProjection[i].split("_");
        interval_name = split_str[1];
        column_name = split_str[2];

        if (base_interval_name != interval_name) {
          database_alias = "db" + interval_name + ".";
          rowid_column = "Rowid" + interval_name;
        } else {
          database_alias = "";
          rowid_column = "Rowid";
        }

        if (column_name == "Resistance") {
          having_clause = "HAVING A."+intersectProjection[i]+"<=MAX(T."+intersectProjection[i]+")";
        } else {
          having_clause = "HAVING A."+intersectProjection[i]+">=MIN(T."+intersectProjection[i]+")";
        }

        // Candidate of deletion
        sql_select =
          "SELECT A.rowid FROM bardataview A"
          " JOIN bardataview T ON T.rowid>=(A.rowid-" + QString::number(PrevBarCount) + ") AND T.rowid<A.rowid"
          " GROUP BY A.rowid " + having_clause;

        query->exec(sql_select);
        if (query->lastError().isValid()) query->lastError();

        QVariantList rowid;
        while (query->next()) {
          rowid.push_back(query->value(0).toString());
        }

//        qDebug() << "#### rowid count:" << rowid.size();

        query->exec("PRAGMA journal_mode = OFF;");
        query->exec("PRAGMA synchronous = OFF;");
        query->exec("PRAGMA cache_size = 50000;");
        query->exec("PRAGMA page_size = 65536;");
        query->exec("BEGIN TRANSACTION;");

//        sql_update =
//          "UPDATE bardataview SET delete_flag=1 WHERE rowid IN("
//          "SELECT A.rowid FROM bardataview A"
//          " JOIN bardataview T ON T.rowid>=(A.rowid-" + QString::number(PrevBarCount) + ") AND T.rowid<A.rowid"
//          " WHERE A.rowid=? AND T.delete_flag=0"
//          " GROUP BY A.rowid " + having_clause + ")";

//        query->prepare(sql_update);
//        query->addBindValue(rowid);
//        query->execBatch();

        for (int j = 0; j < rowid.size(); ++j) {
          sql_update =
            "UPDATE bardataview SET delete_flag=1 WHERE rowid IN("
            "SELECT A.rowid FROM bardataview A"
            " JOIN bardataview T ON T.rowid>=(A.rowid-" + QString::number(PrevBarCount) + ") AND T.rowid<A.rowid"
            " WHERE A.rowid=" + rowid[j].toString() + " AND T.delete_flag=0"
            " GROUP BY A.rowid " + having_clause + ")";
//          qDebug() << sql_update;
          query->exec(sql_update);
//          if (query->lastError().isValid()) qDebug() << query->lastError();
        }

        query->exec("COMMIT;");
      }
    }

    void extend_view_resistance_support(QSqlQuery *query,
                                        QStringList *projection,
                                        const int &id_threshold,
                                        const double &threshold,
                                        const IntervalWeight &base_weight,
                                        const IntervalWeight &parent_weight,
                                        const IntervalWeight &projection_weight) const {
      QStringList res_date, res_time, res_count, res_duration, res_value;
      QStringList sup_date, sup_time, sup_count, sup_duration, sup_value;
      QStringList cc;
      QStringList con;
      QStringList dur;
      QString sql_alter_table;
      QString sql_count_maxcolumn;
      QString sql_select_detail;
      QString sql_update;
      QString set_columns;
      QString interval = get_interval_name(parent_weight);
      QString resistance_table = TABLE_NAME_RESISTANCE;
      QString support_table = TABLE_NAME_SUPPORT;
      QString column_name;
      QString child_alias = "a";
      QString parent_database = "";
      QString left_join_clause = "";
      QString group_by_clause = "";
//      QString r = (base_weight < WEIGHT_DAILY)? "0" : QString::number(threshold);
      QString r = QString::number(threshold);
      QString tempDatabaseAlias = "tempResult" + get_interval_name(projection_weight);
      QString tempTableName = "BardataView";
      int max_column;

//      if (projection_weight < parent_weight) {
//        IntervalWeight w = projection_weight;
      if (base_weight < parent_weight) {
        IntervalWeight w = base_weight;
        QString parent_bardata;
        QString parent_rowid;
        QString child_idparent;

        while (w != parent_weight && w != WEIGHT_INVALID) {
          w = get_parent_interval(w);
          parent_database = "db" + get_interval_name(w);
          parent_bardata = parent_database + "." + TABLE_NAME_BARDATA;
          parent_rowid = parent_bardata + "." + COLUMN_NAME_ROWID;
          if (w == WEIGHT_MONTHLY) {
            child_idparent = "a." + COLUMN_NAME_IDPARENT_PREV_MONTHLY;
          } else {
            child_idparent = child_alias + "." + COLUMN_NAME_IDPARENT_PREV;
          }
          left_join_clause += " left join " + parent_bardata + " on " + parent_rowid + "=" + child_idparent;
          if (!group_by_clause.isEmpty()) group_by_clause += ",";
          group_by_clause += child_alias + ".date_," + child_alias + ".time_";
          child_alias = parent_bardata;
        }

        if (!parent_database.isEmpty())
          parent_database += ".";
      }
      else if (projection_weight > base_weight) {
        parent_database = "db" + get_interval_name(projection_weight) + ".";
      }
//      else if (projection_weight > base_weight && projection_weight == parent_weight) {
//        parent_database = "db" + get_interval_name(projection_weight) + ".";
//      }

      if (!group_by_clause.isEmpty()) group_by_clause += ",";
      group_by_clause += child_alias + ".date_," + child_alias + ".time_";

      query->exec("PRAGMA journal_mode = OFF;");
      query->exec("PRAGMA synchronous = OFF;");
      query->exec("BEGIN TRANSACTION;");

      //
      // ALTER RESISTANCE COLUMN
      //
//      sql_count_maxcolumn =
//        "select count(1) as C from temp.bardataview a" + left_join_clause +
//        " join " + parent_database + resistance_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
//        " where id_threshold=" + QString::number(id_threshold) + " and "
//        " (b.resistance-"  + r + ")<=CAST(a.high_ as Real) and (b.resistance+" + r + ")>=CAST(a.open_ as Real) and " +
//        " (b.resistance+" + r + ")>=CAST(a.close_ as Real)" +
//        " group by " + group_by_clause +
//        " order by C desc limit 1";

      sql_count_maxcolumn =
        "select max(cast(" + tempTableName + ".'Resistance (#)' as int)) from " + tempDatabaseAlias + "." + tempTableName;

      // XX-- 2015-06-17
//      if (!parent_database.isEmpty()) {
//        sql_count_maxcolumn =
//          "select count(1) as C from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
//          " join " + parent_database + resistance_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
//          " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
//           "(b.resistance-" + r + ")<=CAST(a.high_ as Real) and " +
//          "((b.resistance+" + r + ")>=CAST(a.open_ as Real) or " +
//           "(b.resistance+" + r + ")>=CAST(a.close_ as Real))" +
//          " group by " + group_by_clause +
//          " order by C desc limit 1";
//      } else {
//        sql_count_maxcolumn =
//          "select count(1) as C from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
//          " join " + parent_database + resistance_table + " b on b.date_=" + child_alias + ".PrevDate and b.time_=" + child_alias + ".PrevTime" +
//          " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and "
//           "(b.resistance-" + r + ")<=CAST(a.high_ as Real) and " +
//          "((b.resistance+" + r + ")>=CAST(a.open_ as Real) or " +
//           "(b.resistance+" + r + ")>=CAST(a.close_ as Real))" +
//          " group by " + group_by_clause +
//          " order by C desc limit 1";
//      }

      qDebug() << "\nMaxColumn Resistance" << sql_count_maxcolumn;
      query->exec(sql_count_maxcolumn);
      if (query->lastError().isValid()) qDebug() << query->lastError();
      max_column = query->next()? query->value(0).toInt() : 0;
//      qDebug() << "max column resistance" << max_column;

      for (int i = 0; i < max_column; ++i) {
        column_name = interval + "Resistance" + QString::number(i);
        projection->append(column_name + " as '" + interval + " Resistance'");
        sql_alter_table = "alter table " + tempDatabaseAlias + "." + tempTableName + " add column '" + column_name + "' TEXT;";
        query->exec(sql_alter_table);
      }

      //
      // ALTER SUPPORT COLUMN
      //
//      sql_count_maxcolumn =
//        "select count(1) as C from temp.bardataview a " + left_join_clause +
//        " join " + parent_database + support_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
//        " where id_threshold=" + QString::number(id_threshold) + " and "
//        " (b.support+" + r + ")>=CAST(a.low_ as Real) and (b.support-" + r + ")<=CAST(a.open_ as Real) and"+
//        " (b.support-" + r + ")<=CAST(a.close_ as Real)" +
//        " group by " + group_by_clause +
//        " order by C desc limit 1";

      sql_count_maxcolumn =
        "select max(cast(" + tempTableName + ".'Support (#)' as int)) from " + tempDatabaseAlias + "." + tempTableName;

      // XX-- 2015-06-17
//      if (!parent_database.isEmpty()) {
//        sql_count_maxcolumn =
//          "select count(1) as C from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
//          " join " + parent_database + support_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
//          " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
//           "(b.support+" + r + ")>=CAST(a.low_ as Real) and " +
//          "((b.support-" + r + ")<=CAST(a.open_ as Real) or " +
//           "(b.support-" + r + ")<=CAST(a.close_ as Real))" +
//          " group by " + group_by_clause +
//          " order by C desc limit 1";
//      } else {
//        sql_count_maxcolumn =
//          "select count(1) as C from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
//          " join " + parent_database + support_table + " b on b.date_=" + child_alias + ".PrevDate and b.time_=" + child_alias + ".PrevTime" +
//          " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
//           "(b.support+" + r + ")>=CAST(a.low_ as Real) and " +
//          "((b.support-" + r + ")<=CAST(a.open_ as Real) or " +
//           "(b.support-" + r + ")<=CAST(a.close_ as Real))" +
//          " group by " + group_by_clause +
//          " order by C desc limit 1";
//      }

      query->exec(sql_count_maxcolumn);
//      if (query->lastError().isValid()) qDebug() << query->lastError();
      max_column = query->next()? query->value(0).toInt() : 0;
//      qDebug() << "max column support" << max_column;

      for (int i = 0; i < max_column; ++i) {
        column_name = interval + "Support" + QString::number(i);
        projection->append(column_name + " as '" + interval + " Support'");
        sql_alter_table = "alter table " + tempDatabaseAlias + "." + tempTableName + " add column '" + column_name + "' TEXT;";
        query->exec(sql_alter_table);
      }

      query->exec("COMMIT;");

      // Insert resistance detail
//      sql_select_detail =
//        "select date_,time_,group_concat(resistance),group_concat(resistance_count),group_concat(resistance_duration) from"
//        "(select a.date_,a.time_,resistance,resistance_count,resistance_duration from temp.bardataview a " + left_join_clause +
//        " join " + parent_database + resistance_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
//        " where id_threshold=" + QString::number(id_threshold) + " and "
//        " (b.resistance-" + r + ")<=CAST(a.high_ as Real) and (b.resistance+" + r + ")>=CAST(a.open_ as Real) and " +
//        " (b.resistance+" + r + ")>=CAST(a.close_ as Real)" +
//        " order by resistance desc)"
//        " group by date_,time_";

      if (parent_database != "") {
        sql_select_detail =
            "select date_,time_,group_concat(resistance),group_concat(resistance_count),group_concat(resistance_duration) from"
            "(select a.date_,a.time_,resistance,resistance_count,resistance_duration from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
            " join " + parent_database + resistance_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
            " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
             "(b.resistance-" + r + ")<=CAST(a.high_ as Real) and " +
            "((b.resistance+" + r + ")>=CAST(a.open_ as Real) or " +
             "(b.resistance+" + r + ")>=CAST(a.close_ as Real))" +
            " order by resistance desc)"
            " group by date_,time_";
      } else {
        sql_select_detail =
            "select date_,time_,group_concat(resistance),group_concat(resistance_count),group_concat(resistance_duration) from"
            "(select a.date_,a.time_,resistance,resistance_count,resistance_duration from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
            " join " + parent_database + resistance_table + " b on b.date_=" + child_alias + ".PrevDate and b.time_=" + child_alias + ".PrevTime" +
            " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
             "(b.resistance-" + r + ")<=CAST(a.high_ as Real) and " +
            "((b.resistance+" + r + ")>=CAST(a.open_ as Real) or " +
             "(b.resistance+" + r + ")>=CAST(a.close_ as Real))" +
            " order by resistance desc)"
            " group by date_,time_";
      }

      qDebug() << "\n" << sql_select_detail;
      query->exec(sql_select_detail);
      if (query->lastError().isValid()) qDebug() << query->lastError();
      while (query->next()) {
        res_date.append(query->value(0).toString());
        res_time.append(query->value(1).toString());
        res_value.append(query->value(2).toString());
        res_count.append(query->value(3).toString());
        res_duration.append(query->value(4).toString());
      }

      // Insert support detail
//      sql_select_detail =
//        "select date_,time_,group_concat(support),group_concat(support_count),group_concat(support_duration) from"
//        "(select a.date_,a.time_,support,support_count,support_duration from temp.bardataview a " + left_join_clause +
//        " join " + parent_database + support_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
//        " where id_threshold=" + QString::number(id_threshold) + " and "
//        " (b.support+" + r + ")>=CAST(a.low_ as Real) and (b.support-" + r + ")<=CAST(a.open_ as Real) and"+
//        " (b.support-" + r + ")<=CAST(a.close_ as Real)" +
//        " order by support desc)"
//        " group by date_,time_";

      if (parent_database != "") {
        sql_select_detail =
          "select date_,time_,group_concat(support),group_concat(support_count),group_concat(support_duration) from"
          "(select a.rowid, a.date_,a.time_,support,support_count,support_duration from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
          " join " + parent_database + support_table + " b on b.date_=" + child_alias + ".date_ and b.time_=" + child_alias + ".time_" +
          " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
           "(b.support+" + r + ")>=CAST(a.low_ as Real) and "
          "((b.support-" + r + ")<=CAST(a.open_ as Real) or " +
           "(b.support-" + r + ")<=CAST(a.close_ as Real))" +
          " order by support desc)"
          " group by date_,time_";
      } else {
        sql_select_detail =
          "select date_,time_,group_concat(support),group_concat(support_count),group_concat(support_duration) from"
          "(select a.rowid, a.date_,a.time_,support,support_count,support_duration from " + tempDatabaseAlias + "." + tempTableName + " a " + left_join_clause +
          " join " + parent_database + support_table + " b on b.date_=" + child_alias + ".PrevDate and b.time_=" + child_alias + ".PrevTime" +
          " where delete_flag=0 and id_threshold=" + QString::number(id_threshold) + " and " +
           "(b.support+" + r + ")>=CAST(a.low_ as Real) and " +
          "((b.support-" + r + ")<=CAST(a.open_ as Real) or " +
           "(b.support-" + r + ")<=CAST(a.close_ as Real))" +
          " order by support desc)"
          " group by date_,time_";
      }

      query->exec(sql_select_detail);
//      if (query->lastError().isValid()) qDebug() << query->lastError();
      while (query->next()) {
        sup_date.append(query->value(0).toString());
        sup_time.append(query->value(1).toString());
        sup_value.append(query->value(2).toString());
        sup_count.append(query->value(3).toString());
        sup_duration.append(query->value(4).toString());
      }

      // update temp table
      query->exec("PRAGMA journal_mode = OFF;");
      query->exec("PRAGMA synchronous = OFF;");
      query->exec("BEGIN TRANSACTION;");
      int N = res_date.length();
      for (int i = 0; i < N; ++i) {
        cc = res_value[i].split(",");
        con = res_count[i].split(",");
        dur = res_duration[i].split(",");
        set_columns = "";
        max_column = cc.length();
        for (int j = 0; j < max_column; ++j) {
          if (j > 0) set_columns += ",";
          set_columns += interval + "Resistance" + QString::number(j) + "='" + cc[j] + " (" + con[j] + "," + dur[j] + ")'";
        }
        sql_update =
          "update " + tempDatabaseAlias + "." + tempTableName + " set "+ set_columns +
          " where date_='" + res_date[i] + "' and time_='" + res_time[i] + "'";
        query->exec(sql_update);
      }

      N = sup_date.length();
      for (int i = 0; i < N; ++i) {
        cc = sup_value[i].split(",");
        con = sup_count[i].split(",");
        dur = sup_duration[i].split(",");
        set_columns = "";
        max_column = cc.length();
        for (int j = 0; j < max_column; ++j) {
          if (j > 0) set_columns += ",";
          set_columns += interval + "Support" + QString::number(j) + "='" + cc[j] +" (" + con[j]+"," + dur[j] + ")'";
        }
        sql_update =
          "update " + tempDatabaseAlias + "." + tempTableName + " set "+ set_columns +
          " where date_='" + sup_date[i] + "' and time_='" + sup_time[i] + "'";
        query->exec(sql_update);
      }
      query->exec("COMMIT;");
    }

    /*QSqlQueryModel* getSqlModel(const QString &databaseName, const QString &sql) {
      QSqlQueryModel *model = new QSqlQueryModel();
      model->setQuery(sql, getDatabase(databaseName));
      if (model->lastError().isValid()) {
        qDebug() << "get_sqlmodel" << model->lastError();
        delete model;
        return NULL;
      }

//      int counter = 0;
//      while (model->canFetchMore() && ++counter <= 500) {
//        model->fetchMore();
//      }
      return model;
    }*/

    /*QSqlQueryModel* getSqlModel(const QSqlQuery &query) {
      QSqlQueryModel *model = new QSqlQueryModel();
      model->setQuery(query);
      if (model->lastError().isValid()) {
        qDebug() << "getSqlModel" << model->lastError();
        delete model;
        return NULL;
      }
      return model;
    }*/

    int selectRowCount(const QString &databaseName, const QString &whereArgs) {
      QSqlDatabase db = getDatabase(databaseName);
      QSqlQuery query(db);
      QString sql = SQLiteHandler::SQL_ROWCOUNT_WHERE(SQLiteHandler::TABLE_NAME_BARDATA, whereArgs);
      query.setForwardOnly(true);
      if (query.exec(sql) && query.next()) {
        return query.value(0).toInt();
      }
      return -1;
    }

    QStringList getColumnNames(const QString &databaseName, const QString &tableName) {
      QSqlRecord rec = getDatabase(databaseName).record(tableName);
      QStringList result;
      int N = rec.count();
      for (int i = 0; i < N; ++i) {
        result.append(rec.fieldName(i));
      }
      return result;
    }

    QSqlQuery* SRLoadDB(const QDate &d, const QTime &t, const QString &symbol, const QString &interval) {
      QSqlDatabase db = getDatabase(symbol,interval);
      QSqlQuery *query = new QSqlQuery(db);
      QString sql = SQL_SELECT_DATETIME(d,t);
      query->setForwardOnly(true);
      query->exec(sql);
      return query;
    }


  private:
    // CREATE TABLE
    static const QString SQL_CREATE_TABLE_BARDATA_V2;
    static const QString SQL_CREATE_TABLE_RESISTANCE_V1;
    static const QString SQL_CREATE_TABLE_SUPPORT_V1;
    static const QString SQL_CREATE_TABLE_RESISTANCE_DATE_V1;
    static const QString SQL_CREATE_TABLE_SUPPORT_DATE_V1;
//    static const QString SQL_CREATE_TABLE_HISTOGRAM_BIN_V1;
//    static const QString SQL_CREATE_TABLE_RESISTANCE_TAB;
//    static const QString SQL_CREATE_TABLE_SUPPORT_TAB;

    // CREATE INDEX
    static const QString SQL_CREATE_INDEX_BARDATA_PARENT;
    static const QString SQL_CREATE_INDEX_BARDATA_PARENT_PREV;
    static const QString SQL_CREATE_INDEX_BARDATA_PARENT_MONTHLY;
    static const QString SQL_CREATE_INDEX_BARDATA_PREVDATETIME;
//    static const QString SQL_CREATE_INDEX_BARDATA_DATETIME_UB;
//    static const QString SQL_CREATE_INDEX_BARDATA_DATETIME_LB;

    static const QString connectionName;
    static const QString tempDatabaseName;

    QHash<QString,QSqlDatabase*> databasePool;
    QHash<QString,QSqlDatabase*> tempDatabasePool;
    mutable QStringList tempdatabase_connection;
    QString application_path = QCoreApplication::applicationDirPath();
    QString database_path;
    static int clone_id;
    int application_id;

    void removeTempDatabase() {
      int N = tempdatabase_connection.size();

      for (int i = 0; i < N; ++i) {
        QSqlDatabase::removeDatabase(tempdatabase_connection[i]);
        QFile::remove(database_path + "/" + tempdatabase_connection[i].split("#")[1]);
      }

      tempdatabase_connection.clear();
    }

    QString create_tempdb_connection_string(const IntervalWeight &interval, const int &id) const {
      return "tempdb#" + getTempResultDatabaseName(interval, id);
    }

    /*void initialize_histogram(const QSqlDatabase &database) {
      QSqlQuery query(database);
      QString sql_insert = "INSERT OR IGNORE INTO " + SQLiteHandler::TABLE_NAME_HISTOGRAM_BIN +
                           "(" + SQLiteHandler::COLUMN_NAME_BIN_ID + ") VALUES(?)";
      QVariantList rowid;

      for (int i = 1; i <= MAX_HISTOGRAM_BIN; ++i) {
        rowid.append(i);
      }

      query.exec("DELETE FROM " + SQLiteHandler::TABLE_NAME_HISTOGRAM_BIN +
                 " WHERE rowid>" + QString::number(MAX_HISTOGRAM_BIN));

      query.exec("PRAGMA journal_mode = OFF;");
      query.exec("PRAGMA synchronous = OFF;");
      query.exec("BEGIN TRANSACTION;");
      query.prepare(sql_insert);
      query.addBindValue(rowid);
      query.execBatch();
      query.exec("COMMIT;");
    }*/

    /*void dumpSql(QString &databaseName, QString &tableName) {
      QString sql_select = "SELECT * FROM " + tableName;
      QSqlDatabase database = getDatabase(databaseName);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      if (query.exec(sql_select)) {
        QString filename = databaseName + "." + tableName + ".sql";
        QStringList columns = getColumnNames(databaseName, tableName);
        QString column_names = columns.join(",");
        QString sql_insert = "";
        QString values;
        int N = columns.length();
        while (query.next()) {
          values = "";
          for (int i = 0; i < N; ++i) {
            if (i > 0) values += ",";
            values += query.value(i).toString();
          }
          sql_insert += "INSERT INTO " + tableName + "(" + column_names + ") VALUES(" + values + ");";
        }
      }
    }*/
};

#endif // SQLITEHANDLER_H
