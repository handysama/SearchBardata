/*QStringList MainWindow::sql_equal_resistance(const int &start_weight, const int &end_weight, const QString &symbol, const int &threshold_index) {
  QString main_alias = "A";
  QString main_resistance;
  QString main_resistance_value = main_alias + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
  QString main_idparent = main_alias + "." + SQLiteHandler::COLUMN_NAME_IDPARENT; // ??
  QString main_rowid = main_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID;
  QString main_database_name;
  QString child_alias = "B";
  QString child_database_alias = "db_child";
  QString child_rowid = child_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID;
  QString child_bardata = child_database_alias + "." + SQLiteHandler::TABLE_NAME_BARDATA;
  QString child_resistance;
  QString child_resistance_value = "B." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
  QString child_database_name;
  QString sql;
  QStringList list_rowid;
  QStringList list_rid;
  QVector<QString> intervals;
  QVector<int> weights;

  main_resistance = SQLiteHandler::TABLE_NAME_RESISTANCE;
  child_resistance = child_database_alias + "." + SQLiteHandler::TABLE_NAME_RESISTANCE;

  if (start_weight > end_weight) {
    get_interval_weight_vector(list_interval_weight, &intervals, &weights, true);

    int start_interval = weights.indexOf(end_weight);
    int end_interval = weights.indexOf(start_weight);

    for (int i = start_interval; i < end_interval; ++i) {
      main_database_name = db.getDatabaseName(symbol, list_interval_weight[weights[i]]);
      child_database_name = application_path + "/" + db.getDatabaseName(symbol, list_interval_weight[weights[i+1]]);

      qDebug() << "main:" << main_database_name;
      qDebug() << "child:" << child_database_name;
      if (list_rowid.length() > 0) {
        sql =
          "select " + main_idparent + ",B.rid from"
          "(select bd.rowid,bd._parent,rd.rowid as 'rid',rd.resistance from bardata bd join " + main_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " join "
          "(select bd.rowid,rd.resistance,rd.rowid as 'rid' from " + child_bardata + " bd join " + child_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_resistance_value + "=" + child_resistance_value + " and " +
          main_idparent + "=" + child_rowid + " and A.rid in(" + list_rid.join(",") + ")" ;

        //child_rowid + " in(" + list_rowid.join(",") + ")";
        list_rowid.clear();
        list_rid.clear();
      } else {
        sql =
          "select " + main_idparent + ",B.rid from"
          "(select bd.rowid,bd._parent,rd.rowid as 'rid', resistance from bardata bd join " + main_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " join "
          "(select bd.rowid,resistance,rd.rowid as 'rid' from " + child_bardata + " bd join " + child_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_resistance_value + "=" + child_resistance_value + " and " +
          main_idparent + "=" + child_rowid;
      }

      qDebug() << "select:" << sql;
      QSqlDatabase database = db.getDatabase(main_database_name);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + child_database_name + "' AS " + child_database_alias);
      query.exec("PRAGMA PRAGMA synchronous = OFF;");
      qDebug() << query.exec(sql);
      while (query.next()) {
        list_rowid.append(query.value(0).toString());
        list_rid.append(query.value(1).toString());
      }

//      for (int i = 0; i < list_values.length(); ++i)
//        qDebug() << list_values[i];

      if (list_rowid.length() == 0)
        break;
    }

  } else {
    get_interval_weight_vector(list_interval_weight, &intervals, &weights, false);

    int start_interval = weights.indexOf(end_weight);
    int end_interval = weights.indexOf(start_weight);

    for (int i = start_interval; i < end_interval; ++i) {
      child_database_name = application_path + "/" + db.getDatabaseName(symbol, list_interval_weight[weights[i]]);;
      main_database_name = db.getDatabaseName(symbol, list_interval_weight[weights[i+1]]);

      qDebug() << main_database_name;
      qDebug() << child_database_name;

      if (list_rowid.length() > 0) {
        sql =
          "select " + main_rowid + ",A.rid from"
          "(select bd.rowid,bd._parent,rd.resistance,rd.rowid AS 'rid' from bardata bd join " + main_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " join "
          "(select bd.rowid,rd.resistance,rd.rowid as 'rid' from " + child_bardata + " bd join " + child_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_resistance_value + "=" + child_resistance_value + " and " +
          main_idparent + "=" + child_rowid + " and B.rid in(" + list_rid.join(",") + ")" ;
        //child_rowid + " in(" + list_rowid.join(",") + ")";

        list_rowid.clear();
        list_rid.clear();
      } else {
        sql =
          "select " + main_rowid + ",A.rid from"
          "(select bd.rowid,bd._parent,resistance,rd.rowid as 'rid' from bardata bd join " + main_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " join "
          "(select bd.rowid,resistance,rd.rowid as 'rid' from " + child_bardata + " bd join " + child_resistance + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_resistance_value + "=" + child_resistance_value + " and " +
          main_idparent + "=" + child_rowid;
      }

      QSqlDatabase database = db.getDatabase(main_database_name);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + child_database_name + "' AS " + child_database_alias);
      query.exec("PRAGMA PRAGMA synchronous = OFF;");
      query.exec(sql);
      while (query.next()) {
        list_rowid.append(query.value(0).toString());
        list_rid.append(query.value(1).toString());
      }

      if (list_rowid.length() == 0)
        break;
    }
  }

  if (list_rowid.length() == 0)
    list_rowid.append("-1");

  qDebug() << "result: " << list_rowid.length();
//  for (int i = 0; i < list_rowid.length(); ++i) {
//    qDebug() << list_rowid[i];
//  }
  return list_rowid;
}*/

