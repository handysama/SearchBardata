#include "XmlConfigHandler.h"

#define MAX_SUPPORT_RESISTANCE_THRESHOLD  5
#define MAX_CLOSE_THRESHOLD               5
#define DEFAULT_DATABASE_DIR  ""
//#define DEFAULT_DATABASE_DIR  "C:/Program Files (x86)/TS Finance/Programs"
#define DEFAULT_INPUT_DIR     "C:/Program Files (x86)/TS Finance/Download/Data"
#define DEFAULT_PROCESS_DIR   "C:/Program Files (x86)/TS Finance/Download/Processed"
#define DEFAULT_RESULT_DIR    "C:/Program Files (x86)/TS Finance/Programs/Program Results"
#define TOKEN_UPDATE_PARENT_INDEX         "UpdateParentIndex"
#define TOKEN_UPDATE_SUPPORT_RESISTANCE   "UpdateSupportResistance"
#define TOKEN_UPDATE_HISTOGRAM            "UpdateHistogram"
#define TOKEN_DEVELOPER_MODE              "DeveloperMode"
#define TOKEN_DATABASE_DIR                "DatabaseDir"
#define TOKEN_INPUT_DIR                   "InputDir"
#define TOKEN_RESULT_DIR                  "ResultDir"
#define TOKEN_PROCESS_DIR                 "ProcessDir"
#define TOKEN_LAST_UPDATE                 "LastUpdate"
#define TOKEN_SYMBOL                      "Symbol"
#define TOKEN_INTERVAL                    "Interval"
#define TOKEN_MACD                        "MACD"
#define TOKEN_RSI                         "RSI"
#define TOKEN_SLOWK                       "SlowK"
#define TOKEN_SLOWD                       "SlowD"
#define TOKEN_CGF                         "CGF"
#define TOKEN_CLF                         "CLF"
#define TOKEN_CGS                         "CGS"
#define TOKEN_CLS                         "CLS"

XmlConfigHandler* XmlConfigHandler::_instance = NULL;

void XmlConfigHandler::initialize_variable() {
  enable_developer_mode = false;
  enable_parent_indexing = true;
  enable_update_support_resistance = true;
  enable_update_histogram = true;
  database_dir = DEFAULT_DATABASE_DIR;
  input_dir = DEFAULT_INPUT_DIR;
  process_dir = DEFAULT_PROCESS_DIR;
  result_dir = DEFAULT_RESULT_DIR;
  list_symbol.clear();
  list_database.clear();
  list_sr_threshold.clear();
  interval_weight.clear();
  interval_threshold.clear();
  t_macd.clear();
  t_rsi.clear();
  t_slowk.clear();
  t_slowd.clear();
  t_cgf.clear();
  t_clf.clear();
  t_cgs.clear();
  t_cls.clear();
}

void appendXmlElement(QXmlStreamWriter *stream, const QString &element, const QString &value) {
  stream->writeStartElement(element);
  stream->writeAttribute("value", value);
  stream->writeEndElement();
}

void appendXmlElement_Interval(QXmlStreamWriter *stream, const QString &interval, const QString &weight, const QString &threshold) {
  stream->writeStartElement("item");
  stream->writeAttribute("name", interval);
  stream->writeAttribute("weight", weight);
  stream->writeAttribute("threshold", threshold);
  stream->writeEndElement();
}

void appendXmlElement_Threshold_1(QXmlStreamWriter *stream, const QString &id, const QString &_operator1, const QString &value1, const QString &_operator2, const QString &value2) {
  stream->writeStartElement("item");
  stream->writeAttribute("id", id);
  stream->writeAttribute("operator1", _operator1);
  stream->writeAttribute("value1", value1);
  stream->writeAttribute("operator2", _operator2);
  stream->writeAttribute("value2", value2);
  stream->writeEndElement();
}

void appendXmlElement_Threshold_2(QXmlStreamWriter *stream, const QString &id, const QString &value) {
  stream->writeStartElement("item");
  stream->writeAttribute("id", id);
  stream->writeAttribute("value", value);
  stream->writeEndElement();
}

