#include "SQLiteHandler.h"

int SQLiteHandler::clone_id = 0;

// Enumeration vars
const QString SQLiteHandler::connectionName = "sqlite.finance.";
const QString SQLiteHandler::tempDatabaseName = "tempResult.";
const QString SQLiteHandler::TABLE_NAME_BARDATA = "Bardata";
const QString SQLiteHandler::TABLE_NAME_BARDATA_VIEW = "BardataView";
const QString SQLiteHandler::TABLE_NAME_RESISTANCE = "ResistanceData";
const QString SQLiteHandler::TABLE_NAME_RESISTANCE_DATE = "ResistanceReactDate";
const QString SQLiteHandler::TABLE_NAME_SUPPORT = "SupportData";
const QString SQLiteHandler::TABLE_NAME_SUPPORT_DATE = "SupportReactDate";
//const QString SQLiteHandler::TABLE_NAME_HISTOGRAM_BIN = "HistogramBin";
//const QString SQLiteHandler::TABLE_NAME_RESISTANCE_TAB = "ResistanceDataTab";
//const QString SQLiteHandler::TABLE_NAME_SUPPORT_TAB = "SupportDataTab";
const QString SQLiteHandler::COLUMN_NAME_ROWID = "rowid";
const QString SQLiteHandler::COLUMN_NAME_IDPARENT = "_parent";
const QString SQLiteHandler::COLUMN_NAME_IDPARENT_WEEKLY = "_parent_weekly";
const QString SQLiteHandler::COLUMN_NAME_IDPARENT_MONTHLY = "_parent_monthly";
const QString SQLiteHandler::COLUMN_NAME_IDPARENT_PREV = "_parent_prev";
const QString SQLiteHandler::COLUMN_NAME_IDPARENT_PREV_WEEKLY = "_parent_prev_weekly";
const QString SQLiteHandler::COLUMN_NAME_IDPARENT_PREV_MONTHLY = "_parent_prev_monthly";
const QString SQLiteHandler::COLUMN_NAME_IDTHRESHOLD = "id_threshold";
const QString SQLiteHandler::COLUMN_NAME_DATE = "date_";
const QString SQLiteHandler::COLUMN_NAME_TIME = "time_";
const QString SQLiteHandler::COLUMN_NAME_OPEN = "open_";
const QString SQLiteHandler::COLUMN_NAME_HIGH = "high_";
const QString SQLiteHandler::COLUMN_NAME_LOW = "low_";
const QString SQLiteHandler::COLUMN_NAME_CLOSE = "close_";
const QString SQLiteHandler::COLUMN_NAME_VOLUME = "volume";
const QString SQLiteHandler::COLUMN_NAME_MACD = "MACD";
const QString SQLiteHandler::COLUMN_NAME_MACDAVG = "MACDAvg";
const QString SQLiteHandler::COLUMN_NAME_MACDDIFF = "MACDDiff";
const QString SQLiteHandler::COLUMN_NAME_RSI = "RSI";
const QString SQLiteHandler::COLUMN_NAME_SLOWK = "SlowK";
const QString SQLiteHandler::COLUMN_NAME_SLOWD = "SlowD";
const QString SQLiteHandler::COLUMN_NAME_DISTF = "DistF";
const QString SQLiteHandler::COLUMN_NAME_DISTS = "DistS";
const QString SQLiteHandler::COLUMN_NAME_DISTFS = "DistFS";
const QString SQLiteHandler::COLUMN_NAME_DISTFS_RANK = "DistFS_Rank";
const QString SQLiteHandler::COLUMN_NAME_N_DISTFS = "N_DistFS";
const QString SQLiteHandler::COLUMN_NAME_FASTAVG = "FastAvg";
const QString SQLiteHandler::COLUMN_NAME_SLOWAVG = "SlowAvg";
const QString SQLiteHandler::COLUMN_NAME_FASTAVG_SLOPE = "FastAvgSlope";
const QString SQLiteHandler::COLUMN_NAME_SLOWAVG_SLOPE = "SlowAvgSlope";
const QString SQLiteHandler::COLUMN_NAME_FCROSS = "FCross";
const QString SQLiteHandler::COLUMN_NAME_SCROSS = "SCross";
const QString SQLiteHandler::COLUMN_NAME_FGS = "FGS";
const QString SQLiteHandler::COLUMN_NAME_FLS = "FLS";
const QString SQLiteHandler::COLUMN_NAME_FGS_RANK = "FGS_Rank";
const QString SQLiteHandler::COLUMN_NAME_FLS_RANK = "FLS_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS = "DistCC_FSCross";
const QString SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK = "DistCC_FSCross_Rank";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL = "Candle_Uptail";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL = "Candle_Downtail";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_BODY = "Candle_Body";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH = "Candle_TotalLength";
const QString SQLiteHandler::COLUMN_NAME_N_UPTAIL = "N_Uptail";
const QString SQLiteHandler::COLUMN_NAME_N_DOWNTAIL = "N_Downtail";
const QString SQLiteHandler::COLUMN_NAME_N_BODY = "N_Body";
const QString SQLiteHandler::COLUMN_NAME_N_TOTALLENGTH = "N_TotalLength";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL_RANK = "Candle_Uptail_Rank";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL_RANK = "Candle_Downtail_Rank";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_BODY_RANK = "Candle_Body_Rank";
const QString SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH_RANK = "Candle_TotalLength_Rank";
const QString SQLiteHandler::COLUMN_NAME_DATERANGE = "DateRange";
const QString SQLiteHandler::COLUMN_NAME_ATR = "ATR";
const QString SQLiteHandler::COLUMN_NAME_ATR_RANK = "ATR_Rank";
const QString SQLiteHandler::COLUMN_NAME_OPENBAR = "Openbar";
const QString SQLiteHandler::COLUMN_NAME_PREV_DATE = "PrevDate";
const QString SQLiteHandler::COLUMN_NAME_PREV_TIME = "PrevTime";
const QString SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR = "PrevBarColor";
const QString SQLiteHandler::COLUMN_NAME_MACD_G0 = "MACD_G0";
const QString SQLiteHandler::COLUMN_NAME_MACD_L0 = "MACD_L0";
const QString SQLiteHandler::COLUMN_NAME_MACD_RANK = "MACD_RANK";
const QString SQLiteHandler::COLUMN_NAME_MACD_G0_RANK = "MACD_G0_RANK";
const QString SQLiteHandler::COLUMN_NAME_MACD_L0_RANK = "MACD_L0_RANK";
const QString SQLiteHandler::COLUMN_NAME_RSI_G70 = "RSI_G70";
const QString SQLiteHandler::COLUMN_NAME_RSI_L30 = "RSI_L30";
//const QString SQLiteHandler::COLUMN_NAME_RSI_G70_RANK = "RSI_G70_RANK";
//const QString SQLiteHandler::COLUMN_NAME_RSI_L30_RANK = "RSI_L30_RANK";
const QString SQLiteHandler::COLUMN_NAME_SLOWK_G80 = "SLOWK_G80";
const QString SQLiteHandler::COLUMN_NAME_SLOWK_L20 = "SLOWK_L20";
//const QString SQLiteHandler::COLUMN_NAME_SLOWK_G80_RANK = "SLOWK_G80_RANK";
//const QString SQLiteHandler::COLUMN_NAME_SLOWK_L20_RANK = "SLOWK_L20_RANK";
const QString SQLiteHandler::COLUMN_NAME_SLOWD_G80 = "SLOWD_G80";
const QString SQLiteHandler::COLUMN_NAME_SLOWD_L20 = "SLOWD_L20";
//const QString SQLiteHandler::COLUMN_NAME_SLOWD_G80_RANK = "SLOWD_G80_RANK";
//const QString SQLiteHandler::COLUMN_NAME_SLOWD_L20_RANK = "SLOWD_L20_RANK";
const QString SQLiteHandler::COLUMN_NAME_DATETIME_UB = "Datetime_UB";
const QString SQLiteHandler::COLUMN_NAME_DATETIME_LB = "Datetime_LB";
const QString SQLiteHandler::COLUMN_NAME_CGF = "CGF";
const QString SQLiteHandler::COLUMN_NAME_CGF_RANK = "CGF_Rank";
const QString SQLiteHandler::COLUMN_NAME_CLF = "CLF";
const QString SQLiteHandler::COLUMN_NAME_CLF_RANK = "CLF_Rank";
const QString SQLiteHandler::COLUMN_NAME_CGS = "CGS";
const QString SQLiteHandler::COLUMN_NAME_CGS_RANK = "CGS_Rank";
const QString SQLiteHandler::COLUMN_NAME_CLS = "CLS";
const QString SQLiteHandler::COLUMN_NAME_CLS_RANK = "CLS_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTOF = "DistOF";
const QString SQLiteHandler::COLUMN_NAME_DISTOS = "DistOS";
const QString SQLiteHandler::COLUMN_NAME_N_DISTOF = "N_DistOF";
const QString SQLiteHandler::COLUMN_NAME_N_DISTOS = "N_DistOS";
const QString SQLiteHandler::COLUMN_NAME_DISTOF_RANK = "DistOF_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTOS_RANK = "DistOS_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTHF = "DistHF";
const QString SQLiteHandler::COLUMN_NAME_DISTHS = "DistHS";
const QString SQLiteHandler::COLUMN_NAME_N_DISTHF = "N_DistHF";
const QString SQLiteHandler::COLUMN_NAME_N_DISTHS = "N_DistHS";
const QString SQLiteHandler::COLUMN_NAME_DISTHF_RANK = "DistHF_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTHS_RANK = "DistHS_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTLF = "DistLF";
const QString SQLiteHandler::COLUMN_NAME_DISTLS = "DistLS";
const QString SQLiteHandler::COLUMN_NAME_N_DISTLF = "N_DistLF";
const QString SQLiteHandler::COLUMN_NAME_N_DISTLS = "N_DistLS";
const QString SQLiteHandler::COLUMN_NAME_DISTLF_RANK = "DistLF_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTLS_RANK = "DistLS_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTCF = "DistCF";
const QString SQLiteHandler::COLUMN_NAME_DISTCS = "DistCS";
const QString SQLiteHandler::COLUMN_NAME_N_DISTCF = "N_DistCF";
const QString SQLiteHandler::COLUMN_NAME_N_DISTCS = "N_DistCS";
const QString SQLiteHandler::COLUMN_NAME_DISTCF_RANK = "DistCF_Rank";
const QString SQLiteHandler::COLUMN_NAME_DISTCS_RANK = "DistCS_Rank";

