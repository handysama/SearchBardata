#include "barbuffer.h"

void insert_resistance_react_date_detail(QSqlQuery *query, const int &last_rowid, const QVariantList &rdate, const QVector<QVariantList> &react_list) {
  QVariantList rid;
  QVariantList react_list_1;
  int M;
  int N = rdate.size();
  int m_rid = last_rowid - rdate.size() + 1;

  for (int i = 0; i < N; ++i) {
    M = react_list[i].size();
    for (int j = 0; j < M; ++j) {
      rid.append(m_rid);
      react_list_1.append(react_list[i][j]);
    }
    ++m_rid;
  }

  query->prepare("insert into " + SQLiteHandler::TABLE_NAME_RESISTANCE_DATE + "(rid, react_date) values(?,?)");
  query->addBindValue(rid);
  query->addBindValue(react_list_1);
  query->execBatch();
  if (query->lastError().isValid()) qDebug() << query->lastError();
}

void insert_support_react_date_detail(QSqlQuery *query, const int &last_rowid, const QVariantList &rdate, const QVector<QVariantList> &react_list) {
  QVariantList rid;
  QVariantList react_list_1;
  int M;
  int N = rdate.size();
  int m_rid = last_rowid - rdate.size() + 1;

  for (int i = 0; i < N; ++i) {
    M = react_list[i].size();
    for (int j = 0; j < M; ++j) {
      rid.append(m_rid);
      react_list_1.append(react_list[i][j]);
    }
    ++m_rid;
  }

  query->prepare("insert into " + SQLiteHandler::TABLE_NAME_SUPPORT_DATE + "(rid, react_date) values(?,?)");
  query->addBindValue(rid);
  query->addBindValue(react_list_1);
  query->execBatch();
  if (query->lastError().isValid()) qDebug() << query->lastError();
}


bool desc_compare(const QVariant &a, const QVariant &b) {
  return a.toDouble() > b.toDouble();
}

void sort_descending_list(QVariantList *v) {
  std::sort(v->begin(), v->end(), desc_compare);
}


/*
void BarBuffer::SRload(QFile *file)
{
    QTextStream in(file);

    // clear data already read
    this->clear();

    // get file size
    int max = file->bytesAvailable();

    // read the field names
    this->indicatorNames = in.readLine().split(QRegExp("\"|\",\"|Date|Time"), QString::SkipEmptyParts);
    this->indicatorNames.pop_back();
    //this->loadIndicatorList(this->indicatorNames);

    // allocate memory for stack
    this->datetime.resize(1000000);
    this->date.resize(1000000);
    this->time.resize(1000000);
    this->open.resize(1000000);
    this->close.resize(1000000);
    this->high.resize(1000000);
    this->low.resize(1000000);
    this->firstbar.resize(1000000);

    // allocate memory for indicators
    for (int i = 0; i < this->indicatorNames.length(); ++i)
    {
        this->indicatorValues.append(QVector<double>());
        this->indicatorValues.back().resize(1000000);
    }

    // set progressbar
    this->progressBar->setRange(0, max);
    this->progressBar->setVisible(true);

    // process each line
    int cnt(-1); // line counter
    while (!in.atEnd())
    {
        // inc progress bar
        QCoreApplication::processEvents();

        // read one line
        QString line = in.readLine();

        // split line
        QStringList cells = line.split(",", QString::SkipEmptyParts);

        // parse
        QDate date      = QDate::fromString(cells.at(0), "MM/dd/yyyy");
        QTime time      = QTime::fromString(cells.at(1), "hh:mm");
        double open     = cells.at(2).toDouble();
        double high     = cells.at(3).toDouble();
        double low      = cells.at(4).toDouble();
        double close    = cells.at(5).toDouble();
        int firstbar    = cells.at(this->indicatorNames.length()+2).toInt();

        //std::cout<<firstbar<<" ";

        // update counter
        cnt++;

        // push date and time
        this->datetime[cnt]= QDateTime(date, time);
        this->date[cnt] = date;
        this->time[cnt] = time;

        // push indicators
        this->open[cnt] = open;
        this->close[cnt] = close;
        this->high[cnt] = high;
        this->low[cnt] = low;
        this->firstbar[cnt]=firstbar;

        for (int i = 0; i < this->indicatorValues.length(); i++)
        {
            this->indicatorValues[i][cnt] = cells.at(2+i).toDouble();
            //std::cout<<this->indicatorValues[i][cnt]<<" ";
        }
        //std::cout<<'\n';
    }

    this->progressBar->setValue(0.8*max);

    // resize time and date vector
    cnt++;
    this->datetime.resize(cnt);
    this->date.resize(cnt);
    this->time.resize(cnt);

    // resize indicator vectors
    this->open.resize(cnt);
    this->close.resize(cnt);
    this->high.resize(cnt);
    this->low.resize(cnt);
    this->firstbar.resize(cnt);

    for (int i = 0; i < this->indicatorValues.length(); i++)
    {
        this->indicatorValues[i].resize(cnt);
    }


    // reverse date and time vectors
    std::reverse(this->datetime.begin(), this->datetime.end());
    std::reverse(this->date.begin(), this->date.end());
    std::reverse(this->time.begin(), this->time.end());

    // reverse indicator vectors
    std::reverse(this->open.begin(), this->open.end());
    std::reverse(this->close.begin(), this->close.end());
    std::reverse(this->high.begin(), this->high.end());
    std::reverse(this->low.begin(), this->low.end());
    std::reverse(this->firstbar.begin(), this->firstbar.end());
    for (int i = 0; i < this->indicatorValues.length(); i++)
    {
        std::reverse(this->indicatorValues[i].begin(), this->indicatorValues[i].end());
    }
    this->progressBar->setValue(max);
}


void BarBuffer::loadIndicatorList(QStringList &items)
{
    for (int i = 0; i < items.length(); i++)
    {

        this->indicatorList->addItem(items.at(i));
        this->indicatorList->item(i)->setFlags(this->indicatorList->item(i)->flags() | Qt::ItemIsUserCheckable);
        this->indicatorList->item(i)->setCheckState(Qt::Checked);

    }
}

*/

