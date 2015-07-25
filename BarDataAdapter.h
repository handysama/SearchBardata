#ifndef BARDATAADAPTER_H
#define BARDATAADAPTER_H

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QQueue>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>
#include <QTime>
#include <QVector>
#include "BarDataList.h"
#include "SQLiteHandler.h"
#include "XmlConfigHandler.h"

#define MA_LINE_FASTLENGTH  10
#define MA_LINE_SLOWLENGTH  50
#define MACD_FASTLENGTH     12
#define MACD_SLOWLENGTH     26
#define MACD_LENGTH         9
#define STOCHASTIC_LENGTH   14

class BarDataAdapter {
  public:
    BarDataAdapter() {
      std::srand(QTime::currentTime().msec());
      _rid = std::rand() % 10000;
      init_sql_insert();
    }

    ~BarDataAdapter() {
      reset();
    }

    long loadFile(const QSqlDatabase &db, const QString &database_path, const QString &file_path,
                  const QString &symbol, const IntervalWeight &base_interval) {
      QFile file(file_path);
      long numOfRows = 0;

      if (file.open(QFile::ReadOnly)) {
        t_bardata last_record = getlastRecord(db);
        QDateTime last_datetime = selectMaxDate(db);
        QDateTime curr_datetime;
        QQueue<double> qTR;
        QStringList barColor;
        QStringList cells;
        QString date_value;
        QString time_value;
        QString prevdate_string = "";
        QString prevtime_string = "";
        QTextStream in(&file);
        double f_current;
        double s_current;
        double f_prev = -1;
        double s_prev = -1;
        double close_prev = 0;
        double close_fs = 0;
        double distFS_current = 0;
        double distFS_prev = 0;
        double sum_TR = 0;
        double m_open;
        double m_high;
        double m_low;
        double m_close;
        double m_fastavg;
        double m_slowavg;
        int counter = 0;
        int last_rsi_g70 = 0;
        int last_rsi_l30 = 0;
        int last_macd_g0 = 0;
        int last_macd_l0 = 0;
        int last_slowk_g80 = 0;
        int last_slowk_l20 = 0;
        int last_slowd_g80 = 0;
        int last_slowd_l20 = 0;
        int last_cgf = 0;
        int last_clf = 0;
        int last_cgs = 0;
        int last_cls = 0;

        if (last_record.rowid > 0) {
          barColor = last_record.prevbarcolor.split(",");
          barColor.pop_front();
          if (last_record.open < last_record.close) {
            barColor.append("G");
          } else if (last_record.open > last_record.close) {
            barColor.append("R");
          } else {
            barColor.append("D");
          }

          close_prev = last_record.close;
          last_rsi_g70 = last_record.rsi_g70;
          last_rsi_l30 = last_record.rsi_l30;
          last_macd_g0 = last_record.macd_g0;
          last_macd_l0 = last_record.macd_l0;
          last_slowk_g80 = last_record.slowk_g80;
          last_slowk_l20 = last_record.slowk_l20;
          last_slowd_g80 = last_record.slowd_g80;
          last_slowd_l20 = last_record.slowd_l20;
          last_cgf = last_record.cgf;
          last_clf = last_record.clf;
          last_cgs = last_record.cgs;
          last_cls = last_record.cls;
          f_prev = last_record.fastavg;
          s_prev = last_record.slowavg;
          distFS_prev = f_prev - s_prev;
        }

        in.readLine(); // discard first line

        QString parent_alias = "dbParent";
        IntervalWeight parent_interval = SQLiteHandler::get_parent_interval(base_interval);

        QSqlQuery qParent(db);
        qParent.setForwardOnly(true);

        if (base_interval < WEIGHT_MONTHLY) {
          QString local_database_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, parent_interval);
          qParent.exec("select max(_parent) from bardata"); qParent.next();
          int last_parent_id = qParent.value(0).toInt();
          qParent.exec("ATTACH DATABASE '" + local_database_path + "' AS " + parent_alias);
          qParent.exec("select rowid, date_, time_ from " + parent_alias + ".bardata" +
                       " where rowid>=" + QString::number(last_parent_id) + " order by rowid asc");
          qParent.next();
          qDebug() << "index from parent_id" << last_parent_id;
        }

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.exec("PRAGMA journal_mode = OFF;");
        query.exec("PRAGMA synchronous = OFF;");
        query.exec("PRAGMA cache_size = 100000;");
        query.exec("BEGIN TRANSACTION;");

        while (!in.atEnd()) {
          cells = in.readLine().split(",", QString::KeepEmptyParts);
          date_value = cells[0].trimmed();
          time_value = cells[1].trimmed();
          curr_datetime = QDateTime::fromString(date_value + " " + time_value, "MM/dd/yyyy hh:mm");

          // discard existing records
          if (last_datetime < curr_datetime) {
            date += QDate::fromString(date_value, "MM/dd/yyyy");
            time += time_value;
            open += cells[2].trimmed().toDouble();
            high += cells[3].trimmed().toDouble();
            low += cells[4].trimmed().toDouble();
            close += cells[5].trimmed().toDouble();
            volume += cells[6].trimmed().toLongLong();
            macd += cells[7].trimmed().toDouble();
            macdavg += cells[8].trimmed().toDouble();
            macddiff += cells[9].trimmed().toDouble();
            rsi += cells[10].trimmed().toDouble();
            slowk += cells[11].trimmed().toDouble();
            slowd += cells[12].trimmed().toDouble();
            fastavg += cells[13].trimmed().toDouble();
            slowavg += cells[14].trimmed().toDouble();
            distf += cells[15].trimmed().toDouble();
            dists += cells[16].trimmed().toDouble();
            fgs += cells[17].trimmed().toInt();
            fls += cells[18].trimmed().toInt();
            openbar += cells[19].trimmed().toInt();

            // indexing on-the-fly
            if (base_interval < WEIGHT_MONTHLY && qParent.isValid()) {
              QDate parent_date = QDate::fromString(qParent.value(1).toString(), "yyyy-MM-dd");
              QTime parent_time = QTime::fromString(qParent.value(2).toString(), "HH:mm");
              QDate child_date = QDate::fromString(date.last().toString(), "yyyy-MM-dd");
              QTime child_time = QTime::fromString(time.last().toString(), "HH:mm");
              QString rowid = qParent.value(0).toString();

              while ((parent_date < child_date || (parent_date == child_date && parent_time < child_time))
                     && qParent.next()) {
                rowid = qParent.value(0).toString();
                parent_date = QDate::fromString(qParent.value(1).toString(), "yyyy-MM-dd");
                parent_time = QTime::fromString(qParent.value(2).toString(), "HH:mm");
              }

              QDateTime child_datetime(child_date, child_time);
              QDateTime parent_LB(parent_date, parent_time);
              QDateTime parent_UB(parent_date, parent_time);

              if (parent_interval >= WEIGHT_DAILY) {
                // Weekly, Daily
                parent_LB = parent_LB.addDays(- parent_interval / WEIGHT_DAILY);
              } else {
                // 60min, 5min
                parent_LB = parent_LB.addSecs(- parent_interval * 60);
              }

              if (child_datetime > parent_LB && child_datetime <= parent_UB) {
                _parent += rowid;
              } else {
                _parent += "0";
              }
            } else {
              _parent += "0";
            }

            // datetime bound
            /*
            datetime_ub += 0;
            datetime_lb += 0;
            datetime_ub += date.last().toDate().toString("yyyyMMdd") + time.last().toTime().toString("HHmm");
            dt = QDateTime::fromString(datetime_ub.last().toString(), "yyyyMMddHHmm");
            if (base_interval >= WEIGHT_DAILY) {
              dt = dt.addDays(-base_interval/WEIGHT_DAILY);
            } else {
              dt = dt.addSecs(-base_interval*60);
            }
            datetime_lb += dt.toString("yyyyMMddHHmm");
            */

            //
            // F-slope and S-Slope
            //
            f_current = fastavg.last().toDouble();
            s_current = slowavg.last().toDouble();

            if (f_prev != -1) {
              fastavgslope += QString::number(f_current - f_prev, 'f', 4);
              slowavgslope += QString::number(s_current - s_prev, 'f', 4);
            } else {
              fastavgslope += 0;
              slowavgslope += 0;
            }

            f_prev = f_current;
            s_prev = s_current;
            QString::number(1);

            m_fastavg = fastavg.last().toDouble();
            m_slowavg = slowavg.last().toDouble();
            m_open = open.last().toDouble();
            m_high = high.last().toDouble();
            m_low = low.last().toDouble();
            m_close = close.last().toDouble();

            distfs += QString::number(fabs(m_fastavg - m_slowavg), 'f', 4);
            n_distfs += QString::number(distfs.last().toDouble() / m_close, 'f', 8);

            distof += QString::number(m_open - m_fastavg, 'f', 4);
            distos += QString::number(m_open - m_slowavg, 'f', 4);
            n_distof += QString::number(distof.last().toDouble() / m_close, 'f', 8);
            n_distos += QString::number(distos.last().toDouble() / m_close, 'f', 8);

            disthf += QString::number(m_high - m_fastavg, 'f', 4);
            disths += QString::number(m_high - m_slowavg, 'f', 4);
            n_disthf += QString::number(disthf.last().toDouble() / m_close, 'f', 8);
            n_disths += QString::number(disths.last().toDouble() / m_close, 'f', 8);

            distlf += QString::number(m_low - m_fastavg, 'f', 4);
            distls += QString::number(m_low - m_slowavg, 'f', 4);
            n_distlf += QString::number(distlf.last().toDouble() / m_close, 'f', 8);
            n_distls += QString::number(distls.last().toDouble() / m_close, 'f', 8);

            distcf += QString::number(m_close - m_fastavg, 'f', 4);
            distcs += QString::number(m_close - m_slowavg, 'f', 4);
            n_distcf += QString::number(distcf.last().toDouble() / m_close, 'f', 8);
            n_distcs += QString::number(distcs.last().toDouble() / m_close, 'f', 8);

            //
            // Candle
            //
            candle_uptail += QString::number(fabs(m_high - std::max(m_open, m_close)), 'f', 4);
            candle_downtail += QString::number(fabs(std::min(m_open, m_close) - m_low), 'f', 4);
            candle_body += QString::number(fabs(m_open - m_close), 'f', 4);
            candle_totallength += QString::number(fabs(m_high - m_low), 'f', 4);

            //
            // ATR
            //
            if (close_prev > -1) {
              double tr_1 = m_high - m_low;
              double tr_2 = fabs(m_high - close_prev);
              double tr_3 = fabs(m_low - close_prev);
              if (qTR.size() + 1 > 14) {
                sum_TR -= qTR.first();
                qTR.pop_front();
              }
              qTR.enqueue(fmax(fmax(tr_1, tr_2), tr_3));
              sum_TR += qTR.last();
              atr +=  QString::number(sum_TR / qTR.size(),'f', 4);
            } else {
              atr += 0;
            }

            //
            // Displacement CC since FS Cross
            //
            distFS_current = m_fastavg - m_slowavg;

            if (distFS_current * distFS_prev < 0 || (date.size() > 1 && distFS_prev == 0 && distFS_current != 0)) {
              close_fs = close_prev;

              if (base_interval >= WEIGHT_DAILY) {
                dist_fscross += QString::number(m_close - close_fs, 'f', 4);
                dist_fscross_atr += QString::number((m_close - close_fs) / atr.last().toDouble(),'f',4);
              } else {
                //
                // update intra-day, divide by daily ATR later
                //
                dist_fscross += QString::number(m_close - close_fs, 'f', 4);
                dist_fscross_atr += QString::number(m_close - close_fs, 'f', 4);
              }

            } else {
              if (close_fs > 0) {

                if (base_interval >= WEIGHT_DAILY) {                  
                  dist_fscross += QString::number(m_close - close_fs, 'f', 4);
                  dist_fscross_atr += QString::number((m_close - close_fs) / atr.last().toDouble(), 'f', 4);
                } else {
                  //
                  // update intra-day, divide by daily ATR later
                  //
                  dist_fscross += QString::number(m_close - close_fs,'f',4);
                  dist_fscross_atr += QString::number(m_close - close_fs,'f',4);
                }
              } else {
                dist_fscross += 0;
                dist_fscross_atr += 0;
              }
            }

            distFS_prev = distFS_current;

            //
            // Prevdate and Prevtime
            //
            prevdate += prevdate_string;
            prevtime += prevtime_string;
            prevdate_string = QDate::fromString(date_value, "MM/dd/yyyy").toString("yyyy-MM-dd");
            prevtime_string = time_value;

            //
            // Barcolor
            //
            prevbarcolor += barColor.join(",");
            if (barColor.size() + 1 > 5) {
              barColor.pop_front();
            }

            if (m_close > m_open) {
              barColor.push_back("G");
            } else if (m_close < m_open) {
              barColor.push_back("R");
            } else {
              barColor.push_back("D");
            }

            //
            // FCross - SCross
            //
            fcross += (m_fastavg >=  m_low && m_fastavg <= m_high) ? 1 : 0;
            scross += (m_slowavg >=  m_low && m_slowavg <= m_high) ? 1 : 0;

            //
            // RSI > 70 and RSI < 30
            //
//            if (rsi.last().toDouble() > 70.0) {
//              rsi_g70 += (++last_rsi_g70);
//              rsi_l30 += 0;
//              last_rsi_l30 = 0;
//            } else if (rsi.last().toDouble() < 30.0) {
//              rsi_l30 += (++last_rsi_l30);
//              rsi_g70 += 0;
//              last_rsi_g70 = 0;
//            } else {
//              rsi_g70 += 0;
//              rsi_l30 += 0;
//              last_rsi_g70 = 0;
//              last_rsi_l30 = 0;
//            }

            //
            // MACD > 0 and MACD < 0
            //
//            if (macd.last().toDouble() > 0) {
//              macd_g0 += (++last_macd_g0);
//              macd_l0 += 0;
//              last_macd_l0 = 0;
//            } else if (macd.last().toDouble() < 0) {
//              macd_l0 += (++last_macd_l0);
//              macd_g0 += 0;
//              last_macd_g0 = 0;
//            } else {
//              macd_g0 += 0;
//              macd_l0 += 0;
//              last_macd_g0 = 0;
//              last_macd_l0 = 0;
//            }

            //
            // SlowK < 20 and SlowK > 80
            //
//            if (slowk.last().toDouble() > 80.0) {
//              slowk_g80 += (++last_slowk_g80);
//              slowk_l20 += 0;
//              last_slowk_l20 = 0;
//            } else if (slowk.last().toDouble() < 20) {
//              slowk_l20 += (++last_slowk_l20);
//              slowk_g80 += 0;
//              last_slowk_g80 = 0;
//            } else {
//              slowk_g80 += 0;
//              slowk_l20 += 0;
//              last_slowk_g80 = 0;
//              last_slowk_l20 = 0;
//            }

            //
            // SlowD < 20 and SlowD > 80
            //
//            if (slowd.last().toDouble() > 80.0) {
//              slowd_g80 += (++last_slowd_g80);
//              slowd_l20 += 0;
//              last_slowd_l20 = 0;
//            } else if (slowd.last().toDouble() < 20) {
//              slowd_l20 += (++last_slowd_l20);
//              slowd_g80 += 0;
//              last_slowd_g80 = 0;
//            } else {
//              slowd_g80 += 0;
//              slowd_l20 += 0;
//              last_slowd_g80 = 0;
//              last_slowd_l20 = 0;
//            }

            //
            // Close > Fast and Close < Fast
            //
            if (m_close > m_fastavg) {
              cgf += (++last_cgf);
              clf += 0;
              last_clf = 0;
            } else if (m_close < m_fastavg) {
              clf += (++last_clf);
              cgf += 0;
              last_cgf = 0;
            } else {
              cgf += 0;
              clf += 0;
              last_cgf = 0;
              last_clf = 0;
            }

            //
            // Close > Slow and Close < Slow
            //
            if (m_close > m_slowavg) {
              cgs += (++last_cgs);
              cls += 0;
              last_cls = 0;
            } else if (m_close < m_slowavg) {
              cls += (++last_cls);
              cgs += 0;
              last_cgs = 0;
            } else {
              cgs += 0;
              cls += 0;
              last_cgs = 0;
              last_cls = 0;
            }

            close_prev = m_close;

            if (++counter >= 100000) {
              insertBatchSqlite_v2(&query);
              numOfRows += counter;
              counter = 0;
              reset();
            }
          }
        }

        file.close();

        if (counter > 0) {
          insertBatchSqlite_v2(&query);
          numOfRows += counter;
          reset();
        }

        query.exec("COMMIT;");

        //
        // _parent_monthly
        //
        if (base_interval < WEIGHT_MONTHLY) {
          QString local_database_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_MONTHLY);
          QString sql_update =
            "update bardata"
            " set _parent_monthly="
             "(select rowid from dbMonthly.bardata a"
             " where strftime('%m%Y',bardata.date_)=strftime('%m%Y',a.date_) LIMIT 1)"
            " where bardata._parent_monthly=0 and exists(select 1 from dbMonthly.bardata a"
             " where strftime('%m%Y',bardata.date_)=strftime('%m%Y',a.date_) LIMIT 1);";

          query.exec("ATTACH DATABASE '" + local_database_path + "' AS dbMonthly");
          query.exec("PRAGMA journal_mode = OFF;");
          query.exec("PRAGMA synchronous = OFF;");
          query.exec("PRAGMA cache_size = 50000;");
          query.exec("BEGIN TRANSACTION;");
          query.exec(sql_update);
          if (query.lastError().isValid()) qDebug() << query.lastError();
          query.exec("COMMIT;");
        }
      }

      return numOfRows;
    }

    /*long loadFile_v2(const QSqlDatabase &db, const QString &file_path) {
      long numOfRows = 0;
      QFile file(file_path);
      if (file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        QStringList cells;
        QDate date_value;
        QString time_value;

        int counter = 0;
        int update_counter = 0;
        in.readLine(); // discard first line

        while (!in.atEnd()) {
          cells = in.readLine().split(",", QString::KeepEmptyParts);
          date_value = QDate::fromString(cells[0].trimmed(), "MM/dd/yyyy");
          time_value = cells[1].trimmed(); // HH:mm

          if (isExists(db, date_value.toString("yyyy-MM-dd"), time_value)) {
            u_date += date_value;
            u_time += time_value;
            u_open += cells[2].trimmed().toDouble();
            u_high += cells[3].trimmed().toDouble();
            u_low += cells[4].trimmed().toDouble();
            u_close += cells[5].trimmed().toDouble();
            u_volume += cells[6].trimmed().toLongLong();
            u_macd += cells[7].trimmed().toDouble();
            u_macdavg += cells[8].trimmed().toDouble();
            u_macddiff += cells[9].trimmed().toDouble();
            u_rsi += cells[10].trimmed().toDouble();
            u_slowk += cells[11].trimmed().toDouble();
            u_slowd += cells[12].trimmed().toDouble();
            u_fastavg += cells[13].trimmed().toDouble();
            u_slowavg += cells[14].trimmed().toDouble();
            u_distf += cells[15].trimmed().toDouble();
            u_dists += cells[16].trimmed().toDouble();
            u_fgs += cells[17].trimmed().toInt();
            u_fls += cells[18].trimmed().toInt();
            u_openbar += cells[19].trimmed().toInt();
            ++update_counter;
          } else {
            date += date_value;
            time += time_value;
            open += cells[2].trimmed().toDouble();
            high += cells[3].trimmed().toDouble();
            low += cells[4].trimmed().toDouble();
            close += cells[5].trimmed().toDouble();
            volume += cells[6].trimmed().toLongLong();
            macd += cells[7].trimmed().toDouble();
            macdavg += cells[8].trimmed().toDouble();
            macddiff += cells[9].trimmed().toDouble();
            rsi += cells[10].trimmed().toDouble();
            slowk += cells[11].trimmed().toDouble();
            slowd += cells[12].trimmed().toDouble();
            fastavg += cells[13].trimmed().toDouble();
            slowavg += cells[14].trimmed().toDouble();
            distf += cells[15].trimmed().toDouble();
            dists += cells[16].trimmed().toDouble();
            fgs += cells[17].trimmed().toInt();
            fls += cells[18].trimmed().toInt();
            openbar += cells[19].trimmed().toInt();
            ++counter;
          }

          if (counter >= 30000) {
            insertBatchSqlite(db);
            numOfRows += counter;
            counter = 0;
            reset();
          }

          if (update_counter >= 30000) {
            updateBatchSqlite(db);
            numOfRows += update_counter;
            update_counter = 0;
            reset_update_list();
          }
        }

        file.close();

        if (counter > 0) {
          insertBatchSqlite(db);
          reset();
        }

        if (update_counter > 0) {
          updateBatchSqlite(db);
          reset_update_list();
        }

        numOfRows += counter + update_counter;
      }
      return numOfRows;
    }*/

    static bool isExists(const QSqlDatabase &database, const QString &date, const QString &time) {
      QSqlQuery query(database);
      QString sql = SQLiteHandler::SQL_SELECT_EXISTS(SQLiteHandler::TABLE_NAME_BARDATA,
        SQLiteHandler::COLUMN_NAME_DATE + "='" + date + "' and " +
        SQLiteHandler::COLUMN_NAME_TIME + "='" + time + "'");
      query.setForwardOnly(true);
      query.exec(sql);
      if (query.next()) {
        return query.value(0).toBool();
      }
      return false;
    }

    static BarDataList* selectList(const QSqlDatabase &database) {
      BarDataList *data = NULL;
      QSqlQuery query(database);
      query.setForwardOnly(true);
      if (query.exec("select * from " + SQLiteHandler::TABLE_NAME_BARDATA)) {
        data = new BarDataList();
        t_bardata* res;
        while (query.next()) {
          res = data->getNextItem();
          res->date = QDate::fromString(query.value(0).toString(),"yyyy-MM-dd");
          res->time = QTime::fromString(query.value(1).toString());
          res->open = query.value(2).toDouble();
          res->high = query.value(3).toDouble();
          res->low = query.value(4).toDouble();
          res->close = query.value(5).toDouble();
          res->volume = query.value(6).toLongLong();
          res->macd = query.value(7).toDouble();
          res->macdavg = query.value(8).toDouble();
          res->macddiff = query.value(9).toDouble();
          res->rsi = query.value(10).toDouble();
          res->slowk = query.value(11).toDouble();
          res->slowd = query.value(12).toDouble();
          res->fastavg = query.value(13).toDouble();
          res->slowavg = query.value(14).toDouble();
          res->distf = query.value(15).toDouble();
          res->dists = query.value(16).toDouble();
          res->fgs = query.value(17).toInt();
          res->fls = query.value(18).toInt();
          res->openbar = query.value(19).toBool();
          res->_parent = query.value(20).toInt();
        }
      }
      return data;
    }

    static QSqlQuery* selectDateBetween(const QSqlDatabase &db, const QDateTime &startDate, const QDateTime &endDate) {
      QDateTime d1 = startDate;
      QDateTime d2 = endDate;
      if (d1 > d2) {
        d1 = endDate;
        d2 = startDate;
      }
      QString dateValue1 = d1.date().toString(Qt::ISODate);
      QString timeValue1 = d1.time().toString("hh:mm");
      QString dateValue2 = d2.date().toString(Qt::ISODate);
      QString timeValue2 = d2.time().toString("hh:mm");
      QString colDate = SQLiteHandler::COLUMN_NAME_DATE;
      QString colTime = SQLiteHandler::COLUMN_NAME_TIME;
      QSqlQuery *query = new QSqlQuery(db);
      QString sql = "SELECT * FROM " + SQLiteHandler::TABLE_NAME_BARDATA + " WHERE "
          /** From */
          "((" + colDate + " = '" + dateValue1 + "' AND " +
                 colTime + " >=  '" + timeValue1 + "') OR " +
                 colDate + " > '" + dateValue1 + "') AND " +
          /** Until */
          "((" + colDate + " = '" + dateValue2 + "' AND " +
                 colTime + " <=  '" + timeValue2 + "') OR " +
                 colDate + " < '" + dateValue2 + "') ;";

      query->setForwardOnly(true);
      if (query->exec(sql)) {
        return query;
      }
      delete query;
      return NULL;
    }

    static QDateTime selectMaxDate(const QSqlDatabase &db, QString whereArgs = "") {
      if (!db.isOpenError()) {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.exec(SQLiteHandler::SQL_SELECT_BARDATA_MAXDATETIME(whereArgs));

        if (query.next()) {
          return QDateTime::fromString(query.value(0).toString(),"yyyy-MM-dd hh:mm");
        }
      }
      return QDateTime();
    }

    static QDateTime selectMinDate(const QSqlDatabase &db, QString whereArgs = "") {
      if (!db.isOpenError()) {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.exec(SQLiteHandler::SQL_SELECT_BARDATA_MINDATETIME(whereArgs));

        if (query.next()) {
          return QDateTime::fromString(query.value(0).toString(),"yyyy-MM-dd hh:mm");
        }
      }
      return QDateTime();
    }

    static double getMaxValue(const QSqlDatabase &database, const QString &column_name) {
      if (!database.isOpenError()) {
        QSqlQuery query(database);
        query.setForwardOnly(true);
        query.exec("select max(" + column_name + ") from bardata");
        return query.next()? query.value(0).toDouble() : 0;
      }
      return 0;
    }

    static double getMinValue(const QSqlDatabase &database, const QString &column_name) {
      if (!database.isOpenError()) {
        QSqlQuery query(database);
        query.setForwardOnly(true);
        query.exec("select min(" + column_name + ") from bardata");
        return query.next()? query.value(0).toDouble() : 0;
      }
      return 0;
    }

    static QString avg_name_by_interval(const IntervalWeight &interval, const int &length) {
      QString N = QString::number(length);
      if (interval == WEIGHT_MONTHLY) return "Month-" + N;
      if (interval == WEIGHT_WEEKLY) return "Week-" + N;
      if (interval == WEIGHT_DAILY) return "Day-" + N;
      if (interval == WEIGHT_60MIN) return "60-" + N;
      if (interval == WEIGHT_5MIN) return "5-" + N;
      if (interval == WEIGHT_1MIN) return "1-" + N;
      return "";
    }

    static IntervalWeight getIntervalWeightFromName(const QString &name) {
      QStringList str_split = name.split("-");
      if (str_split.size() > 1) {
        QString s = str_split[0].toLower();
        if (s == "month") return WEIGHT_MONTHLY;
        if (s == "week") return WEIGHT_WEEKLY;
        if (s == "day") return WEIGHT_DAILY;
        if (s == "60") return WEIGHT_60MIN;
        if (s == "5") return WEIGHT_5MIN;
        if (s == "1") return WEIGHT_1MIN;
      }
      return WEIGHT_INVALID;
    }

    void update_index_parent(const QSqlDatabase &child_database, const QString &symbol,
        const QString &parent_database_name, const int &parent_weight, bool enable_optimization = true) {
      QString monthly_database_path = QCoreApplication::applicationDirPath() + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_MONTHLY);
      QString parent_database_path = QCoreApplication::applicationDirPath() + "/" + parent_database_name;
      QString parentDatabase = "parent" + QTime::currentTime().toString("zzz");
      QString tempDatabase = "temp" + QTime::currentTime().toString("sszzz");
      QString sql_insert_select;
      QString date_modifier;
      QString time_modifier;
      QString sql_create_table =
        "CREATE TABLE IF NOT EXISTS " +
          tempDatabase + "." + SQLiteHandler::TABLE_NAME_BARDATA + "("
          "date_ TEXT NOT NULL,"
          "time_ TEXT NOT NULL,"
          "_rowid INTEGER NOT NULL,"
          "date_LB TEXT,"
          "time_LB TEXT,"
          "date_prev TEXT,"
          "date_MMYYYY TEXT,"
          "intersect_prev_day INTEGER DEFAULT 0,"
          "PRIMARY KEY(date_ ASC, time_ ASC));";

      QSqlQuery query(child_database);
      query.setForwardOnly(true);
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("ATTACH DATABASE '" + parent_database_path + "' AS " + parentDatabase);
      query.exec("ATTACH DATABASE '" + monthly_database_path + "' AS dbMonthly");