/*QStringList MainWindow::sql_equal_support(const int &start_weight, const int &end_weight, const QString &symbol, const int &threshold_index) {
  QString main_alias = "A";
  QString main_support;
  QString main_support_value = main_alias + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
  QString main_idparent = main_alias + "." + SQLiteHandler::COLUMN_NAME_IDPARENT;
  QString main_rowid = main_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID;
  QString main_database_name;
  QString child_alias = "B";
  QString child_database_alias = "db_child";
  QString child_rowid = child_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID;
  QString child_bardata = child_database_alias + "." + SQLiteHandler::TABLE_NAME_BARDATA;
  QString child_support;
  QString child_support_value = child_alias + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
  QString child_database_name;
  QString sql;
  QStringList list_rowid;
  QStringList list_rid;
  QVector<QString> intervals;
  QVector<int> weights;

  main_support = SQLiteHandler::TABLE_NAME_SUPPORT;
  child_support = child_database_alias + "." + SQLiteHandler::TABLE_NAME_SUPPORT;

  if (start_weight > end_weight) {
    get_interval_weight_vector(this->list_interval_weight, &intervals, &weights, true);

    int start_interval = weights.indexOf(end_weight);
    int end_interval = weights.indexOf(start_weight);

    for (int i = start_interval; i < end_interval; ++i) {
      main_database_name = db.getDatabaseName(symbol, list_interval_weight[weights[i]]);
      child_database_name = application_path + "/" + db.getDatabaseName(symbol, list_interval_weight[weights[i+1]]);
      qDebug() << main_database_name << child_database_name;

      if (list_rowid.length() > 0) {
        sql =
          "select " + main_idparent + ",B.rid from"
          "(select bd.rowid,bd._parent,rd.support,rd.rowid as 'rid' from bardata bd join " + main_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " inner join "
          "(select bd.rowid,rd.support,rd.rowid as 'rid' from " + child_bardata + " bd inner join " + child_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_support_value + "=" + child_support_value + " and " +
          main_idparent + "=" + child_rowid + " and A.rid in(" + list_rid.join(",") + ")";

      //child_rowid + " in(" + list_rowid.join(",") + ")";
        list_rowid.clear();
        list_rid.clear();
      } else {
        sql =
          "select " + main_idparent + ",B.rid from"
          "(select bd.rowid,bd._parent,support,rd.rowid as 'rid' from bardata bd join " + main_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " inner join "
          "(select bd.rowid,support,rd.rowid as 'rid' from " + child_bardata + " bd join " + child_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_support_value + "=" + child_support_value + " and " +
          main_idparent + "=" + child_rowid;
      }

      qDebug() << sql;
      QSqlDatabase database = db.getDatabase(main_database_name);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + child_database_name + "' AS " + child_database_alias);
      query.exec("PRAGMA PRAGMA synchronous = OFF;");
      query.exec(sql);
      while (query.next()) {
        list_rowid.append(query.value(0).toString());
        list_rid.append(query.value(1).toString());
      }

      if (list_rowid.length() == 0)
        break;
    }
  } else {
    get_interval_weight_vector(this->list_interval_weight, &intervals, &weights, false);

    int start_interval = weights.indexOf(end_weight);
    int end_interval = weights.indexOf(start_weight);

    for (int i = start_interval; i < end_interval; ++i) {
      child_database_name = application_path + "/" + db.getDatabaseName(symbol, list_interval_weight[weights[i]]);;
      main_database_name = db.getDatabaseName(symbol, list_interval_weight[weights[i+1]]);

      if (list_rowid.length() > 0) {
        sql =
          "select " + main_rowid + ",A.rid from"
          "(select bd.rowid,bd._parent,rd.support,rd.rowid as 'rid' from bardata bd join " + main_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " inner join "
          "(select bd.rowid,rd.support,rd.rowid as 'rid' from " + child_bardata + " bd inner join " + child_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_support_value + "=" + child_support_value + " and " +
          main_idparent + "=" + child_rowid + " and B.rid in(" + list_rid.join(",") + ")";

        //child_rowid + " in(" + list_rowid.join(",") + ")";
        list_rowid.clear();
        list_rid.clear();
      } else {
        sql =
          "select " + main_rowid + ",A.rid from"
          "(select bd.rowid,bd._parent,rd.rowid as 'rid',support from bardata bd join " + main_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + main_alias + " inner join "
          "(select bd.rowid,rd.rowid as 'rid',support from " + child_bardata + " bd join " + child_support + " rd "
          "on bd.date_=rd.date_ and bd.time_=rd.time_)" + child_alias + " on " +
          main_support_value + "=" + child_support_value + " and " +
          main_idparent + "=" + child_rowid;
      }

      qDebug() << sql;
      QSqlDatabase database = db.getDatabase(main_database_name);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + child_database_name + "' AS " + child_database_alias);
      query.exec("PRAGMA PRAGMA synchronous = OFF;");
      query.exec(sql);
      while (query.next()) {
        list_rowid.append(query.value(0).toString());
        list_rid.append(query.value(1).toString());
      }

      if (list_rowid.length() == 0)
        break;
    }
  }

  if (list_rowid.length() == 0)
    list_rowid.append("-1");

//  qDebug() << "result: " << list_rowid.length();
//  for (int i = 0; i < list_rowid.length(); ++i) {
//    qDebug() << list_rowid[i];
//  }
  return list_rowid;
}*/