void BarBuffer::initialize(int CacheSize)
{
  this->CacheSize=CacheSize;
  this->datetime.resize(CacheSize);
  this->date.resize(CacheSize);
  this->time.resize(CacheSize);
  this->open.resize(CacheSize);
  this->high.resize(CacheSize);
  this->low.resize(CacheSize);
  this->close.resize(CacheSize);
  this->firstbar.resize(CacheSize);
  this->MACD.resize(CacheSize);
  this->MACDAvg.resize(CacheSize);\
  this->MACDDiff.resize(CacheSize);
  this->RSI.resize(CacheSize);
  this->SlowK.resize(CacheSize);
  this->SlowD.resize(CacheSize);
  this->FastAvg.resize(CacheSize);
  this->SlowAvg.resize(CacheSize);
  this->Volume.resize(CacheSize);
  this->DistF.resize(CacheSize);
  this->DistS.resize(CacheSize);
  this->FGS.resize(CacheSize);
  this->FLS.resize(CacheSize);
}

void BarBuffer::clear() {
  datetime.erase(datetime.begin(), datetime.end());
  open.erase(open.begin(), open.end());
  close.erase(close.begin(), close.end());
  high.erase(high.begin(), high.end());
  low.erase(low.begin(), low.end());
  date.erase(date.begin(), date.end());
  time.erase(time.begin(), time.end());

  // erase indicators
  /*
  for (auto it = this->indicatorValues.begin(); it < this->indicatorValues.end(); ++it)
  {
    it->erase(it->begin(), it->end());
  }
  this->indicatorValues.erase(this->indicatorValues.begin(), this->indicatorValues.end());
  this->indicatorNames.erase(this->indicatorNames.begin(), this->indicatorNames.end());
  */

  // clear list widget
  //this->clearIndicatorList();
}

/*void BarBuffer::clearIndicatorList() {
  this->indicatorList->clear();
}*/

/*int BarBuffer::findIndexWeekly(QDate& date) {
//  return this->date.indexOf(date);

  for (int i = 0; i < this->date.length(); i++) {
    if (this->date.at(i) < date)return i-1;
  }
  return -1; // should warn out of bound
}*/

/*int BarBuffer::findIndexWithDateTime(QDate &date, QTime &time) {
  QDateTime dateTime(date, time);
  if(this->date.indexOf(date)==-1)
  {
    return -1;
  }
  for (int i = 0; i < this->datetime.length(); i++)
  {
    if (this->datetime.at(i) <= dateTime) return i;
  }
  return -1; // should warn out of bound
}*/

/*QString BarBuffer::tuple(int i) {
  return this->date.at(i).toString("MM/dd/yyyy")+","+this->time.at(i).toString("hh:mm");
}*/

void BarBuffer::ResistanceChecking(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
  std::vector<int> *Doutput,std::vector<int> *NumberOfHit, std::vector<double> *duration) {
  QVector<double> maoc;

  for (int i = 0; i < open.size(); ++i) {
    maoc.push_back(open[i]>close[i]? open[i] : close[i]);
  }

  for (int i = index; (i <= index+scope && i < this->getSize()); ++i) {
    double H_0 = high[i];
    int hits = 0;
    std::vector<int> hd;
    bool breaked = false;
    for (int j = index; j <= i; ++j)
    {
      if (b+H_0-maoc[j] < 0)
        //if(maoc[j]-b-H_0>0)
      {
        breaked = true;
        break;
      }

      //cout<<H_0-r<<" "<<high[j]<<" "<<H_0-r-high[j]<<'\n';

      //if((H_0-r-high[j])==0) cout<<"x"<<'\n';

      if (high[j] > H_0-r) {
        ++hits;
        hd.push_back(j);
      }
      else if((H_0-r-high[j]) == 0) {
        ++hits;
        hd.push_back(j);
      }
    }

    if (hits>=threshold && (!breaked)) {
      bool duplicate = false;
      for (int m = 0; m < Doutput->size(); ++m) {
        if (this->high[i] == this->high[(*Doutput)[m]]) {
          if (hits >= (*NumberOfHit)[m]) {
            (*Doutput)[m] = i;
            (*NumberOfHit)[m] = hits;
            (*duration)[m] = i-hd[0]+1;
            duplicate = true;
          }
        }
      }
      if (!duplicate) {
        Doutput->push_back(i);
        NumberOfHit->push_back(hits);
        duration->push_back(i-hd[0]+1);
      }
           //for(int k=0; k<hd.size(); k++)HittingDate->push_back(hd[k]);

/*
           if(*(react->end()-1))
           {
               qDebug()<<"y";
           }
           else
           {
               qDebug()<<"n";
           }
*/
       }
    }
}