//      query.exec("ATTACH DATABASE ':memory:' AS " + tempDatabase);
      query.exec("ATTACH DATABASE '' AS " + tempDatabase);
      query.exec(sql_create_table);

      int parent_start_from_rowid = 0;
      int child_start_from_rowid = 0;

      if (enable_optimization) {
        /** Reduce parent records, by finding latest parent rowid in child */
        query.exec("SELECT MAX(_parent) FROM Bardata");
        parent_start_from_rowid = query.next()? query.value(0).toInt() : 0;

        /** Reduce child records, by finding last _parent rowid isnot zero */
        query.exec("SELECT ROWID FROM Bardata WHERE _parent=" + QString::number(parent_start_from_rowid));
        child_start_from_rowid = query.next()? query.value(0).toInt() : 0;
      }

      qDebug() << "Begin updating:" << child_database.databaseName();
      qDebug() << "Parent start rowid:" << parent_start_from_rowid;
      qDebug() << "Child start rowid:" << child_start_from_rowid;

      // query builder
      QStringList sql_update = SQLiteHandler::SQL_UPDATE_BARDATA_PARENT_INDEX_V2(tempDatabase, parent_weight, child_start_from_rowid);

      // Weekly
      if (parent_weight >= 43200) {
        date_modifier = "'-" + QString::number(parent_weight/43200) + " months'";
        sql_insert_select =
          "INSERT INTO "+ tempDatabase +".Bardata(date_, time_, date_LB, date_MMYYYY, _rowid)"
          " SELECT date_, time_, date(date_," + date_modifier + "), strftime('%m%Y',date_), rowid"
          " FROM " + parentDatabase+ ".Bardata "
          " WHERE ROWID >= " + QString::number(parent_start_from_rowid);
      }
      // Daily, 60min
      else if (parent_weight >= 1440) {
        date_modifier = "'-" + QString::number(parent_weight/1440) + " days'";
        sql_insert_select =
          "INSERT INTO "+ tempDatabase +".Bardata(date_, time_, date_LB, _rowid)"
          " SELECT date_, time_, date(date_," + date_modifier + "), rowid"
          " FROM " + parentDatabase+ ".Bardata "
          " WHERE ROWID >= " + QString::number(parent_start_from_rowid);
      }
      // 5min, 1min
      else {
        time_modifier = "'-" + QString::number(parent_weight) + " minutes'";
        sql_insert_select =
          "INSERT INTO "+ tempDatabase +".Bardata(date_, time_, time_LB, date_prev, _rowid, intersect_prev_day)"
          " SELECT date_, time_, time(time_," + time_modifier + "), date(date_,'-1 days'), rowid,"
                  "case when time(time_," + time_modifier + ") > time_ then 1 else 0 end "
          " FROM " + parentDatabase+ ".Bardata "
          " WHERE ROWID >= " + QString::number(parent_start_from_rowid);
      }

      /** prepare temp table */
      query.exec("PRAGMA journal_mode = OFF;");
      query.exec("PRAGMA synchronous = OFF;");
      query.exec("BEGIN TRANSACTION;");
      qDebug() << "insert temp:" << query.exec(sql_insert_select);
      if (query.lastError().isValid())
        qDebug() << "error indexing:" << query.lastError() << parent_database_name << child_database.databaseName();
      query.exec("COMMIT;");

      // Assert inserted records
//      query.exec("select count(1) from " + tempDatabase + ".bardata;"); query.next();
//      qDebug() << "Rowcount temp table:" << query.value(0).toInt();
//      qDebug() << "Update length:" << sql_update.length() << parent_database_name << child_database.databaseName();

      /** main update index */
      query.exec("PRAGMA journal_mode = OFF;");
      query.exec("PRAGMA synchronous = OFF;");
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("BEGIN TRANSACTION;");
      for (int i = 0; i < sql_update.length(); ++i) {
//        qDebug() << sql_update[i];
        qDebug() << "update("+QString::number(i)+"):" << query.exec(sql_update[i]);
        if (query.lastError().isValid()) {
          qDebug() << "error indexing:" << query.lastError() << parent_database_name << child_database.databaseName();
        }
      }
      query.exec("COMMIT;");
      query.clear();
    }

    /*void update_index_parent_v2(const QSqlDatabase &base_database, const QString &symbol, const IntervalWeight &interval) {
      QSqlQuery query(base_database);
      QSqlQuery qUpdate(base_database);
      QString parent_database = QCoreApplication::applicationDirPath() + "/" + SQLiteHandler::getDatabaseName(symbol, interval);
      QString parent_alias = "db" + SQLiteHandler::get_interval_name(interval);
      QString sql_update = "update bardata set _parent=? where rowid=?";
      QString sql_select;
      QVariantList rowid;
      QVariantList parent;

      qDebug() << "Update index:" << base_database.databaseName();
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + parent_database + "' AS " + parent_alias);
      query.exec("select max(_parent) from bardata");

      sql_select =
        "select a.rowid, b.rowid "
        " from bardata a join " + parent_alias + ".bardata b on "
        " a.datetime_ub>b.datetime_lb and a.datetime_ub<=b.datetime_ub"
        " where a._parent=0 and b.rowid>=" + (query.next()? query.value(0).toString():"0");

      query.exec(sql_select);
      if (query.lastError().isValid()) qDebug()<< "WTF" << query.lastError();

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (query.next()) {
        rowid.append(query.value(0).toString());
        parent.append(query.value(1).toString());

        if (rowid.size() >= 80000) {
          qUpdate.prepare(sql_update);
          qUpdate.addBindValue(parent);
          qUpdate.addBindValue(rowid);
          qUpdate.execBatch();
          rowid.clear();
          parent.clear();
        }
      }

      if (rowid.size() > 0) {
        qUpdate.prepare(sql_update);
        qUpdate.addBindValue(parent);
        qUpdate.addBindValue(rowid);
        qUpdate.execBatch();
        rowid.clear();
        parent.clear();
      }

      qUpdate.exec("COMMIT;");

      // TODO: update _parent_monthly

//      if (interval < WEIGHT_MONTHLY) {
//        sql_select =
//          "update bardata"
//          " set "+ SQLiteHandler::COLUMN_NAME_IDPARENT_MONTHLY + "="
//          "(select rowid from dbMonthly.bardata a " +
//          " where strftime('%m%Y'," + child_date + ")=strftime('%m%Y',a.date_) LIMIT 1)"
//          " where exists(select 1 from dbMonthly.bardata a " +
//          " where strftime('%m%Y'," + child_date + ")=strftime('%m%Y',a.date_) LIMIT 1);";

//        parent_database = QCoreApplication::applicationDirPath() + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_MONTHLY);
//        parent_alias = "dbMonthly";
//        query.exec("ATTACH DATABASE '" + parent_database + "' AS " + parent_alias);
//      }
    }*/

    static void update_index_parent_prev(const QSqlDatabase &base_database, const QString &database_path,
                                         const QString &symbol, const IntervalWeight &parent) {
      QSqlQuery query(base_database);
      QString base_bardata = SQLiteHandler::TABLE_NAME_BARDATA;
      QString parent_alias;
      QString parent_bardata;
      QString parent_database_path;
      QString sql_update;
      int start_from_rowid_base = 0;
      int start_from_rowid_parent = 0;
      bool enable_optimization = true;

      if (enable_optimization) {
        query.exec("select max(rowid) from bardata where _parent_prev>0");
        start_from_rowid_base = query.next()? query.value(0).toInt() : 0;

        query.exec("select _parent_prev from bardata where rowid=" + QString::number(start_from_rowid_base));
        start_from_rowid_parent = query.next()? query.value(0).toInt() : 0;
      }

      parent_alias = "db" + SQLiteHandler::get_interval_name(parent);
      parent_bardata = parent_alias + "." + SQLiteHandler::TABLE_NAME_BARDATA;
      parent_database_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, parent);

      sql_update =
        "update " + base_bardata +
        " set _parent_prev=ifnull("
        "(select max(rowid) from " + parent_bardata + " a where " +
        (enable_optimization? base_bardata + ".rowid>="+ QString::number(start_from_rowid_parent) + " and " : "") +
        "(" + base_bardata + ".date_>a.date_ or (" + base_bardata + ".date_=a.date_ and " + base_bardata + ".time_>a.time_))),0)" +
        (enable_optimization? " where rowid>=" + QString::number(start_from_rowid_base) : "");