void appendXmlElement_SupportResistance(QXmlStreamWriter *stream, const QString &symbol, const QString &index,
                                        const QString &threshold, const QString &b, const QString &recalculate) {
  stream->writeStartElement("market");
  stream->writeAttribute("symbol", symbol);

  stream->writeAttribute("index", index);
  stream->writeAttribute("threshold", threshold);
  stream->writeAttribute("b", b);
  stream->writeAttribute("recalculate", recalculate);

  stream->writeEndElement();
}

void appendXmlElement_SupportResistance_V2(QXmlStreamWriter *stream, const QString &index, const QString &test_point,
                                           const QString &break_threshold, const QString &recalculate) {
  stream->writeStartElement("param");
  stream->writeAttribute("index", index);
  stream->writeAttribute("test_point", test_point);
  stream->writeAttribute("break_threshold", break_threshold);
  stream->writeAttribute("recalculate", recalculate);
  stream->writeEndElement();
}

QVector<double> lookup_close_threshold(const QString &symbol) {
  QMap< QString , QVector<double> > my_threshold;
  QVector<double> data;

  data.clear();
  data.push_back(0);
  data.push_back(0.001);
  data.push_back(0.002);
  my_threshold.insert( "@AD" , data );

  data.clear();
  data.push_back(0);
  data.push_back(0.1);
  data.push_back(0.2);
  data.push_back(0.4);
  my_threshold.insert( "@CL" , data );

  data.clear();
  data.push_back(0);
  data.push_back(0.001);
  data.push_back(0.002);
  my_threshold.insert( "@EC" , data );

  data.clear();
  data.push_back(0);
  data.push_back(1);
  data.push_back(2);
  data.push_back(4);
  my_threshold.insert( "@ES" , data );

  data.clear();
  data.push_back(0);
  data.push_back(1);
  data.push_back(2);
  my_threshold.insert( "@GC" , data );

  data.clear();
  data.push_back(0);
  data.push_back(0.001);
  data.push_back(0.002);
  my_threshold.insert( "@JY" , data );

  data.clear();
  data.push_back(0);
  data.push_back(10);
  data.push_back(20);
  data.push_back(40);
  my_threshold.insert( "@NIY" , data );

  data.clear();
  data.push_back(0);
  data.push_back(2);
  data.push_back(4);
  data.push_back(8);
  my_threshold.insert( "@NQ" , data );

  return my_threshold[symbol];
}

