/**
 * MySqlConnector
 */
#ifndef MYSQLCONN_H
#define MYSQLCONN_H

#include <QDebug>
#include <QString>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>

class MySqlConnector {
  private:
    // configuration should be flexible in future
    // for development purpose, we hard-coded these parameters
    const QString connectionName = "2015.finance.project";
    const QString databaseName = "finance";
    const QString hostName = "localhost";
    const QString userName = "root";
    const QString password = "root";
    const static int port = 3306;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);

  public:
    MySqlConnector() {}
    ~MySqlConnector() { closeDb(); }

    bool connectDb() {
      if (db.isOpen()) return true;
      db.setDatabaseName(databaseName);
      db.setHostName(hostName);
      db.setPort(port);
      db.setUserName(userName);
      db.setPassword(password);
      return db.open();
    }

    void closeDb() {
      if (db.isOpen()) {
        QString conn = db.connectionName();
        db.close();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(conn);
      }
    }

    QSqlDatabase getDatabase() {
      return db;
    }

    QStringList getColumnNames(QString tableName) {
      QStringList res;
      if (db.isOpen()) {
        QSqlRecord rec = db.record(tableName);
        int N = rec.count();
        for (int i = 0; i < N; ++i) {
          res.append(rec.fieldName(i));
        }
      }
      return res;
    }

};

#endif // MYSQLCONN_H
