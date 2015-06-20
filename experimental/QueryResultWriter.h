#ifndef QUERYRESULTWRITER
#define QUERYRESULTWRITER

#include <QSqlDatabase>
#include <QThread>
#include "BarDataAdapter.h"

class QueryResultWriter : public QThread {
  Q_OBJECT
  public:
    QueryResultWriter() {
      connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
      setTerminationEnabled(true);
    }

    ~QueryResultWriter() {
      if (query != NULL) {
        delete query;
        query = NULL;
      }
      if (query_view != NULL) {
        delete query_view;
        query_view = NULL;
      }
    }

    void execute_process(QSqlQuery *query, QSqlQuery *query_view, const QString &filename) {
      if (!isRunning()) {
        this->query = query;
        this->query_view = query_view;
        this->filename = filename;
//        this->sql = sql;
        start();
      }
    }

  private:
    QSqlQuery *query = NULL;
    QSqlQuery *query_view = NULL;
    QString filename;
//    QSqlDatabase database;
//    QString sql;

    void run() {
//      BarDataAdapter::write_datetime_from_query(database, sql, filename, false);

      if (query != NULL) {
        BarDataAdapter::write_datetime_from_query(*query, filename, false);
        delete query;
        query = NULL;
      }

      if (query_view != NULL) {
        BarDataAdapter::write_result_from_query(*query_view, filename + ".csv");
        delete query_view;
        query_view = NULL;
      }

      quit();
    }
};

#endif // QUERYRESULTWRITER
