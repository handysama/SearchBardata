#ifndef BARBUFFER_H
#define BARBUFFER_H
#include <algorithm>
#include <vector>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QLineEdit>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QTextStream>
#include <QTime>
#include <QVariant>
#include <QVariantList>
#include <QVector>

using std::vector;

class BarBuffer {
  public:
    void SRload(QFile *file);
    void load(QFile *file);
    void clear();

//  int findIndexWeekly(QDate &date); // find index with Date
//  int findIndexWithDateTime(QDate &date, QTime &time);
//  QString tuple(int i);
//  void debug() {
//    for (int i = 0; i < this->datetime.length(); i++) {
//      qDebug() << this->datetime[i]
//               << this->date[i]
//               << this->time[i]
//               << this->open[i]
//               << this->high[i]
//               << this->low[i]
//               << this->close[i];
//    }
//  }


  //Check the support
  void SupportChecking(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    std::vector<int> *Doutput, std::vector<int> *NumberOfHit, std::vector<double> *duration);

  void SupportChecking_v2(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    QVector<int> *Doutput,QVector<int> *NumberOfHit, QVector<double> *duration, QVector<QVector<int>> *ReactDate);

  //Check the Resistance
  void ResistanceChecking(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    std::vector<int> *Doutput,std::vector<int> *NumberOfHit, std::vector<double> *duration);

  void ResistanceChecking_v2(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    QVector<int> *Doutput,QVector<int> *NumberOfHit, QVector<double> *duration, QVector<QVector<int>> *ReactDate);


// Return the output of the resistance
//  QString ShowResistance(vector<int> *Doutput, vector<int> *NumberOfHit, vector<int> *HittingDate);

// Return the output of the support
//  QString ShowSupport(vector<int> *Doutput, vector<int> *NumberOfHit, vector<int> *HittingDate);

// Return the results of the support
//  void SupportResults(int Index, vector<int> *Doutput, vector<int> *NumberOfHit,vector<QString> *FirstReact, vector<QString> *SR
//                     , vector<QString> *TestDays);

// Return the results of the resistance
//  void ResistanceResults(int Index, vector<int> *Doutput, vector<int> *NumberOfHit, vector<QString> *FirstReact, vector<QString> *SR
//                     , vector<QString> *TestDays);

//  void loadIndicatorList(QStringList& items);
//  void clearIndicatorList();
//  int findIndexDaily(const QDate &SDate);
//  int FindCloseLastDay(int Index);
//  void WriteSR(QString fileName, int SR, double b, double r, int s);
//  void LoadFromDB(QDate &d, QTime &t, QString symbol, QString interval, int CacheSize);
//  void LoadFromDB(QSqlQuery* qLoad, int CacheSize);

  void ClearMemory();

  int getSize();

  void initialize(int CacheSize);

  int maxReactSupport(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    QVariantList *v_value, QVariantList *v_date, QVariantList *v_time, QVariantList *v_rcount, QVariantList *v_duration,
    QVector<QVariantList>* ReactList,QVariantList* LastDuration, QVariantList *FirstDuration);

  int maxReactResistance(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    QVariantList *v_value, QVariantList *v_date, QVariantList *v_time, QVariantList *v_rcount, QVariantList *v_duration,
    QVector<QVariantList>* ReactList, QVariantList* LastDuration, QVariantList *FirstDuration);

  void DBResistanceSupportGen(const QSqlDatabase &db, const double &b, const double &r,
    const int &threshold, const int &start_rowid_from, const int &id_threshold);

  void DBResistanceSupportGen_Recalculate(const QSqlDatabase &database, const double &b,
    const double &r, const int &threshold, const int &id_threshold);

  void insert_resistance_detail(QSqlQuery *query, const QDate &date_, const QTime &time_,
    const QVariantList &rdate, const QVariantList &rtime, const QVariantList &v_value, const QVariantList &v_rcount,
    const QVariantList &v_duration, const QVariantList &v_last_duration, const QVariantList &v_first_duration, const int &id_threshold);

  void insert_support_detail(QSqlQuery *query, const QDate &date_, const QTime &time_,
    const QVariantList &rdate, const QVariantList &rtime, const QVariantList &v_value, const QVariantList &v_rcount,
    const QVariantList &v_duration, const QVariantList &v_last_duration, const QVariantList &v_first_duration, const int &id_threshold);

  // denormalize
//  void sqlite_insert_resistance_wtf(QSqlQuery *query, const int &column_count, const QDate &date_,
//      const QTime &time_, const QVariantList &v_value, const int &threshold_index);

  // denormalize
//  void sqlite_insert_support_wtf(QSqlQuery *query, const int &column_count, const QDate &date_,
//      const QTime &time_, const QVariantList &v_value, const int &threshold_index);


//  QString rangeOfBars;
//  QVector<QVector<double> > indicatorValues;
//  QStringList indicatorNames;
//  QListWidget *indicatorList;
  QVector<QDateTime> datetime;
  QVector<QDate> date;
  QVector<QTime> time;
  QVector<double> open;
  QVector<double> close;
  QVector<double> high;
  QVector<double> low;
  QVector<int>  firstbar;
  QVector<double> MACD;
  QVector<double> MACDAvg;
  QVector<double> MACDDiff;
  QVector<double> RSI;
  QVector<double> SlowK;
  QVector<double> SlowD;
  QVector<double> Volume;
  QVector<double> FastAvg;
  QVector<double> SlowAvg;
  QVector<double> DistF;
  QVector<double> DistS;
  QVector<double> FGS;
  QVector<double> FLS;

  QLineEdit *numberOfBars;
  int CacheSize;
};

#endif // BARBUFFER_H
