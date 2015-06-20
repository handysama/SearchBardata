#include "XmlConfigHandler.h"

#define DEFAULT_DATABASE_DIR  "C:/Program Files (x86)/TS Finance/Programs"
#define DEFAULT_INPUT_DIR     "C:/Program Files (x86)/TS Finance/Download/Data"
#define DEFAULT_PROCESS_DIR   "C:/Program Files (x86)/TS Finance/Download/Processed"
#define DEFAULT_RESULT_DIR    "C:/Program Files (x86)/TS Finance/Programs/Program Results"

XmlConfigHandler* XmlConfigHandler::_instance = NULL;

void XmlConfigHandler::init_config_variable() {
  enable_developer_mode = false;
  enable_parent_indexing = true;
  enable_test_point_update = true;
  enable_distfc_distsc_rank = true;
  enable_distfs_rank = true;
  database_dir = DEFAULT_DATABASE_DIR;
  input_dir = DEFAULT_INPUT_DIR;
  process_dir = DEFAULT_PROCESS_DIR;
  result_dir = DEFAULT_RESULT_DIR;
  list_symbol.clear();
  list_database.clear();
  list_threshold.clear();
  interval_weight.clear();
  interval_threshold.clear();
}

void appendXmlElement(QXmlStreamWriter *stream, const QString &element, const QString &value) {
  stream->writeStartElement(element);
  stream->writeAttribute("value", value);
  stream->writeEndElement();
}

void appendXmlElement_Interval(QXmlStreamWriter *stream, const QString &interval, const QString &weight, const QString &threshold) {
  stream->writeStartElement("item");
  stream->writeAttribute("interval", interval);
  stream->writeAttribute("weight", weight);
  stream->writeAttribute("threshold", threshold);
  stream->writeEndElement();
}

void appendXmlElement_SupportResistance(QXmlStreamWriter *stream, const QString &symbol, const QString &index,
                                        const QString &threshold, const QString &b, const QString &recalculate) {
  stream->writeStartElement("item");
  stream->writeAttribute("symbol", symbol);
  stream->writeAttribute("index", index);
  stream->writeAttribute("threshold", threshold);
  stream->writeAttribute("b", b);
  stream->writeAttribute("recalculate", recalculate);
  stream->writeEndElement();
}