/*QStringList MainWindow::sql_equal_column(const int &start_weight, const int &end_weight, const QString &symbol, const QString &column_name) {
  QStringList list_rowid;
  QString column_rowid = SQLiteHandler::COLUMN_NAME_ROWID;
  QString column_idparent = SQLiteHandler::COLUMN_NAME_IDPARENT;
  QString parent_alias = "B";
//  QString parent_rowid = parent_alias + "." + SQLiteHandler::COLUMN_NAME_ROWID;
  QString parent_idparent = parent_alias + "." + SQLiteHandler::COLUMN_NAME_IDPARENT;
  QString parent_column_name = parent_alias + "." + column_name;
  QString parent_bardata = SQLiteHandler::TABLE_NAME_BARDATA + " " + parent_alias;
  QString child_database_alias = "db_child";
  QString child_bardata = child_database_alias + "." + SQLiteHandler::TABLE_NAME_BARDATA;
  QString sql;
  QString parent_database_name;
  QString child_database_name;

  QSqlDatabase database;
  QVector<QString> intervals;
  QVector<int> weights;

  if (start_weight > end_weight) {
    get_interval_weight_vector(this->list_interval_weight,&intervals, &weights, true);

    // daily to 5min
    // bottom-up from 5min > 60min > daily
    // parent: 5min, child:60min  result idparent 5min which is 60min
    // parent: 60min, child:daily  result idparent which is daily

    int start_interval = weights.indexOf(end_weight);
    int end_interval = weights.indexOf(start_weight);

    for (int i = start_interval; i < end_interval; ++i) {
      parent_database_name = db.getDatabaseName(symbol, list_interval_weight[weights[i]]);
      child_database_name = application_path + "/" + db.getDatabaseName(symbol, list_interval_weight[weights[i+1]]);

      if (list_rowid.length() > 0) {
        sql =
          "select " + column_idparent + " from " + parent_bardata +
          " where " + column_rowid + " in(" + list_rowid.join(",") + ") and " +
                      column_idparent + " in(select " + column_rowid + " from " + child_bardata +
          " where " + column_name + "=" + parent_column_name + " and " +
                      column_rowid + "=" + parent_idparent + ")";
        list_rowid.clear();
      } else {
        sql =
          "select " + column_idparent + " from " + parent_bardata +
          " where " + column_idparent + " in(select " + column_rowid + " from " + child_bardata +
          " where " + column_name + "=" + parent_column_name + " and " +
                      column_rowid + "=" + parent_idparent + ")";
      }

//      qDebug() << "debug:" << sql;
      database = db.getDatabase(parent_database_name);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + child_database_name + "' AS " + child_database_alias);
      query.exec(sql);
      while (query.next()) {
        list_rowid.append(query.value(0).toString());
      }

      database.close();
      database = QSqlDatabase();

      // if there's no result for current resolution then break
      if (list_rowid.length() == 0)
        break;
    }
  } else {
    get_interval_weight_vector(this->list_interval_weight, &intervals, &weights, false);

    // 5min to daily
    // top-down from daily > 60min > 5min
    // parent: daily select rowid which is _parent of child, result rowid
    // parent:60min child:daily >> select rowid where _parent is daily
    // parent:5min child:60min >> select rowid where _parent is daily

    int start_interval = weights.indexOf(end_weight);
    int end_interval = weights.indexOf(start_weight);

    for (int i = start_interval; i < end_interval; ++i) {
      child_database_name = application_path + "/" + db.getDatabaseName(symbol, list_interval_weight.value(weights[i]));
      parent_database_name = db.getDatabaseName(symbol, list_interval_weight.value(weights[i+1]));

      if (list_rowid.length() > 0) {
        sql =
          "select " + column_rowid + " from " + parent_bardata +
          " where " + column_idparent + " in(" + list_rowid.join(",") + ") and " +
                      column_idparent + " in(select " + column_rowid + " from " + child_bardata +
          " where " + column_name + "=" + parent_column_name + " and " + column_rowid + "=" + parent_idparent + ")";
        list_rowid.clear();
      } else {
        sql =
          "select " + column_rowid + " from " + parent_bardata +
          " where " + column_idparent + " in(select " + column_rowid + " from " + child_bardata +
          " where " + column_name + "=" + parent_column_name + " and " + column_rowid + "=" + parent_idparent + ")";
      }

//      qDebug() << "debug:" << sql;
      database = db.getDatabase(parent_database_name);
      QSqlQuery query(database);
      query.setForwardOnly(true);
      query.exec("ATTACH DATABASE '" + child_database_name + "' AS " + child_database_alias);
      query.exec(sql);
      while (query.next()) {
        list_rowid.append(query.value(0).toString());
      }

      database.close();
      database = QSqlDatabase();

      // if there's no result for current resolution then break
      if (list_rowid.length() == 0)
        break;
    }
  }

  // prevent empty result
  if (list_rowid.length() < 1) {
    list_rowid.append("-1");
  }

//  qDebug() << "res:" << list_rowid.length();
//  for (int i = 0; i < list_rowid.length(); ++i) {
//    qDebug() << list_rowid[i];
//  }
  return list_rowid;
}*/

