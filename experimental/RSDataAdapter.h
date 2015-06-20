/**
 * Resistance & Support Data Adapter
 */
#ifndef RSDATAADAPTER_H
#define RSDATAADAPTER_H

#include <QVariantList>

#include "SQLiteHandler.h"

class RSDataAdapter {
  public:
    void insertBatch(const QSqlDatabase &db, int file_type) {
      QSqlQuery query(db);
      query.exec("PRAGMA journal_mode = MEMORY;");
      query.exec("PRAGMA cache_size = -64;");
      query.exec("PRAGMA page_size = 8192;");
      query.exec("BEGIN TRANSACTION;");
      if (file_type == RESISTANCE_FILE) {
        query.prepare(SQLiteHandler::SQL_INSERT_RESISTANCE_V1);
      } else {
        query.prepare(SQLiteHandler::SQL_INSERT_SUPPORT_V1);
      }
      query.addBindValue(date_);
      query.addBindValue(time_);
      query.addBindValue(react_date);
      query.addBindValue(react_time);
      query.addBindValue(rs_value);
      query.execBatch();
      query.exec("COMMIT;");
    }

    long loadFile(const QSqlDatabase &db, QString workDir, QString filename) {
      QFile file(workDir + "//" + filename);
      int file_mode = -1;
      int interval_mode = -1;
      long numOfRows = 0;
      QStringList s = filename.split(".");
      QStringList ss = s[0].split("_");
      if (ss.length() > 2) {
        if (ss[1].contains("min", Qt::CaseInsensitive)) {
          interval_mode = MINUTE_INTERVAL;
        } else {
          interval_mode = DAY_INTERVAL;
        }
        if (ss[2].toLower() == "resistance") {
          file_mode = RESISTANCE_FILE ;
        } else {
          file_mode = SUPPORT_FILE;
        }
      }

      if (file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        QStringList cells;
        QStringList dateCells;
        QDate r_date;
        QString r_time;
        int r_count;
        int r_start;
        int counter = 0;

        while (!in.atEnd()) {
          line = in.readLine();
          cells = line.split(" ", QString::KeepEmptyParts);
          if (interval_mode == MINUTE_INTERVAL) {
            dateCells = cells[0].split(",");
            r_date = QDate::fromString(dateCells[0].trimmed(), "MM/dd/yyyy");
            r_time = dateCells[1].trimmed();
          } else {
            r_date = QDate::fromString(cells.at(0).trimmed(), "MM/dd/yyyy");
            r_time = "00:00";
          }
          r_count = cells[1].trimmed().toInt();
          r_start = r_count + 2;
          for (int i = 0; i < r_count; ++i) {
            date_ += r_date;
            time_ += r_time;
            if (interval_mode == MINUTE_INTERVAL) {
              dateCells = cells[2+i].split(",");
              react_date += QDate::fromString(dateCells[0].trimmed(), "MM/dd/yyyy");
              react_time += dateCells[1].trimmed();
            } else {
              react_date += QDate::fromString(cells[2+i].trimmed(), "MM/dd/yyyy");
              react_time += "00:00";
            }
            rs_value += cells[r_start+i].trimmed().toDouble();
            if (++counter >= 30000) {
              insertBatch(db, file_mode);
              numOfRows += counter;
              counter = 0;
              reset();
            }
          }
        }
        file.close();
        insertBatch(db, file_mode);
        numOfRows += counter;
        reset();
      }
      return numOfRows;
    }

  private:
    static const int RESISTANCE_FILE = 0;
    static const int SUPPORT_FILE = 1;
    static const int MINUTE_INTERVAL = 0;
    static const int DAY_INTERVAL = 1;
    QVariantList date_;
    QVariantList time_;
    QVariantList react_date;
    QVariantList react_time;
    QVariantList rs_value;

    void reset() {
      date_.clear();
      time_.clear();
      react_date.clear();
      react_time.clear();
      rs_value.clear();
    }
};

#endif // RSDATAADAPTER_H