void XmlConfigHandler::write_config_default() {
  QFile file(config_path);

  if (file.open(QFile::ReadWrite)) {
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();
    stream.writeStartElement("Application");
    stream.writeAttribute("name", "TS Finance");
    stream.writeAttribute("version", QString(VERSION));

    appendXmlElement(&stream, "lastupdate", "");
    appendXmlElement(&stream, "inputdir", DEFAULT_INPUT_DIR);
    appendXmlElement(&stream, "resultdir", DEFAULT_RESULT_DIR);
    appendXmlElement(&stream, "databasedir", DEFAULT_DATABASE_DIR);
    appendXmlElement(&stream, "processdir", DEFAULT_PROCESS_DIR);

    appendXmlElement(&stream, "update_indexing", "1");
    appendXmlElement(&stream, "update_testpoint", "1");
    appendXmlElement(&stream, "update_distfs_rank", "1");
    appendXmlElement(&stream, "update_distfc_distsc_rank", "1");
    appendXmlElement(&stream, "developermode", "0");

    stream.writeStartElement("symbol");
    stream.writeTextElement("item","@AD");
    stream.writeTextElement("item","@CL");
    stream.writeTextElement("item","@EC");
    stream.writeTextElement("item","@ES");
    stream.writeTextElement("item","@GC");
    stream.writeTextElement("item","@JY");
    stream.writeTextElement("item","@NIY");
    stream.writeTextElement("item","@NQ");
    stream.writeEndElement();

    /** Intervals (use minute as weight measurement unit) */
    stream.writeStartElement("interval");
    appendXmlElement_Interval(&stream, "1min", "1", "0");
    appendXmlElement_Interval(&stream, "5min", "5", "0");
    appendXmlElement_Interval(&stream, "60min", "60", "0");
    appendXmlElement_Interval(&stream, "Daily", "1440", "0");
    appendXmlElement_Interval(&stream, "Weekly", "10080", "0");
    appendXmlElement_Interval(&stream, "Monthly", "43200", "0");
    stream.writeEndElement();

    /** Support/Resistance threshold parameters */
    {
      stream.writeStartElement("thresholds");
      /** @CL */
      appendXmlElement_SupportResistance(&stream, "@CL", "0", "2", "0.2", "0");

      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@CL", QString::number(i), "2", "0", "0");
      }

      /** @ES */
      appendXmlElement_SupportResistance(&stream, "@ES", "0", "2", "1", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@ES", QString::number(i), "2", "0", "0");
      }

      /** @GC */
      appendXmlElement_SupportResistance(&stream, "@GC", "0", "2", "1", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@GC", QString::number(i), "2", "0", "0");
      }

      /** @EC */
      appendXmlElement_SupportResistance(&stream, "@EC", "0", "2", "0.001", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@EC", QString::number(i), "2", "0", "0");
      }

      /** @NIY */
      appendXmlElement_SupportResistance(&stream, "@NIY", "0", "2", "10", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@NIY", QString::number(i), "2", "0", "0");
      }

      /** @AD */
      appendXmlElement_SupportResistance(&stream, "@AD", "0", "2", "0.001", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@AD", QString::number(i), "2", "0", "0");
      }

      /** @JY */
      appendXmlElement_SupportResistance(&stream, "@JY", "0", "2", "0.001", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@JY", QString::number(i), "2", "0", "0");
      }

      /** @NQ */
      appendXmlElement_SupportResistance(&stream, "@NQ", "0", "2", "2", "0");
      for (int i = 1; i <= 4; ++i) {
        appendXmlElement_SupportResistance(&stream, "@NQ", QString::number(i), "2", "0", "0");
      }

      stream.writeEndElement();
    }

    stream.writeEndElement();
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

    // base config
    appendXmlElement(&stream, "lastupdate", last_update.toString("yyyy/MM/dd hh:mm:ss"));
    appendXmlElement(&stream, "inputdir", input_dir);
    appendXmlElement(&stream, "resultdir", result_dir);
    appendXmlElement(&stream, "databasedir", database_dir);
    appendXmlElement(&stream, "processdir", process_dir);

    // update database config
    appendXmlElement(&stream, "update_indexing", enable_parent_indexing? "1":"0");
    appendXmlElement(&stream, "update_testpoint", enable_test_point_update? "1":"0");
    appendXmlElement(&stream, "update_distfc_distsc_rank", enable_distfc_distsc_rank? "1":"0");
    appendXmlElement(&stream, "update_distfs_rank", enable_distfs_rank? "1":"0");
    appendXmlElement(&stream, "developermode", enable_developer_mode? "1":"0");

    stream.writeStartElement("symbol");
    for (int i = 0; i < list_symbol.length(); ++i) {
      stream.writeTextElement("item", list_symbol[i]);
    }
    stream.writeEndElement();

    //
    // Interval Weight
    //
    stream.writeStartElement("interval");
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
    // SR Threshold
    //
    stream.writeStartElement("thresholds");
    QMap<QString,QVector<t_threshold>>::const_iterator threshold_iterator = list_threshold.constBegin();
    while (threshold_iterator != list_threshold.constEnd()) {
      QVector<t_threshold> v = threshold_iterator.value();
      for (int i = 0; i < v.length(); ++i) {
        appendXmlElement_SupportResistance(&stream,
                                           threshold_iterator.key(),
                                           QString::number(v[i].index),
                                           QString::number(v[i].threshold),
                                           QString::number(v[i].b),
                                           v[i].recalculate?"1":"0");
      }
      ++threshold_iterator;
    }
    stream.writeEndElement();

    stream.writeEndElement();
    stream.writeEndDocument();
  }
  file.close();
}