const QString SQLiteHandler::COLUMN_NAME_RES = "Res";
const QString SQLiteHandler::COLUMN_NAME_DISTRES = "DistRes";
const QString SQLiteHandler::COLUMN_NAME_DISTRES_ATR = "DistResATR";
const QString SQLiteHandler::COLUMN_NAME_DISTRES_RANK = "DistResRank";
const QString SQLiteHandler::COLUMN_NAME_SUP = "Sup";
const QString SQLiteHandler::COLUMN_NAME_DISTSUP = "DistSup";
const QString SQLiteHandler::COLUMN_NAME_DISTSUP_ATR = "DistSupATR";
const QString SQLiteHandler::COLUMN_NAME_DISTSUP_RANK = "DistSupRank";
const QString SQLiteHandler::COLUMN_NAME_DAILY_RLINE = "Daily_RLine";
const QString SQLiteHandler::COLUMN_NAME_DAILY_RLINE_F = "Daily_RLineF";
const QString SQLiteHandler::COLUMN_NAME_DAILY_RLINE_S = "Daily_RLineS";
const QString SQLiteHandler::COLUMN_NAME_DAILY_SLINE = "Daily_SLine";
const QString SQLiteHandler::COLUMN_NAME_DAILY_SLINE_F = "Daily_SLineF";
const QString SQLiteHandler::COLUMN_NAME_DAILY_SLINE_S = "Daily_SLineS";
const QString SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE = "Weekly_RLine";
const QString SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE_F = "Weekly_RLineF";
const QString SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE_S = "Weekly_RLineS";
const QString SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE = "Weekly_SLine";
const QString SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE_F = "Weekly_SLineF";
const QString SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE_S = "Weekly_SLineS";
const QString SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE = "Monthly_RLine";
const QString SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE_F = "Monthly_RLineF";
const QString SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE_S = "Monthly_RLineS";
const QString SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE = "Monthly_SLine";
const QString SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE_F = "Monthly_SLineF";
const QString SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE_S = "Monthly_SLineS";

const QString SQLiteHandler::COLUMN_NAME_ZONE = "Zone";
const QString SQLiteHandler::COLUMN_NAME_ZONE_60MIN = "Zone60min";
const QString SQLiteHandler::COLUMN_NAME_ZONE_DAILY = "ZoneDaily";
const QString SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY = "ZoneWeekly";
const QString SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY = "ZoneMonthly";
const QString SQLiteHandler::COLUMN_NAME_RESISTANCE_COUNT = "resistance_count";
const QString SQLiteHandler::COLUMN_NAME_RESISTANCE_DURATION = "resistance_duration";
const QString SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE = "resistance";
const QString SQLiteHandler::COLUMN_NAME_SUPPORT_COUNT = "support_count";
const QString SQLiteHandler::COLUMN_NAME_SUPPORT_DURATION = "support_duration";
const QString SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE = "support";
const QString SQLiteHandler::COLUMN_NAME_DIST_POINT = "Dist_Point";
const QString SQLiteHandler::COLUMN_NAME_DIST_ATR = "Dist_ATR";