//      qDebug() << sql_update;
      query.exec("ATTACH DATABASE '" + parent_database_path + "' AS " + parent_alias);
      query.exec("PRAGMA journal_mode = OFF;");
      query.exec("PRAGMA synchronous = OFF;");
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("BEGIN TRANSACTION;");

      if (parent < WEIGHT_DAILY) {
        query.exec("update bardata set _parent_prev=_parent-1 where rowid>=" + QString::number(start_from_rowid_base) + " and _parent>0");
      } else {
        query.exec(sql_update);
      }

      if (query.lastError().isValid()) qDebug() << "indexing _parent_prev:" << query.lastError();
      query.exec("COMMIT;");
      query.exec("DETACH " + parent_alias);
    }

    static void update_parent_monthly_prev(const QSqlDatabase &base_database, const QString &database_path, const QString &symbol) {
      QSqlQuery query(base_database);
      const QString base_bardata = SQLiteHandler::TABLE_NAME_BARDATA;
      const QString parent_alias = "dbMonthly";
      const QString parent_bardata = parent_alias + "." + SQLiteHandler::TABLE_NAME_BARDATA;
      const QString parent_database_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol,WEIGHT_MONTHLY);
      QString start_rowid_base = "";
      QString start_rowid_parent = "";
      int start_from_rowid_base = 0;
      int start_from_rowid_parent = 0;

      query.setForwardOnly(true);
      query.exec("select max(rowid) from bardata where _parent_prev_monthly > 0");
      start_from_rowid_base = query.next()? query.value(0).toInt() : 0;

      query.exec("select _parent_prev_monthly from bardata where rowid=" + QString::number(start_from_rowid_base));
      start_from_rowid_parent = query.next()? query.value(0).toInt() : 0;

      if (start_from_rowid_base > 0) {
        start_rowid_base = "rowid>=" + QString::number(start_from_rowid_base) + " and ";
      }

      if (start_from_rowid_parent > 0) {
        start_rowid_parent = base_bardata + ".rowid>="+ QString::number(start_from_rowid_parent) + " and ";
      }

      QString sql_update =
        "update " + base_bardata +
        " set _parent_prev_monthly=ifnull((select max(rowid) from " + parent_bardata + " a where " + start_rowid_parent +
        "(" + base_bardata + ".date_>a.date_ or (" + base_bardata + ".date_=a.date_ and " + base_bardata + ".time_>a.time_))),0)" +
        " where " + start_rowid_base +
        "exists(select 1 from " + parent_bardata + " a where " + start_rowid_parent +
        "(" + base_bardata + ".date_>a.date_ or (" + base_bardata + ".date_=a.date_ and " + base_bardata + ".time_>a.time_)) limit 1)";