/*void MainWindow::sql_multi_interval(const FilterOptionFactory &table, const QString &symbol, const int &threshold_index, QString *out_sql) {
  typedef struct {
    QString database_name;
    QString sql;
    QString sql_distf_2001;
    QString sql_distf_2006;
    QString sql_distf_2011;
    QString sql_dists_2001;
    QString sql_dists_2006;
    QString sql_dists_2011;
    QString distf_table;
    QString dists_table;
    bool distrank_flag;
  } t_query;

  QString interval;
  QMap<int,t_query*> whereArgs;
  int weight;
  int N = table.length();

  for (int i = 0; i < N; ++i) {
    interval = table.getInterval(i);
    weight = table.getIntervalWeight(i);

    if (!whereArgs.contains(weight)) {
      t_query *t = new t_query;
      t->database_name = symbol + "_" + interval + ".db";
      t->distrank_flag = false;
      whereArgs.insert(weight,t);
    }

    if (!whereArgs[weight]->sql.isEmpty()) {
      whereArgs[weight]->sql += " and ";
    }

    if (table.is_equal_to_interval_condition(i)) {
      QStringList args = table.getSqlString(i).split(",");
      if (args[0] != args[1]) {
        QStringList res = sql_equal_column(args[0].toInt(),
            args[1].toInt(),symbol,args[2]);
        whereArgs[weight]->sql +=  SQLiteHandler::COLUMN_NAME_ROWID + " in(" + res.join(",") + ")";
      }
    }
    else if (table.is_equal_to_resistance(i)) {
      QStringList args = table.getSqlString(i).split(",");
      if (args[0] != args[1]) {
        QStringList res = sql_equal_resistance(args[0].toInt(),args[1].toInt(),symbol, threshold_index);
        whereArgs[weight]->sql +=  SQLiteHandler::COLUMN_NAME_ROWID + " in(" + res.join(",") + ")";
      }
    }
    else if (table.is_equal_to_support(i)) {
      QStringList args = table.getSqlString(i).split(",");
      if (args[0] != args[1]) {
        QStringList res = sql_equal_support(args[0].toInt(),args[1].toInt(),symbol, threshold_index);
        whereArgs[weight]->sql +=  SQLiteHandler::COLUMN_NAME_ROWID + " in(" + res.join(",") + ")";
      }
    }
    else if (table.is_distrank_condition(i)) {
      whereArgs[weight]->sql += "1=1";
      whereArgs[weight]->distrank_flag += true;
      whereArgs[weight]->distf_table = SQLiteHandler::TABLE_NAME_DISTF_RANK;
      whereArgs[weight]->dists_table = SQLiteHandler::TABLE_NAME_DISTS_RANK;
      int year = table.get_distrank_year(i);

      if (table.is_distf_column(i)) {
        if (year == 2001) {
          if (!whereArgs[weight]->sql_distf_2001.isEmpty())
            whereArgs[weight]->sql_distf_2001 += " and ";
          whereArgs[weight]->sql_distf_2001 += table.getSqlDistRankCondition(i);
        }
        else if (year == 2006) {
          if (!whereArgs[weight]->sql_distf_2006.isEmpty())
            whereArgs[weight]->sql_distf_2006 += " and ";
          whereArgs[weight]->sql_distf_2006 += table.getSqlDistRankCondition(i);
        }
        else if (year == 2011) {
          if (!whereArgs[weight]->sql_distf_2011.isEmpty())
            whereArgs[weight]->sql_distf_2011 += " and ";
          whereArgs[weight]->sql_distf_2011 += table.getSqlDistRankCondition(i);
        }
      } else {
        if (year == 2001) {
          if (!whereArgs[weight]->sql_dists_2001.isEmpty())
            whereArgs[weight]->sql_dists_2001 += " and ";
          whereArgs[weight]->sql_dists_2001 += table.getSqlDistRankCondition(i);
        }
        else if (year == 2006) {
          if (!whereArgs[weight]->sql_dists_2006.isEmpty())
            whereArgs[weight]->sql_dists_2006 += " and ";
          whereArgs[weight]->sql_dists_2006 += table.getSqlDistRankCondition(i);
        }
        else if (year == 2011) {
          if (!whereArgs[weight]->sql_dists_2011.isEmpty())
            whereArgs[weight]->sql_dists_2011 += " and ";
          whereArgs[weight]->sql_dists_2011 += table.getSqlDistRankCondition(i);
        }
      }
    }
    else {
      whereArgs[weight]->sql += table.getSqlString(i);
    }
  }

  QStringList list_rowid;
  QStringList list_rowid_distrank;
  QMap<int,t_query*>::const_iterator it_start = whereArgs.constBegin();
  QMap<int,t_query*>::const_iterator it_end = whereArgs.constEnd(); --it_end;
  QString sql_select = "";
  QVector<QString> list_interval;
  QVector<int> weights;

  get_interval_weight_vector(this->list_interval_weight, &list_interval, &weights, false);

  QString sql_select_distrank;
  QString sql_select_distf_1 =
    "select a.rowid from bardata a"
    " left join " + SQLiteHandler::TABLE_NAME_DISTF_RANK + " b on a.date_=b.date_ and a.time_=b.time_";
  QString sql_select_dists_1 =
    "select a.rowid from bardata a"
    " left join " + SQLiteHandler::TABLE_NAME_DISTS_RANK + " b on a.date_=b.date_ and a.time_=b.time_";

  QString database_name;
  int start_interval = weights.indexOf(it_end.key());
  int end_interval = weights.indexOf(it_start.key());

  for (int i = start_interval; i <= end_interval; ++i) {
    t_query *t = whereArgs.value(weights[i], NULL);

    if (t != NULL) {
      database_name = t->database_name;
      sql_select = SQLiteHandler::SQL_SELECT_BARDATA_ROWID + " where " + t->sql;

      if (t->distrank_flag) {
        sql_select_distrank = "";

        if (!t->sql_distf_2001.isEmpty()) {
          sql_select_distrank += sql_select_distf_1 + " where " + t->sql_distf_2001;
        }

        if (!t->sql_distf_2006.isEmpty()) {
          if (!sql_select_distrank.isEmpty())
            sql_select_distrank += " intersect ";
          sql_select_distrank += sql_select_distf_1 + " where " + t->sql_distf_2006;
        }

        if (!t->sql_distf_2011.isEmpty()) {
          if (!sql_select_distrank.isEmpty())
            sql_select_distrank += " intersect ";
          sql_select_distrank += sql_select_distf_1 + " where " + t->sql_distf_2011;
        }

        if (!t->sql_dists_2001.isEmpty()) {
          if (!sql_select_distrank.isEmpty())
            sql_select_distrank += " intersect ";
          sql_select_distrank += sql_select_dists_1 + " where " + t->sql_dists_2001;
        }

        if (!t->sql_dists_2006.isEmpty()) {
          if (!sql_select_distrank.isEmpty())
            sql_select_distrank += " intersect ";
          sql_select_distrank += sql_select_dists_1 + " where " + t->sql_dists_2006;
        }

        if (!t->sql_dists_2011.isEmpty()) {
          if (!sql_select_distrank.isEmpty())
            sql_select_distrank += " intersect ";
          sql_select_distrank += sql_select_dists_1 + " where " + t->sql_dists_2011;
        }

        qDebug() << "select (distrank)" << sql_select_distrank;
        QSqlQuery *query_1 = db.execQuery(database_name, sql_select_distrank);
        while (query_1->next()) {
          list_rowid_distrank.append(query_1->value(0).toString());
        }

        delete query_1;
        if (list_rowid_distrank.length() == 0) {
          list_rowid.clear();
          break;
        }
      }
    } else {
      database_name = symbol + "_" + list_interval[i] + ".db";
      sql_select = SQLiteHandler::SQL_SELECT_BARDATA_ROWID + " where ";
    }

    if (list_rowid_distrank.length() > 0) {
      list_rowid_distrank.removeDuplicates();
      if (t != NULL) sql_select += " and ";
      sql_select += SQLiteHandler::COLUMN_NAME_ROWID + " in(" + list_rowid_distrank.join(",") + ")";
      list_rowid_distrank.clear();
    }

    if (list_rowid.length() > 0) {
      list_rowid.removeDuplicates();
      if (t != NULL) sql_select += " and ";
      sql_select += SQLiteHandler::COLUMN_NAME_IDPARENT + " in(" + list_rowid.join(",") + ")";
      list_rowid.clear();
    }

//    qDebug() << "select (base): " << sql_select;
    QSqlQuery *query = db.execQuery(database_name, sql_select);
    while (query->next()) {
      list_rowid.append(query->value(0).toString());
    }

    delete query;
    if (t != NULL)
      delete t;

    // if there's no result at current resolution then quit
    if (list_rowid.length() == 0)
      break;
  }

//  qDebug() << "result::" << list_rowid.length();
//  for (int i=0;i<list_rowid.length();++i)
//    qDebug() << list_rowid[i];

  if (list_rowid.length() > 0) {
    *out_sql += "rowid in("+ list_rowid.join(",") + ")";
  } else {
    *out_sql = "1=0";
  }
}*/