void BarBuffer::ResistanceChecking_v2(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
  QVector<int> *Doutput,QVector<int> *NumberOfHit, QVector<double> *duration, QVector<QVector<int>> *ReactDate) {
    QVector<double> maoc;

    for (int i = 0; i < open.size(); ++i) {
      maoc.push_back(open[i]>close[i]? open[i] : close[i]);
    }

    for (int i = index; (i <= index+scope && i < this->getSize()); ++i) {
      double H_0 = high[i];
      int hits = 0;
      QVector<int> hd;
      bool breaked = false;
      for (int j = index; j <= i; ++j) {
        if (b + H_0-maoc[j] < 0)
          //if(maoc[j]-b-H_0>0)
        {
          breaked = true;
          break;
        }

        if (high[j] > H_0-r) {
          ++hits;
          hd.push_back(j);
        }
        else if((H_0-r-high[j]) == 0) {
          ++hits;
          hd.push_back(j);
        }
      }

      if (hits >= threshold && (!breaked)) {
        bool duplicate = false;
        for (int m = 0; m < Doutput->size(); ++m) {
          if (this->high[i] == this->high[(*Doutput)[m]]) {
            if (hits >= (*NumberOfHit)[m]) {
              (*Doutput)[m] = i;
              (*NumberOfHit)[m] = hits;
              (*ReactDate)[m]=hd;
              (*duration)[m] = i-hd[0]+1;
              duplicate = true;
            }
          }
        }
        if (!duplicate) {
          Doutput->push_back(i);
          NumberOfHit->push_back(hits);
          ReactDate->push_back(hd);
          duration->push_back(i-hd[0]+1);
        }
      }
    }
}

void BarBuffer::SupportChecking(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
  std::vector<int> *Doutput, std::vector<int> *NumberOfHit, std::vector<double> *duration) {
  QVector<double> mioc;

  for (int i = 0; i < open.size(); ++i) {
    mioc.push_back(open[i]<close[i]? open[i] : close[i]);
  }

  for (int i = index;(i <= index+scope && i < this->getSize()); ++i) {
    double L_0 = low[i];
    int hits = 0;
    std::vector<int> hd;
    bool breaked = false;
    for (int j = index; j <= i; ++j) {
      if (L_0-b-mioc[j] > 0){
        breaked = true;
        break;
      }
      if (low[j] < L_0+r){
        ++hits;
        hd.push_back(j);
      }
      else if (L_0+r-low[j] == 0){
        ++hits;
        hd.push_back(j);
      }
    }

    if (hits >= threshold && (!breaked)){
      bool duplicate = false;
      for (int m = 0; m < Doutput->size(); ++m){
        if (this->low[i] == this->low[(*Doutput)[m]]) {
          if (hits >= (*NumberOfHit)[m]){
            (*Doutput)[m] = i;
            (*NumberOfHit)[m] = hits;
            (*duration)[m] = i-hd[0]+1;
            duplicate = true;
          }
        }
      }
      if (!duplicate) {
        Doutput->push_back(i);
        NumberOfHit->push_back(hits);
        duration->push_back(i-hd[0]+1);
      }
    }
  }
}

void BarBuffer::SupportChecking_v2(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
  QVector<int> *Doutput,QVector<int> *NumberOfHit, QVector<double> *duration, QVector<QVector<int>> *ReactDate) {
    QVector<double> mioc;

    for (int i = 0; i < open.size(); ++i) {
      mioc.push_back(open[i]<close[i]? open[i] : close[i]);
    }

    for (int i = index;(i <= index+scope && i < this->getSize()); ++i) {
      double L_0 = low[i];
      int hits = 0;
      QVector<int> hd;
      bool breaked = false;
      for (int j = index; j <= i; ++j) {
        if (L_0-b-mioc[j] > 0){
          breaked = true;
          break;
        }
        if (low[j] < L_0+r){
          ++hits;
          hd.push_back(j);
        }
        else if (L_0+r-low[j] == 0){
          ++hits;
          hd.push_back(j);
        }
      }

      if (hits >= threshold && (!breaked)){
        bool duplicate = false;
        for (int m = 0; m < Doutput->size(); ++m){
          if (this->low[i] == this->low[(*Doutput)[m]]) {
            if (hits >= (*NumberOfHit)[m]){
              (*Doutput)[m] = i;
              (*NumberOfHit)[m] = hits;
              (*ReactDate)[m]=hd;
              (*duration)[m] = i-hd[0]+1;
              duplicate = true;
            }
          }
        }
        if (!duplicate) {
          Doutput->push_back(i);
          NumberOfHit->push_back(hits);
          ReactDate->push_back(hd);
          duration->push_back(i-hd[0]+1);
        }
      }
    }
}

/*QString BarBuffer::ShowResistance(vector<int> *Doutput, vector<int> *NumberOfHit, vector<int> *HittingDate) {
  QString outputs;
  int count = 0;
  if (rangeOfBars=="Daily" || rangeOfBars=="Weekly") {
    for (int i = 0; i < Doutput->size(); ++i) {
      int d = (*Doutput)[i];

      outputs += "Resistance line at "+showDate(d)+ " with high "
          +QString::number(high[d])+ "\n";
      outputs += QString("The hitting points are \n");
      int n = (*NumberOfHit)[i];
      for (int j = 0; j < n; ++j) {
        int h = (*HittingDate)[count];
        outputs+=showDate(h)+ " with high "+
            QString::number(high[h])+" and max(Close,Open) ";
        if (open[h]>close[h]) outputs+=QString::number(open[h])+"\n";
        else outputs+=QString::number(close[h])+"\n";
        count++;
      }
      outputs+="\n";
    }
  }
  else {
    for (int i = 0; i < Doutput->size(); ++i) {
      int d = (*Doutput)[i];

      outputs+="Resistance line at "+tuple(d)+ " with high "
          +QString::number(high[d])+ "\n";
      outputs+=QString("The hitting points are \n");
      int n = (*NumberOfHit)[i];
      for (int j=0; j < n; ++j) {
        int h = (*HittingDate)[count];
        outputs+=tuple(h)+ " with high "+
            QString::number(high[h])+" and max(Close,Open) ";
        if (open[h] > close[h]) outputs+=QString::number(open[h])+"\n";
        else outputs+=QString::number(close[h])+"\n";
        count++;
      }
      outputs+="\n";
    }
  }

  Doutput->clear();
  NumberOfHit->clear();
  HittingDate->clear();
  return outputs;
}*/