/**
 * BarData V2 (2015-01-30)
 * Removed column : OI, ZeroLine, OverBot, OverSld
 * Add column     : DistF, DistS, FGS (F greater than S), FLS (F less than S)
 * Change primary key using (date_, time_)
 *
 * Rev1: 2015-02-01
 * Add column _parent to enable multi resolution search.
 *
 * Rev2: 2015-02-10 (v0.5)
 * Add column resistance_test_point, support_test_point.
 *
 * Rev3: 2015-03-01 (v0.6)
 * Add column for multiple threshold resistance/support test point
 *
 * Rev4: 2015-03-18 - 2015-03-20 (v0.7)
 * Denormalized the table for increase performance
 * Add FastAvg and SlowAvg of the parent
 * Add _parent_prev for store index of previous bar of parent
 * Add DistF/DistS Rank (denormalize)
 *
 * Rev5: 2015-04-19 (v0.8.14)
 * Add DistFS (distance between Fast and Slow)
 *
 * Rev6: 2015-04-21 (v0.8.15)
 * Add FastAvgSlope and SlowAvgSlope, PrevDate, PrevTime
 *
 * Rev7: 2015-04-29 (v0.9.2)
 * Add DistFC, DistFC_Rank, DistSC, DistSC_Rank
 *
 * Rev8: 2015-05-11 (v0.9.4)
 * Add ATR, RSI_GREATER_70, RSI_LESS_30, MACD_GREATER_0, MACD_LESS_0,
 * SLOWK_GREATER_80, SLOWK_LESS_20, SLOWD_GREATER_80, SLOWD_LESS_20
 *
 * Rev9: 2015-05-12 (v0.9.4)
 * Add uptail, downtail, body, totallength, daterange
 *
 * Rev10: 2015-05-19 (v0.9.5)
 * Add DistResistance, DistSupport, Datetime_UB, Datetime_LB
 *
 * Rev11: ??? - 2015-06-14 (v0.10.2)
 * Add Zone, RLine, SLine, Res_1, Sup_1
 * Renamed columns, such as MACD_GREATER_0 into MACD_G0
 */
const QString SQLiteHandler::SQL_CREATE_TABLE_BARDATA_V2 =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_BARDATA + "("
    /* Base */
    "date_ TEXT NOT NULL,"
    "time_ TEXT NOT NULL,"
    "open_ REAL NOT NULL,"
    "high_ REAL NOT NULL,"
    "low_ REAL NOT NULL,"
    "close_ REAL NOT NULL,"
    "Volume INTEGER NOT NULL,"
    "MACD REAL NOT NULL,"
    "MACDAvg REAL NOT NULL,"
    "MACDDiff REAL NOT NULL,"
    "RSI REAL NOT NULL,"
    "SlowK REAL NOT NULL,"
    "SlowD REAL NOT NULL,"
    "FastAvg REAL NOT NULL,"
    "SlowAvg REAL NOT NULL,"
    "DistF REAL NOT NULL,"
    "DistS REAL NOT NULL,"
    "FGS INTEGER NOT NULL,"
    "FLS INTEGER NOT NULL,"
    "OpenBar INTEGER NOT NULL,"

    /* Indexing */
    "_parent INTEGER DEFAULT 0,"
    "_parent_monthly INTEGER DEFAULT 0,"
    "_parent_prev INTEGER DEFAULT 0,"
    "_parent_prev_monthly INTEGER DEFAULT 0,"
    "PrevDate TEXT,"
    "PrevTime TEXT,"
//    "_parent_weekly INTEGER DEFAULT 0,"
//    "_parent_prev_weekly INTEGER DEFAULT 0,"
//    "DateTime_UB INTEGER,"
//    "DateTime_LB INTEGER,"

    /* Derived column */
    "PrevBarColor TEXT,"
    "ATR REAL,"
    "ATR_RANK REAL DEFAULT 0,"
    "FGS_Rank REAL,"
    "FLS_Rank REAL,"
    "FastAvgSlope REAL,"
    "SlowAvgSlope REAL,"
    "FCross INTEGER,"
    "SCross INTEGER,"
    "DistCC_FSCross REAL," // (Close_curr - Close_since_FS_cross) / ATR
    "DistCC_FSCross_Rank REAL,"
    "DistOF REAL," // (Open - FastAvg)
    "N_DistOF REAL,"
    "DistOF_Rank REAL DEFAULT 0,"
    "DistOS REAL," // (Open - SlowAvg)
    "N_DistOS REAL,"
    "DistOS_Rank REAL DEFAULT 0,"
    "DistHF REAL," // (High - FastAvg)
    "N_DistHF REAL,"
    "DistHF_Rank REAL DEFAULT 0,"
    "DistHS REAL," // (High - SlowAvg)
    "N_DistHS REAL,"
    "DistHS_Rank REAL DEFAULT 0,"
    "DistLF REAL," // (Low - FastAvg)
    "N_DistLF REAL,"
    "DistLF_Rank REAL DEFAULT 0,"
    "DistLS REAL," // (Low - SlowAvg)
    "N_DistLS REAL,"
    "DistLS_Rank REAL DEFAULT 0,"
    "DistCF REAL," // (Close - FastAvg)
    "N_DistCF REAL,"
    "DistCF_Rank REAL DEFAULT 0,"
    "DistCS REAL," // (Close - SlowAvg)
    "N_DistCS REAL,"
    "DistCS_Rank REAL DEFAULT 0,"
    "DistFS REAL," // (FastAvg - SlowAvg)
    "N_DistFS REAL,"
    "DistFS_Rank REAL DEFAULT 0,"

    "MACD_Rank REAL DEFAULT 0,"
    "MACD_G0 INTEGER DEFAULT 0,"
    "MACD_L0 INTEGER DEFAULT 0,"
    "MACD_G0_Rank REAL DEFAULT 0,"
    "MACD_L0_Rank REAL DEFAULT 0,"
    "RSI_G70 INTEGER DEFAULT 0,"
    "RSI_L30 INTEGER DEFAULT 0,"
    "SLOWK_G80 INTEGER DEFAULT 0,"
    "SLOWK_L20 INTEGER DEFAULT 0,"
    "SLOWD_G80 INTEGER DEFAULT 0,"
    "SLOWD_L20 INTEGER DEFAULT 0,"

    "Candle_Uptail REAL,"
    "Candle_Downtail REAL,"
    "Candle_Body REAL,"
    "Candle_TotalLength REAL,"
    "N_Uptail REAL,"
    "N_Downtail REAL,"
    "N_Body REAL,"
    "N_TotalLength REAL,"
    "Candle_Uptail_Rank REAL,"
    "Candle_Downtail_Rank REAL,"
    "Candle_Body_Rank REAL,"
    "Candle_TotalLength_Rank REAL,"
    "DateRange REAL," // ( Max(high) - Min(low) ) / ATR
    "CGF INTEGER DEFAULT 0,"
    "CGF_Rank REAL DEFAULT 0,"
    "CLF INTEGER DEFAULT 0,"
    "CLF_Rank REAL DEFAULT 0,"
    "CGS INTEGER DEFAULT 0,"
    "CGS_Rank REAL DEFAULT 0,"
    "CLS INTEGER DEFAULT 0,"
    "CLS_Rank REAL DEFAULT 0,"
    "Zone INTEGER DEFAULT 0,"
    "Zone60min INTEGER,"
    "ZoneDaily INTEGER,"
    "ZoneWeekly INTEGER,"
    "ZoneMonthly INTEGER,"
    // DistRes
    "Res_1 REAL,"
    "DistRes_1 REAL," // Min(Resistance) - Close
    "DistResATR_1 REAL," // DistRes / ATR
    "DistResRank_1 REAL,"
    "Res_2 REAL,"
    "DistRes_2 REAL,"
    "DistResATR_2 REAL,"
    "DistResRank_2 REAL,"
    "Res_3 REAL,"
    "DistRes_3 REAL,"
    "DistResATR_3 REAL,"
    "DistResRank_3 REAL,"
    // DistSup
    "Sup_1 REAL,"
    "DistSup_1 REAL," // Max(Support) - Close
    "DistSupATR_1 REAL," // DistSup / ATR
    "DistSupRank_1 REAL,"
    "Sup_2 REAL,"
    "DistSup_2 REAL,"
    "DistSupATR_2 REAL,"
    "DistSupRank_2 REAL,"
    "Sup_3 REAL,"
    "DistSup_3 REAL,"
    "DistSupATR_3 REAL,"
    "DistSupRank_3 REAL,"
    // Daily
    "Daily_RLine_1 INTEGER," // Resistance (#)
    "Daily_SLine_1 INTEGER," // Support (#)
    "Daily_RLine_2 INTEGER,"
    "Daily_SLine_2 INTEGER,"
    "Daily_RLine_3 INTEGER,"
    "Daily_SLine_3 INTEGER,"
    // Weekly
    "Weekly_RLine_1 INTEGER,"
    "Weekly_SLine_1 INTEGER,"
    "Weekly_RLine_2 INTEGER,"
    "Weekly_SLine_2 INTEGER,"
    "Weekly_RLine_3 INTEGER,"
    "Weekly_SLine_3 INTEGER,"
    // Monthly
    "Monthly_RLine_1 INTEGER,"
    "Monthly_SLine_1 INTEGER,"
    "Monthly_RLine_2 INTEGER,"
    "Monthly_SLine_2 INTEGER,"
    "Monthly_RLine_3 INTEGER,"
    "Monthly_SLine_3 INTEGER,"
    "PRIMARY KEY(date_ ASC,time_ ASC));";

