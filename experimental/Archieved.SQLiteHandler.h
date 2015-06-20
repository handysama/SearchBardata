/**
 * List of obsolete function from SQLiteHandler
 */

const QString SQLiteHandler::TABLE_NAME_DISTF_RANK = "DistF_Rank";
const QString SQLiteHandler::TABLE_NAME_DISTS_RANK = "DistS_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTFS_YEAR = "year_";
const QString SQLiteHandler::COLUMN_NAME_DISTFS_RANK = "rank";

const QString SQLiteHandler::SQL_CREATE_TABLE_DISTF_RANK =
  "CREATE TABLE IF NOT EXISTS " + TABLE_NAME_DISTF_RANK + "("
    "date_ TEXT NOT NULL,"
    "time_ TEXT NOT NULL,"
    "year_ INT NOT NULL,"
    "rank REAL DEFAULT 0,"
    "PRIMARY KEY(date_ ASC, time_ ASC, year_ ASC));";

const QString SQLiteHandler::SQL_CREATE_TABLE_DISTS_RANK =
  "CREATE TABLE IF NOT EXISTS " + TABLE_NAME_DISTS_RANK + "("
    "date_ TEXT NOT NULL,"
    "time_ TEXT NOT NULL,"
    "year_ int NOT NULL,"
    "rank REAL DEFAULT 0,"
    "PRIMARY KEY(date_ ASC, time_ ASC, year_ ASC));";

const QString SQLiteHandler::SQL_CREATE_INDEX_DISTF_YEAR =
  "CREATE INDEX IF NOT EXISTS _distf_year ON " + TABLE_NAME_DISTF_RANK + "(" + COLUMN_NAME_DISTFS_YEAR + ");";

const QString SQLiteHandler::SQL_CREATE_INDEX_DISTS_YEAR =
  "CREATE INDEX IF NOT EXISTS _dists_year ON " + TABLE_NAME_DISTS_RANK + "(" + COLUMN_NAME_DISTFS_YEAR + ");";

const QString SQLiteHandler::SQL_INSERT_DISTF_RANK =
  "insert or ignore into " + TABLE_NAME_DISTF_RANK +
  "(date_,time_,year_,rank) values(?,?,?,?)";

const QString SQLiteHandler::SQL_INSERT_DISTS_RANK =
  "insert or ignore into " + TABLE_NAME_DISTS_RANK +
  "(date_,time_, year_,rank) values(?,?,?,?)";

void attach_temp_distrank(QSqlQuery *query) {
    QString sql_create_tempDistRank =
            "create table tempDist.tempDistRank("
            "date_ TEXT NOT NULL,"
            "time_ TEXT NOT NULL,"
            "distf01 REAL,"
            "dists01 REAL,"
            "distf06 REAL,"
            "dists06 REAL,"
            "distf11 REAL,"
            "dists11 REAL,"
            "primary key(date_ asc,time_ asc));";

    QString sql_insert_tempDistRank1 =
            "insert into tempDist.tempDistRank(date_,time_,distf01)"
            "select date_,time_,rank from distf_rank where year_=2001 order by date_ asc, time_ asc";

    QString sql_update_tempDistRank1 =
            "update tempDist.tempDistRank"
            " set dists01=(select rank from dists_rank where year_=2001 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)"
            "where exists(select rank from dists_rank where year_=2001 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)";

    QString sql_update_tempDistRank2 =
            "update tempDist.tempDistRank"
            " set distf06=(select rank from distf_rank where year_=2006 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)"
            "where exists(select rank from distf_rank where year_=2006 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)";

    QString sql_update_tempDistRank3 =
            "update tempDist.tempDistRank"
            " set dists06=(select rank from dists_rank where year_=2006 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)"
            "where exists(select rank from dists_rank where year_=2006 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)";

    QString sql_update_tempDistRank4 =
            "update tempDist.tempDistRank"
            " set distf11=(select rank from distf_rank where year_=2011 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)"
            "where exists(select rank from distf_rank where year_=2011 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)";

    QString sql_update_tempDistRank5 =
            "update tempDist.tempDistRank"
            " set dists11=(select rank from dists_rank where year_=2011 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)"
            "where exists(select rank from dists_rank where year_=2011 and "
            "date_=tempdistrank.date_ and time_=tempdistrank.time_ limit 1)";

    query->exec("attach database ':memory:' as 'tempDist';");
    query->exec("PRAGMA page_size = 65536;");
    query->exec(sql_create_tempDistRank);

    query->exec("PRAGMA journal_mode = OFF;");
    query->exec("PRAGMA synchronous = OFF;");
    query->exec("begin transaction;");
    query->exec(sql_insert_tempDistRank1);
    query->exec(sql_update_tempDistRank1);
    query->exec(sql_update_tempDistRank2);
    query->exec(sql_update_tempDistRank3);
    query->exec(sql_update_tempDistRank4);
    query->exec(sql_update_tempDistRank5);
    query->exec("commit;");
}


/**
 * BarData V0
 * Inital release, it's only single database file that contains symbol and interval, and everything.
 */
const QString SQL_CREATE_TABLE_BARDATA_V0 =
  "CREATE TABLE IF NOT EXISTS " + TABLE_NAME_BARDATA + "("
    "t_symbol TEXT NOT NULL,"
    "t_interval TEXT NOT NULL,"
    "t_date TEXT NOT NULL,"
    "t_time TEXT NOT NULL,"
    "t_open REAL NOT NULL,"
    "t_high REAL NOT NULL,"
    "t_low REAL NOT NULL,"
    "t_close REAL NOT NULL,"
    "Volume INTEGER NOT NULL,"
    "OI INTEGER NOT NULL,"
    "MACD REAL,"
    "MACDAvg REAL,"
    "MACDDiff REAL,"
    "ZeroLine REAL,"
    "RSI REAL,"
    "OverBot1 REAL,"
    "OverSld1 REAL,"
    "SlowK REAL,"
    "SlowD REAL,"
    "OverBot2 REAL,"
    "OverSld2 REAL,"
    "FastAvg REAL,"
    "SlowAvg REAL,"
    "OpenBar INTERGER,"
    "PRIMARY KEY (t_symbol ASC, t_interval ASC, t_date ASC, t_time ASC,"
    "t_open, t_high, t_low, t_close, Volume, OI));";