/*QString BarBuffer::ShowSupport(vector<int> *Doutput, vector<int> *NumberOfHit, vector<int> *HittingDate) {
  QString outputs;
  int count = 0;
  if (rangeOfBars=="Daily" || rangeOfBars=="Weekly") {
    for (int i = 0; i < Doutput->size(); ++i) {
      int d = (*Doutput)[i];
      outputs+="Support line at "+showDate(d)+ " with low "
          +QString::number(low[d])+ "\n";
      outputs+=QString("The hitting points are \n");
      int n=(*NumberOfHit)[i];
      for (int j = 0; j < n; ++j) {
        int h = (*HittingDate)[count];
        outputs+=showDate(h)+ " with low "+
            QString::number(low[h])+" and min(Close,Open) ";
        if (open[h]<close[h]) outputs+=QString::number(open[h])+"\n";
        else outputs+=QString::number(close[h])+"\n";
        count++;
      }
      outputs+="\n";
    }
  }
  else {
    for (int i = 0; i < Doutput->size(); ++i) {
      int d = (*Doutput)[i];
      outputs+="Support line at "+tuple(d)+ " with low "
          +QString::number(low[d])+ "\n";
      outputs+=QString("The hitting points are \n");
      int n = (*NumberOfHit)[i];
      for (int j = 0; j < n; ++j)
      {
        int h=(*HittingDate)[count];
        outputs+=tuple(h)+ " with low "+
            QString::number(low[h])+" and min(Close,Open) ";
        if(open[h]<close[h]) outputs+=QString::number(open[h])+"\n";
        else outputs+=QString::number(close[h])+"\n";
        count++;
      }
      outputs+="\n";
    }
  }

  Doutput->clear();
  NumberOfHit->clear();
  HittingDate->clear();
  return outputs;
}*/

/*void BarBuffer::SupportResults(int Index, vector<int> *Doutput,
  vector<int> *NumberOfHit,vector<QString> *FirstReact,
  vector<QString> *SR, vector<QString> *TestDays) {
  if (rangeOfBars=="Daily" || rangeOfBars=="Weekly") {
    for (int i = 0; i < Doutput->size(); ++i) {
      int d = (*Doutput)[i];
      FirstReact->push_back(showDate(d));
      SR->push_back(QString::number(low[d]));
      TestDays->push_back(QString::number(NumberOfHit->at(i)));
    }
  }
  else {
    for (int i = 0; i < Doutput->size(); ++i)
    {
      int d=(*Doutput)[i];
      FirstReact->push_back(tuple(d));
      SR->push_back(QString::number(low[d]));
      TestDays->push_back(QString::number(NumberOfHit->at(i)));
    }
  }
}*/

/*void BarBuffer::ResistanceResults(int Index, vector<int> *Doutput,
  vector<int> *NumberOfHit,vector<QString> *FirstReact,
  vector<QString> *SR, vector<QString> *TestDays) {
  if (rangeOfBars=="Daily" || rangeOfBars=="Weekly")
  {
    for (int i = 0; i < Doutput->size(); ++i)
    {
      int d=(*Doutput)[i];
      FirstReact->push_back(showDate(d));
      SR->push_back(QString::number(high[d]));
      TestDays->push_back(QString::number(NumberOfHit->at(i)));
    }
  } else {
    for (int i = 0; i < Doutput->size(); ++i)
    {
      int d=(*Doutput)[i];
      FirstReact->push_back(tuple(d));
      SR->push_back(QString::number(high[d]));
      TestDays->push_back(QString::number(NumberOfHit->at(i)));
    }
  }
}*/

/*int BarBuffer::FindCloseLastDay(int Index) {
  int N = Index+2000;
  for (int i = Index; i < N; ++i) {
    //std::cout<<this->firstbar[i]<<" ";
    if (this->firstbar[i] == 1)
      return i+1;
  }
  return -1;
}*/

/*QString BarBuffer::ShowMACDAvg(int Index)
{
   QVector<double> *c=&close;
   int MACDLength=9;
   QVector<double> MyMACD;
   int i=0;
   for(; i<MACDLength; i++)
   {
      if((Index+i)>=close.size())break;
      MyMACD.push_back(MACD(c,Index+i));
   }
   QVector<double> *M=&MyMACD;
   double s=2.0/(i+2.0);
   double r=ExpMovingAverage(M,i+1,s,Index);
   return QString::number(r);
}*/

/*void BarBuffer::WriteSR(QString fileName, int SR, double b, double r, int s) {
    int sr=SR;
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream in(&file);
    for(int i=0; i<this->date.size()-251; i++)
    {
        std::vector<int> v_d;
        std::vector<int> * Doutput=&v_d;
        std::vector<int> v_n;
        std::vector<int> * NumberOfHit=&v_n;
        std::vector<int> v_h;
        std::vector<int> * HittingDate=&v_h;
        std::vector<QString> v_f;
        std::vector<QString> * FirstReact= &v_f;
        std::vector<QString> v_sr;
        std::vector<QString> * SR=&v_sr;
        std::vector<QString> v_t;
        std::vector<QString> * TestDays=&v_t;

        if(sr==1)
        {
            ResistanceChecking(i, b, r, s, 2,Doutput,NumberOfHit,HittingDate);
            ResistanceResults(i, Doutput, NumberOfHit ,FirstReact, SR, TestDays);
        }
        else if(sr==2)
        {
            SupportChecking(i, b, r, s, 2,Doutput,NumberOfHit,HittingDate);
            SupportResults(i, Doutput, NumberOfHit ,FirstReact, SR, TestDays);
        }

        if(Doutput->size()!=0)
        {
            QString result;
            if(this->rangeOfBars=="Weekly"||this->rangeOfBars=="Daily")
            {
                result=showDate(i)+" "+QString::number(Doutput->size());
            }
            else
            {
                result=tuple(i)+" "+QString::number(Doutput->size());
            }

            for(int j=0; j<Doutput->size(); j++)
            {
                result+=" "+FirstReact->at(j);
            }
            for(int j=0; j<Doutput->size(); j++)
            {
                result+=" "+SR->at(j);
            }

            in<<result<<"\n";
        }



    }
    file.close(); 
}*/