void XmlConfigHandler::write_config_default() {
  QFile file(config_path);

  if (file.open(QFile::WriteOnly)) {
    QVector<std::pair<QString,QString>> sr_threshold;

    // @AD
    sr_threshold.append( std::make_pair("@AD", "0.001") );
    sr_threshold.append( std::make_pair("@AD", "0") );
    sr_threshold.append( std::make_pair("@AD", "0") );
    sr_threshold.append( std::make_pair("@AD", "0") );
    sr_threshold.append( std::make_pair("@AD", "0") );
    // @CL
    sr_threshold.append( std::make_pair("@CL", "0.2") );
    sr_threshold.append( std::make_pair("@CL", "0.1") );
    sr_threshold.append( std::make_pair("@CL", "0") );
    sr_threshold.append( std::make_pair("@CL", "0") );
    sr_threshold.append( std::make_pair("@CL", "0") );
    // @EC
    sr_threshold.append( std::make_pair("@EC", "0.001") );
    sr_threshold.append( std::make_pair("@EC", "0") );
    sr_threshold.append( std::make_pair("@EC", "0") );
    sr_threshold.append( std::make_pair("@EC", "0") );
    sr_threshold.append( std::make_pair("@EC", "0") );
    // @ES
    sr_threshold.append( std::make_pair("@ES", "1") );
    sr_threshold.append( std::make_pair("@ES", "2") );
    sr_threshold.append( std::make_pair("@ES", "0") );
    sr_threshold.append( std::make_pair("@ES", "0") );
    sr_threshold.append( std::make_pair("@ES", "0") );
    // @GC
    sr_threshold.append( std::make_pair("@GC", "1") );
    sr_threshold.append( std::make_pair("@GC", "0") );
    sr_threshold.append( std::make_pair("@GC", "0") );
    sr_threshold.append( std::make_pair("@GC", "0") );
    sr_threshold.append( std::make_pair("@GC", "0") );
    // @JY
    sr_threshold.append( std::make_pair("@JY", "0.001") );
    sr_threshold.append( std::make_pair("@JY", "0") );
    sr_threshold.append( std::make_pair("@JY", "0") );
    sr_threshold.append( std::make_pair("@JY", "0") );
    sr_threshold.append( std::make_pair("@JY", "0") );
    // @NIY
    sr_threshold.append( std::make_pair("@NIY", "10") );
    sr_threshold.append( std::make_pair("@NIY", "20") );
    sr_threshold.append( std::make_pair("@NIY", "0") );
    sr_threshold.append( std::make_pair("@NIY", "0") );
    sr_threshold.append( std::make_pair("@NIY", "0") );
    //@NQ
    sr_threshold.append( std::make_pair("@NQ", "2") );
    sr_threshold.append( std::make_pair("@NQ", "4") );
    sr_threshold.append( std::make_pair("@NQ", "0") );
    sr_threshold.append( std::make_pair("@NQ", "0") );
    sr_threshold.append( std::make_pair("@NQ", "0") );

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();
    stream.writeStartElement("Application");
    stream.writeAttribute("name", "TS Finance");
    stream.writeAttribute("version", QString(VERSION));

    appendXmlElement(&stream, TOKEN_LAST_UPDATE, "");
    appendXmlElement(&stream, TOKEN_DATABASE_DIR, DEFAULT_DATABASE_DIR);
    appendXmlElement(&stream, TOKEN_RESULT_DIR, DEFAULT_RESULT_DIR);
    appendXmlElement(&stream, TOKEN_INPUT_DIR, DEFAULT_INPUT_DIR);
    appendXmlElement(&stream, TOKEN_PROCESS_DIR, DEFAULT_PROCESS_DIR);
    appendXmlElement(&stream, TOKEN_UPDATE_PARENT_INDEX , "1");
    appendXmlElement(&stream, TOKEN_UPDATE_SUPPORT_RESISTANCE, "1");
    appendXmlElement(&stream, TOKEN_UPDATE_HISTOGRAM, "1");
    appendXmlElement(&stream, TOKEN_DEVELOPER_MODE, "0");

    stream.writeStartElement(TOKEN_SYMBOL);
    stream.writeTextElement("item","@AD");
    stream.writeTextElement("item","@CL");
    stream.writeTextElement("item","@EC");
    stream.writeTextElement("item","@ES");
    stream.writeTextElement("item","@GC");
    stream.writeTextElement("item","@JY");
    stream.writeTextElement("item","@NIY");
    stream.writeTextElement("item","@NQ");
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_MACD);
    appendXmlElement_Threshold_1(&stream, "1", "<","0", ">", "0");
    appendXmlElement_Threshold_1(&stream, "2", "","", "", "");
    appendXmlElement_Threshold_1(&stream, "3", "","", "", "");
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_RSI);
    appendXmlElement_Threshold_1(&stream, "1", "<", "30", ">", "70");
    appendXmlElement_Threshold_1(&stream, "2", "<", "35", ">", "65");
    appendXmlElement_Threshold_1(&stream, "3", "<", "40", ">", "60");
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_SLOWK);
    appendXmlElement_Threshold_1(&stream, "1", "<", "20", ">", "80");
    appendXmlElement_Threshold_1(&stream, "2", "<", "25", ">", "75");
    appendXmlElement_Threshold_1(&stream, "3", "<", "30", ">", "70");
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_SLOWD);
    appendXmlElement_Threshold_1(&stream, "1", "<", "20", ">", "80");
    appendXmlElement_Threshold_1(&stream, "2", "<", "25", ">", "75");
    appendXmlElement_Threshold_1(&stream, "3", "<", "30", ">", "70");
    stream.writeEndElement();

    /** Intervals (use minute as weight measurement unit) */
    stream.writeStartElement(TOKEN_INTERVAL);
    appendXmlElement_Interval(&stream, "1min", "1", "0");
    appendXmlElement_Interval(&stream, "5min", "5", "0");
    appendXmlElement_Interval(&stream, "60min", "60", "0");
    appendXmlElement_Interval(&stream, "Daily", "1440", "0");
    appendXmlElement_Interval(&stream, "Weekly", "10080", "0");
    appendXmlElement_Interval(&stream, "Monthly", "43200", "0");
    stream.writeEndElement();

    /** Threshold parameters */
    QString prev_symbol = "";

    stream.writeStartElement("thresholds");

    for (int i = 0; i < sr_threshold.size(); ++i) {

      if ( !prev_symbol.isEmpty() && prev_symbol != sr_threshold[i].first ) {
        stream.writeEndElement(); // end market
      }

      prev_symbol = sr_threshold[i].first;
      stream.writeComment( sr_threshold[i].first );
      stream.writeStartElement("market");
      stream.writeAttribute("symbol", sr_threshold[i].first);
      stream.writeStartElement("sr_threshold");

      for (int j = 0; i < sr_threshold.size() &&
                      j < MAX_SUPPORT_RESISTANCE_THRESHOLD; ++j) {
        appendXmlElement_SupportResistance_V2(&stream, QString::number(j), "2", sr_threshold[i].second, "0");
        ++i;
      }

      --i;
      stream.writeEndElement(); // end sr_threshold

      QVector<double> d = lookup_close_threshold( prev_symbol );

      stream.writeStartElement(TOKEN_CGF);
      for (int i = 0 ; i < MAX_CLOSE_THRESHOLD; ++i) {
        if (i < d.size()) {
          appendXmlElement_Threshold_2(&stream, QString::number(i), QString::number(d[i],'f',4));
        } else {
          appendXmlElement_Threshold_2(&stream, QString::number(i), "");
        }
      }
      stream.writeEndElement();

      stream.writeStartElement(TOKEN_CLF);
      for (int i = 0 ; i < MAX_CLOSE_THRESHOLD; ++i) {
        if (i < d.size()) {
          appendXmlElement_Threshold_2(&stream, QString::number(i), QString::number(d[i],'f',4));
        } else {
          appendXmlElement_Threshold_2(&stream, QString::number(i), "");
        }
      }
      stream.writeEndElement();

      stream.writeStartElement(TOKEN_CGS);
      for (int i = 0 ; i < MAX_CLOSE_THRESHOLD; ++i) {
        if (i < d.size()) {
          appendXmlElement_Threshold_2(&stream, QString::number(i), QString::number(d[i],'f',4));
        } else {
          appendXmlElement_Threshold_2(&stream, QString::number(i), "");
        }
      }
      stream.writeEndElement();

      stream.writeStartElement(TOKEN_CLS);
      for (int i = 0 ; i < MAX_CLOSE_THRESHOLD; ++i) {
        if (i < d.size()) {
          appendXmlElement_Threshold_2(&stream, QString::number(i), QString::number(d[i],'f',4));
        } else {
          appendXmlElement_Threshold_2(&stream, QString::number(i), "");
        }
      }
      stream.writeEndElement();
    }

    stream.writeEndElement(); // end market (last)
    stream.writeEndElement(); // end threshold
    stream.writeEndElement(); // end application
    stream.writeEndDocument();
  }

  file.close();
}