//    "DistFS_bin_id INTEGER DEFAULT 0,"
//    "Daily_RLineF_1 INTEGER," // Res-F (#)
//    "Daily_RLineS_1 INTEGER," // Res-S (#)
//    "Daily_SLineF_1 INTEGER," // Sup-F (#)
//    "Daily_SLineS_1 INTEGER," // Sup-S (#)
//    "Daily_RLineF_2 INTEGER,"
//    "Daily_RLineS_2 INTEGER,"
//    "Daily_SLineF_2 INTEGER,"
//    "Daily_SLineS_2 INTEGER,"
//    "Daily_RLineF_3 INTEGER,"
//    "Daily_RLineS_3 INTEGER,"
//    "Daily_SLineF_3 INTEGER,"
//    "Daily_SLineS_3 INTEGER,"
//    "Weekly_RLineF_1 INTEGER,"
//    "Weekly_RLineS_1 INTEGER,"
//    "Weekly_SLineF_1 INTEGER,"
//    "Weekly_SLineS_1 INTEGER,"
//    "Weekly_RLineF_2 INTEGER,"
//    "Weekly_RLineS_2 INTEGER,"
//    "Weekly_SLineF_2 INTEGER,"
//    "Weekly_SLineS_2 INTEGER,"
//    "Weekly_RLineF_3 INTEGER,"
//    "Weekly_RLineS_3 INTEGER,"
//    "Weekly_SLineF_3 INTEGER,"
//    "Weekly_SLineS_3 INTEGER,"
//    "Monthly_RLineF_1 INTEGER,"
//    "Monthly_RLineS_1 INTEGER,"
//    "Monthly_SLineF_1 INTEGER,"
//    "Monthly_SLineS_1 INTEGER,"
//    "Monthly_RLineF_2 INTEGER,"
//    "Monthly_RLineS_2 INTEGER,"
//    "Monthly_SLineF_2 INTEGER,"
//    "Monthly_SLineS_2 INTEGER,"
//    "Monthly_RLineF_3 INTEGER,"
//    "Monthly_RLineS_3 INTEGER,"
//    "Monthly_SLineF_3 INTEGER,"
//    "Monthly_SLineS_3 INTEGER,"

/**
 * Resistance & Support V1
 * 2015-04-02 : add column duration
 */
const QString SQLiteHandler::SQL_CREATE_TABLE_RESISTANCE_V1 =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_RESISTANCE + "("
  "date_ TEXT NOT NULL,"
  "time_ TEXT NOT NULL,"
  "react_date TEXT,"
  "react_time TEXT,"
  "resistance_count INTEGER,"
  "resistance_duration INTEGER,"
  "resistance REAL,"
  "last_duration INTEGER,"
  "id_threshold INTEGER,"
  "dist_point REAL,"
  "dist_atr REAL,"
  "PRIMARY KEY(id_threshold, date_, time_, react_date, react_time));";

const QString SQLiteHandler::SQL_CREATE_TABLE_SUPPORT_V1 =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_SUPPORT + "("
  "date_ TEXT NOT NULL,"
  "time_ TEXT NOT NULL,"
  "react_date TEXT,"
  "react_time TEXT,"
  "support_count INTEGER,"
  "support_duration INTEGER,"
  "support REAL,"
  "last_duration INTEGER,"
  "id_threshold INTEGER,"
  "dist_point REAL,"
  "dist_atr REAL,"
  "PRIMARY KEY(id_threshold, date_, time_, react_date, react_time));";

const QString SQLiteHandler::SQL_CREATE_TABLE_RESISTANCE_DATE_V1 =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_RESISTANCE_DATE + "("
  "rid INTEGER NOT NULL,"
  "react_date TEXT NOT NULL);";

const QString SQLiteHandler::SQL_CREATE_TABLE_SUPPORT_DATE_V1 =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_SUPPORT_DATE + "("
  "rid INTEGER NOT NULL,"
  "react_date TEXT NOT NULL);";

/*const QString SQLiteHandler::SQL_CREATE_TABLE_HISTOGRAM_BIN_V1 =
  "CREATE TABLE IF NOT EXISTS HistogramBin("
    "bin_id INTEGER PRIMARY KEY,"
    "min_DistFS REAL,"
    "max_DistFS REAL,"
    "size_DistFS INTEGER DEFAULT 0,"
    "min_DistCF REAL,"
    "max_DistCF REAL,"
    "size_DistCF INTEGER DEFAULT 0,"
    "min_DistCS REAL,"
    "max_DistCS REAL,"
    "size_DistCS INTEGER DEFAULT 0,"
    "min_FGS REAL,"
    "max_FGS REAL,"
    "size_FGS INTEGER DEFAULT 0,"
    "min_FLS REAL,"
    "max_FLS REAL,"
    "size_FLS INTEGER DEFAULT 0,"
    "min_CGF REAL,"
    "max_CGF REAL,"
    "size_CGF INTEGER DEFAULT 0);";*/


/**
 * Resistance & Support Denormalized version
 * 2015-03-20:
 * Created to prestore detail view of value
 */
/*
const QString SQLiteHandler::SQL_CREATE_TABLE_RESISTANCE_TAB =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_RESISTANCE_TAB + "("
  "index_ INTEGER NOT NULL,"
  "date_ TEXT NOT NULL,"
  "time_ TEXT NOT NULL,"
  "column_count INTEGER DEFAULT 0,"
  "v0 REAL,"
  "PRIMARY KEY(index_, date_, time_));";

const QString SQLiteHandler::SQL_CREATE_TABLE_SUPPORT_TAB =
  "CREATE TABLE IF NOT EXISTS " + SQLiteHandler::TABLE_NAME_SUPPORT_TAB + "("
  "index_ INTEGER NOT NULL,"
  "date_ TEXT NOT NULL,"
  "time_ TEXT NOT NULL,"
  "column_count INTEGER DEFAULT 0,"
  "v0 REAL,"
  "PRIMARY KEY(index_, date_, time_));";
*/