/*void MainWindow::on_tblData_clicked(const QModelIndex &index) {
  if (enable_developer_mode) {
    int row = index.row();
    QString d = ui->tblData->model()->data(ui->tblData->model()->index(row,0)).toString();
    QString t = ui->tblData->model()->data(ui->tblData->model()->index(row,1)).toString();
    QDate date = QDate::fromString(d,"MM/dd/yyyy");
    QTime time = QTime::fromString(t,"hh:mm");
    QString symbol = ui->cbSymbol->currentData().toString();
    QString interval = filter->get_smallest_interval();
    int threshold_index = ui->cbThreshold->currentData().toInt();

    QVector<PriceLevel> *price = new QVector<PriceLevel>();
    getResistance(symbol, interval, date, time, threshold_index, price);
    getSupport(symbol, interval, date, time, threshold_index, price);

    QString p_interval;
    QDate pd;
    QTime pt;

    while (get_parent_interval(interval, &p_interval)) {
      if (!get_parent_datetime(date, time, symbol, interval, p_interval, pd, pt)) break;
      getResistance(symbol, p_interval, pd, pt, threshold_index, price);
      getSupport(symbol, p_interval, pd, pt, threshold_index, price);
      interval = p_interval;
      date = pd;
      time = pt;
    }

    std::sort(price->begin(),price->end(),compare_price);

    QString tooltip_text = "";
    int price_length = price->size();

    for (int i = 0; i < price_length; ++i) {
      if (i > 0) tooltip_text += "\n";
      tooltip_text += (*price)[i].interval + " " + (*price)[i].Description + ": " + QString::number((*price)[i].price);
    }

    delete price;
    if (tooltip_text.isEmpty())
      tooltip_text = "There is no resistance/support for this datetime";
    QToolTip::showText(QCursor::pos(), tooltip_text);
  }
}*/