void BarBuffer::ClearMemory() {
  datetime.erase(datetime.begin(), datetime.end());
  open.erase(open.begin(), open.end());
  close.erase(close.begin(), close.end());
  high.erase(high.begin(), high.end());
  low.erase(low.begin(), low.end());
  date.erase(date.begin(), date.end());
  time.erase(time.begin(), time.end());
  MACD.erase(MACD.begin(), MACD.end());
  MACDAvg.erase(MACDAvg.begin(), MACDAvg.end());
  MACDDiff.erase(MACDDiff.begin(), MACDDiff.end());
  this->FastAvg.clear();
  this->SlowAvg.clear();
  this->SlowD.clear();
  this->SlowK.clear();
  this->RSI.clear();
  this->Volume.clear();
  this->DistF.clear();
  this->DistS.clear();
  this->FGS.clear();
  this->FLS.clear();
  this->firstbar.clear();
}

/*void BarBuffer::LoadFromDB(QDate &d, QTime &t, QString symbol, QString interval, int CacheSize) {
  QSqlQuery *qLoad=handler->SRLoadDB(d,t,symbol,interval);
  ClearMemory();

  int i = 0;
  QVector<QVariant> LoadBuffer;
  QSqlRecord r;
  QDate date;
  QTime time;

  while (i < CacheSize && qLoad->next())
  {
    r = qLoad->record();
    LoadBuffer.clear();
    for (int j = 0; j < r.count(); ++j) {
      LoadBuffer.push_back(r.value(j));
    }

    date = LoadBuffer[0].toDate();
    this->date.push_back(date);

    //qDebug()<<i;
    //qDebug()<<d.toString("yy-MM-dd");

    time = LoadBuffer[1].toTime();
    this->time.push_back(time);
    //qDebug()<<t.toString("hh:mm");

    this->datetime.push_back(QDateTime(date, time));
    this->open.push_back(LoadBuffer.at(2).toDouble());
    this->high.push_back(LoadBuffer.at(3).toDouble());
    this->low.push_back(LoadBuffer.at(4).toDouble());
    this->close.push_back(LoadBuffer.at(5).toDouble());
    this->Volume.push_back(LoadBuffer.at(6).toDouble());
    this->MACD.push_back(LoadBuffer.at(7).toDouble());
    this->MACDAvg.push_back(LoadBuffer.at(8).toDouble());
    this->MACDDiff.push_back(LoadBuffer.at(9).toDouble());
    this->RSI.push_back(LoadBuffer.at(10).toDouble());
    this->SlowK.push_back(LoadBuffer.at(11).toDouble());
    this->SlowD.push_back(LoadBuffer.at(12).toDouble());
    this->FastAvg.push_back(LoadBuffer.at(13).toDouble());
    this->SlowAvg.push_back(LoadBuffer.at(14).toDouble());
    this->DistF.push_back(LoadBuffer.at(15).toDouble());
    this->DistS.push_back(LoadBuffer.at(16).toDouble());
    this->FGS.push_back(LoadBuffer.at(17).toDouble());
    this->FLS.push_back(LoadBuffer.at(18).toDouble());
    this->firstbar.push_back(LoadBuffer.at(19).toDouble());
    ++i;
  }
}*/

/*void BarBuffer::LoadFromDB(QSqlQuery* qLoad, int CacheSize) {
  ClearMemory();

  int i = 0;
  QVector<QVariant> *LoadBuffer = new QVector<QVariant>();
  //qDebug()<<"x";
  while (i < CacheSize && qLoad->next()) {
    QSqlRecord r = qLoad->record();
    LoadBuffer->clear();
    for (int j = 0; j < r.count(); ++j) {
      LoadBuffer->push_back(r.value(j));
    }

    QDate d = (*LoadBuffer)[0].toDate();
    this->date.push_back(d);

    //qDebug()<<i;
    //qDebug()<<d.toString("yy-MM-dd");

    QTime t = (*LoadBuffer)[1].toTime();
    this->time.push_back(t);
    //qDebug()<<t.toString("hh:mm");

    this->datetime.push_back(QDateTime(d, t));
    this->open.push_back(LoadBuffer->at(2).toDouble());
    this->high.push_back(LoadBuffer->at(3).toDouble());
    this->low.push_back(LoadBuffer->at(4).toDouble());
    this->close.push_back(LoadBuffer->at(5).toDouble());
    this->Volume.push_back(LoadBuffer->at(6).toDouble());
    this->MACD.push_back(LoadBuffer->at(7).toDouble());
    this->MACDAvg.push_back(LoadBuffer->at(8).toDouble());
    this->MACDDiff.push_back(LoadBuffer->at(9).toDouble());
    this->RSI.push_back(LoadBuffer->at(10).toDouble());
    this->SlowK.push_back(LoadBuffer->at(11).toDouble());
    this->SlowD.push_back(LoadBuffer->at(12).toDouble());
    this->FastAvg.push_back(LoadBuffer->at(13).toDouble());
    this->SlowAvg.push_back(LoadBuffer->at(14).toDouble());
    this->DistF.push_back(LoadBuffer->at(15).toDouble());
    this->DistS.push_back(LoadBuffer->at(16).toDouble());
    this->FGS.push_back(LoadBuffer->at(17).toDouble());
    this->FLS.push_back(LoadBuffer->at(18).toDouble());
    this->firstbar.push_back(LoadBuffer->at(19).toDouble());
    i++;
  }
  delete LoadBuffer;
}*/