//
// Create Index
//
const QString SQLiteHandler::SQL_CREATE_INDEX_BARDATA_PARENT =
  "CREATE INDEX IF NOT EXISTS _bardata_parent ON " + TABLE_NAME_BARDATA + "(" + COLUMN_NAME_IDPARENT + ");";

const QString SQLiteHandler::SQL_CREATE_INDEX_BARDATA_PARENT_PREV =
  "CREATE INDEX IF NOT EXISTS _bardata_parent_prev ON " + TABLE_NAME_BARDATA + "(" + COLUMN_NAME_IDPARENT_PREV + ");";

const QString SQLiteHandler::SQL_CREATE_INDEX_BARDATA_PARENT_MONTHLY =
  "CREATE INDEX IF NOT EXISTS _bardata_parent_monthly ON " + TABLE_NAME_BARDATA + "(" + COLUMN_NAME_IDPARENT_MONTHLY + ");";

const QString SQLiteHandler::SQL_CREATE_INDEX_BARDATA_PREVDATETIME =
  "CREATE INDEX IF NOT EXISTS _bardata_prevdatetime ON " + TABLE_NAME_BARDATA +
    "(" + COLUMN_NAME_PREV_DATE + "," + COLUMN_NAME_PREV_TIME + ");";

//const QString SQLiteHandler::SQL_CREATE_INDEX_BARDATA_DATETIME_UB =
//  "CREATE INDEX IF NOT EXISTS _bardata_datetime_ub ON " + TABLE_NAME_BARDATA + "(" + COLUMN_NAME_DATETIME_UB + ");";

//const QString SQLiteHandler::SQL_CREATE_INDEX_BARDATA_DATETIME_LB =
//  "CREATE INDEX IF NOT EXISTS _bardata_datetime_lb ON " + TABLE_NAME_BARDATA + "(" + COLUMN_NAME_DATETIME_LB + ");";


//
// Bardata Function
//
//const QString SQLiteHandler::SQL_SELECT_BARDATA_ROWID = "SELECT " + COLUMN_NAME_ROWID + " FROM " + TABLE_NAME_BARDATA;
const QString SQLiteHandler::SQL_SELECT_BARDATA_MAX_ROWID = "SELECT MAX(" + COLUMN_NAME_ROWID + ") FROM " + TABLE_NAME_BARDATA;
const QString SQLiteHandler::SQL_SELECT_BARDATA_DESC = "SELECT * FROM " + TABLE_NAME_BARDATA + " ORDER BY date_ DESC, time_ DESC";

//const QString SQLiteHandler::SQL_VIEW1_BARDATA_V2 =
//  "SELECT "
//    "strftime('%m/%d/%Y',date_) AS 'Date',"
//    "strftime('%H:%M',time_) AS 'Time',"
//    "open_ AS Open,"
//    "high_ AS High,"
//    "low_ AS Low,"
//    "close_ AS Close,"
//    "Volume,"
//    "MACD,"
//    "MACDAvg,"
//    "MACDDiff,"
//    "RSI,"
//    "SlowK,"
//    "SlowD,"
//    "FastAvg,"
//    "SlowAvg,"
//    "DistF,"
//    "DistS,"
//    "FGS AS 'F > S',"
//    "FLS AS 'F < S',"
//    "resistance_test_point AS 'Resistance',"
//    "support_test_point AS 'Support'"
//  " FROM " + TABLE_NAME_BARDATA;

QString SQLiteHandler::SQL_LAST_FROM_ROWID(const int &start_from_rowid) {
  QString sql = "SELECT * FROM " + TABLE_NAME_BARDATA +
    ((start_from_rowid>0)? " WHERE " + COLUMN_NAME_ROWID + ">=" + QString::number(start_from_rowid): "") +
    " ORDER BY date_ DESC, time_ DESC";
  return sql;
}

const QString SQLiteHandler::SQL_INSERT_BARDATA_V2 =
  "INSERT OR IGNORE INTO " + TABLE_NAME_BARDATA +
  "(date_, time_, open_, high_, low_, close_, Volume, MACD, MACDAvg, MACDDiff,"
  "RSI, SlowK, SlowD, FastAvg, SlowAvg, DistF, DistS, FGS, FLS, OpenBar,"
  "FastAvgSlope, SlowAvgSlope, PrevDate, PrevTime, DistCF, DistCS, ATR, PrevBarColor, RSI_GREATER_70, RSI_LESS_30,"
  "MACD_GREATER_0, MACD_LESS_0, SlowK_Greater_t1, SlowK_Less_t1, SlowD_Greater_t1, SlowD_Less_t1, DispFS, Candle_Uptail,Candle_DownTail, Candle_Body, "
  "Candle_TotalLength, N_DistCF, N_DistCS, N_DistFS)"
  "VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?,?)";

const QString SQLiteHandler::SQL_UPDATE_BARDATA_V2 =
  "UPDATE " + TABLE_NAME_BARDATA + " SET "
    "open_=?,"
    "high_=?,"
    "low_=?,"
    "close_=?,"
    "volume=?,"
    "MACD=?,"
    "MACDAvg=?,"
    "MACDDiff=?,"
    "RSI=?,"
    "SlowK=?,"
    "SlowD=?,"
    "FastAvg=?,"
    "SlowAvg=?,"
    "DistF=?,"
    "DistS=?,"
    "FGS=?,"
    "FLS=?,"
    "OpenBar=?"
  " WHERE date_=? AND time_=?";