//      qDebug() << sql_update;
      query.exec("ATTACH DATABASE '" + parent_database_path + "' AS " + parent_alias);
      query.exec("PRAGMA journal_mode = OFF;");
      query.exec("PRAGMA synchronous = OFF;");
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("BEGIN TRANSACTION;");
      qDebug() << "update _parent_monthly_prev" << query.exec(sql_update);
      if (query.lastError().isValid()) qDebug() << query.lastError();
      query.exec("COMMIT;");
      query.exec("DETACH " + parent_alias);
    }

    static int getLastRowid(const QSqlDatabase &database) {
      if (!database.isOpenError()) {
        QSqlQuery query(database);
        query.setForwardOnly(true);
        query.exec(SQLiteHandler::SQL_SELECT_BARDATA_MAX_ROWID);
        if (query.next()) return query.value(0).toInt();
      }
      return -1;
    }

    static void write_datetime_from_query(const QSqlDatabase &database, const QString &sql, const QString &filename) {
      write_datetime_from_query(database, sql, filename, true);
    }

    static void write_datetime_from_query(const QSqlDatabase &database, const QString &sql, const QString &filename, const bool &append) {
      QSqlQuery *query = new QSqlQuery(database);
      query->setForwardOnly(true);
      query->exec("PRAGMA journal_mode = OFF;");
      query->exec("PRAGMA synchronous = OFF;");
      bool b = query->exec(sql);
      qDebug() << sql.length() << b << database.databaseName() << filename ;
      qDebug() << query->lastError();

      if (b) {
        QFile file(filename);
        QFile::OpenModeFlag mode;

        if (!append) {
          QFile::remove(filename);
          mode = QFile::ReadWrite;
        } else {
          mode =  QFile::Append;
        }

        if (file.open(mode)) {
          QTextStream stream(&file);
          QDate dt;
          while (query->next()) {
            dt = QDate::fromString(query->value(0).toString(),"MM/dd/yyyy");
            stream << dt.toString("yyyy/MM/dd") << " " << query->value(1).toString() << "\n";
          }
          stream.flush();
          file.close();
        }
      }
      query->clear();
      delete query;
    }

    static void write_datetime_from_query(QSqlQuery &query, const QString &filename, const bool &append) {
      QFile file(filename);
      QFile::OpenModeFlag mode;
      if (!append) {
        QFile::remove(filename);
        mode = QFile::ReadWrite;
      } else {
        mode =  QFile::Append;
      }

      if (file.open(mode)) {
        QTextStream stream(&file);
        QDate dt;

        // 0 = rowid, 1 = date, 2 = time
        while (query.next()) {
          dt = QDate::fromString(query.value(1).toString(),"MM/dd/yyyy");
          if (dt.isNull()) {
            dt = QDate::fromString(query.value(1).toString(),"yyyy-MM-dd");
          }
          stream << dt.toString("yyyy/MM/dd") << " " << query.value(2).toString() << "\n";
        }
        file.close();
      }
    }

    static void write_result_from_query(QSqlQuery &query, const QString &filename) {
      QFile file(filename);
      if (file.exists()) file.remove();
      if (file.open(QFile::ReadWrite)) {
        QTextStream stream(&file);
        QDate dt;
        int column_count = query.record().count();
        // column name, 0 = date, 1 = time
        for (int i = 0; i < column_count; ++i) {
          if (i > 0) stream << ",";
          stream << query.record().fieldName(i);
        }
        stream << "\n";
        // values
        while (query.next()) {
          dt = QDate::fromString(query.value(0).toString(),"MM/dd/yyyy");
          stream << dt.toString("yyyy/MM/dd");
          for (int i = 1; i < column_count; ++i) {
            stream << "," << query.value(i).toString();
          }
          stream << "\n";
        }
        file.close();
      }
    }

    static void export_result_to_csv(QSqlQuery *query, const QString &sql, const QString &filename) {
      QFile file(filename);

      if (file.open(QFile::WriteOnly)) {
        QTextStream stream(&file);

        query->setForwardOnly(true);
        query->exec(sql);

        QSqlRecord r = query->record();
        QString line = "\"" + r.fieldName(0) + "\"";
        int column_count = query->record().count();

        for (int i = 1; i < column_count; ++i) {
          line += ",\"" + r.fieldName(i) + "\"";
        }
        stream << line << "\n";

        query->exec(sql);
        if (query->lastError().isValid()) qDebug() << query->lastError();

        while (query->next()) {
          line = query->value(0).toString();
          for (int i = 1; i < column_count; ++i) {
            line += "," + query->value(i).toString();
          }
          stream << line << "\n";
        }

        file.close();
      }
    }

    /*static QDateTime select_first_bar_datetime(const QSqlDatabase &database,
      const QString &date_, const QString &time_, const int &count) {
      QSqlQuery query(database);
      QString sql_select_rowid_last =
        "select rowid from bardata where date_='" + date_ + "' and time_='" + time_ + "'";
      query.setForwardOnly(true);
      query.exec(sql_select_rowid_last);
      if (query.next()) {
        int last_rowid = query.value(0).toInt();
        QString sql_select_time_series =
          "select date_,time_ from "
          "(select rowid,date_,time_ from bardata where rowid <=" + QString::number(last_rowid) +
           " order by rowid desc limit " + QString::number(count) + ") order by rowid asc limit 1";
        query.exec(sql_select_time_series);
        query.next();
        return QDateTime::fromString(
          query.value(0).toString() + " " + query.value(1).toString(),
          "yyyy-MM-dd HH:mm");
      }
      return QDateTime();
    }*/

    static QStringList select_rowid_from_series(const QSqlDatabase &database, const QString &date_, const QString &time_, const int &count) {
      QSqlQuery query(database);
      QStringList list_rowid;
      QString sql_select_rowid_last =
        "select rowid from bardata where date_='" + date_ + "' and time_='" + time_ + "'";
      query.setForwardOnly(true);
      query.exec(sql_select_rowid_last);

      if (query.next()) {
        int last_rowid = query.value(0).toInt();
        QString sql_select_time_series =
          "select rowid from (select rowid from bardata"
          " where rowid <=" + QString::number(last_rowid) +
          " order by rowid desc limit " + QString::number(count) + ") order by rowid asc";
        query.exec(sql_select_time_series);
        while (query.next()) {
          list_rowid.append(query.value(0).toString());
        }
      }

      if (query.lastError().isValid())
        qDebug() << query.lastError();

      return list_rowid;
    }

    /*static QVector<QDateTime> select_series_datetime(const QSqlDatabase &database,
      const QString &last_date, const QString &last_time, const int &series_length) {
      QSqlQuery query(database);
      QString sql_select_rowid_last =
        "select rowid from bardata where date_='" + last_date + "' and time_='" + last_time + "'";
      query.exec(sql_select_rowid_last);

      if (query.next()) {
        int last_rowid = query.value(0).toInt();
        QVector<QDateTime> result;
        QString date_value;
        QString time_value;
        QString sql_select_time_series =
          "select date_,time_ from bardata where rowid <=" + QString::number(last_rowid) +
           " order by rowid desc limit " + QString::number(series_length);

        query.setForwardOnly(true);
        query.exec(sql_select_time_series);

        while (query.next()) {
          date_value = query.value(0).toString();
          time_value = query.value(1).toString();
          result.push_front(QDateTime::fromString(date_value + " " + time_value,"yyyy-MM-dd HH:mm"));
        }
        return result;
      }
      return QVector<QDateTime>();
    }*/

    static void update_distOHLC_v3(const QSqlDatabase &database, const int &year, const QString &column_dist, const QString &column_n_dist) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QSqlQuery query_3(database);
      QSqlQuery query_4(database);
      QVariantList rowid_1;
      QVariantList rowid_2;
      QVariantList rank_1;
      QVariantList rank_2;
      QString sql_select_1;
      QString sql_select_2;
      QString sql_select_3;
      QString sql_select_4;
      QString sql_update_1 = "update bardata set " + column_dist + "_Rank=? where rowid=?";
      double count_1;
      double count_2;
      long i = 0;
      bool b1;
      bool b2;
      bool b3;
      bool b4;

      sql_select_1 = "select count(1) from bardata where date_>='"+ QString::number(year) + "-01-01' and " + column_dist + ">0";
      sql_select_2 = "select count(1) from bardata where date_>='"+ QString::number(year) + "-01-01' and " + column_dist + "<0";

      query_1.setForwardOnly(true);
      query_2.setForwardOnly(true);
      query_3.setForwardOnly(true);
      query_4.setForwardOnly(true);

      query_1.exec(sql_select_1);
      count_1 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec(sql_select_2);
      count_2 = query_1.next()? query_1.value(0).toDouble() : 0;

      sql_select_1 =
        "select rowid from bardata where date_>='"+ QString::number(year) + "-01-01' and " +
         column_dist + ">0 order by " + column_n_dist + " asc";

      sql_select_2 =
        "select rowid from bardata where date_>='"+ QString::number(year) + "-01-01' and " +
         column_dist + "<0 order by " + column_n_dist + " desc";

      query_1.exec(sql_select_1);
      query_2.exec(sql_select_2);

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 80000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (true) {
        b1 = query_1.next();
        b2 = query_2.next();
        if (!b1 && !b2) break;

        if (b1) {
          rowid_1.append(query_1.value(0).toInt());
          rank_1.append(QString::number(i/count_1,'f',4));
        }

        if (b2) {
          rowid_2.append(query_2.value(0).toInt());
          rank_2.append(QString::number(i/count_2,'f',4));
        }

        if (rowid_1.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank_1);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          rank_1.clear();
          rowid_1.clear();
        }

        if (rowid_2.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank_2);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          rank_2.clear();
          rowid_2.clear();
        }

        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank_1);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        rowid_1.clear();
        rank_1.clear();
      }

      if (rowid_2.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank_2);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        rowid_2.clear();
        rank_2.clear();
      }

      qUpdate.exec("COMMIT;");


      //
      // Update previous
      //

      sql_select_1 =
        "select rowid, " + column_n_dist + " from bardata where date_<'"+ QString::number(year) + "-01-01' and " +
         column_dist + ">0 order by " + column_n_dist + " asc";

      sql_select_2 =
        "select rowid, " + column_n_dist + " from bardata where date_<'"+ QString::number(year) + "-01-01' and " +
         column_dist + "<0 order by " + column_n_dist + " desc";

      sql_select_3 =
        "select " + column_n_dist + " from bardata where date_>='"+ QString::number(year) + "-01-01' and " +
         column_dist + ">0 order by " + column_n_dist + " asc";

      sql_select_4 =
        "select " + column_n_dist + " from bardata where date_>='"+ QString::number(year) + "-01-01' and " +
         column_dist + "<0 order by " + column_n_dist + " desc";

      query_1.exec(sql_select_1);
      query_2.exec(sql_select_2);
      query_3.exec(sql_select_3);
      query_4.exec(sql_select_4);
      b3 = query_3.next();
      b4 = query_4.next();

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 80000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      long count_H0 = 0;
      long count_H1 = 0;
      double distValue;

      while (true) {
        b1 = query_1.next();
        b2 = query_2.next();
        if (!b1 && !b2) break;

        if (b1) {
          distValue = query_1.value(1).toDouble();

          while (b3 && query_3.value(0).toDouble() <= distValue) {
            ++count_H0;
            b3 = query_3.next();
          }

          rowid_1.append(query_1.value(0).toInt());
          rank_1.append(QString::number(count_H0 / (count_1 + 1),'f',4));
        }

        if (b2) {
          distValue = query_2.value(1).toDouble();

          while (b4 && query_4.value(0).toDouble() >= distValue) {
            ++count_H1;
            b4 = query_4.next();
          }

          rowid_2.append(query_2.value(0).toInt());
          rank_2.append(QString::number(count_H1 / (count_2 + 1),'f',4));
        }

        if (rowid_1.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank_1);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          rowid_1.clear();
          rank_1.clear();
        }

        if (rowid_2.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank_2);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          rowid_2.clear();
          rank_2.clear();
        }
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank_1);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        rowid_1.clear();
        rank_1.clear();
      }

      if (rowid_2.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank_2);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        rowid_2.clear();
        rank_2.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static void update_fgs_fls_rank(const QSqlDatabase &database) {
      QSqlQuery query(database);
      QSqlQuery qUpdate(database);
      QVariantList v_fgs;
      QVariantList v_fls;
      QVariantList v_rowid;
      QString sql_update_fgs_fls_rank =
        "update bardata set " +
          SQLiteHandler::COLUMN_NAME_FGS_RANK + "=?," +
          SQLiteHandler::COLUMN_NAME_FLS_RANK + "=?"
        " where rowid=?";

      query.setForwardOnly(true);
      query.exec("select count(1) from bardata");
      double count = query.next()? query.value(0).toDouble() : 0;
      double fgs_rank;
      double fls_rank;
      long i = 0;

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");
      query.exec("select rowid, fgs, case when fgs>fls then fgs else fls end as F from bardata order by F asc, rowid asc");

      while (query.next()) {
        if (query.value(1).toInt() > 0) {
          fgs_rank = i / count;
          fls_rank = 0;
        } else {
          fgs_rank = 0;
          fls_rank = i / count;
        }

        v_fgs.append(QString::number(fgs_rank,'f',4));
        v_fls.append(QString::number(fls_rank,'f',4));
        v_rowid.append(query.value(0).toString());

        if (v_rowid.count() >= 100000) {
          qUpdate.prepare(sql_update_fgs_fls_rank);
          qUpdate.addBindValue(v_fgs);
          qUpdate.addBindValue(v_fls);
          qUpdate.addBindValue(v_rowid);
          qUpdate.execBatch();
          v_fgs.clear();
          v_fls.clear();
          v_rowid.clear();
        }
        ++i;
      }

      if (v_rowid.count() > 0) {
        qUpdate.prepare(sql_update_fgs_fls_rank);
        qUpdate.addBindValue(v_fgs);
        qUpdate.addBindValue(v_fls);
        qUpdate.addBindValue(v_rowid);
        qUpdate.execBatch();
        v_fgs.clear();
        v_fls.clear();
        v_rowid.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    //
    // Dist(FS-Cross), Dist(F-S), ATR
    //
    static void update_rank_distfs_distcc_atr(const QSqlDatabase &database) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QSqlQuery query_3a(database);
      QSqlQuery query_3b(database);
      QVariantList atr_rank;
      QVariantList distFS_rank;
      QVariantList distCC_rank_1;
      QVariantList distCC_rank_2;
      QVariantList rowid_1;
      QVariantList rowid_2;
      QVariantList rowid_3a;
      QVariantList rowid_3b;
      QString sql_update_1 = "update bardata set " + SQLiteHandler::COLUMN_NAME_DISTFS_RANK + "=? where rowid=?";
      QString sql_update_2 = "update bardata set " + SQLiteHandler::COLUMN_NAME_ATR_RANK + "=? where rowid=?";
      QString sql_update_3 = "update bardata set " + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK + "=? where rowid=?";
      double count;
      double count_3a;
      double count_3b;
      long i = 0;
      bool b1, b3a, b2, b3b;

      query_1.setForwardOnly(true);
      query_2.setForwardOnly(true);
      query_3a.setForwardOnly(true);
      query_3b.setForwardOnly(true);

      query_1.exec("select count(1) from bardata");
      count = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where DistCC_FSCross_ATR > 0");
      count_3a = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where DistCC_FSCross_ATR < 0");
      count_3b = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select rowid from bardata order by N_DistFS asc, rowid asc");
      query_2.exec("select rowid from bardata order by ATR asc, rowid asc");
      query_3a.exec("select rowid from bardata where DistCC_FSCross_ATR > 0 order by DistCC_FSCross_ATR asc, rowid asc");
      query_3b.exec("select rowid from bardata where DistCC_FSCross_ATR < 0 order by DistCC_FSCross_ATR desc, rowid asc");

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");
      qUpdate.exec("update bardata set " + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK + "=0 where DistCC_FSCross_ATR=0");

      while ( true ) {
        b1 = query_1.next();
        b2 = query_2.next();
        b3a = query_3a.next();
        b3b = query_3b.next();
        if (!b1 && !b2 && !b3a && !b3b) break;

        if (b1) {
          rowid_1.append(query_1.value(0).toString());
          distFS_rank.append(QString::number(i / count, 'f', 4));
        }

        if (b2) {
          rowid_2.append(query_2.value(0).toString());
          atr_rank.append(QString::number(i / count, 'f', 4));
        }

        if (b3a) {
          rowid_3a.append(query_3a.value(0).toString());
          distCC_rank_1.append(QString::number(i / count_3a, 'f', 4));
        }

        if (b3b) {
          rowid_3b.append(query_3b.value(0).toString());
          distCC_rank_2.append(QString::number(i / count_3b, 'f', 4));
        }

        if (rowid_1.count() >= 80000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(distFS_rank);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          distFS_rank.clear();
          rowid_1.clear();
        }

        if (rowid_2.count() >= 80000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(atr_rank);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          atr_rank.clear();
          rowid_2.clear();
        }

        if (rowid_3a.count() >= 80000) {
          qUpdate.prepare(sql_update_3);
          qUpdate.addBindValue(distCC_rank_1);
          qUpdate.addBindValue(rowid_3a);
          qUpdate.execBatch();
          distCC_rank_1.clear();
          rowid_3a.clear();
        }

        if (rowid_3b.count() >= 80000) {
          qUpdate.prepare(sql_update_3);
          qUpdate.addBindValue(distCC_rank_2);
          qUpdate.addBindValue(rowid_3b);
          qUpdate.execBatch();
          distCC_rank_2.clear();
          rowid_3b.clear();
        }

        ++i;
      }

      if (rowid_1.count() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(distFS_rank);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        distFS_rank.clear();
        rowid_1.clear();
      }

      if (rowid_2.count() > 0) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(atr_rank);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        atr_rank.clear();
        rowid_2.clear();
      }

      if (rowid_3a.count() > 0) {
        qUpdate.prepare(sql_update_3);
        qUpdate.addBindValue(distCC_rank_1);
        qUpdate.addBindValue(rowid_3a);
        qUpdate.execBatch();
        distCC_rank_1.clear();
        rowid_3a.clear();
      }

      if (rowid_3b.count() > 0) {
        qUpdate.prepare(sql_update_3);
        qUpdate.addBindValue(distCC_rank_2);
        qUpdate.addBindValue(rowid_3b);
        qUpdate.execBatch();
        distCC_rank_2.clear();
        rowid_3b.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static void update_candle(const QSqlDatabase &database) {
      QSqlQuery query(database);
      QString count_string;
      QString sql_select;

      query.setForwardOnly(true);
      query.exec("select count(1) from bardata");
      count_string = query.next()? query.value(0).toString() : "0";
      sql_select =
        "select"
        " round(sum(Candle_Uptail)/" + count_string + ",8),"
        " round(sum(Candle_Downtail)/" + count_string + ",8),"
        " round(sum(Candle_Body)/" + count_string + ",8),"
        " round(sum(Candle_TotalLength)/" + count_string + ",8)"
        " from bardata";

      query.exec(sql_select);
      query.next();
      double avg_uptail = query.value(0).toDouble();
      double avg_downtail = query.value(1).toDouble();
      double avg_body = query.value(2).toDouble();
      double avg_totallength = query.value(3).toDouble();

//      qDebug() << "avg" << avg_uptail << avg_downtail << avg_body << avg_totallength;

      sql_select =
        "select"
        " round(sum((candle_uptail-" + QString::number(avg_uptail) + ")*"
                   "(candle_uptail-" + QString::number(avg_uptail) + "))/" + count_string + ",8),"
        " round(sum((candle_downtail-" + QString::number(avg_downtail) + ")*"
                   "(candle_downtail-" + QString::number(avg_downtail) + "))/" + count_string + ",8),"
        " round(sum((candle_body-" + QString::number(avg_body) + ")*"
                   "(candle_body-" + QString::number(avg_body) + "))/" + count_string + ",8),"
        " round(sum((candle_totallength-" + QString::number(avg_totallength) + ")*"
                   "(candle_totallength-" + QString::number(avg_totallength) + "))/" + count_string + ",8)"
        " from bardata order by rowid asc";

      query.exec(sql_select);
      query.next();

      double stddev_uptail = sqrt(query.value(0).toDouble());
      double stddev_downtail = sqrt(query.value(1).toDouble());
      double stddev_body = sqrt(query.value(2).toDouble());
      double stddev_totallength = sqrt(query.value(3).toDouble());

//      qDebug() << "stddev" << stddev_uptail << stddev_downtail << stddev_body << stddev_totallength;

      sql_select =
        "update bardata set"
        " n_uptail=round((candle_uptail-" + QString::number(avg_uptail,'f',4) + ")/" + QString::number(stddev_uptail) + ",4),"
        " n_downtail=round((candle_downtail-" + QString::number(avg_downtail,'f',4) + ")/" + QString::number(stddev_downtail) + ",4),"
        " n_body=round((candle_body-" + QString::number(avg_body,'f',4) + ")/" + QString::number(stddev_body) + ",4),"
        " n_totallength=round((candle_totallength-" + QString::number(avg_totallength,'f',4) + ")/" + QString::number(stddev_totallength) + ",4)";

      query.exec("PRAGMA journal_mode = OFF;");
      query.exec("PRAGMA synchronous = OFF;");
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("BEGIN TRANSACTION;");
      query.exec(sql_select);
      query.exec("COMMIT;");
    }

    static void update_candle_rank(const QSqlDatabase &database) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QSqlQuery query_3(database);
      QSqlQuery query_4(database);
      QString sql_update_1 = "update bardata set candle_uptail_rank=? where rowid=?";
      QString sql_update_2 = "update bardata set candle_downtail_rank=? where rowid=?";
      QString sql_update_3 = "update bardata set candle_body_rank=? where rowid=?";
      QString sql_update_4 = "update bardata set candle_totallength_rank=? where rowid=?";
      QVariantList rowid_1;
      QVariantList rowid_2;
      QVariantList rowid_3;
      QVariantList rowid_4;
      QVariantList rank;
      long i = 0;

      query_1.exec("select count(1) from bardata"); query_1.next();
      double total = query_1.value(0).toDouble();

      query_1.setForwardOnly(true);
      query_1.exec("select rowid from bardata order by n_uptail asc");

      query_2.setForwardOnly(true);
      query_2.exec("select rowid from bardata order by n_downtail asc");

      query_3.setForwardOnly(true);
      query_3.exec("select rowid from bardata order by n_body asc");

      query_4.setForwardOnly(true);
      query_4.exec("select rowid from bardata order by n_totallength asc");

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (query_1.next()) {
        query_2.next();
        query_3.next();
        query_4.next();

        rowid_1.append(query_1.value(0).toInt());
        rowid_2.append(query_2.value(0).toInt());
        rowid_3.append(query_3.value(0).toInt());
        rowid_4.append(query_4.value(0).toInt());
        rank.append(QString::number(i/total,'f',4));

        if (rowid_1.size() >= 80000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();

          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(rank);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();

          qUpdate.prepare(sql_update_3);
          qUpdate.addBindValue(rank);
          qUpdate.addBindValue(rowid_3);
          qUpdate.execBatch();

          qUpdate.prepare(sql_update_4);
          qUpdate.addBindValue(rank);
          qUpdate.addBindValue(rowid_4);
          qUpdate.execBatch();

          rowid_1.clear();
          rowid_2.clear();
          rowid_3.clear();
          rowid_4.clear();
          rank.clear();
        }
        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();

        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(rank);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();

        qUpdate.prepare(sql_update_3);
        qUpdate.addBindValue(rank);
        qUpdate.addBindValue(rowid_3);
        qUpdate.execBatch();

        qUpdate.prepare(sql_update_4);
        qUpdate.addBindValue(rank);
        qUpdate.addBindValue(rowid_4);
        qUpdate.execBatch();

        rowid_1.clear();
        rowid_2.clear();
        rowid_3.clear();
        rowid_4.clear();
        rank.clear();
      }

      qUpdate.exec("COMMIT");
    }

    //
    // DateRange, DistCC(FS-Cross) intraday, and PrevDailyATR
    //
    static void update_daterange(const QSqlDatabase &database, const QString &database_path, const QString &symbol, const IntervalWeight &base_interval) {
      //
      // process intraday only
      //
      if (base_interval > WEIGHT_60MIN) return;

      QSqlQuery query(database);
      QSqlQuery query2(database);
      QSqlQuery qUpdate(database);
      QVariantList v_rowid;
      QVariantList v_daterange;
      QVariantList v_distCC;
      QVariantList v_prevatr;
      QString dbdaily_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_DAILY);
      QString db60min_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_60MIN);
      QString db5min_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_5MIN);
      QString sql_select;
      QString sql_update =
        "update bardata set " +
          SQLiteHandler::COLUMN_NAME_DATERANGE + "=?," +
          SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS + "=?," +
          SQLiteHandler::COLUMN_NAME_PREV_DAILY_ATR + "=?" +
        " where rowid=?";

      if (base_interval == WEIGHT_60MIN) {
        sql_select =
          "select a.rowid, b.rowid,"
          "round(a." + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS + "/" +
                "b." + SQLiteHandler::COLUMN_NAME_ATR + ",4), b.ATR"
          " from bardata a"
          " join dbDaily.bardata b on a._parent_prev=b.rowid"
          " order by a.rowid";
      } else if (base_interval == WEIGHT_5MIN) {
        sql_select =
          "select a.rowid, b.rowid,"
          "round(a." + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS + "/" +
                "b." + SQLiteHandler::COLUMN_NAME_ATR + ",4), b.ATR"
          " from bardata a"
          " join db60min.bardata x on a._parent=x.rowid"
          " join dbDaily.bardata b on x._parent_prev=b.rowid"
          " order by a.rowid";
      } else if (base_interval == WEIGHT_1MIN) {
        sql_select =
          "select a.rowid, b.rowid,"
          "round(a." + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS + "/" +
                "b." + SQLiteHandler::COLUMN_NAME_ATR + ",4), b.ATR"
          " from bardata a"
          " join db5min.bardata y on a._parent=y.rowid"
          " join db60min.bardata x on y._parent=x.rowid"
          " join dbDaily.bardata b on x._parent_prev=b.rowid"
          " order by a.rowid";
      }

      query2.setForwardOnly(true);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + dbdaily_path + "' AS dbDaily");
      query.exec("ATTACH DATABASE '" + db60min_path + "' AS db60min");
      query.exec("ATTACH DATABASE '" + db5min_path + "' AS db5min");
      query.exec(sql_select);
      if (query.lastError().isValid()) qDebug() << query.lastError();

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (query.next()) {
        if (base_interval == WEIGHT_60MIN) {
          sql_select =
            "select a.rowid, round((max(a.high_)-min(a.low_))/b.ATR,4)"
            " from bardata a join dbDaily.bardata b on a._parent_prev=b.rowid"
            " where a.rowid<=" + query.value(0).toString() + " and b.rowid=" + query.value(1).toString() +
            " group by b.rowid";
        } else if (base_interval == WEIGHT_5MIN) {
          sql_select =
            "select a.rowid, round((max(a.high_)-min(a.low_))/b.ATR,4)"
            " from bardata a"
            " join db60min.bardata x on a._parent=x.rowid"
            " join dbDaily.bardata b on x._parent_prev=b.rowid"
            " where a.rowid<=" + query.value(0).toString() + " and b.rowid=" + query.value(1).toString() +
            " group by b.rowid";
        }

        query2.exec(sql_select);
        query2.next();

        v_rowid.append( query.value(0).toString() );
        v_distCC.append( query.value(2).toString() );
        v_daterange.append( QString::number( query2.value(1).toDouble() , 'f' , 4 ) );
        v_prevatr.append( query.value(3).toDouble() );

        if (v_rowid.size() >= 100000) {
          qUpdate.prepare(sql_update);
          qUpdate.addBindValue(v_daterange);
          qUpdate.addBindValue(v_distCC);
          qUpdate.addBindValue(v_prevatr);
          qUpdate.addBindValue(v_rowid);
          qUpdate.execBatch();
          v_daterange.clear();
          v_distCC.clear();
          v_prevatr.clear();
          v_rowid.clear();
        }
      }

      if (v_rowid.size() > 0) {
        qUpdate.prepare(sql_update);
        qUpdate.addBindValue(v_daterange);
        qUpdate.addBindValue(v_distCC);
        qUpdate.addBindValue(v_prevatr);
        qUpdate.addBindValue(v_rowid);
        qUpdate.execBatch();
        v_daterange.clear();
        v_distCC.clear();
        v_prevatr.clear();
        v_rowid.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    // DistResistance using Max(Resistance) and Pre-Date
    static void update_dist_resistance(const QSqlDatabase &database, const QString &database_path,
                                       const QString &symbol, const IntervalWeight &base_interval, const int &id_threshold) {
      QSqlQuery query(database);
      QSqlQuery qUpdate(database);
      QVariantList rowid;
      QVariantList value1;
      QVariantList value2;
      QVariantList value3;
      QString database_path_1 = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_DAILY);
      QString database_path_2 = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_60MIN);
      QString database_path_3 = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_5MIN);
      // TODO: use zero based index in next version
      QString distres = "DistRes_" + QString::number(id_threshold+1);
      QString distres_rank = "DistResRank_" + QString::number(id_threshold+1);
      QString distres_atr = "DistResATR_" + QString::number(id_threshold+1);
      QString res_value = "Res_" + QString::number(id_threshold+1);
      QString sql_select = "";
      QString sql_update_1 = "update bardata set " + distres + "=?," + res_value + "=?," + distres_atr + "=? where rowid=?";
      QString sql_update_2 = "update bardata set " + distres_rank + "=? where rowid=?";

      if (base_interval < WEIGHT_DAILY) {
        sql_select += "select a.rowid,"
                       "round(abs(max(b.resistance)-a.close_),4),"
                       "max(b.resistance),"
                       "round(abs(max(b.resistance)-a.close_)/y.ATR,6)"
                      " from bardata a";

        if (base_interval < WEIGHT_60MIN) {
          // 5min
          sql_select += " join db60min.bardata x on a._parent=x.rowid";
          sql_select += " join dbDaily.bardata y on x._parent_prev=y.rowid";
        } else if (base_interval < WEIGHT_5MIN) {
          // 1min
          sql_select += " join db5min.bardata z on a._parent=z.rowid";
          sql_select += " join db60min.bardata x on z._parent=x.rowid";
          sql_select += " join dbDaily.bardata y on x._parent_prev=y.rowid";
        } else {
          // 60min
          sql_select += " join dbDaily.bardata y on a._parent_prev=y.rowid";
        }

        sql_select +=
          " join dbdaily.resistancedata b on y.date_=b.date_ and y.time_=b.time_"
          " where b.resistance_count>=3"
          " group by a.rowid";
      } else {
        sql_select =
          "select a.rowid,"
           "round(abs(max(b.resistance)-a.close_), 4),"
           "max(b.resistance),"
           "round(abs(max(b.resistance)-a.close_)/a.ATR,6)"
          " from bardata a join resistancedata b on a.prevdate=b.date_ and a.prevtime=b.time_"
          " where b.resistance_count>=3"
          " group by a.rowid";
      }

      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + database_path_1 + "' AS dbDaily");
      query.exec("ATTACH DATABASE '" + database_path_2 + "' AS db60min");
      query.exec("ATTACH DATABASE '" + database_path_3 + "' AS db5min");
      query.exec(sql_select);
      if (query.lastError().isValid()) qDebug() << query.lastError();

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while ( query.next() ) {
        rowid.append(query.value(0).toString());
        value1.append(query.value(1).toString());
        value2.append(query.value(2).toString());
        value3.append(query.value(3).toString());
        if (rowid.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(value1);
          qUpdate.addBindValue(value2);
          qUpdate.addBindValue(value3);
          qUpdate.addBindValue(rowid);
          qUpdate.execBatch();
          value1.clear();
          value2.clear();
          value3.clear();
          rowid.clear();
        }
      }

      if ( rowid.size() > 0 ) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(value1);
        qUpdate.addBindValue(value2);
        qUpdate.addBindValue(value3);
        qUpdate.addBindValue(rowid);
        qUpdate.execBatch();
        value1.clear();
        value2.clear();
        value3.clear();
        rowid.clear();
      }

      qUpdate.exec("COMMIT;");

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("BEGIN TRANSACTION;");

      query.exec("select count(1) from bardata where " + distres_atr + " is not null");
      double count = query.next()? query.value(0).toDouble() : 0;
      int i = 0;

      query.exec("select rowid from bardata where " + distres_atr + " is not null order by " + distres_atr);

      while ( query.next() ) {
        rowid.append(query.value(0).toString());
        value1.append(QString::number(i++/count,'f',4));
        if (rowid.size() >= 50000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(value1);
          qUpdate.addBindValue(rowid);
          qUpdate.execBatch();
          value1.clear();
          rowid.clear();
        }
      }

      if ( rowid.size() > 0 ) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(value1);
        qUpdate.addBindValue(rowid);
        qUpdate.execBatch();
        value1.clear();
        rowid.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    // DistSupport using Min(Support) and Pre-Date
    static void update_dist_support(const QSqlDatabase &database, const QString &database_path,
                                    const QString &symbol, const IntervalWeight &base_interval, const int &id_threshold) {
      QSqlQuery query(database);
      QSqlQuery qUpdate(database);
      QVariantList rowid;
      QVariantList value1;
      QVariantList value2;
      QVariantList value3;
      QString database_path_1 = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_DAILY);
      QString database_path_2 = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_60MIN);
      QString database_path_3 = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_5MIN);
      // TODO: use zero base index in next version
      QString distsup = "DistSup_" + QString::number(id_threshold + 1);
      QString distsup_rank = "DistSupRank_" + QString::number(id_threshold + 1);
      QString distsup_atr = "DistSupATR_" + QString::number(id_threshold + 1);
      QString sup_value = "Sup_" + QString::number(id_threshold + 1);
      QString sql_update_1 = "update bardata set " + distsup + "=?," + sup_value +  "=?," + distsup_atr + "=? where rowid=?";
      QString sql_update_2 = "update bardata set " + distsup_rank + "=? where rowid=?";
      QString sql_select;

      if (base_interval < WEIGHT_DAILY) {
        sql_select += "select a.rowid,"
                       "round(abs(min(b.support)-a.close_),4),"
                       "min(b.support),"
                       "round(abs(min(b.support)-a.close_)/y.ATR,6)"
                      " from bardata a";

        if (base_interval < WEIGHT_60MIN) {
          // 5min
          sql_select +=
            " join db60min.bardata x on a._parent=x.rowid"
            " join dbDaily.bardata y on x._parent_prev=y.rowid";
        } else if (base_interval < WEIGHT_5MIN) {
          // 1min
          sql_select +=
            " join db5min.bardata z on a._parent=x.rowid"
            " join db60min.bardata x on z._parent=x.rowid"
            " join dbDaily.bardata y on x._parent_prev=y.rowid";
        } else {
          // 60min
          sql_select += " join dbDaily.bardata y on a._parent_prev=y.rowid";
        }

        sql_select +=
          " join dbDaily.supportdata b on y.date_=b.date_ and y.time_=b.time_"
          " where support_count>=3"
          " group by a.rowid";
      } else {
        sql_select =
          "select a.rowid,"
           "round(abs(min(b.support)-a.close_),4),"
           "min(b.support),"
           "round(abs(min(b.support)-a.close_)/a.ATR,6)"
          " from bardata a join supportdata b on a.prevdate=b.date_ and a.prevtime=b.time_"
          " where support_count>=3"
          " group by a.rowid";
      }

      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + database_path_1 + "' AS dbDaily");
      query.exec("ATTACH DATABASE '" + database_path_2 + "' AS db60min");
      query.exec("ATTACH DATABASE '" + database_path_3 + "' AS db5min");
      query.exec(sql_select);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while ( query.next() ) {
        rowid.append(query.value(0).toString());
        value1.append(query.value(1).toString());
        value2.append(query.value(2).toString());
        value3.append(query.value(3).toString());
        if (rowid.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(value1);
          qUpdate.addBindValue(value2);
          qUpdate.addBindValue(value3);
          qUpdate.addBindValue(rowid);
          qUpdate.execBatch();
          value1.clear();
          value2.clear();
          value3.clear();
          rowid.clear();
        }
      }

      if ( rowid.size() > 0 ) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(value1);
        qUpdate.addBindValue(value2);
        qUpdate.addBindValue(value3);
        qUpdate.addBindValue(rowid);
        qUpdate.execBatch();
        value1.clear();
        value2.clear();
        value3.clear();
        rowid.clear();
      }

      qUpdate.exec("COMMIT;");

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("BEGIN TRANSACTION;");

      query.exec("select count(1) from bardata where " + distsup_atr + " is not null");
      double count = query.next()? query.value(0).toDouble() : 0;
      int i = 0;

      query.exec("select rowid from bardata where " + distsup_atr + " is not null order by " + distsup_atr);

      while ( query.next() ) {
        rowid.append(query.value(0).toString());
        value1.append(QString::number(i++/count,'f',4));
        if (rowid.size() >= 50000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(value1);
          qUpdate.addBindValue(rowid);
          qUpdate.execBatch();
          value1.clear();
          rowid.clear();
        }
      }

      if ( rowid.size() > 0 ) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(value1);
        qUpdate.addBindValue(rowid);
        qUpdate.execBatch();
        value1.clear();
        rowid.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static void update_rank_cgf_clf_cgs_cls(const QSqlDatabase &database) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QSqlQuery query_3(database);
      QSqlQuery query_4(database);
      QVariantList rank_1;
      QVariantList rank_2;
      QVariantList rank_3;
      QVariantList rank_4;
      QVariantList rowid_1;
      QVariantList rowid_2;
      QVariantList rowid_3;
      QVariantList rowid_4;
      QString sql_update_1 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CGF_RANK + "=? where rowid=?";
      QString sql_update_2 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CLF_RANK + "=? where rowid=?";
      QString sql_update_3 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CGS_RANK + "=? where rowid=?";
      QString sql_update_4 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CLS_RANK + "=? where rowid=?";
      long i = 0;
      bool b[4] = {false, false, false, false};

      query_1.setForwardOnly(true);
      query_2.setForwardOnly(true);
      query_3.setForwardOnly(true);
      query_4.setForwardOnly(true);

      query_1.exec("select count(1) from bardata where CGF > 0");
      double count_1 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where CLF > 0");
      double count_2 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where CGS > 0");
      double count_3 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where CLS > 0");
      double count_4 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select rowid from bardata where CGF > 0 order by CGF asc, rowid asc");
      query_2.exec("select rowid from bardata where CLF > 0 order by CLF asc, rowid asc");
      query_3.exec("select rowid from bardata where CGS > 0 order by CGS asc, rowid asc");
      query_4.exec("select rowid from bardata where CLS > 0 order by CLS asc, rowid asc");

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (true) {
        b[0] = query_1.next();
        b[1] = query_2.next();
        b[2] = query_3.next();
        b[3] = query_4.next();
        if (!b[0] && !b[1] && !b[2] && !b[3]) break;

        if (b[0]) {
          rowid_1.append(query_1.value(0).toString());
          rank_1.append(QString::number(i / count_1,'f',4));
        }

        if (b[1]) {
          rowid_2.append(query_2.value(0).toString());
          rank_2.append(QString::number(i / count_2,'f',4));
        }

        if (b[2]) {
          rowid_3.append(query_3.value(0).toString());
          rank_3.append(QString::number(i / count_3,'f',4));
        }

        if (b[3]) {
          rowid_4.append(query_4.value(0).toString());
          rank_4.append(QString::number(i / count_4,'f',4));
        }

        if (rowid_1.size() >= 80000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank_1);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          rank_1.clear();
          rowid_1.clear();
        }

        if (rowid_2.size() >= 80000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(rank_2);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          rank_2.clear();
          rowid_2.clear();
        }

        if (rowid_3.size() >= 80000) {
          qUpdate.prepare(sql_update_3);
          qUpdate.addBindValue(rank_3);
          qUpdate.addBindValue(rowid_3);
          qUpdate.execBatch();
          rank_3.clear();
          rowid_3.clear();
        }

        if (rowid_4.size() >= 80000) {
          qUpdate.prepare(sql_update_4);
          qUpdate.addBindValue(rank_4);
          qUpdate.addBindValue(rowid_4);
          qUpdate.execBatch();
          rank_4.clear();
          rowid_4.clear();
        }

        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank_1);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        rank_1.clear();
        rowid_1.clear();
      }

      if (rowid_2.size() > 0) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(rank_2);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        rank_2.clear();
        rowid_2.clear();
      }

      if (rowid_3.size() > 0) {
        qUpdate.prepare(sql_update_3);
        qUpdate.addBindValue(rank_3);
        qUpdate.addBindValue(rowid_3);
        qUpdate.execBatch();
        rank_3.clear();
        rowid_3.clear();
      }

      if (rowid_4.size() > 0) {
        qUpdate.prepare(sql_update_4);
        qUpdate.addBindValue(rank_4);
        qUpdate.addBindValue(rowid_4);
        qUpdate.execBatch();
        rank_4.clear();
        rowid_4.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static void update_zone(const QSqlDatabase &database, const QString &database_path, const QString &symbol, const IntervalWeight &base_interval) {
      QSqlQuery query(database);
      QSqlQuery qUpdate(database);
      QStringList database_alias;
      QStringList table_alias;
      QStringList projection;
      QVariantList rowid;
      QVariantList o_zone;
      QVariantList o_zone_60min;
      QVariantList o_zone_daily;
      QVariantList o_zone_weekly;
      QVariantList o_zone_monthly;
      QVariantList h_zone;
      QVariantList h_zone_60min;
      QVariantList h_zone_daily;
      QVariantList h_zone_weekly;
      QVariantList h_zone_monthly;
      QVariantList l_zone;
      QVariantList l_zone_60min;
      QVariantList l_zone_daily;
      QVariantList l_zone_weekly;
      QVariantList l_zone_monthly;
      QVariantList c_zone;
      QVariantList c_zone_60min;
      QVariantList c_zone_daily;
      QVariantList c_zone_weekly;
      QVariantList c_zone_monthly;
      QString local_database_path;
      QString sql_select = "";
      QString sql_update;
      IntervalWeight w = SQLiteHandler::get_parent_interval(base_interval);
      double open, high, low, close;
      double fast;
      double slow;
      int index;

      query.setForwardOnly(true);

      // Init dummy field name
      // Monthly, Weekly, Daily, 60min, 5min
      for (int i = 0; i < 5; ++i) {
        projection.append("0"); // Fast
        projection.append("0"); // Slow
      }

      while (w != WEIGHT_INVALID) {
        local_database_path = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, w);
        table_alias.append("t_" + SQLiteHandler::get_interval_name(w));
        database_alias.append("db" + table_alias.last());
        query.exec("ATTACH DATABASE '" + local_database_path + "' AS " + database_alias.last());

        switch (w) {
          case WEIGHT_MONTHLY: index = 0; break;
          case WEIGHT_WEEKLY: index = 2; break;
          case WEIGHT_DAILY: index = 4; break;
          case WEIGHT_60MIN: index = 6; break;
          case WEIGHT_5MIN: index = 8; break;
          default: index = -1;
        }

        projection[index] = table_alias.last() + ".FastAvg";
        projection[index+1] = table_alias.last() + ".SlowAvg";
        w = SQLiteHandler::get_parent_interval(w);
      }

      sql_select +=
        "SELECT A.rowid, A.open_, A.high_, A.low_, A.close_, A.fastavg, A.slowavg," + projection.join(",") +
        " FROM bardata A";

      if (database_alias.size() > 0) {
        QString prev_table_alias = table_alias[0];

        sql_select +=
          " LEFT JOIN " + database_alias[0] + ".bardata " + table_alias[0] +
          " ON A._parent=" + table_alias[0] + ".rowid";

        for (int i = 1; i < database_alias.size(); ++i) {
          sql_select +=
            " LEFT JOIN " + database_alias[i] + ".bardata " + table_alias[i] +
            " ON " + prev_table_alias + "._parent=" + table_alias[i] + ".rowid";

          prev_table_alias = table_alias[i];
        }
      }

      query.exec(sql_select);
      if (query.lastError().isValid()) qDebug() << query.lastError();
//      qDebug() << sql_select;

      sql_update =
        "update bardata set"
        " zone=?, zonemonthly=?, zoneweekly=?, zonedaily=?, zone60min=?," +
          SQLiteHandler::COLUMN_NAME_OPEN_ZONE +"=?," +
          SQLiteHandler::COLUMN_NAME_OPEN_ZONE_MONTHLY + "=?," +
          SQLiteHandler::COLUMN_NAME_OPEN_ZONE_WEEKLY + "=?," +
          SQLiteHandler::COLUMN_NAME_OPEN_ZONE_DAILY + "=?," +
          SQLiteHandler::COLUMN_NAME_OPEN_ZONE_60MIN + "=?," +

          SQLiteHandler::COLUMN_NAME_HIGH_ZONE +"=?," +
          SQLiteHandler::COLUMN_NAME_HIGH_ZONE_MONTHLY + "=?," +
          SQLiteHandler::COLUMN_NAME_HIGH_ZONE_WEEKLY + "=?," +
          SQLiteHandler::COLUMN_NAME_HIGH_ZONE_DAILY + "=?," +
          SQLiteHandler::COLUMN_NAME_HIGH_ZONE_60MIN + "=?," +

          SQLiteHandler::COLUMN_NAME_LOW_ZONE +"=?," +
          SQLiteHandler::COLUMN_NAME_LOW_ZONE_MONTHLY + "=?," +
          SQLiteHandler::COLUMN_NAME_LOW_ZONE_WEEKLY + "=?," +
          SQLiteHandler::COLUMN_NAME_LOW_ZONE_DAILY + "=?," +
          SQLiteHandler::COLUMN_NAME_LOW_ZONE_60MIN + "=?," +

          SQLiteHandler::COLUMN_NAME_CLOSE_ZONE +"=?," +
          SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_MONTHLY + "=?," +
          SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_WEEKLY + "=?," +
          SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_DAILY + "=?," +
          SQLiteHandler::COLUMN_NAME_CLOSE_ZONE_60MIN + "=?" +
        " where rowid=?";

      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (query.next()) {
        rowid.append(query.value(0).toInt());
        open = query.value(1).toDouble();
        high = query.value(2).toDouble();
        low = query.value(3).toDouble();
        close = query.value(4).toDouble();
        fast = query.value(5).toDouble();
        slow = query.value(6).toDouble();

        o_zone.append(check_zone(open, fast, slow));
        o_zone_monthly.append(check_zone(open, query.value(7).toDouble(), query.value(8).toDouble()));
        o_zone_weekly.append(check_zone(open, query.value(9).toDouble(), query.value(10).toDouble()));
        o_zone_daily.append(check_zone(open, query.value(11).toDouble(), query.value(12).toDouble()));
        o_zone_60min.append(check_zone(open, query.value(13).toDouble(), query.value(14).toDouble()));

        h_zone.append(check_zone(high, fast, slow));
        h_zone_monthly.append(check_zone(high, query.value(7).toDouble(), query.value(8).toDouble()));
        h_zone_weekly.append(check_zone(high, query.value(9).toDouble(), query.value(10).toDouble()));
        h_zone_daily.append(check_zone(high, query.value(11).toDouble(), query.value(12).toDouble()));
        h_zone_60min.append(check_zone(high, query.value(13).toDouble(), query.value(14).toDouble()));

        l_zone.append(check_zone(low, fast, slow));
        l_zone_monthly.append(check_zone(low, query.value(7).toDouble(), query.value(8).toDouble()));
        l_zone_weekly.append(check_zone(low, query.value(9).toDouble(), query.value(10).toDouble()));
        l_zone_daily.append(check_zone(low, query.value(11).toDouble(), query.value(12).toDouble()));
        l_zone_60min.append(check_zone(low, query.value(13).toDouble(), query.value(14).toDouble()));

        c_zone.append(check_zone(close, fast, slow));
        c_zone_monthly.append(check_zone(close, query.value(7).toDouble(), query.value(8).toDouble()));
        c_zone_weekly.append(check_zone(close, query.value(9).toDouble(), query.value(10).toDouble()));
        c_zone_daily.append(check_zone(close, query.value(11).toDouble(), query.value(12).toDouble()));
        c_zone_60min.append(check_zone(close, query.value(13).toDouble(), query.value(14).toDouble()));

        if (rowid.size() >= 50000) {
          qUpdate.prepare(sql_update);
          qUpdate.addBindValue(c_zone);
          qUpdate.addBindValue(c_zone_monthly);
          qUpdate.addBindValue(c_zone_weekly);
          qUpdate.addBindValue(c_zone_daily);
          qUpdate.addBindValue(c_zone_60min);
          qUpdate.addBindValue(o_zone);
          qUpdate.addBindValue(o_zone_monthly);
          qUpdate.addBindValue(o_zone_weekly);
          qUpdate.addBindValue(o_zone_daily);
          qUpdate.addBindValue(o_zone_60min);
          qUpdate.addBindValue(h_zone);
          qUpdate.addBindValue(h_zone_monthly);
          qUpdate.addBindValue(h_zone_weekly);
          qUpdate.addBindValue(h_zone_daily);
          qUpdate.addBindValue(h_zone_60min);
          qUpdate.addBindValue(l_zone);
          qUpdate.addBindValue(l_zone_monthly);
          qUpdate.addBindValue(l_zone_weekly);
          qUpdate.addBindValue(l_zone_daily);
          qUpdate.addBindValue(l_zone_60min);
          qUpdate.addBindValue(c_zone);
          qUpdate.addBindValue(c_zone_monthly);
          qUpdate.addBindValue(c_zone_weekly);
          qUpdate.addBindValue(c_zone_daily);
          qUpdate.addBindValue(c_zone_60min);
          qUpdate.addBindValue(rowid);
          qUpdate.execBatch();
          rowid.clear();
          o_zone.clear();
          o_zone_monthly.clear();
          o_zone_weekly.clear();
          o_zone_daily.clear();
          o_zone_60min.clear();
          h_zone.clear();
          h_zone_monthly.clear();
          h_zone_weekly.clear();
          h_zone_daily.clear();
          h_zone_60min.clear();
          l_zone.clear();
          l_zone_monthly.clear();
          l_zone_weekly.clear();
          l_zone_daily.clear();
          l_zone_60min.clear();
          c_zone.clear();
          c_zone_monthly.clear();
          c_zone_weekly.clear();
          c_zone_daily.clear();
          c_zone_60min.clear();
        }
      }

      if (rowid.size() > 0) {
        qUpdate.prepare(sql_update);
        qUpdate.addBindValue(c_zone);
        qUpdate.addBindValue(c_zone_monthly);
        qUpdate.addBindValue(c_zone_weekly);
        qUpdate.addBindValue(c_zone_daily);
        qUpdate.addBindValue(c_zone_60min);
        qUpdate.addBindValue(o_zone);
        qUpdate.addBindValue(o_zone_monthly);
        qUpdate.addBindValue(o_zone_weekly);
        qUpdate.addBindValue(o_zone_daily);
        qUpdate.addBindValue(o_zone_60min);
        qUpdate.addBindValue(h_zone);
        qUpdate.addBindValue(h_zone_monthly);
        qUpdate.addBindValue(h_zone_weekly);
        qUpdate.addBindValue(h_zone_daily);
        qUpdate.addBindValue(h_zone_60min);
        qUpdate.addBindValue(l_zone);
        qUpdate.addBindValue(l_zone_monthly);
        qUpdate.addBindValue(l_zone_weekly);
        qUpdate.addBindValue(l_zone_daily);
        qUpdate.addBindValue(l_zone_60min);
        qUpdate.addBindValue(c_zone);
        qUpdate.addBindValue(c_zone_monthly);
        qUpdate.addBindValue(c_zone_weekly);
        qUpdate.addBindValue(c_zone_daily);
        qUpdate.addBindValue(c_zone_60min);
        qUpdate.addBindValue(rowid);
        qUpdate.execBatch();
        rowid.clear();
        o_zone.clear();
        o_zone_monthly.clear();
        o_zone_weekly.clear();
        o_zone_daily.clear();
        o_zone_60min.clear();
        h_zone.clear();
        h_zone_monthly.clear();
        h_zone_weekly.clear();
        h_zone_daily.clear();
        h_zone_60min.clear();
        l_zone.clear();
        l_zone_monthly.clear();
        l_zone_weekly.clear();
        l_zone_daily.clear();
        l_zone_60min.clear();
        c_zone.clear();
        c_zone_monthly.clear();
        c_zone_weekly.clear();
        c_zone_daily.clear();
        c_zone_60min.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static int check_zone(const double &price, const double &fast, const double &slow) {
      if ( fast < 0 || slow < 0 ) return 0; // invalid zone
      if ( price >= fast && fast >= slow ) return 1;
      if ( fast >= price && price >= slow ) return 2;
      if ( fast >= slow && slow >= price ) return 3;
      if ( price >= slow && slow >= fast ) return 4;
      if ( slow >= price && price >= fast ) return 5;
      if ( slow >= fast && fast >= price ) return 6;
      return 0;
    }

    static void update_macd_rank(const QSqlDatabase &database) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QVariantList rank_1;
      QVariantList rank_2;
      QVariantList rowid_1;
      QVariantList rowid_2;
      QString sql_update_rank_1 = "update bardata set " + SQLiteHandler::COLUMN_NAME_MACD_RANK + "=? where rowid=?";
      QString sql_update_rank_2;
      long i = 0;
      bool b1;
      bool b2;

      query_1.setForwardOnly(true);
      query_2.setForwardOnly(true);

      query_1.exec("select count(1) from bardata where MACD > 0");
      double count_1 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where MACD < 0");
      double count_2 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select rowid from bardata where MACD>0 order by MACD asc");
      query_2.exec("select rowid from bardata where MACD<0 order by MACD desc");

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (true) {
        b1 = query_1.next();
        b2 = query_2.next();
        if (!b1 && !b2) break;

        if (b1) {
          rowid_1.append(query_1.value(0).toString());
          rank_1.append(QString::number(i / count_1,'f',4));
        }

        if (b2) {
          rowid_1.append(query_2.value(0).toString());
          rank_1.append(QString::number(i / count_2,'f',4));
        }

        if (rowid_1.size() >= 80000) {
          qUpdate.prepare( sql_update_rank_1 );
          qUpdate.addBindValue( rank_1 );
          qUpdate.addBindValue( rowid_1 );
          qUpdate.execBatch();
          rank_1.clear();
          rowid_1.clear();
        }

        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare( sql_update_rank_1 );
        qUpdate.addBindValue( rank_1 );
        qUpdate.addBindValue( rowid_1 );
        qUpdate.execBatch();
        rank_1.clear();
        rowid_1.clear();
      }

      qUpdate.exec("COMMIT;");

      //
      // MACD > 0 and MACD < 0
      //
      /*
      sql_update_rank_1 = "update bardata set " + SQLiteHandler::COLUMN_NAME_MACD_G0_RANK + "=? where rowid=?";
      sql_update_rank_2 = "update bardata set " + SQLiteHandler::COLUMN_NAME_MACD_L0_RANK + "=? where rowid=?";

      query_1.setForwardOnly(true);
      query_2.setForwardOnly(true);

      query_1.exec("select count(1) from bardata where MACD_G0 > 0");
      count_1 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where MACD_L0 > 0");
      count_2 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select rowid from bardata where MACD_G0 > 0 order by MACD_G0 asc, rowid asc");
      query_2.exec("select rowid from bardata where MACD_L0 > 0 order by MACD_L0 asc, rowid asc");
      i = 0;

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (true) {
        b1 = query_1.next();
        b2 = query_2.next();
        if (!b1 && !b2) break;

        if (b1) {
          rowid_1.append(query_1.value(0).toString());
          rank_1.append(QString::number(i / count_1,'f',4));
        }

        if (b2) {
          rowid_2.append(query_2.value(0).toString());
          rank_2.append(QString::number(i / count_2,'f',4));
        }


        if (rowid_1.size() >= 80000) {
          qUpdate.prepare(sql_update_rank_1);
          qUpdate.addBindValue(rank_1);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          rowid_1.clear();
          rank_1.clear();
        }

        if (rowid_2.size() >= 80000) {
          qUpdate.prepare(sql_update_rank_2);
          qUpdate.addBindValue(rank_2);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          rowid_2.clear();
          rank_2.clear();
        }

        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_rank_1);
        qUpdate.addBindValue(rank_1);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        rowid_1.clear();
        rank_1.clear();
      }

      if (rowid_2.size() > 0) {
        qUpdate.prepare(sql_update_rank_2);
        qUpdate.addBindValue(rank_2);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        rowid_2.clear();
        rank_2.clear();
      }

      qUpdate.exec("COMMIT;");
      */
    }

    //
    // MACD, RSI, SlowK, SlowD threshold - Non-Zero based
    //
    static void update_column_threshold_1(const QSqlDatabase &database, const QString &column, const XmlConfigHandler::t_threshold_1 &params) {
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QSqlQuery qUpdate(database);
      QString t_id = QString::number(params.t_id);
      QString op1 = params.operator1;
      QString op2 = params.operator2;
      QString value1 = QString::number(params.value1);
      QString value2 = QString::number(params.value2);
      QString column_value1 = column + "_VALUE1_" + t_id;
      QString column_value2 = column + "_VALUE2_" + t_id;
      QString sql_update = "update bardata set " + column_value1 + "=?," + column_value2 + "=? where rowid=?";
      QVariantList rowid_1;
      QVariantList rowid_2;
      QVariantList column_1;
      QVariantList column_2;
      double m_column;
      int n_column1 = 0;
      int n_column2 = 0;

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      query_1.setForwardOnly(true);
      query_1.exec("select rowid," + column + " from bardata order by rowid asc");

      while (query_1.next()) {
        m_column = query_1.value(1).toDouble();

        if ((op1 == ">" && m_column > params.value1) ||
            (op1 == "<" && m_column < params.value1) ||
            (op1 == "=" && m_column == params.value1)) {
          ++n_column1;
        } else {
          n_column1 = 0;
        }

        if ((op2 == ">" && m_column > params.value2) ||
            (op2 == "<" && m_column < params.value2) ||
            (op2 == "=" && m_column == params.value2)) {
          ++n_column2;
        } else {
          n_column2 = 0;
        }

        rowid_1.append(query_1.value(0).toInt());
        column_1.append(n_column1);
        column_2.append(n_column2);

        if (rowid_1.size() >= 60000) {
          qUpdate.prepare(sql_update);
          qUpdate.addBindValue(column_1);
          qUpdate.addBindValue(column_2);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          column_1.clear();
          column_2.clear();
          rowid_1.clear();
        }
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update);
        qUpdate.addBindValue(column_1);
        qUpdate.addBindValue(column_2);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        column_1.clear();
        column_2.clear();
        rowid_1.clear();
      }

      qUpdate.exec("COMMIT;");

      //
      // Create Histogram Rank
      //
      qDebug() << "Create rank:" << column;
      QString column_rank1 = column + "_RANK1_" + t_id;
      QString column_rank2 = column + "_RANK2_" + t_id;
      QString sql_select_1 = "select rowid from bardata where " + column + op1 + value1 + " order by " + column_value1 + ",rowid";
      QString sql_select_2 = "select rowid from bardata where " + column + op2 + value2 + " order by " + column_value2 + ",rowid";
      QString sql_update_1 = "update bardata set " + column_rank1 + "=? where rowid=?";
      QString sql_update_2 = "update bardata set " + column_rank2 + "=? where rowid=?";
      int i = 0;
      bool b1, b2;

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      query_1.exec("select count(1) from bardata where " + column + op1 + value1);
      double m_count1 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where " + column + op2 + value2);
      double m_count2 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec(sql_select_1);

      query_2.setForwardOnly(true);
      query_2.exec(sql_select_2);

      while (true) {
        b1 = query_1.next();
        b2 = query_2.next();
        if (!b1 && !b2) break;

        if (b1) {
          rowid_1.append(query_1.value(0));
          column_1.append(QString::number(i/ m_count1,'f',4));
        }

        if (b2) {
          rowid_2.append(query_2.value(0));
          column_2.append(QString::number(i/ m_count2,'f',4));
        }

        if (rowid_1.size() >= 50000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(column_1);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          column_1.clear();
          rowid_1.clear();
        }

        if (rowid_2.size() >= 50000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(column_2);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          column_2.clear();
          rowid_2.clear();
        }

        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(column_1);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        column_1.clear();
        rowid_1.clear();
      }

      if (rowid_2.size() > 0) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(column_2);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        column_2.clear();
        rowid_2.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static void update_sline_rline_count(const QSqlDatabase &database, const QString &database_path, const double &threshold,
                                         const QString &symbol, const IntervalWeight &base_interval, const int &id_threshold) {
      const int NUMBER_OF_QUERY = 2;
      QSqlQuery qUpdate(database);
      QSqlQuery query[NUMBER_OF_QUERY] = { QSqlQuery(database), QSqlQuery(database)};
      QVariantList m_rowid[NUMBER_OF_QUERY];
      QVariantList m_count[NUMBER_OF_QUERY];
      QString sql_update[NUMBER_OF_QUERY];
      QString m_threshold = QString::number(threshold,'f',6);
      // TODO: use zero based index in next version
      QString id_thres = "_" + QString::number(id_threshold+1);
      QString dbmonthly = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_MONTHLY);
      QString dbweekly = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_WEEKLY);
      QString dbdaily = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_DAILY);
      QString db60min = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_60MIN);
      QString db5min = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_5MIN);
      QString RLine;
      QString SLine;
      IntervalWeight w = base_interval;
      bool b[NUMBER_OF_QUERY];

      for (int i = 0; i < NUMBER_OF_QUERY; ++i) {
        query[i].setForwardOnly(true);
        query[i].exec("ATTACH DATABASE '" + dbmonthly + "' AS dbMonthly");
        query[i].exec("ATTACH DATABASE '" + dbweekly + "' AS dbWeekly");
        query[i].exec("ATTACH DATABASE '" + dbdaily + "' AS dbDaily");
        query[i].exec("ATTACH DATABASE '" + db60min + "' AS db60min");
        query[i].exec("ATTACH DATABASE '" + db5min + "' AS db5min");
      }

      while (w != WEIGHT_INVALID) {
        //
        // Determine target column
        //
        if (w == WEIGHT_MONTHLY) {
          RLine = SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE + id_thres;
          SLine = SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE + id_thres;
        } else if (w == WEIGHT_WEEKLY) {
          RLine = SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE + id_thres;
          SLine = SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE + id_thres;
        } else {
          RLine = SQLiteHandler::COLUMN_NAME_DAILY_RLINE + id_thres;
          SLine = SQLiteHandler::COLUMN_NAME_DAILY_SLINE + id_thres;
        }

        sql_update[0] = "update bardata set " + RLine + "=? where rowid=?";
        sql_update[1] = "update bardata set " + SLine + "=? where rowid=?";

        //
        // Handle join
        //
        if (base_interval < w) {
          QString sql_select_1 = "";
          QString sql_select_2 = "";
          QString alias = "a";

          QString parent_database = "db" + SQLiteHandler::get_interval_name(w) + ".";
          QString parent_alias = "";

          if (w == WEIGHT_MONTHLY) parent_alias = "m.";
          else if (w == WEIGHT_WEEKLY) parent_alias = "w.";
          else parent_alias = "d.";

          sql_select_1 += "select a.rowid, count(1) from bardata a";
          sql_select_2 += "select a.rowid, count(1) from bardata a";

          if (base_interval < WEIGHT_5MIN) {
            if (base_interval < WEIGHT_1MIN) alias = "m1";
            sql_select_1 += " left join db5min.bardata m5 ON " + alias + "._parent_prev=m5.rowid";
            sql_select_2 += " left join db5min.bardata m5 ON " + alias + "._parent_prev=m5.rowid";
          }

          if (base_interval < WEIGHT_60MIN) {
            if (base_interval < WEIGHT_5MIN) alias = "m5";
            sql_select_1 += " left join db60min.bardata m60 ON " + alias + "._parent_prev=m60.rowid";
            sql_select_2 += " left join db60min.bardata m60 ON " + alias + "._parent_prev=m60.rowid";
          }

          if (base_interval < WEIGHT_DAILY) {
            if (base_interval < WEIGHT_60MIN) alias = "m60";
            sql_select_1 += " left join dbDaily.bardata d ON " + alias + "._parent_prev=d.rowid";
            sql_select_2 += " left join dbDaily.bardata d ON " + alias + "._parent_prev=d.rowid";
          }

          if (base_interval < WEIGHT_WEEKLY) {
            if (base_interval < WEIGHT_DAILY) alias = "d";
            sql_select_1 += " left join dbWeekly.bardata w ON " + alias + "._parent_prev=w.rowid";
            sql_select_2 += " left join dbWeekly.bardata w ON " + alias + "._parent_prev=w.rowid";
          }

          if (base_interval < WEIGHT_MONTHLY) {
            sql_select_1 += " left join dbMonthly.bardata m ON a._parent_prev_monthly=m.rowid";
            sql_select_2 += " left join dbMonthly.bardata m ON a._parent_prev_monthly=m.rowid";
          }

          sql_select_1 +=
            " join " + parent_database + "resistancedata b on "
                     + parent_alias + "date_=b.date_ and "
                     + parent_alias + "time_=b.time_"
            " where b.id_threshold=" + QString::number(id_threshold) + " and "
             "(b.resistance-" + m_threshold + ")<=a.high_ and "
            "((b.resistance+" + m_threshold + ")>=a.open_ or "
             "(b.resistance+" + m_threshold + ")>=a.close_)"
            " group by a.rowid";

          sql_select_2 +=
            " join " + parent_database + "supportdata b on "
                     + parent_alias + "date_=b.date_ and "
                     + parent_alias + "time_=b.time_"
            " where b.id_threshold=" + QString::number(id_threshold) + " and "
             "(b.support+" + m_threshold + ")>=a.low_ and "
            "((b.support-" + m_threshold + ")<=a.open_ or " +
             "(b.support-" + m_threshold + ")<=a.close_)"
            " group by a.rowid";

          query[0].exec(sql_select_1);
          query[1].exec(sql_select_2);

          if (query[0].lastError().isValid()) {
            qDebug() << query[0].lastError() << "\n" << base_interval << w << "\n" << sql_select_1 << "\n";
          }

          if (query[1].lastError().isValid()) {
            qDebug() << query[1].lastError() << "\n" << base_interval << w << "\n" << sql_select_2 << "\n";
          }

//          qDebug() << "WTF " << base_interval << w;
//          qDebug() << sql_select_1;
        } else {
          query[0].exec("select a.rowid, count(1) from bardata a "
                        " join resistancedata b on a.prevdate=b.date_ and a.prevtime=b.time_"
                        " where b.id_threshold=" + QString::number(id_threshold) + " and "
                        " (b.resistance-" + m_threshold + ")<=a.high_ and "
                        "((b.resistance+" + m_threshold + ")>=a.open_ or "
                        " (b.resistance+" + m_threshold + ")>=a.close_) group by a.rowid");

          query[1].exec("select a.rowid, count(1) from bardata a "
                        " join supportdata b on a.prevdate=b.date_ and a.prevtime=b.time_"
                        " where b.id_threshold=" + QString::number(id_threshold) + " and "
                        " (b.support+" + m_threshold + ")>=a.low_ and "
                        "((b.support-" + m_threshold + ")<=a.open_ or "
                        " (b.support-" + m_threshold + ")<=a.close_) group by a.rowid");

          if (query[0].lastError().isValid()) qDebug() << query[0].lastError();
          if (query[1].lastError().isValid()) qDebug() << query[1].lastError();

//          qDebug() << ("select a.rowid, count(1) from bardata a "
//                       " join resistancedata b on a.prevdate=b.date_ and a.prevtime=b.time_"
//                       " where b.id_threshold=" + QString::number(id_threshold) + " and "
//                       " (b.resistance-" + m_threshold + ")<=a.high_ and "
//                       "((b.resistance+" + m_threshold + ")>=a.open_ or "
//                       " (b.resistance+" + m_threshold + ")>=a.close_) group by a.rowid") << "\n";

//          qDebug() << ("select a.rowid, count(1) from bardata a "
//                       " join supportdata b on a.prevdate=b.date_ and a.prevtime=b.time_"
//                       " where b.id_threshold=" + QString::number(id_threshold) + " and "
//                       " (b.support+" + m_threshold + ")>=a.low_ and "
//                       "((b.support-" + m_threshold + ")<=a.open_ or "
//                       " (b.support-" + m_threshold + ")<=a.close_) group by a.rowid") << "\n";
        }

        //
        // Execute here
        //
        qUpdate.setForwardOnly(true);
        qUpdate.exec("PRAGMA journal_mode = OFF;");
        qUpdate.exec("PRAGMA synchronous = OFF;");
        qUpdate.exec("PRAGMA cache_size = 50000;");
        qUpdate.exec("BEGIN TRANSACTION;");

        while (true) {
          b[0] = query[0].next();
          b[1] = query[1].next();
          if (!b[0] && !b[1]) break;

          for (int i = 0; i < NUMBER_OF_QUERY; ++i) {
            if (b[i]) {
              m_rowid[i].append(query[i].value(0).toString());
              m_count[i].append(query[i].value(1).toString());
            }

            if (m_rowid[i].size() >= 50000) {
              qUpdate.prepare(sql_update[i]);
              qUpdate.addBindValue(m_count[i]);
              qUpdate.addBindValue(m_rowid[i]);
              qUpdate.execBatch();
              m_count[i].clear();
              m_rowid[i].clear();
            }
          }
        }

        for (int i = 0; i < NUMBER_OF_QUERY; ++i) {
          if (m_rowid[i].size() > 0) {
            qUpdate.prepare(sql_update[i]);
            qUpdate.addBindValue(m_count[i]);
            qUpdate.addBindValue(m_rowid[i]);
            qUpdate.execBatch();
            m_count[i].clear();
            m_rowid[i].clear();
          }
        }

        qUpdate.exec("COMMIT;");
        w = SQLiteHandler::get_parent_interval(w);
      }
    }

    static void update_dist_SRlines(const QSqlDatabase &database, const int &id_threshold) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QVariantList res_rowid;
      QVariantList res_distPoint;
      QVariantList res_distATR;
      QVariantList sup_rowid;
      QVariantList sup_distPoint;
      QVariantList sup_distATR;
      QString lastDate_res = "";
      QString lastDate_sup = "";
      QString sql_select_resistance;
      QString sql_select_support;
      QString sql_update_resistance =
        "update " + SQLiteHandler::TABLE_NAME_RESISTANCE + " set " +
          SQLiteHandler::COLUMN_NAME_DIST_POINT + "=?, " +
          SQLiteHandler::COLUMN_NAME_DIST_ATR + "=? where " +
          SQLiteHandler::COLUMN_NAME_ROWID + "=?";

      QString sql_update_support =
        "update " + SQLiteHandler::TABLE_NAME_SUPPORT + " set " +
        SQLiteHandler::COLUMN_NAME_DIST_POINT + "=?, " +
        SQLiteHandler::COLUMN_NAME_DIST_ATR + "=? where " +
        SQLiteHandler::COLUMN_NAME_ROWID + "=?";

      double lastResistance = 0;
      double lastSupport = 0;
      double distPoint;
      double distATR;
      double sr_value;
      bool b1;
      bool b2;

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      sql_select_resistance =
        "select a.rowid, a.resistance, b.atr, a.date_"
        " from ResistanceData a join Bardata b on a.date_=b.date_"
        " where b.atr is not null and a.id_threshold=" + QString::number(id_threshold) +
        " order by a.date_, resistance desc";

      sql_select_support =
        "select a.rowid, a.support, b.atr, a.date_"
        " from SupportData a join Bardata b on a.date_=b.date_"
        " where b.atr is not null and a.id_threshold=" + QString::number(id_threshold) +
        " order by a.date_, support asc";