inline int BarBuffer::getSize() {
  return date.size();
}

/*int BarBuffer::findIndexDaily(const QDate &SDate) {
  return this->date.indexOf(SDate);
}*/

int BarBuffer::maxReactResistance(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    QVariantList *v_value, QVariantList *v_date, QVariantList *v_time, QVariantList *v_rcount,
    QVariantList *v_duration, QVector<QVariantList>* ReactList, QVariantList* LastDuration) {
  QVector<int> v_d;
  QVector<int> * Doutput = &v_d;
  QVector<int> v_n;
  QVector<int> * NumberOfHit = &v_n;
  QVector<double> v_r;
  QVector<double> * duration = &v_r;
  QVector<QVector<int>> v_rd;
  QVector<QVector<int>> * React_Date = &v_rd;

  ResistanceChecking_v2(index, b, r, scope, threshold, Doutput, NumberOfHit, duration, React_Date);
  int maxReact = 0;
  int doutput_size = Doutput->size();
  int doutput_idx;
  bool insert_flag = (v_value != NULL && v_date != NULL && v_time != NULL);

  for (int i = 0; i < doutput_size; ++i) {
    doutput_idx = (*Doutput)[i];
    if (insert_flag) {
      v_value->append(high[doutput_idx]);
      v_date->append(date[doutput_idx].toString("yyyy-MM-dd"));
      v_time->append(time[doutput_idx].toString("hh:mm"));
      v_rcount->append((*NumberOfHit)[i]);
      v_duration->append((*duration)[i]);

      QVariantList rd;
      for (int j = 0; j < React_Date->at(i).size(); ++j) {
        rd.append(date[(*React_Date)[i][j]]);
      }
      ReactList->push_back(rd);
      LastDuration->append((*React_Date)[i].front()-index);
    }

    if ((*NumberOfHit)[i] > maxReact) {
      maxReact = (*NumberOfHit)[i];
    }
  }
  return maxReact;
}

int BarBuffer::maxReactSupport(const int &index, const double &b, const double &r, const int &scope, const int &threshold,
    QVariantList *v_value, QVariantList *v_date, QVariantList *v_time, QVariantList *v_rcount,
    QVariantList *v_duration, QVector<QVariantList>* ReactList, QVariantList* LastDuration){
  QVector<int> v_d;
  QVector<int> * Doutput = &v_d;
  QVector<int> v_n;
  QVector<int> * NumberOfHit = &v_n;
  QVector<double> v_r;
  QVector<double> * duration = &v_r;
  QVector<QVector<int>> v_rd;
  QVector<QVector<int>> * React_Date = &v_rd;

  SupportChecking_v2(index, b, r, scope, threshold, Doutput, NumberOfHit, duration, React_Date);

  int maxReact = 0;
  int doutput_size = Doutput->size();
  int doutput_idx;
  bool insert_flag = (v_value != NULL && v_date != NULL && v_time != NULL);

  for (int i = 0; i < doutput_size; ++i) {
    doutput_idx = (*Doutput)[i];
    if (insert_flag) {
      v_value->append(low[doutput_idx]);
      v_date->append(date[doutput_idx].toString("yyyy-MM-dd"));
      v_time->append(time[doutput_idx].toString("hh:mm"));
      v_rcount->append((*NumberOfHit)[i]);
      v_duration->append((*duration)[i]);

      QVariantList rd;
      for (int j = 0; j < React_Date->at(i).size(); ++j) {
        rd.append(date[(*React_Date)[i][j]]);
      }
      ReactList->push_back(rd);
      LastDuration->append((*React_Date)[i].front()-index);
    }

    if ((*NumberOfHit)[i] > maxReact) {
      maxReact = (*NumberOfHit)[i];
    }
  }
  return maxReact;
}