// Update _parent with rowid of parent table to indexing the resolution
QStringList SQLiteHandler::SQL_UPDATE_BARDATA_PARENT_INDEX_V2(const QString &parent_database, const int &weight, const int &start_rowid) {
  QString child_date = TABLE_NAME_BARDATA + "." + COLUMN_NAME_DATE;
  QString child_time = TABLE_NAME_BARDATA + "." + COLUMN_NAME_TIME;
  QString child_start_from_rowid = start_rowid > 0? "ROWID>"+ QString::number(start_rowid)+" AND ":"";
  QString child_parent_equal_to_zero = COLUMN_NAME_IDPARENT + "=0";
  QString child_parent_monthly_equal_to_zero = COLUMN_NAME_IDPARENT_MONTHLY + "=0";
  QString parent_table = parent_database + "." + TABLE_NAME_BARDATA + " B ";
  QString parent_date = "B." + COLUMN_NAME_DATE;
  QString parent_time = "B." + COLUMN_NAME_TIME;
  // Derived column in temporary table
  QString parent_rowid = "B._ROWID";
  QString parent_date_LB = "B.date_LB";
  QString parent_time_LB = "B.time_LB";
  QString parent_dateprev = "B.date_prev";
  QString parent_monthyear = "B.date_MMYYYY";
  QString parent_intersect_prev_day = "B.intersect_prev_day";
  QStringList sql;

  // Weekly, Daily, 60min
  if (weight >= 1440) {
    /**
     * 1. BASE CONSTRAINT
     *    (child_date > parent_date_LB) AND (child_date <= parent_date) AND
     *    (child_time <= parent_time)
     *
     * 2. PREVIOUS DAY AFTER CLOSING HOUR
     *    (child_date = parent_date_LB) AND (child_time > parent_time)
     */
    sql +=
      "UPDATE " + TABLE_NAME_BARDATA +
      " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
      " FROM " + parent_table +
      " WHERE " +
        child_date + "<=" + parent_date + " AND " +
        child_date + ">"  + parent_date_LB + " AND " +
        child_time + "<=" + parent_time + " LIMIT 1)"
      " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
      " AND EXISTS(SELECT 1 FROM " + parent_table +
      " WHERE " +
        child_date + "<=" + parent_date + " AND " +
        child_date + ">"  + parent_date_LB + " AND " +
        child_time + "<=" + parent_time + " LIMIT 1);";

    sql +=
      "UPDATE " + TABLE_NAME_BARDATA +
      " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
      " FROM " + parent_table +
      " WHERE " +
        child_date + "=" + parent_date_LB + " AND " +
        child_time + ">" + parent_time + " LIMIT 1)"
      " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
      " AND EXISTS(SELECT 1 FROM " + parent_table +
      " WHERE " +
        child_date + "=" + parent_date_LB + " AND " +
        child_time + ">" + parent_time + " LIMIT 1);";

    // Weekly
    if (weight >= 43200) {
      /** Monthly: check child_MMYYYY = parent_MMYYYY */
      sql +=
        "UPDATE " + TABLE_NAME_BARDATA +
        " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
        " FROM " + parent_table +
        " WHERE strftime('%m%Y'," + child_date + ")=" + parent_monthyear + " LIMIT 1)"
        " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
        " AND EXISTS(SELECT 1 FROM " + parent_table +
        " WHERE strftime('%m%Y'," + child_date + ")=" + parent_monthyear + " LIMIT 1);";
    }
  }
  // 5min, 1min
  else {
    /**
     * Because one-shot query is slow, we distribute the constraint into 3 query.
     * The query sequence should begin with highest affected rows.
     * Tip: put most tight condition first check.
     *
     * 1. BASE CONSTRAINT
     *    (child_date = parent_date) AND
     *    (child_time > parent_time_LB) AND (child_time <= parent_time)
     *
     * 2. PREVIOUS DAY INTERSECT
     *    (child_date = parent_prev_date) AND
     *    (parent_time_LB > parent_time) AND (child_time > parent_time_LB)
     *
     * 3. EQUAL TIME
     *    (child_date = parent_date) AND (child_time = parent_time)
     */

    // Equal time
    sql +=
      "UPDATE " + TABLE_NAME_BARDATA +
      " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
      " FROM " + parent_table +
      " WHERE " +
      child_date + "=" + parent_date + " AND " +
      child_time + "=" + parent_time + " LIMIT 1)"
      " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
      " AND EXISTS(SELECT 1 FROM " + parent_table +
      " WHERE " +
      child_date + "=" + parent_date + " AND " +
      child_time + "=" + parent_time + " LIMIT 1);";

    // Base constraint
    sql +=
      "UPDATE " + TABLE_NAME_BARDATA +
      " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
      " FROM " + parent_table +
      " WHERE " +
      child_date + "="  + parent_date + " AND " +
      child_time + ">"  + parent_time_LB + " AND " +
      child_time + "<=" + parent_time + " LIMIT 1)"
      " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
      " AND EXISTS(SELECT 1 FROM " + parent_table +
      " WHERE " +
      child_date + "="  + parent_date + " AND " +
      child_time + ">"  + parent_time_LB + " AND " +
      child_time + "<=" + parent_time + " LIMIT 1);";

    // TODO: improve this query, this is bottleneck for refresh
    // Previous day intersect
//    sql +=
//      "UPDATE " + TABLE_NAME_BARDATA +
//      " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
//      " FROM " + parent_table +
//      " WHERE " +
//      parent_time_LB + ">" + parent_time + " AND " +
//      child_time     + ">" + parent_time_LB + " AND " +
//      child_date     + "=" + parent_dateprev + " LIMIT 1)"
//      " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
//      " AND EXISTS(SELECT 1 FROM " + parent_table +
//      " WHERE " +
//      parent_time_LB + ">" + parent_time + " AND " +
//      child_time     + ">" + parent_time_LB + " AND " +
//      child_date     + "=" + parent_dateprev + " LIMIT 1);";

    // Previous day intersect V2
    sql +=
      "UPDATE " + TABLE_NAME_BARDATA +
      " SET "+ COLUMN_NAME_IDPARENT + "=(SELECT " + parent_rowid +
      " FROM " + parent_table +
      " WHERE " +
      parent_intersect_prev_day + "=1 AND " +
      child_date + "=" + parent_dateprev + " AND " +
      child_time + ">" + parent_time_LB + " LIMIT 1)"
      " WHERE " + child_start_from_rowid + child_parent_equal_to_zero +
      " AND EXISTS(SELECT 1 FROM " + parent_table +
      " WHERE " +
      parent_intersect_prev_day + "=1 AND " +
      child_date + "=" + parent_dateprev + " AND " +
      child_time + ">" + parent_time_LB + " LIMIT 1);";
  }

  // _parent_monthly
  sql +=
    "update " + TABLE_NAME_BARDATA +
    " set "+ COLUMN_NAME_IDPARENT_MONTHLY + "="
    "(select rowid from dbMonthly.bardata a " +
    " where strftime('%m%Y'," + child_date + ")=strftime('%m%Y',a.date_) LIMIT 1)"
    " where " + child_start_from_rowid + child_parent_monthly_equal_to_zero + " and" +
    " exists(select 1 from dbMonthly.bardata a " +
    " where strftime('%m%Y'," + child_date + ")=strftime('%m%Y',a.date_) LIMIT 1);";

  return sql;
}


//
// Resistance and Support Function
//
const QString SQLiteHandler::SQL_INSERT_RESISTANCE_V1 =
  "INSERT OR IGNORE INTO " + TABLE_NAME_RESISTANCE +
  "(date_, time_, react_date, react_time, resistance_count, resistance,"
   "resistance_duration, last_duration, id_threshold)"
  " VALUES(?,?,?,?,?, ?,?,?,?)";

const QString SQLiteHandler::SQL_INSERT_SUPPORT_V1 =
  "INSERT OR IGNORE INTO " + TABLE_NAME_SUPPORT +
  "(date_, time_, react_date, react_time, support_count, support,"
  "support_duration, last_duration, id_threshold)"
  " VALUES(?,?,?,?,?, ?,?,?,?)";


//
// DistF and DistS Function
//
/*QString SQLiteHandler::SQL_SELECT_DISTF_ASC_FROM_YEAR(const int &year) {
  return "select date_, time_ from " + TABLE_NAME_BARDATA +
         " where date_>='" + QString::number(year) + "-01-01' order by DistF ASC";
}*/

/*QString SQLiteHandler::SQL_SELECT_DISTS_ASC_FROM_YEAR(const int &year) {
  return "select date_, time_ from " + TABLE_NAME_BARDATA +
         " where date_>='" + QString::number(year) + "-01-01' order by DistS ASC";
}*/

/*QString SQLiteHandler::SQL_SELECT_COUNT_FROM_YEAR(int year) {
  return "select count(1) from " + TABLE_NAME_BARDATA +
         " where date_>='" + QString::number(year) + "-01-01'";
}*/