void MainWindow::read_matcher_file(const QString &filename, const QString &symbol,const QString &interval) {
  QFile file(filename);

  if (!file.exists())
    return;

  if (file.open(QFile::ReadOnly)) {
    QStringList _datetime;
    QTextStream stream(&file);
    stream.readLine(); // number of rows (discard)
    int series_length = stream.readLine().toInt(); // series length
    while (!stream.atEnd()) {
      _datetime.push_front(stream.readLine());
    }

    file.close();
    _datetime.sort();

    QVector<QDateTime> datetime_series;
    QStringList line;
    int matcher_length = _datetime.length();
    int start_from = 0;
    int idx;
    int N;

    for (int i = 0; i < matcher_length; ++i) {
      line = _datetime[i].split(" ");
      datetime_series = BarDataAdapter::select_series_datetime(
        db.getDatabase(symbol,interval),
        line[0].replace("/","-"),
        line[1].replace("/","-"), series_length);

      N = datetime_series.length();

      for (int j = 0; j < N; ++j) {
        // find index position in QTableView
        idx = qtableview_search_datetime(&start_from,
                datetime_series[j].toString("yyyy/MM/dd"),
                datetime_series[j].toString("HH:mm"));

        // if found its position then highlight that row
//        if (idx > -1) {
//          row_highlight_index.append(idx);
//          row_highlighter.append(new BackgroundColorDelegate());
//          ui->tblData->setItemDelegateForRow(idx, row_highlighter.last());
//          ui->tblData->showRow(idx);
//        }
      }
    }

    int row_count = ui->tblData->model()->rowCount();
    while (start_from < row_count) {
      ui->tblData->hideRow(start_from++);
    }
  } // end if file open
}