void XmlConfigHandler::write_config() {
  QFile file(config_path);
  QFile::remove(config_path);

  if (file.open(QFile::ReadWrite)) {
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("Application");
    stream.writeAttribute("name", "TS Finance");
    stream.writeAttribute("version", QString(VERSION));

    appendXmlElement(&stream, TOKEN_LAST_UPDATE, last_update.toString("yyyy/MM/dd hh:mm:ss"));
    appendXmlElement(&stream, TOKEN_DATABASE_DIR, database_dir);
    appendXmlElement(&stream, TOKEN_RESULT_DIR, result_dir);
    appendXmlElement(&stream, TOKEN_INPUT_DIR, input_dir);
    appendXmlElement(&stream, TOKEN_PROCESS_DIR, process_dir);
    appendXmlElement(&stream, TOKEN_UPDATE_PARENT_INDEX, enable_parent_indexing? "1":"0");
    appendXmlElement(&stream, TOKEN_UPDATE_SUPPORT_RESISTANCE, enable_update_support_resistance? "1":"0");
    appendXmlElement(&stream, TOKEN_UPDATE_HISTOGRAM, enable_update_histogram? "1":"0");
    appendXmlElement(&stream, TOKEN_DEVELOPER_MODE, enable_developer_mode? "1":"0");

    stream.writeStartElement(TOKEN_SYMBOL);
    for (int i = 0; i < list_symbol.length(); ++i) {
      stream.writeTextElement("item", list_symbol[i]);
    }
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_MACD);
    for (int i = 0; i < t_macd.size(); ++i) {
      appendXmlElement_Threshold_1(&stream, QString::number(t_macd[i].t_id),
                                 t_macd[i].operator1, QString::number(t_macd[i].value1),
                                 t_macd[i].operator2, QString::number(t_macd[i].value2));
    }
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_RSI);
    for (int i = 0; i < t_rsi.size(); ++i) {
      appendXmlElement_Threshold_1(&stream, QString::number(t_rsi[i].t_id),
                                 t_rsi[i].operator1, QString::number(t_rsi[i].value1),
                                 t_rsi[i].operator2, QString::number(t_rsi[i].value2));
    }
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_SLOWK);
    for (int i = 0; i < t_slowk.size(); ++i) {
      appendXmlElement_Threshold_1(&stream, QString::number(t_slowk[i].t_id),
                                 t_slowk[i].operator1, QString::number(t_slowk[i].value1),
                                 t_slowk[i].operator2, QString::number(t_slowk[i].value2));
    }
    stream.writeEndElement();

    stream.writeStartElement(TOKEN_SLOWD);
    for (int i = 0; i < t_slowd.size(); ++i) {
      appendXmlElement_Threshold_1(&stream, QString::number(t_slowd[i].t_id),
                                 t_slowd[i].operator1, QString::number(t_slowd[i].value1),
                                 t_slowd[i].operator2, QString::number(t_slowd[i].value2));
    }
    stream.writeEndElement();

    //
    // Interval Weight
    //
    stream.writeStartElement(TOKEN_INTERVAL);
    QMap<int,QString>::const_iterator interval_iterator = interval_weight.constBegin();

    while (interval_iterator != interval_weight.constEnd()) {
      appendXmlElement_Interval(&stream,
                                interval_iterator.value(),
                                QString::number(interval_iterator.key()),
                                QString::number(interval_threshold[interval_iterator.key()]));
      ++interval_iterator;
    }
    stream.writeEndElement();

    //
    // Threshold
    //
    QMap<QString,QVector<t_sr_threshold>>::const_iterator it = list_sr_threshold.constBegin();
    QVector<t_sr_threshold> data;
    stream.writeStartElement("thresholds");

    while (it != list_sr_threshold.constEnd()) {
      data = it.value();
      stream.writeComment( it.key() );
      stream.writeStartElement("market");
      stream.writeAttribute("symbol", it.key());
      stream.writeStartElement("sr_threshold");

      for (int j = 0; j < data.size(); ++j) {
        appendXmlElement_SupportResistance_V2(
          &stream, QString::number(j),
          QString::number(data[j].test_point),
          QString::number(data[j].break_threshold,'f',4),
          data[j].recalculate? "1":"0");
      }

      stream.writeEndElement(); // end sr threshold

      stream.writeStartElement(TOKEN_CGF);
      for (int j = 0; j < t_cgf[it.key()].size(); ++j) {
        appendXmlElement_Threshold_2(&stream, QString::number(t_cgf[it.key()][j].t_id), t_cgf[it.key()][j].value);
      }
      stream.writeEndElement();

      stream.writeStartElement(TOKEN_CLF);
      for (int j = 0; j < t_clf[it.key()].size(); ++j) {
        appendXmlElement_Threshold_2(&stream, QString::number(t_clf[it.key()][j].t_id), t_clf[it.key()][j].value);
      }
      stream.writeEndElement();

      stream.writeStartElement(TOKEN_CGS);
      for (int j = 0; j < t_cgs[it.key()].size(); ++j) {
        appendXmlElement_Threshold_2(&stream, QString::number(t_cgs[it.key()][j].t_id), t_cgs[it.key()][j].value);
      }
      stream.writeEndElement();

      stream.writeStartElement(TOKEN_CLS);
      for (int j = 0; j < t_cls[it.key()].size(); ++j) {
        appendXmlElement_Threshold_2(&stream, QString::number(t_cls[it.key()][j].t_id), t_cls[it.key()][j].value);
      }
      stream.writeEndElement();

      stream.writeEndElement(); // end market
      ++it;
    }

    stream.writeEndElement(); // end threshold
    stream.writeEndElement(); // end application
    stream.writeEndDocument();
  }
  file.close();
}