const QString SQL_INSERT_BARDATA =
  "INSERT INTO " + TABLE_NAME_BARDATA +
  "(t_symbol, t_interval, t_date, t_time, t_open, t_high, t_low, t_close,"
  "Volume, OI, MACD, MACDAvg, MACDDiff, ZeroLine, RSI, OverBot1, OverSld1,"
  "SlowK, SlowD, OverBot2, OverSld2, FastAvg, SlowAvg, OpenBar)"
  " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

const QString SQL_REPLACE_BARDATA =
  "REPLACE INTO " + TABLE_NAME_BARDATA +
  "(t_symbol, t_interval, t_date, t_time, t_open, t_high, t_low, t_close,"
  "Volume, OI, MACD, MACDAvg,MACDDiff, ZeroLine, RSI, OverBot1, OverSld1,"
  "SlowK, SlowD, OverBot2, OverSld2, FastAvg, SlowAvg, OpenBar)"
  " VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

const QString SQL_VIEW_BARDATA_1 =
  "SELECT "
    "strftime('%m/%d/%Y',t_date) AS 'Date',"
    "strftime('%H:%M',t_time) AS 'Time',"
    "t_open AS Open,"
    "t_high AS High,"
    "t_low AS Low,"
    "t_close As Close,"
    "Volume,"
    "MACD,"
    "MACDAvg,"
    "MACDDiff,"
    "RSI,"
    "SlowK,"
    "SlowD,"
    "FastAvg,"
    "SlowAvg"
  " FROM " + TABLE_NAME_BARDATA;

/**
 * BarData V1 (2015-01-26)
 * In this release, symbol and interval has been lifted,
 * thus each database file represent unique pair symbol and interval.
 * This multiple database enable us to employ concurrent database process.
 * Tip: use underscore after column name to avoid conflict and disambiguation with SQL keywords.
 */
const QString SQL_CREATE_TABLE_BARDATA_V1 =
  "CREATE TABLE IF NOT EXISTS " + TABLE_NAME_BARDATA + "("
    "date_ TEXT NOT NULL,"
    "time_ TEXT NOT NULL,"
    "open_ REAL NOT NULL,"
    "high_ REAL NOT NULL,"
    "low_ REAL NOT NULL,"
    "close_ REAL NOT NULL,"
    "Volume INTEGER NOT NULL,"
    "OI INTEGER NOT NULL,"
    "MACD REAL,"
    "MACDAvg REAL,"
    "MACDDiff REAL,"
    "ZeroLine REAL,"
    "RSI REAL,"
    "OverBot1 REAL,"
    "OverSld1 REAL,"
    "SlowK REAL,"
    "SlowD REAL,"
    "OverBot2 REAL,"
    "OverSld2 REAL,"
    "FastAvg REAL,"
    "SlowAvg REAL,"
    "OpenBar INTERGER,"
    "PRIMARY KEY (date_ ASC, time_ ASC, open_, high_, low_, close_, Volume, OI));";

const QString SQL_VIEW1_BARDATA_V1 =
  "SELECT "
    "strftime('%m/%d/%Y',date_) AS 'Date',"
    "strftime('%H:%M',time_) AS 'Time',"
    "open_ AS Open,"
    "high_ AS High,"
    "low_ AS Low,"
    "close_ As Close,"
    "Volume,"
    "MACD,"
    "MACDAvg,"
    "MACDDiff,"
    "RSI,"
    "SlowK,"
    "SlowD,"
    "FastAvg,"
    "SlowAvg"
  " FROM " + TABLE_NAME_BARDATA;

const QString SQL_INSERT_BARDATA_V1 =
  "INSERT OR IGNORE INTO " + TABLE_NAME_BARDATA +
  "(date_, time_, open_, high_, low_, close_,"
  "Volume, OI, MACD, MACDAvg, MACDDiff, ZeroLine, RSI, OverBot1, OverSld1,"
  "SlowK, SlowD, OverBot2, OverSld2, FastAvg, SlowAvg, OpenBar)"
  " VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?)";

const QString SQL_REPLACE_BARDATA_V1 =
  "REPLACE INTO " + TABLE_NAME_BARDATA +
  "(date_, time_, open_, high_, low_, close_,"
  "Volume, OI, MACD, MACDAvg, MACDDiff, ZeroLine, RSI, OverBot1, OverSld1,"
  "SlowK, SlowD, OverBot2, OverSld2, FastAvg, SlowAvg, OpenBar)"
  " VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?)";

const QString SQL_VIEW1_BARDATA_V2 =
  "SELECT "
    "strftime('%m/%d/%Y',date_) AS 'Date',"
    "strftime('%H:%M',time_) AS 'Time',"
    "open_ AS Open,"
    "high_ AS High,"
    "low_ AS Low,"
    "close_ AS Close,"
    "Volume,"
    "MACD,"
    "MACDAvg,"
    "MACDDiff,"
    "RSI,"
    "SlowK,"
    "SlowD,"
    "FastAvg,"
    "SlowAvg,"
    "DistF,"
    "DistS,"
    "FGS AS 'F > S',"
    "FLS AS 'F < S'"
  " FROM " + TABLE_NAME_BARDATA;