/** Select Bar from d-Date and t-Time backward to previous n-bars */
QString SQLiteHandler::SQL_SELECT_BARDATA_LIMIT(const QDate &d, const QTime &t, const int &limit) {
  QString date_value = d.toString("yyyy-MM-dd");
  QString time_value = t.toString("HH:mm");
  return
    "SELECT * FROM " + TABLE_NAME_BARDATA +
    " WHERE " +
        COLUMN_NAME_DATE + "<" + date_value + " OR(" +
        COLUMN_NAME_DATE + "=" + date_value + " AND " +
        COLUMN_NAME_TIME + "<=" + time_value +
    ")ORDER BY t_date DESC, t_time DESC"
    " LIMIT " + QString::number(limit);
}

QString SQLiteHandler::SQL_ROWCOUNT_WHERE(const QString &tableName, const QString &whereArgs) {
  QString sql = "SELECT COUNT(1) FROM " + tableName;
  if (!whereArgs.isEmpty()) sql += " WHERE " + whereArgs;
  return sql;
}

QString SQLiteHandler::SQL_SELECT_EXISTS(const QString &tableName, const QString &whereArgs) {
  return "SELECT 1 FROM " + tableName + " WHERE " + whereArgs + " LIMIT 1;";
}

QString SQLiteHandler::SQL_SELECT_BARDATA_MAXDATETIME(QString whereArgs = "") {
  if (!whereArgs.isEmpty()) whereArgs = " where " + whereArgs;
  return
    "select " + COLUMN_NAME_DATE + "||' '||" + COLUMN_NAME_TIME +
    " from " + TABLE_NAME_BARDATA + whereArgs +
    " order by " + COLUMN_NAME_DATE + " desc," + COLUMN_NAME_TIME + " desc limit 1;";
}

QString SQLiteHandler::SQL_SELECT_BARDATA_MINDATETIME(QString whereArgs = "") {
  if (!whereArgs.isEmpty()) whereArgs = " where " + whereArgs;
  return
    "select " + COLUMN_NAME_DATE + "||' '||" + COLUMN_NAME_TIME +
    " from " + TABLE_NAME_BARDATA + whereArgs +
    " order by " + COLUMN_NAME_DATE + " asc," + COLUMN_NAME_TIME + " asc limit 1;";
}

QString SQLiteHandler::SQL_SELECT_DATETIME(const QDate &d, const QTime &t) {
  return
    "select * from " + TABLE_NAME_BARDATA + " where " +
    COLUMN_NAME_DATE + "<'" + d.toString("yyyy-MM-dd") + "' or (" +
    COLUMN_NAME_DATE + "='" + d.toString("yyyy-MM-dd") + "' and "+
    COLUMN_NAME_TIME + "<='" + t.toString("HH:mm:ss") + "')"+
    " order by date_ desc, time_ desc";
}

QString SQLiteHandler::SQL_SELECT_BARDATA_BUILDER(const QStringList &projection, const QString &where_args) {
  QString sql;
  if (projection.isEmpty()) {
    sql = "SELECT " + COLUMN_NAME_ROWID + ",* FROM " + TABLE_NAME_BARDATA;
  } else {
    sql = "SELECT " + projection.join(",") + " FROM " + TABLE_NAME_BARDATA;
  }
  if (!where_args.isEmpty())
    sql += " WHERE " + where_args;
  return sql;
}

// main function for sql string builder
QString SQLiteHandler::SQL_SELECT_BARDATA_BUILDER_JOIN(
    const QStringList &projection,
    const QStringList &databaseAlias,
    const QString &where_args,
    const QStringList &SRjoinAlias,
    const QVector<bool> &SRjoin,
    const QString &target_alias, // TODO: remove this args in next version
    const bool &count_query) {
  QString sql;
  QString resistance_table = TABLE_NAME_RESISTANCE;
  QString support_table = TABLE_NAME_SUPPORT;

  if (count_query) {
    sql = "SELECT COUNT(DISTINCT " + target_alias + ".ROWID) FROM ";
  }
  else {
    if (projection.isEmpty()) {
      sql = "SELECT " + COLUMN_NAME_ROWID + ",* FROM ";
    } else {
      sql = "SELECT " + projection.join(",") + " FROM ";
//      sql = "SELECT " + target_alias + ".ROWID," + projection.join(",") + " FROM ";
    }
  }

  int start_idx = 0;
  sql += TABLE_NAME_BARDATA + " A";

  QString curr_table_name;
  QString prev_table_name;
  QString prev_alias = "A";

  for (int i = databaseAlias.length()-1; i >= 1 ; --i) {
    curr_table_name = databaseAlias[i] + "." + TABLE_NAME_BARDATA;

    if (databaseAlias[start_idx].isEmpty()) {
      prev_table_name = "A";
    } else {
      prev_table_name = databaseAlias[start_idx] + "." + TABLE_NAME_BARDATA;
    }

    /*if (curr_table_name.contains("Monthly")) {
      sql += " LEFT JOIN " + curr_table_name + " ON " + curr_table_name + "." + COLUMN_NAME_ROWID + "=A." + COLUMN_NAME_IDPARENT_PREV_MONTHLY;
//      sql += " LEFT JOIN " + curr_table_name + " ON " + curr_table_name + "." + COLUMN_NAME_ROWID + "=A." + COLUMN_NAME_IDPARENT_MONTHLY;
//      sql += " LEFT JOIN " + curr_table_name + " ON " + curr_table_name + "." + COLUMN_NAME_ROWID +
//             "=(CASE WHEN A." + COLUMN_NAME_IDPARENT_MONTHLY + ">0 THEN " +
//                         "A." + COLUMN_NAME_IDPARENT_MONTHLY + " ELSE " +
//                         "A." + COLUMN_NAME_IDPARENT_PREV_MONTHLY + " END)";
    } else {
      sql += " LEFT JOIN " + curr_table_name + " ON " + curr_table_name + "." + COLUMN_NAME_ROWID + "=" + prev_table_name + "." + COLUMN_NAME_IDPARENT_PREV;
//      sql += " LEFT JOIN " + curr_table_name + " ON " + curr_table_name + "." + COLUMN_NAME_ROWID + "=" + prev_table_name + "." + COLUMN_NAME_IDPARENT;
//      sql += " LEFT JOIN " + curr_table_name + " ON " + curr_table_name + "." + COLUMN_NAME_ROWID +
//             "=(CASE WHEN "+ prev_table_name + "." + COLUMN_NAME_IDPARENT + ">0 THEN " +
//                             prev_table_name + "." + COLUMN_NAME_IDPARENT + " ELSE " +
//                             prev_table_name + "." + COLUMN_NAME_IDPARENT_PREV + " END)";
    }*/

    // for intersect price
    prev_table_name = prev_alias;

    if (curr_table_name.contains("Daily")) prev_alias = "R1";
    else if (curr_table_name.contains("Weekly")) prev_alias = "R2";
    else if (curr_table_name.contains("Monthly")) prev_alias = "R3";
    else if (curr_table_name.contains("60min")) prev_alias = "R4";
    else if (curr_table_name.contains("5min")) prev_alias = "R5";

    if (curr_table_name.contains("Monthly")) {
      sql += " LEFT JOIN " + curr_table_name + " " + prev_alias + " ON " +
             prev_alias + "." + COLUMN_NAME_ROWID + "=A." + COLUMN_NAME_IDPARENT_MONTHLY;
    } else {
      sql += " LEFT JOIN " + curr_table_name + " " + prev_alias + " ON " +
             prev_alias + "." + COLUMN_NAME_ROWID + "=" + prev_table_name + "." + COLUMN_NAME_IDPARENT;
    }

    start_idx = i;
  }

  QString SRAlias = NULL;
  QStringList SAlias;
  QStringList RAlias;
  QString Pre_SRAlias;
  int N = SRjoin.size();
  bool resistance_flag = false;
  bool support_flag = false;

  for (int i = 0; i < N; ++i) {
    // multi interval
    if (!SRjoinAlias[i].isEmpty() && !SRjoinAlias[i].startsWith("#")) {
      SRAlias = SRjoinAlias[i] + ".";
      if (SRjoinAlias[i] == "dbDaily") Pre_SRAlias = "R1";
      else if (SRjoinAlias[i] == "dbWeekly") Pre_SRAlias = "R2";
      else if (SRjoinAlias[i] == "dbMonthly") Pre_SRAlias = "R3";

      if (SRjoin[i]) {
        if (!RAlias.contains(SRjoinAlias[i])) {
          sql += " LEFT JOIN " + SRAlias + resistance_table + " ON " +
            SRAlias + resistance_table + "." + COLUMN_NAME_DATE + "=" + Pre_SRAlias + "." + COLUMN_NAME_PREV_DATE + " AND " +
            SRAlias + resistance_table + "." + COLUMN_NAME_TIME + "=" + Pre_SRAlias + "." + COLUMN_NAME_PREV_TIME;
          RAlias.push_back(SRjoinAlias[i]);
        }
      } else {
        if (!SAlias.contains(SRjoinAlias[i])) {
          sql += " LEFT JOIN " + SRAlias + support_table + " ON " +
            SRAlias + support_table + "." + COLUMN_NAME_DATE + "=" + Pre_SRAlias + "." + COLUMN_NAME_PREV_DATE + " AND " +
            SRAlias + support_table + "." + COLUMN_NAME_TIME + "=" + Pre_SRAlias + "." + COLUMN_NAME_PREV_TIME;
          SAlias.push_back(SRjoinAlias[i]);
        }
      }
    }
    // single interval
    else {
      if (SRjoin[i] & !resistance_flag) {
        sql += " LEFT JOIN " + resistance_table + " RT ON " +
               "RT." + COLUMN_NAME_DATE + "=A." + COLUMN_NAME_PREV_DATE + " AND " +
               "RT." + COLUMN_NAME_TIME + "=A." + COLUMN_NAME_PREV_TIME;
        resistance_flag = true;
      } else if (!support_flag) {
        sql += " LEFT JOIN " + support_table + " ST ON " +
               "ST." + COLUMN_NAME_DATE + "=A." + COLUMN_NAME_PREV_DATE + " AND " +
               "ST." + COLUMN_NAME_TIME + "=A." + COLUMN_NAME_PREV_TIME;
        support_flag = true;
      }
    }
  }

  if (!where_args.isEmpty()) {
    sql += " WHERE " + where_args + " AND " + target_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID + " IS NOT NULL";
  }

  if (!count_query) {
    sql += " GROUP BY " + target_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID;
  }

  sql += " ORDER BY " + target_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID + " ASC";
  return sql;
}