void XmlConfigHandler::xml_parse_database(QXmlStreamReader &xml) {
  xml.readNext();
  QString symbol;
  QString interval;
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "database")) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        symbol = xml.attributes().value("symbol").toString();
        interval = xml.attributes().value("interval").toString();
        xml.readNext();
        list_database.append(xml.text().toString());
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_interval(QXmlStreamReader &xml) {
  xml.readNext();
  QString interval;
  int thres;
  int weight;
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == TOKEN_INTERVAL)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        interval = xml.attributes().value("name").toString();
        weight = xml.attributes().value("weight").toInt();
        thres = xml.attributes().value("threshold").toInt();
        interval_weight.insert(weight, interval);
        interval_threshold.insert(weight, thres);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_close_fs(QXmlStreamReader &xml, const QString &symbol, const QString &column_name) {
  QVector<t_threshold_2> vec;

  for (int i = 1; i <= MAX_CLOSE_THRESHOLD; ++i) {
    while ( !xml.readNextStartElement() );

    t_threshold_2 data;
    data.t_id = xml.attributes().value("id").toInt();
    data.value = xml.attributes().value("value").toString();

//    qDebug() << symbol << column_name << data.t_id << data.value;
    vec.push_back( data );
  }

  if (column_name == "CGF") {
    t_cgf.insert( symbol , vec );
  }
  else if (column_name == "CLF") {
    t_clf.insert( symbol , vec );
  }
  else if (column_name == "CGS") {
    t_cgs.insert( symbol , vec );
  }
  else if (column_name == "CLS") {
    t_cls.insert( symbol , vec );
  }
}