int MainWindow::qtableview_search_datetime(int *start_from, const QString &_date, const QString &_time) {
  QAbstractItemModel *_model = ui->tblData->model();
  QString date_value;
  QString time_value;
  int size = ui->tblData->model()->rowCount();

  // align date
  QDate dt = QDate::fromString(_model->data(_model->index(*start_from,0)).toString(), "MM/dd/yyyy");
  date_value = dt.toString("yyyy/MM/dd");

  while (date_value < _date && *start_from < size) {
//    ui->tblData->hideRow(*start_from);
    ui->tblData->setRowHidden(*start_from,true);
    dt = QDate::fromString(_model->data(_model->index(++(*start_from),0)).toString(), "MM/dd/yyyy");
    date_value = dt.toString("yyyy/MM/dd");
  }

  // align time
  time_value = _model->data(_model->index(*start_from,1)).toString();

  while (date_value == _date && time_value < _time && *start_from < size) {
//    ui->tblData->hideRow(*start_from);
    ui->tblData->setRowHidden(*start_from,true);
    time_value = _model->data(_model->index(++(*start_from),1)).toString();
  }

  // exact match
  if (date_value == _date && time_value == _time) {
    *start_from = *start_from + 1;
    return (*start_from - 1);
  }

  return -1;
}

void MainWindow::clear_row_highlighter() {
  int row_count = ui->tblData->model()->rowCount();
  int start_from = 0;
  while (start_from < row_count) {
    ui->tblData->showRow(start_from++);
  }

//  int size = row_highlighter.length();
//  QAbstractItemDelegate *item_delegate = ui->tblData->itemDelegate();
//  for (int i = 0; i < size; ++i) {
//    ui->tblData->setItemDelegateForRow(row_highlight_index[i], item_delegate);
//    delete row_highlighter[i];
//  }
//  row_highlighter.clear();
//  row_highlight_index.clear();
}

/** Developer function */
/*void MainWindow::search_resistance_support() {
  QString symbol = ui->cbSymbol->currentData().toString();
  QString interval = filter->getInterval(0);
  QString sql_select;

  if (enable_developer_mode) {
    sql_select = "select rowid,* from " + ((search_option == "res")?
      SQLiteHandler::TABLE_NAME_RESISTANCE: SQLiteHandler::TABLE_NAME_SUPPORT);
  } else {
    sql_select = (search_option == "res")?
      SQLiteHandler::SQL_VIEW1_RESISTANCE_V1: SQLiteHandler::SQL_VIEW1_SUPPORT_V1;
  }

  QString database_name= db.getDatabaseName(symbol,interval);
  model = db.getSqlModel(database_name, sql_select);
  ui->tblData->setModel(model);
  ui->tblData->resizeColumnsToContents();
  ui->tblData->show();
}*/
