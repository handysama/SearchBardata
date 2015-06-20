#ifndef SEARCHRESULTWRITER
#define SEARCHRESULTWRITER

#include <QRunnable>
#include <QSqlQuery>
#include "BarDataAdapter.h"
#include "SQLiteHandler.h"

class SearchResultWriter : public QRunnable {
  public:
    SearchResultWriter(QSqlQuery *query, const QString &filename, const QSqlDatabase &dbQuery, bool delete_flag = false) {
      this->query = query;
      this->filename = filename;
      this->db_query = dbQuery;
      this->delete_after_finish = delete_flag;
      this->setAutoDelete(true);
    }

    ~SearchResultWriter() {
      SQLiteHandler::removeDatabase(&db_query);
    }

    void run() {
      if (query != NULL) {
        BarDataAdapter::write_datetime_from_query(*query, filename, false);
        query->clear();
        if (delete_after_finish) {
          delete query;
          query = NULL;
        }
      }
    }

  private:
    QSqlQuery *query = NULL;
    QSqlDatabase db_query;
    QString filename;
    bool delete_after_finish;
};

#endif // SEARCHRESULTWRITER