/*QString SQLiteHandler::SQL_SELECT_RESISTANCE_BY_DATE(const int &index, const QDate &date) {
  QString d = date.toString("yyyy-MM-dd");
  QString table_name = TABLE_NAME_RESISTANCE;
  QString sql;
  sql = "select distinct " + COLUMN_NAME_RESISTANCE_VALUE +
        " from " + table_name +
        " where id_threshold=" + QString::number(index) + " and " + COLUMN_NAME_DATE + "='" + d + "'"
        " order by " + COLUMN_NAME_RESISTANCE_VALUE + " DESC";
  return sql;
}*/

/*QString SQLiteHandler::SQL_SELECT_SUPPORT_BY_DATE(const int &index, const QDate &date) {
  QString d = date.toString("yyyy-MM-dd");
  QString table_name = TABLE_NAME_SUPPORT;
  QString sql;
  sql = "select distinct " + COLUMN_NAME_SUPPORT_VALUE +
        " from " + table_name +
        " where id_threshold=" + QString::number(index) + " and " + COLUMN_NAME_DATE + "='" + d + "'"
        " order by " + COLUMN_NAME_SUPPORT_VALUE + " DESC";
  return sql;
}*/

/*QString SQLiteHandler::SQL_SELECT_RESISTANCE_BY_DATETIME(const int &index, const QDate &date, const QTime &time) {
  QString d = date.toString("yyyy-MM-dd");
//  QString t = time.toString("hh:mm");
  QString table_name;
  QString sql;
  switch (index) {
    case 0: table_name = TABLE_NAME_RESISTANCE; break;
    case 1: table_name = TABLE_NAME_RESISTANCE_1; break;
    case 2: table_name = TABLE_NAME_RESISTANCE_2; break;
    case 3: table_name = TABLE_NAME_RESISTANCE_3; break;
    case 4: table_name = TABLE_NAME_RESISTANCE_4; break;
    default: table_name = TABLE_NAME_RESISTANCE;
  }
  sql = "select distinct " + COLUMN_NAME_RESISTANCEVALUE +
        " from " + table_name +
        " where " + COLUMN_NAME_DATE + "='" + d + "'"
        " order by " + COLUMN_NAME_RESISTANCEVALUE + " DESC";
  return sql;
}*/

/*QString SQLiteHandler::SQL_SELECT_SUPPORT_BY_DATETIME(const int &index, const QDate &date, const QTime &time) {
  QString d = date.toString("yyyy-MM-dd");
//  QString t = time.toString("hh:mm");
  QString table_name;
  QString sql;
  switch (index) {
    case 0: table_name = TABLE_NAME_SUPPORT; break;
    case 1: table_name = TABLE_NAME_SUPPORT_1; break;
    case 2: table_name = TABLE_NAME_SUPPORT_2; break;
    case 3: table_name = TABLE_NAME_SUPPORT_3; break;
    case 4: table_name = TABLE_NAME_SUPPORT_4; break;
    default: table_name = TABLE_NAME_SUPPORT;
  }
  sql = "select distinct " + COLUMN_NAME_SUPPORTVALUE +
        " from " + table_name +
        " where " + COLUMN_NAME_DATE + "='" + d + "'"
        " order by " + COLUMN_NAME_SUPPORTVALUE + " DESC";
  return sql;
}*/

/*QString SQLiteHandler::SQL_SELECT_PID_BY_DATETIME(const QDate &date, const QTime &time) {
  QString d = date.toString("yyyy-MM-dd");
  QString t = time.toString("hh:mm");
  return "select " + COLUMN_NAME_IDPARENT + " from " + TABLE_NAME_BARDATA +
         " where " + COLUMN_NAME_DATE + "='" + d + "' and " + COLUMN_NAME_TIME + "='" + t + "'";
}*/

/*QString SQLiteHandler::SQL_SELECT_DATETIME_BY_RID(const int &pid) {
  return "select " + COLUMN_NAME_DATE + "," + COLUMN_NAME_TIME+ " from " + TABLE_NAME_BARDATA +
         " where " + COLUMN_NAME_ROWID + "='" + QString::number(pid) + "'";
}*/
