#ifndef DAILYSUPPORT_H
#define DAILYSUPPORT_H

#include <QVector>
#include "BarDataList.h"
#include "SQLiteHandler.h"

class DailySupport {

  public :
    void prepare_data(const QSqlDatabase &database) {
      QSqlQuery query(database);
      QString sql_select = "select date_, time_, open_, high_, low_, close_ from bardata order by rowid desc";

      query.setForwardOnly(true);
      query.exec(sql_select);

      while (query.next()) {
       m_date.push_back( QDate::fromString(query.value(0).toString(), "yyyy-MM-dd") );
       m_time.push_back( QTime::fromString(query.value(1).toString(), "hh:mm") );
       m_open.push_back( query.value(2).toDouble() );
       m_high.push_back( query.value(3).toDouble() );
       m_low.push_back( query.value(4).toDouble() );
       m_close.push_back( query.value(5).toDouble() );
      }
    }

    void calculate_daily_support(
        QVector<QDate> *oDate,
        QVector<QTime> *oTime,
        QVector<double> *oFirstLow,
        const int &barIndex,
        int period = 250,
        double breakThreshold = 1,
        double reactThreshold = 1,
        int testNumber = 2) {
      QDate d1, d2;
      QTime t1, t2;
      double p1, p2; // price
      int bb1 = 0; // bar back
      int bb2 = 0;
      double firstLow, priceCap;
      int testCount, lastTestBarBack;

      for ( int totalBar = period; totalBar >= 1; --totalBar ) {

        MinPriceAmongBar ( barIndex , m_date , m_time , m_open , m_close , totalBar , &d1 , &t1 , &p1 , &bb1 );
        priceCap = p1 + breakThreshold;

        FirstLowBelowPrice ( barIndex , m_date , m_time , m_low , priceCap , totalBar , &d2 , &t2 , &p2 , &bb2 );
        firstLow = p2;
        testCount = 0;

//        qDebug() << "Check" << priceCap << firstLow ;

        for ( int barBack = bb2 ; barBack >= 0 ; --barBack ) {
          if ( m_low[ barIndex + barBack ] <= firstLow + reactThreshold ) {
            ++testCount;
            lastTestBarBack = barBack;
          }
        }

        if ( testCount >= testNumber ) {
          for ( int barBack = bb2 ; barBack >= 0; --barBack ) {
            if ( m_low[ barIndex + barBack ] <= firstLow + reactThreshold ) {
              qDebug() << " BarBack" << barBack << m_date[ barIndex + barBack ] << m_time[ barIndex + barBack ] << firstLow;
              oDate->push_back( m_date[ barIndex + barBack ] );
              oTime->push_back( m_time[ barIndex + barBack ] );
              oFirstLow->push_back( firstLow );
            }
          }
        }

        qDebug() << "-----------------------------------------------WTF " << totalBar << lastTestBarBack;
        totalBar = lastTestBarBack + 1 ;

        oDate->clear();
        oTime->clear();
        oFirstLow->clear();
      }
    }

    void MinPriceAmongBar (const int &barIndex, const QVector<QDate> &iDate, const QVector<QTime> &iTime,
        const QVector<double> &iPrice1, // Open
        const QVector<double> &iPrice2, // Close
        const int &iTotalBar,
        QDate *oDate, QTime *oTime, double *oPrice, int *oBarBack) {

      double min_price;
      int vBarBack = iTotalBar - 1;
      int idx = barIndex + vBarBack;
      *oDate = iDate[idx];
      *oTime = iTime[idx];
      *oPrice = std::min( iPrice1[idx] , iPrice2[idx] );
      *oBarBack = vBarBack;

//      qDebug() << "MPB1" << (*oDate) << (*oTime) << (*oPrice) << (*oBarBack) << iPrice1[idx] << iPrice2[idx];

      for (vBarBack = iTotalBar - 2; vBarBack >= 0; --vBarBack) {
        idx = barIndex + vBarBack;
        min_price = std::min( iPrice1[idx] , iPrice2[idx] );
        if ( min_price < *oPrice ) {
          *oDate = iDate[idx];
          *oTime = iTime[idx];
          *oPrice = min_price;
          *oBarBack = vBarBack;
        }
      }

//      qDebug() << "MPB2" << (*oDate) << (*oTime) << (*oPrice) << (*oBarBack);
    }

    void FirstLowBelowPrice(const int &barIndex, const QVector<QDate> &iDate, const QVector<QTime> &iTime,
        const QVector<double> &iLow, const double &iPriceCap, const int &iTotalBar,
        QDate *oDate, QTime *oTime, double *oLow, int *oBarBack) {
      int idx;
      for (int vBarBack = iTotalBar - 1; vBarBack >= 0; --vBarBack) {
        idx = barIndex + vBarBack;
        if (iLow[idx] <= iPriceCap) {
          *oDate = iDate[idx];
          *oTime = iTime[idx];
          *oLow = iLow[idx];
          *oBarBack = vBarBack;
          vBarBack = 0;
        }
      }
    }


  private:
    QVector<QDate> m_date;
    QVector<QTime> m_time;
    QVector<double> m_open;
    QVector<double> m_high;
    QVector<double> m_close;
    QVector<double> m_low;
};

#endif // DAILYSUPPORT_H