void BarBuffer::DBResistanceSupportGen(const QSqlDatabase &database, const double &b, const double &r,
    const int &threshold, const int &start_rowid_from, const int &id_threshold) {
  ClearMemory();

  QString LastRecord = SQLiteHandler::SQL_LAST_FROM_ROWID(start_rowid_from);
  QSqlQuery query(database);
  QSqlQuery qInsert(database);
  QSqlQuery qLast(database);
  qLast.setForwardOnly(true);
  qLast.exec("PRAGMA synchronous = OFF;");
  qLast.exec(LastRecord);
  query.setForwardOnly(true);

  QDate d;
  QTime t;
  int i = 0;
  while (i < 250 && qLast.next()) {
    d = qLast.value(0).toDate();
    t = qLast.value(1).toTime();
    date.push_back(d);
    time.push_back(t);
    datetime.push_back(QDateTime(d, t));
    open.push_back(qLast.value(2).toDouble());
    high.push_back(qLast.value(3).toDouble());
    low.push_back(qLast.value(4).toDouble());
    close.push_back(qLast.value(5).toDouble());
    ++i;
  }

  // table bardata
//  QVariantList date_;
//  QVariantList time_;
//  QVariantList rline;
//  QVariantList sline;
  // table resistance/support
  QVariantList res_value;
  QVariantList res_date;
  QVariantList res_time;
  QVariantList res_rcount;
  QVariantList res_duration;
  QVector<QVariantList> res_reactList;
  QVariantList res_LastDuration;
  QVariantList sup_value;
  QVariantList sup_date;
  QVariantList sup_time;
  QVariantList sup_rcount;
  QVariantList sup_duration;
  QVector<QVariantList> sup_reactList;
  QVariantList sup_LastDuration;
//  int column_count_resistance = 0;
//  int column_count_support = 0;
  int maxReact_support;
  int maxReact_resistance;
  long last_rowid;

  qInsert.exec("PRAGMA journal_mode = OFF;");
  qInsert.exec("PRAGMA synchronous = OFF;");
  qInsert.exec("BEGIN TRANSACTION;");

  while (this->getSize() > 0) {
    maxReact_resistance = maxReactResistance(0, b, r, this->getSize(), threshold, &res_value, &res_date, &res_time,
                                             &res_rcount, &res_duration, &res_reactList, &res_LastDuration);
    maxReact_support = maxReactSupport(0, b, r, this->getSize(), threshold, &sup_value, &sup_date, &sup_time,
                                       &sup_rcount, &sup_duration, &sup_reactList, &sup_LastDuration);

//    rline.append(maxReact_resistance);
//    sline.append(maxReact_support);
//    date_.append(date[0].toString("yyyy-MM-dd"));
//    time_.append(time[0].toString("hh:mm"));

    // Resistance
    insert_resistance_detail(&qInsert, date[0], time[0], res_date, res_time, res_value, res_rcount, res_duration, res_LastDuration, id_threshold);

    query.exec("select max(rowid) from " + SQLiteHandler::TABLE_NAME_RESISTANCE);
    last_rowid = query.next()? query.value(0).toInt() : 0;

    insert_resistance_react_date_detail(&qInsert, last_rowid, res_date, res_reactList);

    // Support
    insert_support_detail(&qInsert, date[0], time[0], sup_date, sup_time, sup_value, sup_rcount, sup_duration, sup_LastDuration, id_threshold);

    query.exec("select max(rowid) from " + SQLiteHandler::TABLE_NAME_SUPPORT);
    last_rowid = query.next()? query.value(0).toInt() : 0;

    insert_support_react_date_detail(&qInsert, last_rowid, sup_date, sup_reactList);

    // denormalized version
//      sort_descending_list(&res_value);
//      sort_descending_list(&sup_value);

//      // column count exclude index_, date_, time_, column_count
//      column_count_resistance = database.record(SQLiteHandler::TABLE_NAME_RESISTANCE_TAB).count()-4;
//      column_count_support = database.record(SQLiteHandler::TABLE_NAME_SUPPORT_TAB).count()-4;

//      sqlite_insert_resistance_wtf(&qInsert, column_count_resistance, date[0], time[0], res_value, threshold_index);
//      sqlite_insert_support_wtf(&qInsert, column_count_support, date[0], time[0], sup_value, threshold_index);

    // release resource
    res_value.clear();
    res_date.clear();
    res_time.clear();
    res_rcount.clear();
    res_duration.clear();
    res_LastDuration.clear();
    res_reactList.clear();
    sup_value.clear();
    sup_date.clear();
    sup_time.clear();
    sup_rcount.clear();
    sup_duration.clear();
    sup_LastDuration.clear();
    sup_reactList.clear();

//    if (date_.size() >= 100000) {
//      sqlite_update_bardata_rline_sline(&qInsert, date_, time_, rline, sline, id_threshold);
//      rline.clear();
//      sline.clear();
//      date_.clear();
//      time_.clear();
//    }

    //pop the current bar and load one new bar into memory
    date.pop_front();
    time.pop_front();
    datetime.pop_front();
    open.pop_front();
    high.pop_front();
    low.pop_front();
    close.pop_front();

    if (qLast.next()) {
      d = qLast.value(0).toDate();
      t = qLast.value(1).toTime();
      date.push_back(d);
      time.push_back(t);
      datetime.push_back(QDateTime(d, t));
      open.push_back(qLast.value(2).toDouble());
      high.push_back(qLast.value(3).toDouble());
      low.push_back(qLast.value(4).toDouble());
      close.push_back(qLast.value(5).toDouble());
    }
  }

//  if (date_.size() > 0) {
//    sqlite_update_bardata_rline_sline(&qInsert, date_, time_, rline, sline, id_threshold);
//    rline.clear();
//    sline.clear();
//    date_.clear();
//    time_.clear();
//  }

  qInsert.exec("COMMIT");
}

void BarBuffer::DBResistanceSupportGen_Recalculate(const QSqlDatabase &database,
    const double &b, const double &r, const int &threshold, const int &id_threshold) {
  QString table_resistance = SQLiteHandler::TABLE_NAME_RESISTANCE;
  QString table_support = SQLiteHandler::TABLE_NAME_SUPPORT;
//  QString column_resistance = "";
//  QString column_support = "";

  QSqlQuery qLast(database);
  qLast.setForwardOnly(true);

  // remove old threshold
  qLast.exec("PRAGMA journal_mode = OFF;");
  qLast.exec("PRAGMA synchronous = OFF;");
  qLast.exec("PRAGMA cache_size = 50000;");
  qLast.exec("BEGIN TRANSACTION;");
  qLast.exec("delete from " + table_resistance + " where id_threshold=" + QString::number(id_threshold));
  qLast.exec("delete from " + table_support + " where id_threshold=" + QString::number(id_threshold));
  qLast.exec("COMMIT;");

  // if breakpoint is zero, then quit (for delete particular threshold value)
  if (b <= 0) return;

  DBResistanceSupportGen(database, b, r, threshold, 0, id_threshold);
}