\
      query_1.setForwardOnly(true);
      query_1.exec(sql_select_resistance);
      if (query_1.lastError().isValid()) qDebug() << query_1.lastError();

      query_2.setForwardOnly(true);
      query_2.exec(sql_select_support);
      if (query_2.lastError().isValid()) qDebug() << query_2.lastError();

      while (true) {
        b1 = query_1.next();
        b2 = query_2.next();
        if (!b1 && !b2) break;

        if (b1) {
          if (lastDate_res != query_1.value(3).toString()) {
            lastDate_res = query_1.value(3).toString();
            lastResistance = query_1.value(1).toDouble();
            distPoint = 1000;
            distATR = distPoint / query_1.value(2).toDouble();
            lastResistance = sr_value;
            res_rowid.append(query_1.value(0).toInt());
            res_distPoint.append(QString::number(distPoint,'f',4));
            res_distATR.append(QString::number(distATR,'f',4));
          } else {
            sr_value = query_1.value(1).toDouble();
            distPoint = lastResistance - sr_value;
            distATR = distPoint / query_1.value(2).toDouble();
            lastResistance = sr_value;
            res_rowid.append(query_1.value(0).toInt());
            res_distPoint.append(QString::number(distPoint,'f',4));
            res_distATR.append(QString::number(distATR,'f',4));
          }
        }

        if (b2) {
          if (lastDate_sup != query_2.value(3).toString()) {
            lastDate_sup = query_2.value(3).toString();
            lastSupport = query_2.value(1).toDouble();
            distPoint = 1000;
            distATR = distPoint / query_2.value(2).toDouble();
            lastSupport = sr_value;
            sup_rowid.append(query_2.value(0).toInt());
            sup_distPoint.append(QString::number(distPoint,'f',4));
            sup_distATR.append(QString::number(distATR,'f',4));
          } else {
            sr_value = query_2.value(1).toDouble();
            distPoint = sr_value - lastSupport;
            distATR = distPoint / query_2.value(2).toDouble();
            lastSupport = sr_value;
            sup_rowid.append(query_2.value(0).toInt());
            sup_distPoint.append(QString::number(distPoint,'f',4));
            sup_distATR.append(QString::number(distATR,'f',4));
          }
        }

        if (res_rowid.size() >= 50000) {
          qUpdate.prepare(sql_update_resistance);
          qUpdate.addBindValue(res_distPoint);
          qUpdate.addBindValue(res_distATR);
          qUpdate.addBindValue(res_rowid);
          qUpdate.execBatch();
          res_distPoint.clear();
          res_distATR.clear();
          res_rowid.clear();
        }

        if (sup_rowid.size() >= 50000) {
          qUpdate.prepare(sql_update_support);
          qUpdate.addBindValue(sup_distPoint);
          qUpdate.addBindValue(sup_distATR);
          qUpdate.addBindValue(sup_rowid);
          qUpdate.execBatch();
          sup_distPoint.clear();
          sup_distATR.clear();
          sup_rowid.clear();
        }
      }

      if (res_rowid.size() > 0) {
        qUpdate.prepare(sql_update_resistance);
        qUpdate.addBindValue(res_distPoint);
        qUpdate.addBindValue(res_distATR);
        qUpdate.addBindValue(res_rowid);
        qUpdate.execBatch();
        res_distPoint.clear();
        res_distATR.clear();
        res_rowid.clear();
      }

      if (sup_rowid.size() > 0) {
        qUpdate.prepare(sql_update_support);
        qUpdate.addBindValue(sup_distPoint);
        qUpdate.addBindValue(sup_distATR);
        qUpdate.addBindValue(sup_rowid);
        qUpdate.execBatch();
        sup_distPoint.clear();
        sup_distATR.clear();
        sup_rowid.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    static void update_intraday_avg(const QSqlDatabase &database, const QString &database_path, const QString &symbol, const IntervalWeight &base_interval) {
      QSqlQuery query_day10(database);
      QSqlQuery query_day50(database);
      QSqlQuery qUpdate(database);
      QString dbdaily = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_DAILY);
      QString db60min = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_60MIN);
      QString db5min = database_path + "/" + SQLiteHandler::getDatabaseName(symbol, WEIGHT_5MIN);
      QString sql_select_day10;
      QString sql_select_day50;
      QString sql_update_1 = "update bardata set day10=? where rowid=?";
      QString sql_update_2 = "update bardata set day50=? where rowid=?";
      QVariantList m_rowid_day10;
      QVariantList m_rowid_day50;
      QVariantList m_day10;
      QVariantList m_day50;
      bool b1, b2;

      query_day10.setForwardOnly(true);
      query_day10.exec("attach database '" + dbdaily + "' as dbdaily");

      query_day50.setForwardOnly(true);
      query_day50.exec("attach database '" + dbdaily + "' as dbdaily");

      if (base_interval == WEIGHT_60MIN) {
        sql_select_day10 =
          "select a.rowid, (sum(b.close_) + (case when a.time_<>b.time_ then a.close_-b.close_ else 0 end))/10 "
          " from bardata a join dbdaily.bardata b on (b.rowid>a._parent-10 and b.rowid<=a._parent)"
          " group by a.rowid"
          " having count(1)=10"
          " order by a.rowid";

        sql_select_day50 =
          "select a.rowid, (sum(b.close_) + (case when a.time_<>b.time_ then a.close_-b.close_ else 0 end))/50 "
          " from bardata a join dbdaily.bardata b on (b.rowid>a._parent-50 and b.rowid<=a._parent)"
          " group by a.rowid"
          " having count(1)=50"
          " order by a.rowid";
      }
      else if (base_interval == WEIGHT_5MIN) {
        query_day10.exec("attach database '" + db60min + "' as db60min");
        query_day50.exec("attach database '" + db60min + "' as db60min");

        sql_select_day10 =
          "select a.rowid, (sum(b.close_) + (case when a.time_<>b.time_ then a.close_-b.close_ else 0 end))/10 "
          " from bardata a "
          " join db60min.bardata x on a._parent=x.rowid"
          " join dbdaily.bardata b on (b.rowid>x._parent-10 and b.rowid<=x._parent)"
          " group by a.rowid"
          " having count(1)=10"
          " order by a.rowid";

        sql_select_day50 =
          "select a.rowid, (sum(b.close_) + (case when a.time_<>b.time_ then a.close_-b.close_ else 0 end))/50 "
          " from bardata a "
          " join db60min.bardata x on a._parent=x.rowid"
          " join dbdaily.bardata b on (b.rowid>x._parent-50 and b.rowid<=x._parent)"
          " group by a.rowid"
          " having count(1)=50"
          " order by a.rowid";
      }
      else {
        query_day10.exec("attach database '" + db60min + "' as db60min");
        query_day50.exec("attach database '" + db60min + "' as db60min");
        query_day10.exec("attach database '" + db5min + "' as db5min");
        query_day50.exec("attach database '" + db5min + "' as db5min");

        sql_select_day10 =
          "select a.rowid, (sum(b.close_) + (case when a.time_<>b.time_ then a.close_-b.close_ else 0 end))/10 "
          " from bardata a "
          " join db5min.bardata y on a._parent=y.rowid"
          " join db60min.bardata x on y._parent=x.rowid"
          " join dbdaily.bardata b on (b.rowid>x._parent-10 and b.rowid<=x._parent)"
          " group by a.rowid"
          " having count(1)=10"
          " order by a.rowid";

        sql_select_day50 =
          "select a.rowid, (sum(b.close_) + (case when a.time_<>b.time_ then a.close_-b.close_ else 0 end))/50 "
          " from bardata a "
          " join db5min.bardata y on a._parent=y.rowid"
          " join db60min.bardata x on y._parent=x.rowid"
          " join dbdaily.bardata b on (b.rowid>x._parent-50 and b.rowid<=x._parent)"
          " group by a.rowid"
          " having count(1)=50"
          " order by a.rowid";
      }

      query_day10.exec(sql_select_day10);
      if (query_day10.lastError().isValid()) qDebug() << query_day10.lastError();

      query_day50.exec(sql_select_day50);
      if (query_day50.lastError().isValid()) qDebug() << query_day50.lastError();

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (true) {
        b1 = query_day10.next();
        b2 = query_day50.next();
        if (!b1 && !b2) break;

        if (b1) {
          m_rowid_day10.append(query_day10.value(0).toString());
          m_day10.append(QString::number( query_day10.value(1).toDouble() , 'f' , 4 ));
        }

        if (b2) {
          m_rowid_day50.append(query_day50.value(0).toString());
          m_day50.append(QString::number( query_day50.value(1).toDouble() , 'f' , 4 ));
        }

        if (m_rowid_day10.size() >= 100000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(m_day10);
          qUpdate.addBindValue(m_rowid_day10);
          qUpdate.execBatch();
          m_day10.clear();
          m_rowid_day10.clear();
        }

        if (m_rowid_day50.size() >= 100000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(m_day50);
          qUpdate.addBindValue(m_rowid_day50);
          qUpdate.execBatch();
          m_day50.clear();
          m_rowid_day50.clear();
        }
      }

      if (m_rowid_day10.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(m_day10);
        qUpdate.addBindValue(m_rowid_day10);
        qUpdate.execBatch();
        m_day10.clear();
        m_rowid_day10.clear();
      }

      if (m_rowid_day50.size() > 0) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(m_day50);
        qUpdate.addBindValue(m_rowid_day50);
        qUpdate.execBatch();
        m_day50.clear();
        m_rowid_day50.clear();
      }

      qUpdate.exec("COMMIT;");
    }

    /* Calculate CGF, CLF, CGS, CLS with Threshold
     * Examples:
     *    if (threshold = 0)        C > F
     *    if (positive threshold)   C > (F - T)
     *    if (negative threshold)   C < (F + T)
     *
     * Description:
     *  C = Close
     *  F = FastAvg
     *  T = threshold
     */
    static void update_close_threshold(const QSqlDatabase &database, const QString &column_name, const int &id_threshold, const double &t_value) {
      QSqlQuery qUpdate(database);
      QSqlQuery query(database);
      QVariantList m_count;
      QVariantList m_rowid;
      QString m_column_name = column_name + "_" + QString::number(id_threshold); // zero-based
      QString sql_update = "update bardata set " + m_column_name + "=? where rowid=?";
      double m_close, m_fastavg, m_slowavg;
      long c_count = 0;
      bool is_greater_sign = (column_name == SQLiteHandler::COLUMN_NAME_CGF ||
                              column_name == SQLiteHandler::COLUMN_NAME_CGS);
      bool is_collate_fast = (column_name == SQLiteHandler::COLUMN_NAME_CGF ||
                              column_name == SQLiteHandler::COLUMN_NAME_CLF);

      query.setForwardOnly(true);
      query.exec("select rowid, close_, fastavg, slowavg from bardata order by rowid asc");

      while (query.next()) {
        m_close = query.value(1).toDouble();
        m_fastavg = query.value(2).toDouble();
        m_slowavg = query.value(3).toDouble();

        if ( is_greater_sign ) {
          if ( is_collate_fast ) {
            c_count = ( m_close > m_fastavg - t_value ) ? c_count + 1 : 0;
          } else {
            c_count = ( m_close > m_slowavg - t_value ) ? c_count + 1 : 0;
          }
        }
        else {
          if ( is_collate_fast ) {
            c_count = ( m_close < m_fastavg + t_value ) ? c_count + 1 : 0;
          } else {
            c_count = ( m_close < m_slowavg + t_value ) ? c_count + 1 : 0;
          }
        }

        m_rowid.append( query.value(0).toString() );
        m_count.append( c_count );

        if (m_rowid.size() >= 50000) {
          qUpdate.prepare( sql_update );
          qUpdate.addBindValue( m_count );
          qUpdate.addBindValue( m_rowid );
          qUpdate.execBatch();
          m_count.clear();
          m_rowid.clear();
        }
      }

      if (m_rowid.size() > 0) {
        qUpdate.prepare( sql_update );
        qUpdate.addBindValue( m_count );
        qUpdate.addBindValue( m_rowid );
        qUpdate.execBatch();
        m_count.clear();
        m_rowid.clear();
      }
    }

    // Calculate Rank for CGF, CLF, CGS, CLS after threshold applied
    static void update_close_threshold_rank(const QSqlDatabase &database, const int &id_threshold) {
      QSqlQuery qUpdate(database);
      QSqlQuery query_1(database);
      QSqlQuery query_2(database);
      QSqlQuery query_3(database);
      QSqlQuery query_4(database);
      QVariantList rank_1;
      QVariantList rank_2;
      QVariantList rank_3;
      QVariantList rank_4;
      QVariantList rowid_1;
      QVariantList rowid_2;
      QVariantList rowid_3;
      QVariantList rowid_4;
      QString tid = "_" + QString::number(id_threshold); // zero-based
      QString CGF = SQLiteHandler::COLUMN_NAME_CGF + tid;
      QString CLF = SQLiteHandler::COLUMN_NAME_CLF + tid;
      QString CGS = SQLiteHandler::COLUMN_NAME_CGS + tid;
      QString CLS = SQLiteHandler::COLUMN_NAME_CLS + tid;
      QString sql_update_1 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CGF_RANK + tid + "=? where rowid=?";
      QString sql_update_2 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CLF_RANK + tid + "=? where rowid=?";
      QString sql_update_3 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CGS_RANK + tid + "=? where rowid=?";
      QString sql_update_4 = "update bardata set " + SQLiteHandler::COLUMN_NAME_CLS_RANK + tid + "=? where rowid=?";
      long i = 0;
      bool b[4] = {false, false, false, false};

      query_1.setForwardOnly(true);
      query_2.setForwardOnly(true);
      query_3.setForwardOnly(true);
      query_4.setForwardOnly(true);

      query_1.exec("select count(1) from bardata where " + CGF + " > 0");
      double count_1 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where " + CLF + " > 0");
      double count_2 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where " + CGS + " > 0");
      double count_3 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select count(1) from bardata where " + CLS + " > 0");
      double count_4 = query_1.next()? query_1.value(0).toDouble() : 0;

      query_1.exec("select rowid from bardata where " + CGF + "> 0 order by " + CGF + " asc,rowid asc");
      query_2.exec("select rowid from bardata where " + CLF + "> 0 order by " + CLF + " asc,rowid asc");
      query_3.exec("select rowid from bardata where " + CGS + "> 0 order by " + CGS + " asc,rowid asc");
      query_4.exec("select rowid from bardata where " + CLS + "> 0 order by " + CLS + " asc,rowid asc");

      qUpdate.setForwardOnly(true);
      qUpdate.exec("PRAGMA journal_mode = OFF;");
      qUpdate.exec("PRAGMA synchronous = OFF;");
      qUpdate.exec("PRAGMA cache_size = 50000;");
      qUpdate.exec("BEGIN TRANSACTION;");

      while (true) {
        b[0] = query_1.next();
        b[1] = query_2.next();
        b[2] = query_3.next();
        b[3] = query_4.next();
        if (!b[0] && !b[1] && !b[2] && !b[3]) break;

        if (b[0]) {
          rowid_1.append(query_1.value(0).toString());
          rank_1.append(QString::number(i / count_1,'f',4));
        }

        if (b[1]) {
          rowid_2.append(query_2.value(0).toString());
          rank_2.append(QString::number(i / count_2,'f',4));
        }

        if (b[2]) {
          rowid_3.append(query_3.value(0).toString());
          rank_3.append(QString::number(i / count_3,'f',4));
        }

        if (b[3]) {
          rowid_4.append(query_4.value(0).toString());
          rank_4.append(QString::number(i / count_4,'f',4));
        }

        if (rowid_1.size() >= 80000) {
          qUpdate.prepare(sql_update_1);
          qUpdate.addBindValue(rank_1);
          qUpdate.addBindValue(rowid_1);
          qUpdate.execBatch();
          rank_1.clear();
          rowid_1.clear();
        }

        if (rowid_2.size() >= 80000) {
          qUpdate.prepare(sql_update_2);
          qUpdate.addBindValue(rank_2);
          qUpdate.addBindValue(rowid_2);
          qUpdate.execBatch();
          rank_2.clear();
          rowid_2.clear();
        }

        if (rowid_3.size() >= 80000) {
          qUpdate.prepare(sql_update_3);
          qUpdate.addBindValue(rank_3);
          qUpdate.addBindValue(rowid_3);
          qUpdate.execBatch();
          rank_3.clear();
          rowid_3.clear();
        }

        if (rowid_4.size() >= 80000) {
          qUpdate.prepare(sql_update_4);
          qUpdate.addBindValue(rank_4);
          qUpdate.addBindValue(rowid_4);
          qUpdate.execBatch();
          rank_4.clear();
          rowid_4.clear();
        }

        ++i;
      }

      if (rowid_1.size() > 0) {
        qUpdate.prepare(sql_update_1);
        qUpdate.addBindValue(rank_1);
        qUpdate.addBindValue(rowid_1);
        qUpdate.execBatch();
        rank_1.clear();
        rowid_1.clear();
      }

      if (rowid_2.size() > 0) {
        qUpdate.prepare(sql_update_2);
        qUpdate.addBindValue(rank_2);
        qUpdate.addBindValue(rowid_2);
        qUpdate.execBatch();
        rank_2.clear();
        rowid_2.clear();
      }

      if (rowid_3.size() > 0) {
        qUpdate.prepare(sql_update_3);
        qUpdate.addBindValue(rank_3);
        qUpdate.addBindValue(rowid_3);
        qUpdate.execBatch();
        rank_3.clear();
        rowid_3.clear();
      }

      if (rowid_4.size() > 0) {
        qUpdate.prepare(sql_update_4);
        qUpdate.addBindValue(rank_4);
        qUpdate.addBindValue(rowid_4);
        qUpdate.execBatch();
        rank_4.clear();
        rowid_4.clear();
      }

      qUpdate.exec("COMMIT;");
    }


    // to remove trailing zero after decimal point
    // examples: 0.2500 into 0.25, 5.0 into 5
    static QString remove_trailing_zero(const QString &value) {
      if (value.toDouble() == 0) return "0";

      QString temp = value;
      const int idx0 = value.indexOf("."); // decimal point position
      int idx1 = value.length() - 1; // last digit position

      while ( idx1 > idx0 ) {
        if ( temp[idx1] != QChar('0') ) break;
        --idx1;
      }

      if ( idx1 < value.length() - 1 && idx1 >= idx0 ) {
        if ( idx1 > idx0 ) {
          temp.remove(idx1 + 1,  value.length() - idx1 - 1);
        } else {
          temp.remove(idx1,  value.length() - idx1);
        }
      }

      return temp;
    }

  private:
    int _rid;
    QStringList projection;
    QString SQL_INSERT_BARDATA;
    QVariantList date;
    QVariantList time;
    QVariantList open;
    QVariantList high;
    QVariantList low;
    QVariantList close;
    QVariantList volume;
    QVariantList macd;
    QVariantList macdavg;
    QVariantList macddiff;
    QVariantList rsi;
    QVariantList slowk;
    QVariantList slowd;
    QVariantList fastavg;
    QVariantList slowavg;
    QVariantList distf;
    QVariantList dists;
    QVariantList fgs;
    QVariantList fls;
    QVariantList openbar;
    QVariantList dist_fscross;
    QVariantList dist_fscross_atr;
    QVariantList atr;
    QVariantList distfc;
    QVariantList distsc;
    QVariantList fastavgslope;
    QVariantList slowavgslope;
    QVariantList prevdate;
    QVariantList prevtime;
    QVariantList prevbarcolor;
    QVariantList macd_g0;
    QVariantList macd_l0;
    QVariantList rsi_g70;
    QVariantList rsi_l30;
    QVariantList slowk_g80;
    QVariantList slowk_l20;
    QVariantList slowd_g80;
    QVariantList slowd_l20;
    QVariantList distcf;
    QVariantList distcs;
    QVariantList distfs;
    QVariantList candle_uptail;
    QVariantList candle_downtail;
    QVariantList candle_body;
    QVariantList candle_totallength;
    QVariantList n_distcf;
    QVariantList n_distcs;
    QVariantList n_distfs;
    QVariantList datetime_ub;
    QVariantList datetime_lb;
    QVariantList _parent;
    QVariantList _parent_prev;
    QVariantList cgf;
    QVariantList clf;
    QVariantList cgs;
    QVariantList cls;
    QVariantList distof;
    QVariantList distos;
    QVariantList n_distof;
    QVariantList n_distos;
    QVariantList disthf;
    QVariantList disths;
    QVariantList n_disthf;
    QVariantList n_disths;
    QVariantList distlf;
    QVariantList distls;
    QVariantList n_distlf;
    QVariantList n_distls;
    QVariantList fcross;
    QVariantList scross;

    void init_sql_insert() {
      projection.clear();
      projection.append(SQLiteHandler::COLUMN_NAME_DATE);
      projection.append(SQLiteHandler::COLUMN_NAME_TIME);
      projection.append(SQLiteHandler::COLUMN_NAME_OPEN);
      projection.append(SQLiteHandler::COLUMN_NAME_HIGH);
      projection.append(SQLiteHandler::COLUMN_NAME_LOW);
      projection.append(SQLiteHandler::COLUMN_NAME_CLOSE);
      projection.append(SQLiteHandler::COLUMN_NAME_VOLUME);
      projection.append(SQLiteHandler::COLUMN_NAME_MACD);
      projection.append(SQLiteHandler::COLUMN_NAME_MACDAVG);
      projection.append(SQLiteHandler::COLUMN_NAME_MACDDIFF);
      projection.append(SQLiteHandler::COLUMN_NAME_RSI);
      projection.append(SQLiteHandler::COLUMN_NAME_SLOWK);
      projection.append(SQLiteHandler::COLUMN_NAME_SLOWD);
      projection.append(SQLiteHandler::COLUMN_NAME_FASTAVG);
      projection.append(SQLiteHandler::COLUMN_NAME_SLOWAVG);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTF);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTS);
      projection.append(SQLiteHandler::COLUMN_NAME_FGS);
      projection.append(SQLiteHandler::COLUMN_NAME_FLS);
      projection.append(SQLiteHandler::COLUMN_NAME_OPENBAR);
      projection.append(SQLiteHandler::COLUMN_NAME_FASTAVG_SLOPE);
      projection.append(SQLiteHandler::COLUMN_NAME_SLOWAVG_SLOPE);
      projection.append(SQLiteHandler::COLUMN_NAME_PREV_DATE);
      projection.append(SQLiteHandler::COLUMN_NAME_PREV_TIME);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTCF);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTCS);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTFS);
      projection.append(SQLiteHandler::COLUMN_NAME_ATR);
      projection.append(SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR);