void XmlConfigHandler::xml_parse_threshold(QXmlStreamReader &xml) {
  xml.readNext();
  QVector<t_sr_threshold> thres;
  QString symbol;
  t_sr_threshold data;

  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "thresholds")) {

    if (xml.tokenType() == QXmlStreamReader::StartElement) {

      if (xml.name() == "market") {
        symbol = xml.attributes().value("symbol").toString();
//        qDebug() << "market" << symbol;

        while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "market")) {

          xml.readNext();

          if (xml.tokenType() == QXmlStreamReader::StartElement) {

            if (xml.name() == "sr_threshold") {
//              qDebug () << "sr_threshold";

              for (int i = 1; i <= MAX_SUPPORT_RESISTANCE_THRESHOLD; ++i) {
                while ( !xml.readNextStartElement() );

                if ( xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "param" ) {
                  data.t_id = xml.attributes().value("index").toInt();
                  data.test_point = xml.attributes().value("test_point").toDouble();
                  data.break_threshold = xml.attributes().value("break_threshold").toDouble();
                  data.react_threshold = data.break_threshold;
                  data.recalculate = (xml.attributes().value("recalculate").toInt() == 1);

//                  qDebug() << "SR" << xml.name() << data.t_id << data.test_point << data.break_threshold ;

                  if ( list_sr_threshold.contains(symbol) ) {
                    thres = list_sr_threshold[symbol];
                  }

                  thres.append( data );
                  list_sr_threshold.insert( symbol , thres );
                  thres.clear();
                }
              }
            }

            if (xml.name() == "CGF" || xml.name() == "CLF" ||
                xml.name() == "CGS" || xml.name() == "CLS") {
              xml_parse_close_fs(xml, symbol, xml.name().toString());
            }
          }
        }
      }
    }

    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_element(QXmlStreamReader &xml, const QString &name, QStringList *out) {
  xml.readNext();
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name().toString() == name)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name().toString().toLower() == "item") {
        xml.readNext();
        out->append(xml.text().toString());
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_macd(QXmlStreamReader &xml) {
  xml.readNext();
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == TOKEN_MACD)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        t_threshold_1 m_data;
        m_data.t_id = xml.attributes().value("id").toInt();
        m_data.operator1 = xml.attributes().value("operator1").toString();
        m_data.operator2 = xml.attributes().value("operator2").toString();
        m_data.value1 = xml.attributes().value("value1").toDouble();
        m_data.value2 = xml.attributes().value("value2").toDouble();
        t_macd.append(m_data);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_rsi(QXmlStreamReader &xml) {
  xml.readNext();
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == TOKEN_RSI)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        t_threshold_1 m_data;
        m_data.t_id = xml.attributes().value("id").toInt();
        m_data.operator1 = xml.attributes().value("operator1").toString();
        m_data.operator2 = xml.attributes().value("operator2").toString();
        m_data.value1 = xml.attributes().value("value1").toDouble();
        m_data.value2 = xml.attributes().value("value2").toDouble();
        t_rsi.append(m_data);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_slowk(QXmlStreamReader &xml) {
  xml.readNext();
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == TOKEN_SLOWK)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        t_threshold_1 m_data;
        m_data.t_id = xml.attributes().value("id").toInt();
        m_data.operator1 = xml.attributes().value("operator1").toString();
        m_data.operator2 = xml.attributes().value("operator2").toString();
        m_data.value1 = xml.attributes().value("value1").toDouble();
        m_data.value2 = xml.attributes().value("value2").toDouble();
        t_slowk.append(m_data);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_slowd(QXmlStreamReader &xml) {
  xml.readNext();
  while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == TOKEN_SLOWD)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        t_threshold_1 m_data;
        m_data.t_id = xml.attributes().value("id").toInt();
        m_data.operator1 = xml.attributes().value("operator1").toString();
        m_data.operator2 = xml.attributes().value("operator2").toString();
        m_data.value1 = xml.attributes().value("value1").toDouble();
        m_data.value2 = xml.attributes().value("value2").toDouble();
        t_slowd.append(m_data);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::read_config() {
  QFile file(config_path);

  initialize_variable();

  // Create new config, if not exists
  if (!file.exists())
    write_config_default();

  if (file.open(QFile::ReadOnly)) {
    QXmlStreamReader xml(&file);
    QXmlStreamReader::TokenType token;
    QString token_name;

    while (!xml.atEnd() && !xml.hasError()) {
      token = xml.readNext();

      if (token == QXmlStreamReader::StartDocument)
        continue;

      if (token == QXmlStreamReader::StartElement) {
        token_name = xml.name().toString();

        if (token_name == TOKEN_INPUT_DIR) {
          input_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == TOKEN_RESULT_DIR) {
          result_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == TOKEN_DATABASE_DIR) {
          database_dir = xml.attributes().value("value").toString();
          if (database_dir.isEmpty()) {
            database_dir = application_path;
          }
        }
        else if (token_name == TOKEN_PROCESS_DIR) {
          process_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == TOKEN_UPDATE_PARENT_INDEX) {
          enable_parent_indexing = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == TOKEN_UPDATE_SUPPORT_RESISTANCE) {
          enable_update_support_resistance = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == TOKEN_UPDATE_HISTOGRAM) {
          enable_update_histogram = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == TOKEN_DEVELOPER_MODE) {
          enable_developer_mode = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == TOKEN_LAST_UPDATE) {
          last_update = QDateTime::fromString(xml.attributes().value("value").toString(),"yyyy/MM/dd hh:mm:ss");
        }
        else if (token_name == TOKEN_SYMBOL) xml_parse_element(xml, TOKEN_SYMBOL, &list_symbol);
        else if (token_name == TOKEN_INTERVAL) xml_parse_interval(xml);
        else if (token_name == "thresholds") xml_parse_threshold(xml);
        else if (token_name == TOKEN_MACD) xml_parse_macd(xml);
        else if (token_name == TOKEN_RSI) xml_parse_rsi(xml);
        else if (token_name == TOKEN_SLOWK) xml_parse_slowk(xml);
        else if (token_name == TOKEN_SLOWD) xml_parse_slowd(xml);
        else if (token_name == "database") xml_parse_database(xml);
      }
    }

    xml.clear();
  }

  /*
  qDebug() << "\n-------------------------------------------------- Assert Config";

  for (int i = 0; i < t_rsi.size(); ++i) {
    qDebug() << "RSI" << t_rsi[i].t_id
             << t_rsi[i].operator1 << t_rsi[i].value1
             << t_rsi[i].operator2 << t_rsi[i].value2;
  }

  for (int i = 0; i < t_slowk.size(); ++i) {
    qDebug() << "SlowK" << t_slowk[i].t_id
             << t_slowk[i].operator1 << t_slowk[i].value1
             << t_slowk[i].operator2 << t_slowk[i].value2;
  }

  for (int i = 0; i < t_slowd.size(); ++i) {
    qDebug() << "SlowD" << t_slowd[i].t_id
             << t_slowd[i].operator1 << t_slowd[i].value1
             << t_slowd[i].operator2 << t_slowd[i].value2;
  }

  for (int i = 0 ; i < list_symbol.size(); ++i) {
    qDebug() << "###" << list_symbol[i] << t_cgf[list_symbol[i]].size();


    for (int j = 0; j < list_sr_threshold[list_symbol[i]].size(); ++j) {
      qDebug() << "SR" << list_sr_threshold[list_symbol[i]][j].t_id
                       << list_sr_threshold[list_symbol[i]][j].test_point
                       << list_sr_threshold[list_symbol[i]][j].break_threshold
                       << list_sr_threshold[list_symbol[i]][j].react_threshold
                       << list_sr_threshold[list_symbol[i]][j].recalculate;
    }

    for ( int j = 0 ; j < t_cgf[list_symbol[i]].size(); ++j ) {
      qDebug() << "CGF" <<  t_cgf[list_symbol[i]][j].t_id << t_cgf[list_symbol[i]][j].value;
    }

    for ( int j = 0 ; j < t_clf[list_symbol[i]].size(); ++j ) {
      qDebug() << "CLF" <<  t_clf[list_symbol[i]][j].t_id << t_clf[list_symbol[i]][j].value;
    }

    for ( int j = 0 ; j < t_cgs[list_symbol[i]].size(); ++j ) {
      qDebug() << "CGS" <<  t_cgs[list_symbol[i]][j].t_id << t_cgs[list_symbol[i]][j].value;
    }

    for ( int j = 0 ; j < t_cls[list_symbol[i]].size(); ++j ) {
      qDebug() << "CLS" <<  t_cls[list_symbol[i]][j].t_id << t_cls[list_symbol[i]][j].value;
    }
  }
  */
}

void XmlConfigHandler::reset_recalculate_threshold() {
  QMap<QString,QVector<t_sr_threshold>>::const_iterator it = list_sr_threshold.constBegin();
  QVector<t_sr_threshold> vec;
  while (it != list_sr_threshold.constEnd()) {
    vec = it.value();
    for (int i = 0; i < vec.length(); ++i) {
      vec[i].recalculate = false;
    }
    list_sr_threshold[it.key()] = vec;
    ++it;
  }
}