void BarBuffer::insert_resistance_detail(QSqlQuery *query,
    const QDate &date_, const QTime &time_, const QVariantList &rdate, const QVariantList &rtime,
    const QVariantList &v_value, const QVariantList &v_rcount, const QVariantList &v_duration,
    const QVariantList &v_last_duration, const int &id_threshold) {
  QString s_date = date_.toString("yyyy-MM-dd");
  QString s_time = time_.toString("hh:mm");
  QVariantList v_date;
  QVariantList v_time;
  QVariantList v_index;
  const int N = rdate.size();

  for (int i = 0; i < N; ++i) {
    v_index.append(id_threshold);
    v_date.append(s_date);
    v_time.append(s_time);
  }

  query->prepare(SQLiteHandler::SQL_INSERT_RESISTANCE_V1);
  query->addBindValue(v_date);
  query->addBindValue(v_time);
  query->addBindValue(rdate);
  query->addBindValue(rtime);
  query->addBindValue(v_rcount);
  query->addBindValue(v_value);
  query->addBindValue(v_duration);
  query->addBindValue(v_last_duration);
  query->addBindValue(v_index); // id_threshold
  query->execBatch();
  if (query->lastError().isValid()) qDebug() << query->lastError();
}

void BarBuffer::insert_support_detail(QSqlQuery *query,
    const QDate &date_, const QTime &time_, const QVariantList &rdate, const QVariantList &rtime,
    const QVariantList &v_value, const QVariantList &v_rcount, const QVariantList &v_duration,
    const QVariantList &v_last_duration, const int &id_threshold) {
  QString s_date = date_.toString("yyyy-MM-dd");
  QString s_time = time_.toString("hh:mm");
  QVariantList v_date;
  QVariantList v_time;
  QVariantList v_index;
  const int N = rdate.size();

  for (int i = 0; i < N; ++i) {
    v_index.append(id_threshold);
    v_date.append(s_date);
    v_time.append(s_time);
  }

  query->prepare(SQLiteHandler::SQL_INSERT_SUPPORT_V1);
  query->addBindValue(v_date);
  query->addBindValue(v_time);
  query->addBindValue(rdate);
  query->addBindValue(rtime);
  query->addBindValue(v_rcount);
  query->addBindValue(v_value);
  query->addBindValue(v_duration);
  query->addBindValue(v_last_duration);
  query->addBindValue(v_index); // id_threshold
  query->execBatch();
  if (query->lastError().isValid()) qDebug() << query->lastError();
}

// denormalize version
/*void BarBuffer::sqlite_insert_resistance_wtf(QSqlQuery *query, const int &column_count,
    const QDate &date_, const QTime &time_, const QVariantList &v_value, const int &threshold_index) {

  // if values is empty then exit
  if (v_value.length() == 0 || query == NULL) {
    return;
  }

  const QString s_index = QString::number(threshold_index);
  const QString s_date = date_.toString("yyyy-MM-dd");
  const QString s_time = time_.toString("hh:mm");
  const int N = v_value.length();

  // alter column
  if (column_count < N) {
    int last_id = column_count-1; // to zero-based index
    int new_column_number = N - column_count;
    for (int i = 1; i <= new_column_number; ++i) {
      query->exec("alter table " + SQLiteHandler::TABLE_NAME_RESISTANCE_TAB +
                 " add column v" + QString::number(last_id+i) + " REAL;");
    }
  }

  // build column projection
  QString join_column = "";
  QString join_value = "";
  for (int i = 0; i < N; ++i) {
    if (i > 0) {
      join_column += ",";
      join_value += ",";
    }
    join_column += "v" + QString::number(i);
    join_value += v_value[i].toString();
  }

  QString sql_insert =
    "insert into " + SQLiteHandler::TABLE_NAME_RESISTANCE_TAB + "(index_,date_,time_,column_count," + join_column + ")" +
    " values(" + s_index + ",'" + s_date + "','" + s_time + "'," + QString::number(N) + "," + join_value + ")";

  // execute insert, commit later
  query->exec(sql_insert);

  if (query->lastError().isValid()) {
    qDebug() << query->lastError();
    qDebug() << sql_insert;
  }
}*/

// denormalize version
/*void BarBuffer::sqlite_insert_support_wtf(QSqlQuery *query, const int &column_count,
    const QDate &date_, const QTime &time_, const QVariantList &v_value, const int &threshold_index) {

  // if values is empty then exit
  if (v_value.length() == 0 || query == NULL) {
    return;
  }

  const QString s_index = QString::number(threshold_index);
  const QString s_date = date_.toString("yyyy-MM-dd");
  const QString s_time = time_.toString("hh:mm");
  const int N = v_value.length();

  // alter column
  if (column_count < N) {
    int last_id = column_count-1; // to zero-based index
    int new_column_number = N - column_count;
    for (int i = 1; i <= new_column_number; ++i) {
      query->exec("alter table " + SQLiteHandler::TABLE_NAME_SUPPORT_TAB +
                 " add column v" + QString::number(last_id+i) + " REAL;");
    }
  }

  // build column projection
  QString join_column = "";
  QString join_value = "";
  for (int i = 0; i < N; ++i) {
    if (i > 0) {
      join_column += ",";
      join_value += ",";
    }
    join_column += "v" + QString::number(i);
    join_value += v_value[i].toString();
  }

  QString sql_insert =
    "insert into " + SQLiteHandler::TABLE_NAME_SUPPORT_TAB + "(index_,date_,time_,column_count," + join_column + ")" +
    " values(" + s_index + ",'" + s_date + "','" + s_time + "'," + QString::number(N) + "," + join_value + ")";

  // execute insert, commit later
  query->exec(sql_insert);

  if (query->lastError().isValid()) {
    qDebug() << query->lastError();
    qDebug() << sql_insert;
  }
}*/