//      projection.append(SQLiteHandler::COLUMN_NAME_RSI_G70);
//      projection.append(SQLiteHandler::COLUMN_NAME_RSI_L30);
//      projection.append(SQLiteHandler::COLUMN_NAME_MACD_G0);
//      projection.append(SQLiteHandler::COLUMN_NAME_MACD_L0);
//      projection.append(SQLiteHandler::COLUMN_NAME_SLOWK_G80);
//      projection.append(SQLiteHandler::COLUMN_NAME_SLOWK_L20);
//      projection.append(SQLiteHandler::COLUMN_NAME_SLOWD_G80);
//      projection.append(SQLiteHandler::COLUMN_NAME_SLOWD_L20);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_ATR);
      projection.append(SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL);
      projection.append(SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL);
      projection.append(SQLiteHandler::COLUMN_NAME_CANDLE_BODY);
      projection.append(SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTCF);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTCS);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTFS);
      projection.append(SQLiteHandler::COLUMN_NAME_IDPARENT);
      projection.append(SQLiteHandler::COLUMN_NAME_CGF);
      projection.append(SQLiteHandler::COLUMN_NAME_CLF);
      projection.append(SQLiteHandler::COLUMN_NAME_CGS);
      projection.append(SQLiteHandler::COLUMN_NAME_CLS);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTOF);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTOS);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTOF);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTOS);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTHF);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTHS);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTHF);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTHS);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTLF);
      projection.append(SQLiteHandler::COLUMN_NAME_DISTLS);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTLF);
      projection.append(SQLiteHandler::COLUMN_NAME_N_DISTLS);
      projection.append(SQLiteHandler::COLUMN_NAME_FCROSS);
      projection.append(SQLiteHandler::COLUMN_NAME_SCROSS);