void XmlConfigHandler::xml_parse_database(QXmlStreamReader &xml) {
  xml.readNext();
  QString symbol;
  QString interval;
  while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
           xml.name() == "database")) {
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
  while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
           xml.name() == "interval")) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        weight = xml.attributes().value("weight").toInt();
        interval = xml.attributes().value("interval").toString();
        thres = xml.attributes().value("threshold").toInt();
        interval_weight.insert(weight, interval);
        interval_threshold.insert(weight, thres);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_threshold(QXmlStreamReader &xml) {
  xml.readNext();
  QVector<t_threshold> thres;
  QString symbol;
  t_threshold data;
  while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
           xml.name() == "thresholds")) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name() == "item") {
        symbol = xml.attributes().value("symbol").toString();
        data.index = xml.attributes().value("index").toInt();
        data.threshold = xml.attributes().value("threshold").toDouble();
        data.b = xml.attributes().value("b").toDouble();
        data.recalculate = (xml.attributes().value("recalculate").toInt()==1);
        if (list_threshold.contains(symbol)) {
          thres = list_threshold[symbol];
        } else {
          thres.clear();
        }
        thres.append(data);
        list_threshold.insert(symbol, thres);
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::xml_parse_element(QXmlStreamReader &xml, const QString &name, QStringList *out) {
  xml.readNext();
  while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
           xml.name().toString().toLower() == name)) {
    if (xml.tokenType() == QXmlStreamReader::StartElement) {
      if (xml.name().toString().toLower() == "item") {
        xml.readNext();
        out->append(xml.text().toString());
      }
    }
    xml.readNext();
  }
}

void XmlConfigHandler::read_config() {
  QFile file(config_path);

  init_config_variable();

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
        token_name = xml.name().toString().toLower(); // just in case

        if (token_name == "inputdir") {
          input_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == "resultdir") {
          result_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == "databasedir") {
          database_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == "processdir") {
          process_dir = xml.attributes().value("value").toString();
        }
        else if (token_name == "update_indexing") {
          enable_parent_indexing = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == "update_testpoint") {
          enable_test_point_update = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == "update_distfc_distsc_rank") {
          enable_distfc_distsc_rank = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == "update_distfs_rank") {
          enable_distfs_rank = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == "developermode") {
          enable_developer_mode = (xml.attributes().value("value").toInt() == 1);
        }
        else if (token_name == "symbol") {
          xml_parse_element(xml, "symbol", &list_symbol);
        }
        else if (token_name == "interval") {
          xml_parse_interval(xml);
        }
        else if (token_name == "thresholds") {
          xml_parse_threshold(xml);
        }
        else if (token_name == "lastupdate") {
          last_update = QDateTime::fromString(xml.attributes().value("value").toString(),"yyyy/MM/dd hh:mm:ss");
        }
        else if (token_name == "database") {
          xml_parse_database(xml);
        }
      }
    }
    xml.clear();
  }

  // Assert xml parse
//  qDebug() << input_dir;
//  qDebug() << process_dir;
//  qDebug() << "parent indexing?" << enable_parent_indexing;
//  qDebug() << "developer?" << enable_developer_mode;
//  for (int i=0; i<list_symbol.length();++i)
//    qDebug() << list_symbol[i];
//  qDebug() << "Interval weight";
//  QMap<int,QString>::const_iterator it = interval_weight.constBegin();
//  while (it != interval_weight.constEnd()) {
//    qDebug() << it.key() << it.value();
//    ++it;
//  }
//  QMap<QString,QVector<t_threshold>>::const_iterator it = list_threshold.constBegin();
//  while (it != list_threshold.constEnd()) {
//    qDebug() << it.key();
//    QVector<t_threshold> v = it.value();
//    for (int i=0; i < v.length(); ++i) {
//      qDebug() << v[i].threshold << " | " << v[i].b << v[i].index;
//    }
//    ++it;
//  }
}

void XmlConfigHandler::reset_recalculate_threshold() {
  QMap<QString,QVector<t_threshold>>::const_iterator it = list_threshold.constBegin();
  QVector<t_threshold> vec;
  while (it != list_threshold.constEnd()) {
    vec = it.value();
    for (int i = 0; i < vec.length(); ++i) {
      vec[i].recalculate = false;
    }
    list_threshold[it.key()] = vec;
    ++it;
  }
}