//      projection.append(SQLiteHandler::COLUMN_NAME_DATETIME_UB);
//      projection.append(SQLiteHandler::COLUMN_NAME_DATETIME_LB);

      QString s = ",?";
      SQL_INSERT_BARDATA =
        "INSERT OR IGNORE INTO " + SQLiteHandler::TABLE_NAME_BARDATA + "(" + projection.join(",") + ")" +
        "VALUES(?" + s.repeated(projection.size()-1) + ")";
    }

    void insertBatchSqlite_v2(QSqlQuery *query) {
      query->prepare(SQL_INSERT_BARDATA);
      query->addBindValue(date);
      query->addBindValue(time);
      query->addBindValue(open);
      query->addBindValue(high);
      query->addBindValue(low);
      query->addBindValue(close);
      query->addBindValue(volume);
      query->addBindValue(macd);
      query->addBindValue(macdavg);
      query->addBindValue(macddiff);
      query->addBindValue(rsi);
      query->addBindValue(slowk);
      query->addBindValue(slowd);
      query->addBindValue(fastavg);
      query->addBindValue(slowavg);
      query->addBindValue(distf);
      query->addBindValue(dists);
      query->addBindValue(fgs);
      query->addBindValue(fls);
      query->addBindValue(openbar);
      query->addBindValue(fastavgslope);
      query->addBindValue(slowavgslope);
      query->addBindValue(prevdate);
      query->addBindValue(prevtime);
      query->addBindValue(distcf);
      query->addBindValue(distcs);
      query->addBindValue(distfs);
      query->addBindValue(atr);
      query->addBindValue(prevbarcolor);
//      query->addBindValue(rsi_g70);
//      query->addBindValue(rsi_l30);
//      query->addBindValue(macd_g0);
//      query->addBindValue(macd_l0);
//      query->addBindValue(slowk_g80);
//      query->addBindValue(slowk_l20);
//      query->addBindValue(slowd_g80);
//      query->addBindValue(slowd_l20);
      query->addBindValue(dist_fscross);
      query->addBindValue(dist_fscross_atr);
      query->addBindValue(candle_uptail);
      query->addBindValue(candle_downtail);
      query->addBindValue(candle_body);
      query->addBindValue(candle_totallength);
      query->addBindValue(n_distcf);
      query->addBindValue(n_distcs);
      query->addBindValue(n_distfs);
      query->addBindValue(_parent);
      query->addBindValue(cgf);
      query->addBindValue(clf);
      query->addBindValue(cgs);
      query->addBindValue(cls);
      query->addBindValue(distof);
      query->addBindValue(distos);
      query->addBindValue(n_distof);
      query->addBindValue(n_distos);
      query->addBindValue(disthf);
      query->addBindValue(disths);
      query->addBindValue(n_disthf);
      query->addBindValue(n_disths);
      query->addBindValue(distlf);
      query->addBindValue(distls);
      query->addBindValue(n_distlf);
      query->addBindValue(n_distls);
      query->addBindValue(fcross);
      query->addBindValue(scross);
//      query->addBindValue(datetime_ub);
//      query->addBindValue(datetime_lb);
      query->execBatch();
    }

    void reset() {
      date.clear();
      time.clear();
      open.clear();
      high.clear();
      low.clear();
      close.clear();
      volume.clear();
      macd.clear();
      macdavg.clear();
      macddiff.clear();
      rsi.clear();
      slowk.clear();
      slowd.clear();
      fastavg.clear();
      slowavg.clear();
      distf.clear();
      dists.clear();
      fgs.clear();
      fls.clear();
      openbar.clear();
      dist_fscross.clear();
      dist_fscross_atr.clear();
      atr.clear();
      distfc.clear();
      distsc.clear();
      fastavgslope.clear();
      slowavgslope.clear();
      prevdate.clear();
      prevtime.clear();
      prevbarcolor.clear();
      macd_g0.clear();
      macd_l0.clear();
      rsi_g70.clear();
      rsi_l30.clear();
      slowk_g80.clear();
      slowk_l20.clear();
      slowd_g80.clear();
      slowd_l20.clear();
      distcf.clear();
      distcs.clear();
      distfs.clear();
      candle_uptail.clear();
      candle_downtail.clear();
      candle_body.clear();
      candle_totallength.clear();
      n_distcf.clear();
      n_distcs.clear();
      n_distfs.clear();
      datetime_ub.clear();
      datetime_lb.clear();
      _parent.clear();
      _parent_prev.clear();
      cgf.clear();
      clf.clear();
      cgs.clear();
      cls.clear();
      distof.clear();
      distos.clear();
      n_distof.clear();
      n_distos.clear();
      disthf.clear();
      disths.clear();
      n_disthf.clear();
      n_disths.clear();
      distlf.clear();
      distls.clear();
      n_distlf.clear();
      n_distls.clear();
      fcross.clear();
      scross.clear();
    }

    t_bardata getlastRecord(const QSqlDatabase &db) {
      QSqlQuery query(db);
      query.setForwardOnly(true);
      query.exec("SELECT MAX(" + SQLiteHandler::COLUMN_NAME_ROWID + ")," +
                 SQLiteHandler::COLUMN_NAME_OPEN + "," +
                 SQLiteHandler::COLUMN_NAME_CLOSE + "," +
                 SQLiteHandler::COLUMN_NAME_FASTAVG + "," +
                 SQLiteHandler::COLUMN_NAME_SLOWAVG + "," +
                 SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR + "," +
//                 SQLiteHandler::COLUMN_NAME_MACD_G0 + "," +
//                 SQLiteHandler::COLUMN_NAME_MACD_L0 + "," +
                 SQLiteHandler::COLUMN_NAME_CGF + "," +
                 SQLiteHandler::COLUMN_NAME_CLF + "," +
                 SQLiteHandler::COLUMN_NAME_CGS + "," +
                 SQLiteHandler::COLUMN_NAME_CLS +
                 " FROM " + SQLiteHandler::TABLE_NAME_BARDATA);
      t_bardata r;
      if (query.next() && !query.value(0).isNull()) {
        r.rowid = query.value(0).toInt();
        r.open = query.value(1).toDouble();
        r.close = query.value(2).toDouble();
        r.fastavg = query.value(3).toDouble();
        r.slowavg = query.value(4).toDouble();
        r.prevbarcolor = query.value(5).toString();
//        r.macd_g0 = query.value(6).toInt();
//        r.macd_l0 = query.value(7).toInt();
        r.cgf = query.value(6).toInt();
        r.clf = query.value(7).toInt();
        r.cgs = query.value(8).toInt();
        r.cls = query.value(9).toInt();
      } else {
        r.rowid = 0;
        r.open = 0;
        r.close = 0;
        r.fastavg = 0;
        r.slowavg = 0;
        r.prevbarcolor = "";
        r.rsi_g70 = 0;
        r.rsi_l30 = 0;
        r.macd_g0 = 0;
        r.macd_l0 = 0;
        r.slowk_g80 = 0;
        r.slowk_l20 = 0;
        r.slowd_g80 = 0;
        r.slowd_l20 = 0;
        r.cgf = 0;
        r.clf = 0;
        r.cgs = 0;
        r.cls = 0;
      }
      return r;
    }

    // deprecated
    /*QVariantList u_date;
    QVariantList u_time;
    QVariantList u_open;
    QVariantList u_high;
    QVariantList u_low;
    QVariantList u_close;
    QVariantList u_volume;
    QVariantList u_macd;
    QVariantList u_macdavg;
    QVariantList u_macddiff;
    QVariantList u_rsi;
    QVariantList u_slowk;
    QVariantList u_slowd;
    QVariantList u_fastavg;
    QVariantList u_slowavg;
    QVariantList u_distf;
    QVariantList u_dists;
    QVariantList u_fgs;
    QVariantList u_fls;
    QVariantList u_openbar;*/

    /*void insertBatchSqlite(const QSqlDatabase &db) {
      QSqlQuery query(db);
      query.exec("PRAGMA journal_mode = DELETE;");
      query.exec("PRAGMA synchronous = FULL;");
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("BEGIN TRANSACTION;");
      query.prepare(SQLiteHandler::SQL_INSERT_BARDATA_V2);
      query.addBindValue(date);
      query.addBindValue(time);
      query.addBindValue(open);
      query.addBindValue(high);
      query.addBindValue(low);
      query.addBindValue(close);
      query.addBindValue(volume);
      query.addBindValue(macd);
      query.addBindValue(macdavg);
      query.addBindValue(macddiff);
      query.addBindValue(rsi);
      query.addBindValue(slowk);
      query.addBindValue(slowd);
      query.addBindValue(fastavg);
      query.addBindValue(slowavg);
      query.addBindValue(distf);
      query.addBindValue(dists);
      query.addBindValue(fgs);
      query.addBindValue(fls);
      query.addBindValue(openbar);
      query.execBatch();
      query.exec("COMMIT;");
    }*/

    /*void updateBatchSqlite(const QSqlDatabase &db) {
      QSqlQuery query(db);
      query.exec("PRAGMA journal_mode = DELETE;");
      query.exec("PRAGMA synchronous = NORMAL;");
      query.exec("PRAGMA cache_size = 50000;");
      query.exec("BEGIN TRANSACTION;");
      query.prepare(SQLiteHandler::SQL_UPDATE_BARDATA_V2);
      query.addBindValue(u_open);
      query.addBindValue(u_high);
      query.addBindValue(u_low);
      query.addBindValue(u_close);
      query.addBindValue(u_volume);
      query.addBindValue(u_macd);
      query.addBindValue(u_macdavg);
      query.addBindValue(u_macddiff);
      query.addBindValue(u_rsi);
      query.addBindValue(u_slowk);
      query.addBindValue(u_slowd);
      query.addBindValue(u_fastavg);
      query.addBindValue(u_slowavg);
      query.addBindValue(u_distf);
      query.addBindValue(u_dists);
      query.addBindValue(u_fgs);
      query.addBindValue(u_fls);
      query.addBindValue(u_openbar);
      query.addBindValue(u_date);
      query.addBindValue(u_time);
      query.execBatch();
      query.exec("COMMIT;");
    }*/

    /*void reset_update_list() {
      u_date.clear();
      u_time.clear();
      u_open.clear();
      u_high.clear();
      u_low.clear();
      u_close.clear();
      u_volume.clear();
      u_macd.clear();
      u_macdavg.clear();
      u_macddiff.clear();
      u_rsi.clear();
      u_slowk.clear();
      u_slowd.clear();
      u_fastavg.clear();
      u_slowavg.clear();
      u_distf.clear();
      u_dists.clear();
      u_fgs.clear();
      u_fls.clear();
      u_openbar.clear();
    }*/
};

#endif // BARDATAADAPTER_H
