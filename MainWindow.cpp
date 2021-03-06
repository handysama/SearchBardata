#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const int &id, QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->btnRefresh->setAutoRepeat(false);
  ui->btnSearch->setAutoRepeat(false);
  ui->lblUpdateStatus->setText("");
  ui->lblUpdateStatus->hide();
  ui->tblData->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tblData_5min->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tblData_60min->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tblData_Daily->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tblData_Weekly->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tblData_Monthly->setEditTriggers(QAbstractItemView::NoEditTriggers);

  SBAR_ID = id;
  enable_sync_result = true;
  sqlitedb.set_application_id(SBAR_ID);
  tid_macd = 1;
  tid_rsi = 1;
  tid_slowk = 1;
  tid_slowd = 1;
  tid_cgf = 0;
  tid_clf = 0;
  tid_cgs = 0;
  tid_cls = 0;

  // table widget for search condition
  filter = new FilterOptionFactory(ui->tableWidget);

  // read config
  initialize(true);

  set_tab_widget_interval(WEIGHT_WEEKLY);
  on_tabWidget_currentChanged(0);

  QPalette pal = ui->btnSearch->palette();
  switch ( id ) {
    case 0: pal.setColor(QPalette::Button, QColor(Qt::yellow)); break;
    case 1: pal.setColor(QPalette::Button, QColor(Qt::cyan)); break;
    case 2: pal.setColor(QPalette::Button, QColor(Qt::green)); break;
    case 3: pal.setColor(QPalette::Button, QColor(Qt::magenta)); break;
    case 4: pal.setColor(QPalette::Button, QColor(Qt::red)); break;
    default: pal.setColor(QPalette::Button, QColor(Qt::black)); break;
  }

  ui->btnSearch->setAutoFillBackground(true);
  ui->btnSearch->setPalette(pal);
  ui->btnSearch->update();

  // status bar
  ui->statusBar->addPermanentWidget(&label_query_result, 100);
  ui->statusBar->addPermanentWidget(&label_threshold_params);
//  ui->statusBar->addPermanentWidget(&label_tab_id);

  label_tab_id.setText("Tab # " + QString::number(SBAR_ID + 1));
  label_threshold_params.setText("(MACD: 0/0), (RSI: 30/70), (SlowK: 20/80), (SlowD: 20/80)");

  // thread pool
  thread_pool->setMaxThreadCount(QThread::idealThreadCount());
  thread_pool->setExpiryTimeout(300000);

//  MySqlConnector my;
//  qDebug() << "mysqlcon" << my.connectDatabase();
//  qDebug() << my.getColumnNames("dummy");

//  DailySupport ds;
//  QVector<QDate> m_date;
//  QVector<QTime> m_time;
//  QVector<double> m_low;
//  ds.prepare_data( sqlitedb.getDatabase( SQLiteHandler::getDatabaseName("@NQ", WEIGHT_DAILY)) );
//  qDebug() << "\n\n## --- Calculate";
//  // 0 : 2015-07-08
//  // 10 : 2015-06-23
//  ds.calculate_daily_support(&m_date, &m_time, &m_low, 0, 250, 2, 2, 2);
}

MainWindow::~MainWindow() {
  thread_pool->clear();
  delete filter;
  delete srmodel;
  delete srmodel_monthly;
  delete srmodel_weekly;
  delete srmodel_daily;
  delete srmodel_60min;
  delete srmodel_5min;
  delete model_query;
  delete model_query_daily;
  delete model_query_weekly;
  delete model_query_monthly;
  delete model_query_60min;
  delete model_query_5min;
  SQLiteHandler::removeDatabase(&model_database);
  delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e) {
  int t_width = ui->centralWidget->width() - 19;
  int t_height = ui->centralWidget->height() - 277;
  ui->tabWidget->setGeometry(10, 277, t_width, t_height);

  t_width = ui->tabWidget->width() - 5;
  t_height = ui->tabWidget->height() - 25;
  ui->tblData->setGeometry(0, 0, t_width, t_height);
  ui->tblData_60min->setGeometry(0, 0, t_width, t_height);
  ui->tblData_Daily->setGeometry(0, 0, t_width, t_height);
  ui->tblData_Weekly->setGeometry(0, 0, t_width, t_height);
  ui->tblData_Monthly->setGeometry(0, 0, t_width, t_height);
  QMainWindow::resizeEvent(e);
}

void MainWindow::initialize(bool update_interface = true) {
  XmlConfigHandler *config = XmlConfigHandler::get_instance();

  input_dir = config->get_input_dir();
  result_dir = config->get_result_dir();
  database_dir = config->get_database_dir();
  last_update = config->get_last_updated();
  list_symbol = config->get_list_symbol();
  list_threshold = config->get_list_threshold();
  list_interval_weight = config->get_interval_weight();
  list_interval_threshold = config->get_interval_threshold();
  enable_parent_indexing = config->is_enable_parent_indexing();
  enable_update_support_resistance = config->is_enable_update_support_resistance();
  enable_update_histogram = config->is_enable_update_histogram();
  enable_developer_mode = config->is_enable_developer_mode();
  t_macd = config->get_macd_threshold();
  t_rsi = config->get_rsi_threshold();
  t_slowk = config->get_slowk_threshold();
  t_slowd = config->get_slowd_threshold();
  t_cgf = config->get_cgf_threshold();
  t_clf = config->get_clf_threshold();
  t_cgs = config->get_cgs_threshold();
  t_cls = config->get_cls_threshold();

  // set database path
  sqlitedb.set_database_path(database_dir);

  // set config for filter option factory
  if (filter != NULL) {
    filter->set_developer_mode(enable_developer_mode);
  }

  // set last update
  if (!last_update.isNull()) {
    ui->lblLastUpdate->setText(last_update.toString("MM/dd/yy, hh:mm"));
  } else {
    ui->lblLastUpdate->setText("Not set");
  }

  if (update_interface) {
    // set symbol
    int symbol_curr_index = ui->cbSymbol->currentIndex();
    ui->cbSymbol->clear();

    for (int i = 0; i < list_symbol.size(); ++i) {
      ui->cbSymbol->addItem(list_symbol[i], list_symbol[i]);
    }

    if (symbol_curr_index > -1)
      ui->cbSymbol->setCurrentIndex(symbol_curr_index);
  }
}

void MainWindow::on_cbSymbol_currentIndexChanged(const int &index) {
  set_data_range_label();
  set_threshold_parameters(ui->cbSymbol->itemData(index).toString());
  on_tabWidget_currentChanged(0);
}

void MainWindow::on_tabWidget_currentChanged(const int &index) {
  if (index < 0 || index >= vec_label_string.count()) return;
  label_query_result.setText(vec_label_string[index]);

  QString symbol = ui->cbSymbol->currentText();
  QString interval_name = ui->tabWidget->tabText(index);
  QString tab_text;
  int vscroll_pos;
//  int hscroll_pos;

  if (prevtab_idx == 0) {
    vscroll_pos = ui->tblData->verticalScrollBar()->value();
//    hscroll_pos = ui->tblData->horizontalScrollBar()->value();
  } else {
    tab_text = ui->tabWidget->tabText(prevtab_idx);
    if (tab_text == "Daily") {
      vscroll_pos = ui->tblData_Daily->verticalScrollBar()->value();
//      hscroll_pos = ui->tblData_Daily->horizontalScrollBar()->value();
    }
    else if (tab_text == "Weekly") {
      vscroll_pos = ui->tblData_Weekly->verticalScrollBar()->value();
//      hscroll_pos = ui->tblData_Weekly->horizontalScrollBar()->value();
    }
    else if (tab_text == "Monthly") {
      vscroll_pos = ui->tblData_Monthly->verticalScrollBar()->value();
//      hscroll_pos = ui->tblData_Monthly->horizontalScrollBar()->value();
    }
    else if (tab_text == "60min") {
      vscroll_pos = ui->tblData_60min->verticalScrollBar()->value();
//      hscroll_pos = ui->tblData_60min->horizontalScrollBar()->value();
    }
    else if (tab_text == "5min") {
      vscroll_pos = ui->tblData_5min->verticalScrollBar()->value();
//      hscroll_pos = ui->tblData_5min->horizontalScrollBar()->value();
    }
    else {
      vscroll_pos = ui->tblData->verticalScrollBar()->value();
//      hscroll_pos = ui->tblData->horizontalScrollBar()->value();
    }
  }

  if (index == 0) {
    ui->tblData->verticalScrollBar()->setValue(vscroll_pos);
//    ui->tblData->horizontalScrollBar()->setValue(hscroll_pos);
  } else {
    tab_text = ui->tabWidget->tabText(index);
    if (tab_text == "Daily") {
      ui->tblData_Daily->verticalScrollBar()->setValue(vscroll_pos);
//      ui->tblData_Daily->horizontalScrollBar()->setValue(hscroll_pos);
    }
    else if (tab_text == "Weekly") {
      ui->tblData_Weekly->verticalScrollBar()->setValue(vscroll_pos);
//      ui->tblData_Weekly->horizontalScrollBar()->setValue(hscroll_pos);
    }
    else if (tab_text == "Monthly") {
      ui->tblData_Monthly->verticalScrollBar()->setValue(vscroll_pos);
//      ui->tblData_Monthly->horizontalScrollBar()->setValue(hscroll_pos);
    }
    else if (tab_text == "60min") {
      ui->tblData_60min->verticalScrollBar()->setValue(vscroll_pos);
//      ui->tblData_60min->horizontalScrollBar()->setValue(hscroll_pos);
    }
    else if (tab_text == "5min") {
      ui->tblData_5min->verticalScrollBar()->setValue(vscroll_pos);
//      ui->tblData_5min->horizontalScrollBar()->setValue(hscroll_pos);
    }
    else {
      ui->tblData->verticalScrollBar()->setValue(vscroll_pos);
//      ui->tblData->horizontalScrollBar()->setValue(hscroll_pos);
    }
  }

  prevtab_idx = index;

  if (!symbol.isEmpty()) {
    double max_v = BarDataAdapter::getMaxValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_UPTAIL);
    double min_v = BarDataAdapter::getMinValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_UPTAIL);
    ui->lblUptail->setText(QString::number(min_v) + " <b>-</b> " + QString::number(max_v));

    max_v = BarDataAdapter::getMaxValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_DOWNTAIL);
    min_v = BarDataAdapter::getMinValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_DOWNTAIL);
    ui->lblDowntail->setText(QString::number(min_v) + " <b>-</b> " + QString::number(max_v));

    max_v = BarDataAdapter::getMaxValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_BODY);
    min_v = BarDataAdapter::getMinValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_BODY);
    ui->lblBody->setText(QString::number(min_v) + " <b>-</b> " + QString::number(max_v));

    max_v = BarDataAdapter::getMaxValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_TOTALLENGTH);
    min_v = BarDataAdapter::getMinValue(sqlitedb.getDatabase(symbol, interval_name), SQLiteHandler::COLUMN_NAME_N_TOTALLENGTH);
    ui->lblLength->setText(QString::number(min_v) + " <b>-</b> " + QString::number(max_v));

    ui->groupBox_2->setTitle(interval_name + " Candle");
  }
}

void MainWindow::on_btnClearResult_clicked() {
  QStringList nameFilter("*." + QString::number(SBAR_ID) + ".sbar");
  QDir dir = result_dir;
  QStringList files = dir.entryList(nameFilter);

  for (int i = 0; i < files.size(); ++i) {
    QFile::remove(result_dir + "/" + files[i]);
  }

  clear_model();
}

void MainWindow::on_btnRefresh_clicked() {
  // disable refresh button
  // button will be enable, once all process finished
  emit start_updating_database();
  enable_components(false);
  clear_model();

  // reload config
  initialize(false);

  ui->lblUpdateStatus->setText("");
  ui->lblUpdateStatus->show();
  label_query_result.setText("Please wait database is updating. Don't close the application.");
  QCoreApplication::processEvents();

  QTime time; time.start();
  QQueue<QThread*> queue;
  QString symbol;
  const int symbol_length = list_symbol.length();
  int max_thread = QThread::idealThreadCount();
  active_thread = 0;

  // label status
  QString msg_loading_file = "(1/4) Loading input text files";
  QString msg_update_index = "(2/4) Indexing database";
  QString msg_update_test_point = "(3/4) Calculating resistance and support";
  QString msg_update_distfs = "(4/4) Calculating Histogram";
  QString last_message_update = "";
  QString dot_animation[5] = {".","..","...","....","....."};
  int counter = 0;
  int dot_idx = 0;

  // set last rowid (before load new data)
  prepare_database_last_rowid();

  // load raw text files
  QStringList nameFilter("*.txt");
  QDir dir = input_dir;
  QStringList files = dir.entryList(nameFilter);
  QString databaseName;
  QVector<int> load_file_by_interval;
  QVector<int> update_parent_by_weight;
  QVector<int> update_resistance_support_by_threshold;
  int update_histogram = 0;
  IntervalWeight w = WEIGHT_MONTHLY;

  // new: group by interval
  while (w != WEIGHT_INVALID) {
    int temp = queue.length();

    for (int i = 0; i < files.size();) {
      QString m_symbol = "";
      IntervalWeight interval = WEIGHT_INVALID;
      databaseName = parse_filename(files[i], &m_symbol, &interval);

      if (interval == w) {
        if (!m_symbol.isEmpty()) {
          FileLoader *thread = new FileLoader(input_dir + "/" + files[i], database_dir, sqlitedb.getCloneDatabase(databaseName), m_symbol, interval);
          connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
          queue.enqueue(thread);
        }

        files.removeAt(i);
        continue;
      }

      ++i;
    }

    if (temp < queue.length()) {
      load_file_by_interval.append(queue.length() - temp);
    }

    w = SQLiteHandler::get_child_interval(w);
  }

  //
  // File load
  //
  last_message_update = msg_loading_file;

  while (true) {
    QCoreApplication::processEvents();

    if (active_thread == 0 && load_file_by_interval.isEmpty()) break;
//    if (active_thread == 0 && load_task_number == 0) break;

    if (++counter >= 100000) {
      dot_idx = (dot_idx + 1) % 5;
      ui->lblUpdateStatus->setText(last_message_update + dot_animation[dot_idx]);
      counter = 0;
    }

    if (active_thread < max_thread) {
      if (!load_file_by_interval.isEmpty()) {
        // wait until indexing (by weight) finish
        if (active_thread > 0 && load_file_by_interval[0] == 0)
          continue;

        // remove finished indexing
        if (load_file_by_interval[0] == 0)
          load_file_by_interval.removeFirst();

        if (!load_file_by_interval.isEmpty()) {
          queue.dequeue()->start();
          start_updating_data();
          --load_file_by_interval[0];
        }
      }
    }

    // old
//    if (active_thread < max_thread) {
//      if (load_task_number > 0) {
//        queue.dequeue()->start();
//        start_updating_data();
//        last_message_update = msg_loading_file;
//        --load_task_number;
//      }
//    }
  }

  qDebug() << "------------------ end of file load";

  prepare_database_update_flag();

  if (enable_parent_indexing) {
    ParentIndexThread *thread;
    int temp = queue.length();
    bool force_update = false;

    // Daily
    for (int i = 0; i < symbol_length; ++i) {
      symbol = list_symbol[i];
      databaseName = SQLiteHandler::getDatabaseName(symbol, WEIGHT_DAILY);
      if (force_update || database_update_flag[databaseName]) {
        thread = new ParentIndexThread(sqlitedb.getCloneDatabase(databaseName), database_dir, symbol, WEIGHT_WEEKLY);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }
    }

    if (temp < queue.length()) {
      update_parent_by_weight.append(queue.length() - temp);
      temp = queue.length();
    }

    // Weekly
    for (int i = 0; i < symbol_length; ++i) {
      symbol = list_symbol[i];
      databaseName = SQLiteHandler::getDatabaseName(symbol, WEIGHT_WEEKLY);
      if (force_update || database_update_flag[databaseName]) {
        thread = new ParentIndexThread(sqlitedb.getCloneDatabase(databaseName), database_dir, symbol, WEIGHT_MONTHLY);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }
    }

    if (temp < queue.length()) {
      update_parent_by_weight.append(queue.length() - temp);
      temp = queue.length();
    }

    // 60min
    for (int i = 0; i < symbol_length; ++i) {
      symbol = list_symbol[i];
      databaseName = SQLiteHandler::getDatabaseName(symbol, WEIGHT_60MIN);
      if (force_update || database_update_flag[databaseName]) {
        thread = new ParentIndexThread(sqlitedb.getCloneDatabase(databaseName), database_dir, symbol, WEIGHT_DAILY);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }
    }

    if (temp < queue.length()) {
      update_parent_by_weight.append(queue.length() - temp);
      temp = queue.length();
    }

    // 5min
    for (int i = 0; i < symbol_length; ++i) {
      symbol = list_symbol[i];
      databaseName = SQLiteHandler::getDatabaseName(symbol, WEIGHT_5MIN);
      if (force_update || database_update_flag[databaseName]) {
        thread = new ParentIndexThread(sqlitedb.getCloneDatabase(databaseName), database_dir, symbol, WEIGHT_60MIN);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }
    }

    if (temp <queue.length()) {
      update_parent_by_weight.append(queue.length() - temp);
      temp = queue.length();
    }

    // 1min
    for (int i = 0; i < symbol_length; ++i) {
      symbol = list_symbol[i];
      databaseName = SQLiteHandler::getDatabaseName(symbol, WEIGHT_1MIN);
      if (force_update || database_update_flag[databaseName]) {
        thread = new ParentIndexThread(sqlitedb.getCloneDatabase(databaseName), database_dir, symbol, WEIGHT_5MIN);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }
    }

    if (temp <queue.length()) {
      update_parent_by_weight.append(queue.length() - temp);
      temp = queue.length();
    }
  }

  if (enable_update_support_resistance) {
    QVector<XmlConfigHandler::t_sr_threshold> vec;
    ResistanceSupportUpdater *thread;
    double b_value;
    double threshold;
    int id_threshold;
    int last_rowid;
    int vec_length = 5;
    int process_count = 0;
    bool doRecalculate;

    for (int j = 0; j < vec_length; ++j) {

      for (IntervalWeight m_interval = WEIGHT_MONTHLY;
           m_interval != WEIGHT_INVALID;
           m_interval = SQLiteHandler::get_child_interval(m_interval)) {
        process_count = 0;

        for (int i = 0; i < symbol_length; ++i) {
          symbol = list_symbol[i];
          vec = list_threshold[symbol];
          threshold = vec[j].test_point; // 2
          b_value = vec[j].break_threshold; // breakpoint
          id_threshold = vec[j].t_id; // id_threshold
          doRecalculate = vec[j].recalculate; // recalculate flag

          if (doRecalculate || (b_value > 0 && threshold > 0)) {
            if (doRecalculate || database_update_flag[SQLiteHandler::getDatabaseName(symbol, m_interval)]) {

              if (m_interval >= WEIGHT_DAILY) {
                last_rowid = database_last_rowid[sqlitedb.getDatabaseName(symbol, m_interval)];
              } else {
                last_rowid = 0;
              }

              thread = new ResistanceSupportUpdater(
                            sqlitedb.getCloneDatabase(symbol,m_interval),
                            database_dir, symbol, m_interval, b_value, threshold, id_threshold, last_rowid);

              thread->set_recalculate(doRecalculate);
              connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
              queue.enqueue(thread);
              ++process_count;
            }
          }
        }

        if (process_count > 0) {
          update_resistance_support_by_threshold.push_back(process_count);
        }
      }
    }
  }

  if (enable_update_histogram) {
    HistogramUpdater *thread;
    update_histogram = queue.length();
    bool force_update = false;
    bool doRecalculate = false; // TODO: flag for recalculate thres

    for (int i = 0; i < symbol_length; ++i) {
      symbol = list_symbol[i];

      databaseName = SQLiteHandler::getDatabaseName(symbol,WEIGHT_MONTHLY);
      if (force_update || doRecalculate || database_update_flag[databaseName]) {
        thread = new HistogramUpdater(sqlitedb.getCloneDatabase(databaseName), 2001, symbol);
        thread->set_recalculate_threshold(doRecalculate);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }

      databaseName = SQLiteHandler::getDatabaseName(symbol,WEIGHT_WEEKLY);
      if (force_update || doRecalculate || database_update_flag[databaseName]) {
        thread = new HistogramUpdater(sqlitedb.getCloneDatabase(databaseName), 2001, symbol);
        thread->set_recalculate_threshold(doRecalculate);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }

      databaseName = SQLiteHandler::getDatabaseName(symbol,WEIGHT_DAILY);
      if (force_update || doRecalculate || database_update_flag[databaseName]) {
        thread = new HistogramUpdater(sqlitedb.getCloneDatabase(databaseName), 2001, symbol);
        thread->set_recalculate_threshold(doRecalculate);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }

      databaseName = SQLiteHandler::getDatabaseName(symbol,WEIGHT_60MIN);
      if (force_update || doRecalculate || database_update_flag[databaseName]) {
        thread = new HistogramUpdater(sqlitedb.getCloneDatabase(databaseName), 2001, symbol);
        thread->set_recalculate_threshold(doRecalculate);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }

      databaseName = SQLiteHandler::getDatabaseName(symbol,WEIGHT_5MIN);
      if (force_update || doRecalculate || database_update_flag[databaseName]) {
        thread = new HistogramUpdater(sqlitedb.getCloneDatabase(databaseName), 2011, symbol);
        thread->set_recalculate_threshold(doRecalculate);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }

      databaseName = SQLiteHandler::getDatabaseName(symbol,WEIGHT_1MIN);
      if (force_update || doRecalculate || database_update_flag[databaseName]) {
        thread = new HistogramUpdater(sqlitedb.getCloneDatabase(databaseName), 2011, symbol);
        thread->set_recalculate_threshold(doRecalculate);
        connect(thread, SIGNAL(finished()), this, SLOT(finish_updating_data()));
        queue.enqueue(thread);
      }
    }

    update_histogram = queue.length() - update_histogram;
  }

  //
  // Indexing database
  //
  last_message_update = msg_update_index;
  while (true) {
    QCoreApplication::processEvents();
    if (active_thread == 0 && update_parent_by_weight.isEmpty()) break;

    if (++counter >= 100000) {
      dot_idx = (dot_idx + 1) % 5;
      ui->lblUpdateStatus->setText(last_message_update + dot_animation[dot_idx]);
      counter = 0;
    }

    if (active_thread < max_thread) {
      if (!update_parent_by_weight.isEmpty()) {
        // wait until indexing (by weight) finish
        if (active_thread > 0 && update_parent_by_weight[0] == 0)
          continue;

        // remove finished indexing
        if (update_parent_by_weight[0] == 0)
          update_parent_by_weight.removeFirst();

        if (!update_parent_by_weight.isEmpty()) {
          queue.dequeue()->start();
          start_updating_data();
          --update_parent_by_weight[0];
        }
      }
    }
  }

  qDebug() << "------------------ end of indexing";

  //
  // Resistance & Support
  //
  last_message_update = msg_update_test_point;
  while (true) {
    QCoreApplication::processEvents();
    if (active_thread == 0 && update_resistance_support_by_threshold.isEmpty()) break;

    if (++counter >= 100000) {
      dot_idx = (dot_idx + 1) % 5;
      ui->lblUpdateStatus->setText(last_message_update + dot_animation[dot_idx]);
      counter = 0;
    }

    if (active_thread < max_thread) {
      if (!update_resistance_support_by_threshold.isEmpty()) {
        if (active_thread > 0 && update_resistance_support_by_threshold[0] == 0) {
          continue;
        }

        if (update_resistance_support_by_threshold[0] == 0)
          update_resistance_support_by_threshold.removeFirst();

        if (!update_resistance_support_by_threshold.isEmpty()) {
          queue.dequeue()->start();
          start_updating_data();        
          --update_resistance_support_by_threshold[0];
        }
      }
    }
  }

  qDebug() << "------------------ end of resistance & support";

  //
  // Histogram Rank
  //
  last_message_update = msg_update_distfs;
  while (true) {
    QCoreApplication::processEvents();
    if (active_thread == 0 && update_histogram == 0) break;

    if (++counter >= 100000) {
      dot_idx = (dot_idx + 1) % 5;
      ui->lblUpdateStatus->setText(last_message_update + dot_animation[dot_idx]);
      counter = 0;
    }

    if (active_thread < max_thread) {
      if (update_histogram > 0) {
        queue.dequeue()->start();
        start_updating_data();
        --update_histogram;
      }
    }
  }

  qDebug() << "------------------ end of histogram rank";
  qDebug() << "Time elapsed:" << time.elapsed()/1000.0 << "sec(s), thread:" << active_thread << "," << queue.isEmpty();

  ui->btnRefresh->setEnabled(true);
  ui->lblUpdateStatus->hide();
  last_update = QDateTime::currentDateTime();

  // refresh last update label
  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  config->set_last_updated(last_update);
  config->reset_recalculate_threshold();
  config->write_config();
  initialize(true);
  enable_components(true);
  ui->btnSearch->animateClick(0);
  emit finish_updating_database();
}


//
// Search function
//
QString case_when_column_null(const QString &column_name, const int &precision) {
  return "case when " + column_name + " is not null then printf(\"%.0" + QString::number(precision) + "f\"," + column_name + ") else '' end";
}

QString printf_number(const QString &column_name, const int &precision) {
  return "printf(\"%.0" + QString::number(precision) + "f\"," + column_name + ")";
}

QString MainWindow::sql_string_builder(FilterOptionFactory *table,
                                       const QString &symbol,
                                       QStringList *databaseNames,
                                       QStringList *databaseAlias,
                                       QStringList *projection_name,
                                       const QStringList &matcherResult,
                                       const IntervalWeight &projection_interval,
                                       const bool &enable_projection,
                                       const int &id_threshold,
                                       const double &breakThreshold,
                                       const double &avgThreshold,
                                       const int &macd_id,
                                       const int &rsi_id,
                                       const int &slowk_id,
                                       const int &slowd_id,
                                       const int &cgf_id,
                                       const int &clf_id,
                                       const int &cgs_id,
                                       const int &cls_id) {
  QVector<bool> SRjoin;
  QStringList projection;
  QStringList SRjoinAlias;
  const QString fastavg_name = BarDataAdapter::avg_name_by_interval(projection_interval, FASTAVG_LENGTH);
  const QString slowavg_name = BarDataAdapter::avg_name_by_interval(projection_interval, SLOWAVG_LENGTH);
  const QString main_alias = "A";
  const QString main_interval = table->get_smallest_interval();

  QString whereArgs = "";
  QString A_ = "A.";
  QString SRAlias = "";
  QString canonical_alias;
  QString intersect_type;
  QString _threshold;
  QString _value1, _value2;
  QString _column1, _column2;
  QString _rank1, _rank2;

  const int main_intervalweight = table->get_smallest_interval_weight();
  int N = table->length();
  int decimal_precision = (symbol == "@AD" || symbol == "@EC" || symbol == "@JY")? 4 : 2;
  bool enable_developer_mode = false;
  IntervalWeight base_interval = table->get_smallest_interval_weight_v2();
  IntervalWeight w = WEIGHT_LARGEST;

  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  QVector<XmlConfigHandler::t_threshold_1> c_macd = config->get_macd_threshold();
  QVector<XmlConfigHandler::t_threshold_1> c_rsi = config->get_rsi_threshold();
  QVector<XmlConfigHandler::t_threshold_1> c_slowk = config->get_slowk_threshold();
  QVector<XmlConfigHandler::t_threshold_1> c_slowd = config->get_slowd_threshold();
  QVector<XmlConfigHandler::t_threshold_2> c_cgf = config->get_cgf_threshold(symbol);
  QVector<XmlConfigHandler::t_threshold_2> c_clf = config->get_clf_threshold(symbol);
  QVector<XmlConfigHandler::t_threshold_2> c_cgs = config->get_cgs_threshold(symbol);
  QVector<XmlConfigHandler::t_threshold_2> c_cls = config->get_cls_threshold(symbol);

  //
  // Index[0] -> main connection
  //
  databaseNames->append(sqlitedb.getDatabaseName(symbol, table->get_smallest_interval_weight_v2()));
  databaseAlias->append("");

  if (!enable_developer_mode) {
    while (w != base_interval) {
      databaseNames->append(sqlitedb.getDatabasePath(symbol, w));
      databaseAlias->append("db" + SQLiteHandler::get_interval_name(w));
      w = SQLiteHandler::get_child_interval(w);
    }

    if (enable_projection) {
      if (projection_interval == main_intervalweight) {
        A_ = "A.";
      } else {
        switch (projection_interval) {
          case WEIGHT_DAILY: A_ = "R1."; break;
          case WEIGHT_WEEKLY: A_ = "R2."; break;
          case WEIGHT_MONTHLY: A_ = "R3."; break;
          case WEIGHT_60MIN: A_ = "R4."; break;
          case WEIGHT_5MIN: A_ = "R5."; break;
          default: A_ = "A.";
        }
      }
    }

    //
    // Show value as current tab interval (projection)
    //
    QString A__;
    if (projection_interval == main_intervalweight) {
      A__ = "A.";
    } else {
      switch (projection_interval) {
        case WEIGHT_DAILY: A__ = "R1."; break;
        case WEIGHT_WEEKLY: A__ = "R2."; break;
        case WEIGHT_MONTHLY: A__ = "R3."; break;
        case WEIGHT_60MIN: A__ = "R4."; break;
        case WEIGHT_5MIN: A__ = "R5."; break;
        default: A__ = "A.";
      }
    }

    projection.append(A_ + SQLiteHandler::COLUMN_NAME_ROWID);
//    projection.append("strftime('%m/%d/%Y',A." + SQLiteHandler::COLUMN_NAME_DATE + ") AS 'Date'");
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_DATE);
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_TIME);
    // OHLC (base interval)
    projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_OPEN, decimal_precision));
    projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_HIGH, decimal_precision));
    projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_LOW, decimal_precision));
    projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_CLOSE, decimal_precision));
    // OHLC (projection interval)
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_OPEN, decimal_precision));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_HIGH, decimal_precision));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_LOW, decimal_precision));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CLOSE, decimal_precision));
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_PREV_BARCOLOR);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_VOLUME);

    projection_name->append(SQLiteHandler::COLUMN_NAME_ROWID);
    projection_name->append(SQLiteHandler::COLUMN_NAME_DATE);
    projection_name->append(SQLiteHandler::COLUMN_NAME_TIME);
    projection_name->append(SQLiteHandler::COLUMN_NAME_OPEN);
    projection_name->append(SQLiteHandler::COLUMN_NAME_HIGH);
    projection_name->append(SQLiteHandler::COLUMN_NAME_LOW);
    projection_name->append(SQLiteHandler::COLUMN_NAME_CLOSE);
    projection_name->append("Open");
    projection_name->append("High");
    projection_name->append("Low");
    projection_name->append("Close");
    projection_name->append("PrevBar");
    projection_name->append("Volume");

    _threshold = "_" + QString::number(macd_id);
    _value1 = SQLiteHandler::COLUMN_NAME_MACD_VALUE1 + _threshold;
    _value2 = SQLiteHandler::COLUMN_NAME_MACD_VALUE2 + _threshold;
    _column1 = "MACD " + c_macd[macd_id-1].operator1 + " " + QString::number(c_macd[macd_id-1].value1);
    _column2 = "MACD " + c_macd[macd_id-1].operator2 + " " + QString::number(c_macd[macd_id-1].value2);
    QString macd_r1 = SQLiteHandler::COLUMN_NAME_MACD_RANK1 + _threshold;
    QString macd_r2 = SQLiteHandler::COLUMN_NAME_MACD_RANK2 + _threshold;

    projection.append(case_when_column_null(A__+SQLiteHandler::COLUMN_NAME_MACD, 6));
    projection.append(case_when_column_null(A__+SQLiteHandler::COLUMN_NAME_MACD_RANK, 4));
    projection.append(A__ + _value1);
    projection.append(case_when_column_null(A__ + macd_r1, 4));
    projection.append(A__ + _value2);
    projection.append(case_when_column_null(A__ + macd_r2, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_MACDAVG, 6));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_MACDDIFF, 6));

    projection_name->append("MACD");
    projection_name->append("MACD (R)");
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");
    projection_name->append(_column2);
    projection_name->append(_column2 + " (R)");
    projection_name->append("MACDAvg");
    projection_name->append("MACDDiff");

//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_MACD_G0);
//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_MACD_L0);
//    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_MACD_G0_RANK, 4));
//    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_MACD_L0_RANK, 4));
//    projection_name->append("MACD > 0");
//    projection_name->append("MACD < 0");
//    projection_name->append("MACD > 0 (R)");
//    projection_name->append("MACD < 0 (R)");

    _threshold = "_" + QString::number(rsi_id);
    _column1 = "RSI " + c_rsi[rsi_id-1].operator1 + " " + QString::number(c_rsi[rsi_id-1].value1);
    _column2 = "RSI " + c_rsi[rsi_id-1].operator2 + " " + QString::number(c_rsi[rsi_id-1].value2);

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_RSI, 2));
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RSI_VALUE1 + _threshold);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RSI_RANK1 + _threshold);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RSI_VALUE2 + _threshold);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RSI_RANK2 + _threshold);

    projection_name->append("RSI");
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");
    projection_name->append(_column2);
    projection_name->append(_column2 + " (R)");

//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RSI_G70);
//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RSI_L30);
//    projection_name->append("RSI > 70");
//    projection_name->append("RSI < 30");


    _threshold = "_" + QString::number(slowk_id);
    _column1 = "SlowK " + c_slowk[slowk_id-1].operator1 + " " + QString::number(c_slowk[slowk_id-1].value1);
    _column2 = "SlowK " + c_slowk[slowk_id-1].operator2 + " " + QString::number(c_slowk[slowk_id-1].value2);

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_SLOWK, 2));
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWK_VALUE1 + _threshold);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWK_RANK1 + _threshold);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWK_VALUE2 + _threshold);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWK_RANK2 + _threshold);

    projection_name->append("SlowK");
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");
    projection_name->append(_column2);
    projection_name->append(_column2 + " (R)");

//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWK_G80);
//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWK_L20);
//    projection_name->append("SlowK > 80");
//    projection_name->append("SlowK < 20");

    _threshold = "_" + QString::number(slowd_id);
    _value1 = SQLiteHandler::COLUMN_NAME_SLOWD_VALUE1 + _threshold;
    _value2 = SQLiteHandler::COLUMN_NAME_SLOWD_VALUE2 + _threshold;
    _rank1 = SQLiteHandler::COLUMN_NAME_SLOWD_RANK1 + _threshold;
    _rank2 = SQLiteHandler::COLUMN_NAME_SLOWD_RANK2 + _threshold;
    _column1 = "SlowD " + c_slowd[slowd_id-1].operator1 + " " + QString::number(c_slowd[slowd_id-1].value1);
    _column2 = "SlowD " + c_slowd[slowd_id-1].operator2 + " " + QString::number(c_slowd[slowd_id-1].value2);

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_SLOWD, 2));
    projection.append(A__ + _value1);
    projection.append(A__ + _rank1);
    projection.append(A__ + _value2);
    projection.append(A__ + _rank2);

    projection_name->append("SlowD");
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");
    projection_name->append(_column2);
    projection_name->append(_column2 + " (R)");

//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWD_G80);
//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SLOWD_L20);
//    projection_name->append("SlowD > 80");
//    projection_name->append("SlowD < 20");

    if (base_interval < WEIGHT_DAILY && projection_interval == WEIGHT_DAILY) {
      projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_PREV_DAILY_ATR, 4));
      projection_name->append("PrevATR");
    }

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_ATR, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_ATR_RANK, 4));
    projection_name->append("ATR");
    projection_name->append("ATR (R)");

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_FASTAVG, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_SLOWAVG, 4));
    projection_name->append(fastavg_name);
    projection_name->append(slowavg_name);

    // Day-10 , Day-50 realtime calculation for intraday
    if (base_interval < WEIGHT_DAILY && projection_interval == WEIGHT_DAILY) {
      projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_DAY10, 4));
      projection.append(case_when_column_null(A_ + SQLiteHandler::COLUMN_NAME_DAY50, 4));
      projection_name->append("Day-10 (F)");
      projection_name->append("Day-50 (F)");
    }

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_FASTAVG_SLOPE, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_SLOWAVG_SLOPE, 4));
    projection_name->append("F-Slope");
    projection_name->append("S-Slope");

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTOF, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTOF_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTHF, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTHF_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTLF, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTLF_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCF, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCF_RANK, 4));
    projection_name->append("Dist(O-F)");
    projection_name->append("Dist(O-F) (R)");
    projection_name->append("Dist(H-F)");
    projection_name->append("Dist(H-F) (R)");
    projection_name->append("Dist(L-F)");
    projection_name->append("Dist(L-F) (R)");
    projection_name->append("Dist(C-F)");
    projection_name->append("Dist(C-F) (R)");

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTOS, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTOS_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTHS, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTHS_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTLS, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTLS_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCS, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCS_RANK, 4));
    projection_name->append("Dist(O-S)");
    projection_name->append("Dist(O-S) (R)");
    projection_name->append("Dist(H-S)");
    projection_name->append("Dist(H-S) (R)");
    projection_name->append("Dist(L-S)");
    projection_name->append("Dist(L-S) (R)");
    projection_name->append("Dist(C-S)");
    projection_name->append("Dist(C-S) (R)");

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTFS, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTFS_RANK, 4));
    projection_name->append("Dist(F-S)");
    projection_name->append("Dist(F-S) (R)");

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_ATR, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTCC_FSCROSS_RANK, 4));
    projection_name->append("Dist(FS-Cross)");
    projection_name->append("Dist(FS-Cross) (ATR)");
    projection_name->append("Dist(FS-Cross) (R)");

    projection.append(A__ + SQLiteHandler::COLUMN_NAME_FLS);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_FLS_RANK, 4));
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_FGS);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_FGS_RANK, 4));
    projection_name->append("F < S");
    projection_name->append("F < S (R)");
    projection_name->append("F > S");
    projection_name->append("F > S (R)");

    // CLF
    _threshold = "_" + QString::number(clf_id);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CLF + _threshold);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CLF_RANK + _threshold , 4));

    if ( c_clf[ clf_id ].value.toDouble() != 0 ) {
      _column1 = "C < F+" + BarDataAdapter::remove_trailing_zero( c_clf[ clf_id ].value ) ;
    } else {
      _column1 = "C < F";
    }
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");

    // CGF
    _threshold = "_" + QString::number(cgf_id);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CGF + _threshold);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CGF_RANK + _threshold , 4));

    if ( c_cgf[ cgf_id ].value.toDouble() != 0 ) {
      _column1 = "C > F-" + BarDataAdapter::remove_trailing_zero( c_cgf[ cgf_id ].value ) ;
    } else {
      _column1 = "C > F";
    }
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");

    // CLS
    _threshold = "_" + QString::number(cls_id);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CLS + _threshold);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CLS_RANK + _threshold , 4));

    if ( c_cls[ cls_id ].value.toDouble() != 0 ) {
      _column1 = "C < S+" + BarDataAdapter::remove_trailing_zero( c_cls[ cls_id ].value ) ;
    } else {
      _column1 = "C < S";
    }
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");

    // CGS
    _threshold = "_" + QString::number(cgs_id);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CGS + _threshold);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CGS_RANK + _threshold , 4));

    if ( c_cgs[ cgs_id ].value.toDouble() != 0 ) {
      _column1 = "C > S-" + BarDataAdapter::remove_trailing_zero( c_cgs[ cgs_id ].value ) ;
    } else {
      _column1 = "C > S";
    }
    projection_name->append(_column1);
    projection_name->append(_column1 + " (R)");

//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CLS);
//    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CLS_RANK, 4));
//    projection_name->append("C < S");
//    projection_name->append("C < S (R)");

//    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CGS);
//    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CGS_RANK, 4));
//    projection_name->append("C > S");
//    projection_name->append("C > S (R)");

    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_N_UPTAIL, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_UPTAIL_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_N_DOWNTAIL, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_DOWNTAIL_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_BODY, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_N_BODY, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_BODY_RANK, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_N_TOTALLENGTH, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_CANDLE_TOTALLENGTH_RANK, 4));

    projection_name->append("Uptail");
    projection_name->append("Uptail (N)");
    projection_name->append("Uptail (R)");
    projection_name->append("Downtail");
    projection_name->append("Downtail (N)");
    projection_name->append("Downtail (R)");
    projection_name->append("Body");
    projection_name->append("Body (N)");
    projection_name->append("Body (R)");
    projection_name->append("TotalLength");
    projection_name->append("TotalLength (N)");
    projection_name->append("TotalLength (R)");

    //
    // DistResistance & DistSupport
    //
    QString t = "_" + QString::number(id_threshold+1); // non-zero based
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_RES + t);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTRES + t, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTRES_ATR + t, 4));
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_DISTRES_RANK + t);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_SUP + t);
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTSUP + t, 4));
    projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DISTSUP_ATR + t, 4));
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_DISTSUP_RANK + t);
    projection_name->append("Resistance");
    projection_name->append("DistResistance");
    projection_name->append("DistResistance (ATR)");
    projection_name->append("DistResistance (R)");
    projection_name->append("Support");
    projection_name->append("DistSupport");
    projection_name->append("DistSupport (ATR)");
    projection_name->append("DistSupport (R)");

    //
    // DateRange
    //
    if (projection_interval < WEIGHT_DAILY) {
      projection.append(case_when_column_null(A__ + SQLiteHandler::COLUMN_NAME_DATERANGE, 4));
      projection_name->append("DateRange");
    }

    //
    // F-Cross / S-Cross
    //
    projection.append(case_when_column_null(A__+SQLiteHandler::COLUMN_NAME_FCROSS, 0));
    projection.append(case_when_column_null(A__+SQLiteHandler::COLUMN_NAME_SCROSS, 0));
    projection_name->append("F-Cross");
    projection_name->append("S-Cross");

    //
    // Zone
    //
    _value1 = SQLiteHandler::get_interval_name(projection_interval);
    projection_name->append("O-Zone" + _value1);
    projection_name->append("H-Zone" + _value1);
    projection_name->append("L-Zone" + _value1);
    projection_name->append("C-Zone" + _value1);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_OPEN_ZONE);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_HIGH_ZONE);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_LOW_ZONE);
    projection.append(A__ + SQLiteHandler::COLUMN_NAME_CLOSE_ZONE);

    if (projection_interval < WEIGHT_60MIN) {
      projection_name->append("O-Zone60min");
      projection_name->append("H-Zone60min");
      projection_name->append("L-Zone60min");
      projection_name->append("C-Zone60min");
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_OPEN_ZONE_60MIN);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_HIGH_ZONE_60MIN);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_LOW_ZONE_60MIN);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_ZONE_60MIN);
    }

    if (projection_interval < WEIGHT_DAILY) {
      projection_name->append("O-ZoneDaily");
      projection_name->append("H-ZoneDaily");
      projection_name->append("L-ZoneDaily");
      projection_name->append("C-ZoneDaily");
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_OPEN_ZONE_DAILY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_HIGH_ZONE_DAILY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_LOW_ZONE_DAILY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_ZONE_DAILY);
    }

    if (projection_interval < WEIGHT_WEEKLY) {
      projection_name->append("O-ZoneWeekly");
      projection_name->append("H-ZoneWeekly");
      projection_name->append("L-ZoneWeekly");
      projection_name->append("C-ZoneWeekly");
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_OPEN_ZONE_WEEKLY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_HIGH_ZONE_WEEKLY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_LOW_ZONE_WEEKLY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_ZONE_WEEKLY);
    }

    if (projection_interval < WEIGHT_MONTHLY) {
      projection_name->append("O-ZoneMonthly");
      projection_name->append("H-ZoneMonthly");
      projection_name->append("L-ZoneMonthly");
      projection_name->append("C-ZoneMonthly");
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_OPEN_ZONE_MONTHLY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_HIGH_ZONE_MONTHLY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_LOW_ZONE_MONTHLY);
      projection.append(A__ + SQLiteHandler::COLUMN_NAME_ZONE_MONTHLY);
    }

    //
    // # Resistance/Support Line
    //
    if (projection_interval >= WEIGHT_DAILY ) {
      QString t = "_" + QString::number(id_threshold+1); // non-zero based

      if (projection_interval == WEIGHT_MONTHLY) {
        projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_MONTHLY_RLINE + t, 0));
        projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_MONTHLY_SLINE + t, 0));
      } else if (projection_interval == WEIGHT_WEEKLY) {
        projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_WEEKLY_RLINE + t, 0));
        projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_WEEKLY_SLINE + t, 0));
      } else {
        projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_DAILY_RLINE + t, 0));
        projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_DAILY_SLINE + t, 0));
      }

      projection_name->append("Resistance (#)");
      projection_name->append("Support (#)");

      // experimental
//      projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_DAILY_RLINE_F_1, 0));
//      projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_DAILY_RLINE_S_1, 0));
//      projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_DAILY_SLINE_F_1, 0));
//      projection.append(case_when_column_null("A." + SQLiteHandler::COLUMN_NAME_DAILY_SLINE_S_1, 0));
//      projection_name->append("Res-F (#)");
//      projection_name->append("Res-S (#)");
//      projection_name->append("Sup-F (#)");
//      projection_name->append("Sup-S (#)");
    }

    projection.append(A_ + SQLiteHandler::COLUMN_NAME_IDPARENT);
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_IDPARENT_MONTHLY);
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_IDPARENT_PREV);
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_IDPARENT_PREV_MONTHLY);
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_PREV_DATE);
    projection.append(A_ + SQLiteHandler::COLUMN_NAME_PREV_TIME);

    projection_name->append(SQLiteHandler::COLUMN_NAME_IDPARENT);
    projection_name->append(SQLiteHandler::COLUMN_NAME_IDPARENT_MONTHLY);
    projection_name->append(SQLiteHandler::COLUMN_NAME_IDPARENT_PREV);
    projection_name->append(SQLiteHandler::COLUMN_NAME_IDPARENT_PREV_MONTHLY);
    projection_name->append(SQLiteHandler::COLUMN_NAME_PREV_DATE);
    projection_name->append(SQLiteHandler::COLUMN_NAME_PREV_TIME);

    IntervalWeight w = SQLiteHandler::get_parent_interval(table->get_smallest_interval_weight_v2());
    QString parent_alias;
    while (w != WEIGHT_INVALID) {
      switch (w) {
        case WEIGHT_DAILY: parent_alias = "R1."; break;
        case WEIGHT_WEEKLY: parent_alias = "R2."; break;
        case WEIGHT_MONTHLY: parent_alias = "R3."; break;
        case WEIGHT_60MIN: parent_alias = "R4."; break;
        case WEIGHT_5MIN: parent_alias = "R5."; break;
        default: parent_alias = "A.";
      }
      projection.append(parent_alias + SQLiteHandler::COLUMN_NAME_ROWID);
      projection_name->append("Rowid" + SQLiteHandler::get_interval_name(w));
      w = SQLiteHandler::get_parent_interval(w);
    }

    /*int column_count;
    for (int i = 0; intervals[i] != main_interval; ++i) {
      column_count = BarDataAdapter::get_resistance_column_count(db.getDatabase(symbol, intervals[i]), threshold_index);
      for (int j = 0; j < column_count; ++j) {
        projection.append("db" + intervals[i] + "." + SQLiteHandler::TABLE_NAME_RESISTANCE_WTF + ".v"+QString::number(j) +
                          " AS '" + intervals[i] + " Resistance'");
      }
      column_count = BarDataAdapter::get_support_column_count(db.getDatabase(symbol, intervals[i]), threshold_index);
      for (int j = 0; j < column_count; ++j) {
        projection.append("db" + intervals[i] + "." + SQLiteHandler::TABLE_NAME_SUPPORT_WTF + ".v"+QString::number(j) +
                          " AS '" + intervals[i] + " Support'");
      }
    }

    if (main_intervalweight >= WEIGHT_DAILY) {
      column_count = BarDataAdapter::get_resistance_column_count(db.getDatabase(symbol, main_interval), threshold_index);
      for (int j = 0; j < column_count; ++j) {
        projection.append("A1.v"+QString::number(j) + " AS '" + main_interval + " Resistance'");
      }

      column_count = BarDataAdapter::get_support_column_count(db.getDatabase(symbol, main_interval), threshold_index);
      for (int j = 0; j < column_count; ++j) {
        projection.append("A2.v"+QString::number(j) + " AS '" + main_interval + " Support'");
      }
    }*/
  }

  //
  // multi interval
  //
  if (!table->is_single_interval()) {
    for (int i = 0; i < N; ++i) {
//      if (!whereArgs.isEmpty()) whereArgs += " and ";

      if (table->getInterval(i) == main_interval) {
        canonical_alias = main_alias;
      } else {
        switch (table->getIntervalWeight(i)) {
          case WEIGHT_MONTHLY: canonical_alias = "R3"; break;
          case WEIGHT_WEEKLY: canonical_alias = "R2"; break;
          case WEIGHT_DAILY: canonical_alias = "R1"; break;
          case WEIGHT_60MIN: canonical_alias = "R4"; break;
          case WEIGHT_5MIN: canonical_alias = "R5"; break;
          case WEIGHT_1MIN: canonical_alias = ""; break;
          default: canonical_alias = ""; break;
        }
      }

      if (table->is_intersect_operator(i)) {
        intersect_type = table->get_intersect_type(i).split("_")[0];
        QString canonical_weight_alias;
        IntervalWeight intersect_weight = table->getIntersectWeight(i);
        bool isSRJoin = false;

        if (intersect_weight == main_intervalweight) {
          if (!SRjoinAlias.contains("#R") || !SRjoinAlias.contains("#S")) {
            SRAlias = "";
            SRjoinAlias.push_back("#");
            isSRJoin = true;
          }
          canonical_weight_alias = main_alias;
        } else {
          if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE ||
              intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_weight_alias = "dbMonthly"; break;
              case WEIGHT_WEEKLY: canonical_weight_alias = "dbWeekly"; break;
              case WEIGHT_DAILY: canonical_weight_alias = "dbDaily"; break;
              case WEIGHT_60MIN: canonical_weight_alias = "db60min"; break;
              case WEIGHT_5MIN: canonical_weight_alias = "db5min"; break;
              case WEIGHT_1MIN: canonical_weight_alias = "db1min"; break;
            }
            SRAlias = canonical_weight_alias;
            SRjoinAlias.push_back(SRAlias);
            isSRJoin = true;
            canonical_weight_alias += "." + SQLiteHandler::TABLE_NAME_BARDATA;
          } else {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_weight_alias = "R3"; break;
              case WEIGHT_WEEKLY: canonical_weight_alias = "R2"; break;
              case WEIGHT_DAILY: canonical_weight_alias = "R1"; break;
              case WEIGHT_60MIN: canonical_weight_alias = "R4"; break;
              case WEIGHT_5MIN: canonical_weight_alias = "R5"; break;
              case WEIGHT_1MIN: canonical_weight_alias = ""; break;
            }
          }
        }

        if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE) {
          whereArgs += table->getSqlResistanceIntersect(i, canonical_alias, SRAlias, breakThreshold, id_threshold);
//          whereArgs += table->getSqlResistanceIntersect_V2(i, canonical_alias, intersect_weight, id_threshold);


          if ( isSRJoin ) {
            SRjoin.push_back(true);
            if (SRjoinAlias.last().startsWith("#"))
              SRjoinAlias[SRjoinAlias.size()-1] = "#R";
          }

          QString resistance_table;
          if (intersect_weight == main_intervalweight) {
            resistance_table = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          } else {
            resistance_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_RESISTANCE + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          }

          if (!projection.contains("MAX(" + resistance_table + ")")) {
            projection.append("MAX(" + resistance_table + ")");
            projection_name->append("Max_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Resistance");
          }
        }
        else if (intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
          whereArgs += table->getSqlSupportIntersect(i, canonical_alias, SRAlias, breakThreshold, id_threshold);

          if ( isSRJoin ) {
            SRjoin.push_back(false);
            if (SRjoinAlias.last().startsWith("#"))
              SRjoinAlias[SRjoinAlias.size()-1] = "#S";
          }

          QString support_table;
          if (intersect_weight == main_intervalweight) {
            support_table = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          } else {
            support_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_SUPPORT + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          }

          if (!projection.contains("MIN(" + support_table + ")")) {
            projection.append("MIN(" + support_table + ")");
            projection_name->append("Min_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Support");
          }
        }
        else {
          whereArgs += table->getSqlPriceIntersect(i, canonical_alias, canonical_weight_alias, avgThreshold);
        }
      }
      else {
        QString intersectTableAlias;
        IntervalWeight _w = BarDataAdapter::getIntervalWeightFromName( table->getSecondOperatorText(i) );

        if (_w == main_intervalweight) {
          intersectTableAlias = "A";
        } else {
          switch (_w) {
            case WEIGHT_MONTHLY: intersectTableAlias = "R3"; break;
            case WEIGHT_WEEKLY: intersectTableAlias = "R2"; break;
            case WEIGHT_DAILY: intersectTableAlias = "R1"; break;
            case WEIGHT_60MIN: intersectTableAlias = "R4"; break;
            case WEIGHT_5MIN: intersectTableAlias = "R5"; break;
            case WEIGHT_1MIN: intersectTableAlias = ""; break;
            default: intersectTableAlias = ""; break;
          }
        }

        whereArgs += table->getSqlString(i, id_threshold, canonical_alias, intersectTableAlias);
      }

      whereArgs += table->getSqlConditionRow(i);
    }
  }
  //
  // single interval
  //
  else {
    for (int i = 0; i < N; ++i) {
//      if (!whereArgs.isEmpty()) whereArgs += " and ";

      if (table->is_intersect_operator(i)) {
        intersect_type = table->get_intersect_type(i).split("_")[0];
        IntervalWeight intersect_weight = table->getIntersectWeight(i);

        if (intersect_weight == main_intervalweight) {
          canonical_alias = main_alias;
          SRAlias = "";
          SRjoinAlias.push_back("");
        } else {
          if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE ||
              intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_alias = "dbMonthly"; break;
              case WEIGHT_WEEKLY: canonical_alias = "dbWeekly"; break;
              case WEIGHT_DAILY: canonical_alias = "dbDaily"; break;
              case WEIGHT_60MIN: canonical_alias = "db60min"; break;
              case WEIGHT_5MIN: canonical_alias = "db5min"; break;
              case WEIGHT_1MIN: canonical_alias = "db1min"; break;
              case WEIGHT_INVALID: canonical_alias = ""; break;
            }
            SRAlias = canonical_alias;
            SRjoinAlias.push_back(SRAlias);
          } else {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_alias = "R3"; break;
              case WEIGHT_WEEKLY: canonical_alias = "R2"; break;
              case WEIGHT_DAILY: canonical_alias = "R1"; break;
              case WEIGHT_60MIN: canonical_alias = "R4"; break;
              case WEIGHT_5MIN: canonical_alias = "R5"; break;
              case WEIGHT_1MIN: canonical_alias = ""; break;
              case WEIGHT_INVALID: canonical_alias = ""; break;
            }
          }
        }

        if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE) {
          whereArgs += table->getSqlResistanceIntersect(i, main_alias, SRAlias, breakThreshold, id_threshold);
//          whereArgs += table->getSqlResistanceIntersect_V2(i, main_alias, intersect_weight, id_threshold);

          SRjoin.push_back(true);

          QString resistance_table;
          if (intersect_weight == main_intervalweight) {
            resistance_table = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          } else {
            resistance_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_RESISTANCE + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          }

          if (!projection.contains("MAX(" + resistance_table + ")")) {
            projection.append("MAX(" + resistance_table + ")");
            projection_name->append("Max_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Resistance");
          }
        }
        else if (intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
          whereArgs += table->getSqlSupportIntersect(i, main_alias, SRAlias, breakThreshold, id_threshold);

          SRjoin.push_back(false);

          QString support_table;
          if (intersect_weight == main_intervalweight) {
            support_table = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          } else {
            support_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_SUPPORT + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          }

          if (!projection.contains("MIN(" + support_table + ")")) {
            projection.append("MIN(" + support_table + ")");
            projection_name->append("Min_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Support");
          }
        }
        else {
          whereArgs += table->getSqlPriceIntersect(i, main_alias, canonical_alias, avgThreshold);
        }
      }
      else {
        QString intersectTableAlias;
        IntervalWeight _w = BarDataAdapter::getIntervalWeightFromName( table->getSecondOperatorText(i) );

        if (_w == main_intervalweight) {
          intersectTableAlias = "A";
        } else {
          switch (_w) {
            case WEIGHT_MONTHLY: intersectTableAlias = "R3"; break;
            case WEIGHT_WEEKLY: intersectTableAlias = "R2"; break;
            case WEIGHT_DAILY: intersectTableAlias = "R1"; break;
            case WEIGHT_60MIN: intersectTableAlias = "R4"; break;
            case WEIGHT_5MIN: intersectTableAlias = "R5"; break;
            case WEIGHT_1MIN: intersectTableAlias = ""; break;
            default: intersectTableAlias = ""; break;
          }
        }

        whereArgs += table->getSqlString(i, id_threshold, main_alias, intersectTableAlias);
      }

      whereArgs += table->getSqlConditionRow(i);
    }
  }

  // intersect with matcher
  if (matcherResult.length() > 0) {
    if (!whereArgs.isEmpty()) whereArgs += " and ";
    whereArgs += A_ + "rowid in(" + matcherResult.join(",") + ")";
  }

  return SQLiteHandler::SQL_SELECT_BARDATA_BUILDER_JOIN(projection,
    *databaseAlias, whereArgs, SRjoinAlias, SRjoin, A_.replace(".",""), false);
}

/*QString MainWindow::sql_string_builder_WTF(FilterOptionFactory *table, const QString &symbol,
    QStringList *databaseNames, QStringList *databaseAlias, QString *sql_count, QStringList *projection_name,
    const int &id_threshold, const double &sr_threshold, const double &avg_threshold) {
  // replace sqlitedb with db
  QVector<bool> SRjoin;
  QStringList projection;
  QStringList SRjoinAlias;
  QString whereArgs = "";
  QString A_ = "A.";
  QString SRAlias = "";
  QString canonical_alias;
  QString intersect_type;
  IntervalWeight base_interval = table->get_smallest_interval_weight_v2();
  IntervalWeight w = WEIGHT_LARGEST;
  const QString main_alias = "A";
  const QString main_interval = table->get_smallest_interval();
  const int main_intervalweight = table->get_smallest_interval_weight();
  int N = table->length();

  databaseNames->append(sqlitedb.getDatabaseName(symbol, table->get_smallest_interval_weight_v2()));
  databaseAlias->append("");

  while (w != base_interval) {
    databaseNames->append(sqlitedb.getDatabasePath(symbol, w));
    databaseAlias->append("db" + SQLiteHandler::get_interval_name(w));
    w = SQLiteHandler::get_child_interval(w);
  }

  projection_name->append(SQLiteHandler::COLUMN_NAME_ROWID);
  projection.append(A_ + SQLiteHandler::COLUMN_NAME_DATE);
  projection.append(A_ + SQLiteHandler::COLUMN_NAME_TIME);
  projection.append(A_ + SQLiteHandler::COLUMN_NAME_IDPARENT);
  projection.append(A_ + SQLiteHandler::COLUMN_NAME_IDPARENT_PREV);
  projection.append(A_ + SQLiteHandler::COLUMN_NAME_PREV_DATE);
  projection.append(A_ + SQLiteHandler::COLUMN_NAME_PREV_TIME);

  projection_name->append(SQLiteHandler::COLUMN_NAME_DATE);
  projection_name->append(SQLiteHandler::COLUMN_NAME_TIME);
  projection_name->append(SQLiteHandler::COLUMN_NAME_IDPARENT);
  projection_name->append(SQLiteHandler::COLUMN_NAME_IDPARENT_PREV);
  projection_name->append(SQLiteHandler::COLUMN_NAME_PREV_DATE);
  projection_name->append(SQLiteHandler::COLUMN_NAME_PREV_TIME);



  //
  // multi interval
  //
  if (!table->is_single_interval()) {
    for (int i = 0; i < N; ++i) {
      if (!whereArgs.isEmpty()) whereArgs += " and ";

      if (table->getInterval(i) == main_interval) {
        canonical_alias = main_alias;
      } else {
        switch (table->getIntervalWeight(i)) {
          case WEIGHT_MONTHLY: canonical_alias = "R3"; break;
          case WEIGHT_WEEKLY: canonical_alias = "R2"; break;
          case WEIGHT_DAILY: canonical_alias = "R1"; break;
          case WEIGHT_60MIN: canonical_alias = "R4"; break;
          case WEIGHT_5MIN: canonical_alias = "R5"; break;
          case WEIGHT_1MIN: canonical_alias = ""; break;
        }
      }

      if (table->is_intersect_operator(i)) {
        intersect_type = table->get_intersect_type(i).split("_")[0];
        QString canonical_weight_alias;
        IntervalWeight intersect_weight = table->getIntersectWeight(i);
        bool isSRJoin = false;

        if (intersect_weight == main_intervalweight) {
          if (!SRjoinAlias.contains("#R") || !SRjoinAlias.contains("#S")) {
            SRAlias = "";
            SRjoinAlias.push_back("#");
            isSRJoin = true;
          }
          canonical_weight_alias = main_alias;
        } else {
          if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE ||
              intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_weight_alias = "dbMonthly"; break;
              case WEIGHT_WEEKLY: canonical_weight_alias = "dbWeekly"; break;
              case WEIGHT_DAILY: canonical_weight_alias = "dbDaily"; break;
              case WEIGHT_60MIN: canonical_weight_alias = "db60min"; break;
              case WEIGHT_5MIN: canonical_weight_alias = "db5min"; break;
              case WEIGHT_1MIN: canonical_weight_alias = "db1min"; break;
            }
            SRAlias = canonical_weight_alias;
            SRjoinAlias.push_back(SRAlias);
            isSRJoin = true;
            canonical_weight_alias += "." + SQLiteHandler::TABLE_NAME_BARDATA;
          } else {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_weight_alias = "R3"; break;
              case WEIGHT_WEEKLY: canonical_weight_alias = "R2"; break;
              case WEIGHT_DAILY: canonical_weight_alias = "R1"; break;
              case WEIGHT_60MIN: canonical_weight_alias = "R4"; break;
              case WEIGHT_5MIN: canonical_weight_alias = "R5"; break;
              case WEIGHT_1MIN: canonical_weight_alias = ""; break;
            }
          }
        }

        if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE) {
          whereArgs += table->getSqlResistanceIntersect(i, canonical_alias, SRAlias, sr_threshold, id_threshold);
//          whereArgs += table->getSqlResistanceIntersect_V2(i, canonical_alias, intersect_weight, id_threshold);

          if (isSRJoin) {
            SRjoin.push_back(true);
            if (SRjoinAlias.last().startsWith("#"))
              SRjoinAlias[SRjoinAlias.size()-1] = "#R";
          }

          QString resistance_table;
          if (intersect_weight == main_intervalweight) {
            resistance_table = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          } else {
            resistance_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_RESISTANCE + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          }

          if (!projection.contains("MAX(" + resistance_table + ")")) {
            projection.append("MAX(" + resistance_table + ")");
            projection_name->append("Max_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Resistance");
          }
        }
        else if (intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
          whereArgs += table->getSqlSupportIntersect(i, canonical_alias, SRAlias, sr_threshold, id_threshold);

          if (isSRJoin) {
            SRjoin.push_back(false);
            if (SRjoinAlias.last().startsWith("#"))
              SRjoinAlias[SRjoinAlias.size()-1] = "#S";
          }

          QString support_table;
          if (intersect_weight == main_intervalweight) {
            support_table = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          } else {
            support_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_SUPPORT + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          }

          if (!projection.contains("MIN(" + support_table + ")")) {
            projection.append("MIN(" + support_table + ")");
            projection_name->append("Min_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Support");
          }
        }
        else {
          whereArgs += table->getSqlPriceIntersect(i, canonical_alias, canonical_weight_alias, avg_threshold);
        }
      }
      else {
        whereArgs += table->getSqlString(i, id_threshold, canonical_alias); // XX
      }
    }
  }
  //
  // single interval
  //
  else {
    for (int i = 0; i < N; ++i) {
      if (!whereArgs.isEmpty()) whereArgs += " and ";

      if (table->is_intersect_operator(i)) {
        intersect_type = table->get_intersect_type(i).split("_")[0];
        IntervalWeight intersect_weight = table->getIntersectWeight(i);

        if (intersect_weight == main_intervalweight) {
          canonical_alias = main_alias;
          SRAlias = "";
          SRjoinAlias.push_back("");
        } else {
          if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE ||
              intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_alias = "dbMonthly"; break;
              case WEIGHT_WEEKLY: canonical_alias = "dbWeekly"; break;
              case WEIGHT_DAILY: canonical_alias = "dbDaily"; break;
              case WEIGHT_60MIN: canonical_alias = "db60min"; break;
              case WEIGHT_5MIN: canonical_alias = "db5min"; break;
              case WEIGHT_1MIN: canonical_alias = "db1min"; break;
              case WEIGHT_INVALID: canonical_alias = ""; break;
            }
            SRAlias = canonical_alias;
            SRjoinAlias.push_back(SRAlias);
          } else {
            switch (intersect_weight) {
              case WEIGHT_MONTHLY: canonical_alias = "R3"; break;
              case WEIGHT_WEEKLY: canonical_alias = "R2"; break;
              case WEIGHT_DAILY: canonical_alias = "R1"; break;
              case WEIGHT_60MIN: canonical_alias = "R4"; break;
              case WEIGHT_5MIN: canonical_alias = "R5"; break;
              case WEIGHT_1MIN: canonical_alias = ""; break;
              case WEIGHT_INVALID: canonical_alias = ""; break;
            }
          }
        }

        if (intersect_type == SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE) {
          whereArgs += table->getSqlResistanceIntersect(i, main_alias, SRAlias, sr_threshold, id_threshold);
//          whereArgs += table->getSqlResistanceIntersect_V2(i, main_alias, intersect_weight, id_threshold);

          SRjoin.push_back(true);

          QString resistance_table;
          if (intersect_weight == main_intervalweight) {
            resistance_table = "RT." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          } else {
            resistance_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_RESISTANCE + "." + SQLiteHandler::COLUMN_NAME_RESISTANCE_VALUE;
          }

          if (!projection.contains("MAX(" + resistance_table + ")")) {
            projection.append("MAX(" + resistance_table + ")");
            projection_name->append("Max_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Resistance");
          }
        }
        else if (intersect_type == SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE) {
          whereArgs += table->getSqlSupportIntersect(i, main_alias, SRAlias, sr_threshold, id_threshold);
          SRjoin.push_back(false);

          QString support_table;
          if (intersect_weight == main_intervalweight) {
            support_table = "ST." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          } else {
            support_table = SRAlias + "." + SQLiteHandler::TABLE_NAME_SUPPORT + "." + SQLiteHandler::COLUMN_NAME_SUPPORT_VALUE;
          }

          if (!projection.contains("MIN(" + support_table + ")")) {
            projection.append("MIN(" + support_table + ")");
            projection_name->append("Min_" + SQLiteHandler::get_interval_name(intersect_weight) + "_Support");
          }
        }
        else {
          whereArgs += table->getSqlPriceIntersect(i, main_alias, canonical_alias, avg_threshold);
        }
      }
      else {
        whereArgs += table->getSqlString(i, id_threshold, main_alias); // XX
      }
    }
  }


  // count query
  if (sql_count != NULL) {
    *sql_count = SQLiteHandler::SQL_SELECT_BARDATA_BUILDER_JOIN(QStringList(),
      *databaseAlias, whereArgs, SRjoinAlias, SRjoin, A_.replace(".",""), true);
  }

  return SQLiteHandler::SQL_SELECT_BARDATA_BUILDER_JOIN(projection,
    *databaseAlias, whereArgs, SRjoinAlias, SRjoin, A_.replace(".",""), false);
}*/

/*QSqlQuery* get_SqlQuery_WTF(const QStringList &databaseName, const QStringList &databaseAlias, const QSqlDatabase &database, const QString &sql_select) {
  QSqlQuery *query_1 = new QSqlQuery(database);
  query_1->setForwardOnly(true);

  for (int i = 1; i < databaseName.length(); ++i) {
    query_1->exec("ATTACH DATABASE '" + databaseName[i] + "' AS " + databaseAlias[i]);
  }

  query_1->exec("PRAGMA journal_mode = OFF;");
  query_1->exec("PRAGMA synchronous = OFF;");
  query_1->exec("PRAGMA cache_size = 50000;");
  query_1->exec(sql_select);

  if (query_1->lastError().isValid() || !query_1->next()) {
    query_1->lastError();
    delete query_1;
    query_1 = NULL;
  }

  return query_1;
}*/

QSqlQuery* MainWindow::local_getSqlQuery() {
  IntervalWeight base_interval = filter->get_smallest_interval_weight_v2();
  int id_threshold = ui->cbThreshold->currentData().toInt();
  double sr_threshold = ui->cbThreshold->currentText().toDouble();

  SRModel *_srmodel = m_srmodel;
  QTableView *_table = m_table;
  IntervalWeight _projection_weight = *m_projection_weight;
  QSqlDatabase _main_database = *m_main_database;
  QStringList _databaseName = *m_databaseName;
  QStringList _databaseAlias = *m_databaseAlias;
  QString _sql_select = *m_sql_select;
  QStringList _projectionNames = *m_projectionNames;
  bool _forwardOnly = *m_forwardOnly;
  int _prevbar_count = *m_prevbar_count;

  QSqlQuery *query= sqlitedb.getSqlQuery(_main_database, base_interval,
    _projection_weight, id_threshold, sr_threshold,
    _databaseName, _databaseAlias, _sql_select,
    _projectionNames, _forwardOnly, _prevbar_count, SBAR_ID);

  _srmodel = new SRModel(sr_threshold);
  _srmodel->setQuery(*query);
  _table->setModel(_srmodel);

  delete query;
  query = NULL;
  SQLiteHandler::removeDatabase(&_main_database);

  return NULL;
}

void MainWindow::on_btnSearch_clicked() {

  if ( !filter->validate_block() ) {
    QMessageBox::information(this, "Information", "Please check your input block condition");
    return;
  }

  enable_components(false);
  initialize(false); // reload config
  label_query_result.setText("Executing query. Please wait..");
  QCoreApplication::processEvents();

  //
  // Prepare matcher result to intersect
  //
  if (ui->chkPlotMatcher->isChecked()) {
    on_chkPlotMatcher_stateChanged(Qt::Checked);
  }

  const QString symbol = ui->cbSymbol->currentData().toString();
  const IntervalWeight base_weight_const = filter->get_smallest_interval_weight_v2();
  const int id_threshold = ui->cbThreshold->currentData().toInt();
  const double sr_threshold = ui->cbThreshold->currentText().toDouble();
  const double avg_threshold = 0;

  QStringList databaseNames; // databaseName for attach database
  QStringList databaseAlias; // table alias for join
  QStringList projectionNames; // column name for reprojection
  QStringList ts_databaseNames;
  QStringList ts_databaseAlias;
  QStringList ts_projectionNames;
  QString sql_select;
  QString sql_select_projection;
  IntervalWeight projection_weight = base_weight_const;
  QTime time; time.start();
  int tabIndex = 0;

  //
  // Remove previous result
  //
  clear_model();
  clear_program_result(symbol);
  set_tab_widget_interval(SQLiteHandler::get_parent_interval(base_weight_const));

//  sql_select = sql_string_builder_v2(filter, symbol,
//                                     &databaseNames,
//                                     &databaseAlias,
//                                     &sql_count,
//                                     matcher_rowid,
//                                     id_threshold,
//                                     sr_threshold,
//                                     avg_threshold);

//  qDebug() << "\nSQL STRING V2\n";
//  qDebug() << sql_select;
//  qDebug() << databaseNames;
//  qDebug() << databaseAlias;
//  qDebug() << "\n\n";
//  databaseNames.clear();
//  databaseAlias.clear();

//  QQueue<QFuture<QSqlQuery*>> queue;

  while (projection_weight != WEIGHT_INVALID) {
    SRModel **_srmodel = NULL;
    QSqlQuery **_model_query = NULL;
    QSqlDatabase *_model_database = &this->model_database;

    if (projection_weight == base_weight_const) {
      _srmodel = &srmodel;
      _model_query = &model_query;
    }
    else if (projection_weight == WEIGHT_5MIN) {
      _srmodel = &srmodel_5min;
      _model_query = &model_query_5min;
    }
    else if (projection_weight == WEIGHT_60MIN) {
      _srmodel = &srmodel_60min;
      _model_query = &model_query_60min;
    }
    else if (projection_weight == WEIGHT_DAILY) {
      _srmodel = &srmodel_daily;
      _model_query = &model_query_daily;
    }
    else if (projection_weight == WEIGHT_WEEKLY) {
      _srmodel = &srmodel_weekly;
      _model_query = &model_query_weekly;
    }
    else if (projection_weight == WEIGHT_MONTHLY) {
      _srmodel = &srmodel_monthly;
      _model_query = &model_query_monthly;
    }

    //
    // Build query string (CORE)
    //
    sql_select = sql_string_builder(filter, symbol, &databaseNames, &databaseAlias,
                                    &projectionNames, matcher_rowid, projection_weight,
                                    false, id_threshold, sr_threshold, avg_threshold,
                                    tid_macd, tid_rsi, tid_slowk, tid_slowd,
                                    tid_cgf, tid_clf, tid_cgs, tid_cls);

    qDebug() << "\n" << sql_select;

    //
    // Execute query (CORE)
    //
    *_model_database = sqlitedb.getCloneDatabase(symbol, base_weight_const);

    *_model_query = sqlitedb.getSqlQuery(*_model_database, base_weight_const, projection_weight, id_threshold, sr_threshold,
                                  databaseNames, databaseAlias, sql_select, projectionNames, false,
                                  list_interval_threshold[base_weight_const], SBAR_ID);


//    bool b = false;
//    m_main_database = _model_database;
//    m_projection_weight = &projection_weight;
//    m_databaseName = &databaseNames;
//    m_databaseAlias = &databaseAlias;
//    m_sql_select = &sql_select;
//    m_projectionNames = &projectionNames;
//    m_forwardOnly = &b;
//    m_prevbar_count = &list_interval_threshold[base_weight_const];
//    m_srmodel = *_srmodel;

    if (projection_weight == base_weight_const) { m_table = ui->tblData; }
    else if (projection_weight == WEIGHT_5MIN) { m_table = ui->tblData_5min; }
    else if (projection_weight == WEIGHT_60MIN) { m_table = ui->tblData_60min; }
    else if (projection_weight == WEIGHT_DAILY) { m_table = ui->tblData_Daily; }
    else if (projection_weight == WEIGHT_WEEKLY) { m_table = ui->tblData_Weekly; }
    else if (projection_weight == WEIGHT_MONTHLY) { m_table = ui->tblData_Monthly; }

//    QFuture<QSqlQuery*> future =
//    QtConcurrent::run(this, &MainWindow::local_getSqlQuery);
//    this->model_database = QSqlDatabase();
//    future.waitForFinished();
//    *model_query = future.result();

    *_srmodel = new SRModel(sr_threshold);
    (*_srmodel)->setQuery(**_model_query);

    for (int x = 0; (*_srmodel)->canFetchMore() && x < 20; ++x)
      (*_srmodel)->fetchMore();

    if (projection_weight == base_weight_const) {
      ui->tblData->setModel(*_srmodel);
      set_table_view(ui->tblData, base_weight_const, projection_weight);
    }
    else if (projection_weight == WEIGHT_5MIN) {
      ui->tblData_5min->setModel(*_srmodel);
      set_table_view(ui->tblData_5min, base_weight_const, projection_weight);
    }
    else if (projection_weight == WEIGHT_60MIN) {
      ui->tblData_60min->setModel(*_srmodel);
      set_table_view(ui->tblData_60min, base_weight_const, projection_weight);
    }
    else if (projection_weight == WEIGHT_DAILY) {
      ui->tblData_Daily->setModel(*_srmodel);
      set_table_view(ui->tblData_Daily, base_weight_const, projection_weight);
    }
    else if (projection_weight == WEIGHT_WEEKLY) {
      ui->tblData_Weekly->setModel(*_srmodel);
      set_table_view(ui->tblData_Weekly, base_weight_const, projection_weight);
    }
    else if (projection_weight == WEIGHT_MONTHLY) {
      ui->tblData_Monthly->setModel(*_srmodel);
      set_table_view(ui->tblData_Monthly, base_weight_const, projection_weight);
    }

    if ((*_srmodel)->rowCount() > 0) {
      (*_model_query)->exec("select count(1) from bardataview where delete_flag=0");
      int rowcount = (*_model_query)->next()? (*_model_query)->value(0).toInt() : 0;
      vec_label_string[tabIndex++] = "Total records: " + QString::number(rowcount);
      (*_model_query)->finish();
    } else {
      vec_label_string[tabIndex++] = "Executed query return empty result.";
    }

    qDebug() << "Executed query finished in:" << time.elapsed()/1000.0 << "sec(s)"; time.start();

    //
    // Write result for TradeStation
    //
    if (enable_sync_result) {
      if (projection_weight != base_weight_const) {

        sql_select_projection = sql_string_builder(filter, symbol, &ts_databaseNames, &ts_databaseAlias,
          &ts_projectionNames, matcher_rowid, projection_weight, true, id_threshold, sr_threshold, avg_threshold,
          tid_macd, tid_rsi, tid_slowk, tid_slowd, tid_cgf, tid_clf, tid_cgs, tid_cls);

//        qDebug() << "----- WTF Query ------";
//        qDebug() << sql_select_projection;

        QSqlDatabase database1 = SQLiteHandler::clone_database(*_model_database);
        QSqlQuery *query_clone =
            sqlitedb.getSqlQuery(database1, base_weight_const, projection_weight, id_threshold,
              sr_threshold, ts_databaseNames, ts_databaseAlias, sql_select_projection,
              QStringList(), true, list_interval_threshold[base_weight_const], SBAR_ID);

        export_result_to_file(symbol, SQLiteHandler::get_interval_name(projection_weight), query_clone, database1, true);

        ts_databaseNames.clear();
        ts_databaseAlias.clear();
        ts_projectionNames.clear();
      }
      else {
        (*_model_query)->exec("select rowid, date_, time_ from bardataview where delete_flag=0 order by rowid asc");
        export_result_to_file(symbol, SQLiteHandler::get_interval_name(projection_weight), *_model_query, QSqlDatabase(), false);
      }
    }

    SQLiteHandler::removeDatabase(&this->model_database);

    databaseNames.clear();
    databaseAlias.clear();
    projectionNames.clear();
    projection_weight = SQLiteHandler::get_parent_interval(projection_weight);
  }

  on_tabWidget_currentChanged(0);
  enable_components(true);
}

// TODO:
void MainWindow::on_btnExport_clicked() {
  if (model_query == NULL) {
    QMessageBox::information(this, "Information", "There's nothing to export.");
    return;
  }

  QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), QCoreApplication::applicationDirPath(), "CSV File (*.csv)");
  QString filename_parse = filename.mid(0, filename.size()-4); // without ".csv"

  if (filename.isEmpty()) return;

  //
  // Write Search Condition
  //
  saveSearchConditions( filename_parse + " (Conditions).txt" );

  //
  // Write Search Result
  //
  QList<QFuture<void>> queue;
  QFuture<void> future;
  QStringList projection;
  QStringList filterList;
  QStringList filterNonBaseInterval;
  QSqlRecord r;
  QString column_name;
  QString sql_select;
  QString fname;
  IntervalWeight interval = filter->get_smallest_interval_weight_v2();
  const IntervalWeight base_interval = filter->get_smallest_interval_weight_v2();

  // filter out unnecessary column
  {
    filterList.append("open_");
    filterList.append("high_");
    filterList.append("low_");
    filterList.append("close_");
    filterList.append("rowid");
    filterList.append("_parent");
    filterList.append("_parent_monthly");
    filterList.append("_parent_prev");
    filterList.append("_parent_prev_monthly");
    filterList.append("PrevDate");
    filterList.append("PrevTime");
    filterList.append("RowidMonthly");
    filterList.append("RowidWeekly");
    filterList.append("RowidDaily");
    filterList.append("Rowid60min");
    filterList.append("Min_Daily_Resistance");
    filterList.append("Min_Daily_Support");
    filterList.append("delete_flag");
  }

  // filter for Non Base Interval
  {
//    filterNonBaseInterval.append("PrevBar");
//    filterNonBaseInterval.append("open_");
//    filterNonBaseInterval.append("high_");
//    filterNonBaseInterval.append("low_");
//    filterNonBaseInterval.append("close_");
//    filterNonBaseInterval.append("Volume");
//    filterNonBaseInterval.append("MACD");
//    filterNonBaseInterval.append("MACD (R)");
//    filterNonBaseInterval.append("MACDAvg");
//    filterNonBaseInterval.append("MACDDiff");
//    filterNonBaseInterval.append("RSI");
//    filterNonBaseInterval.append("SlowK");
//    filterNonBaseInterval.append("SlowD");
  }

  while (interval != WEIGHT_INVALID) {
    QSqlQuery **model_query = NULL;
    if (interval == base_interval) {
      model_query = &this->model_query;
    } else {
      switch (interval) {
        case WEIGHT_5MIN: model_query = &this->model_query_5min; break;
        case WEIGHT_60MIN: model_query = &this->model_query_60min; break;
        case WEIGHT_DAILY: model_query = &this->model_query_daily; break;
        case WEIGHT_WEEKLY: model_query = &this->model_query_weekly; break;
        case WEIGHT_MONTHLY: model_query = &this->model_query_monthly; break;
      }
    }

    projection.clear();
    (*model_query)->exec("select * from bardataview limit 1");
    r = (*model_query)->record();

    for (int i = 0; i < r.count(); ++i) {
      if (interval != base_interval && filterNonBaseInterval.contains(r.fieldName(i))) {
        continue;
      }

      if (!filterList.contains(r.fieldName(i))) {
        column_name = "a.'" + r.fieldName(i) + "'";

        if (column_name.contains("Resistance") || column_name.contains("Support") || column_name.contains("PrevBar")) {
          column_name = "\"\"\"\"||" + column_name + "||\"\"\"\" AS '" + r.fieldName(i) + "'";
        }
        else if (r.fieldName(i) == "date_") {
          column_name = "strftime('%Y-%m-%d'," + column_name + ") AS 'Date'";
        }
        else if (r.fieldName(i) == "time_") {
          column_name = column_name + " AS 'Time'";
        }
        else if (r.fieldName(i) == "open_") {
          column_name = column_name + " AS 'Open'";
        }
        else if (r.fieldName(i) == "high_") {
          column_name = column_name + " AS 'High'";
        }
        else if (r.fieldName(i) == "low_") {
          column_name = column_name + " AS 'Low'";
        }
        else if (r.fieldName(i) == "close_") {
          column_name = column_name + " AS 'Close'";
        }

        projection.append(column_name);
      }
    }

    sql_select = "select " + projection.join(",") + " from bardataview a where delete_flag=0 order by _rowid_ asc";
//    BarDataAdapter::export_result_to_csv(*model_query, sql_select, filename_parse + " (" + SQLiteHandler::get_interval_name(interval) + ").csv");

    fname = filename_parse + " (" + SQLiteHandler::get_interval_name(interval) + ").csv";
    future = QtConcurrent::run(&BarDataAdapter::export_result_to_csv,*model_query, sql_select, fname);
    queue.push_back(future);
    interval = SQLiteHandler::get_parent_interval(interval);
  }

  //
  // TODO: fix last remove queue item
  //
  for (int i = 0; !queue.isEmpty(); ) {
    if ( i < queue.size() && queue[i].isFinished() ) {
//      qDebug() << "removeAt" << i << queue.size() << queue[i].isFinished();
      if (queue.size() == 1) {
//        queue[i].waitForFinished();
        break;
//        queue.removeAt(i);
//        queue.clear();
      }
      else {
        queue.removeAt(i);
//        qDebug() << "isFinished?" << queue[i].isFinished() << i << queue.size();
      }
    }
    i = (i + 1) % queue.size();
    QThread::msleep(100);
  }

  QMessageBox::information(this, "Information", "Export complete");
}

void MainWindow::openSearchConditions(const QString &filename) {
  QFile file(filename);

  if (file.open(QFile::ReadOnly)) {
    QTextStream stream(&file);
    QStringList line;
    int i = 0;
    bool b = true;

    // remove all rows
    while ( filter->length() > 0 ) {
      filter->removeAt(0);
    }

    while ( !stream.atEnd() ) {
      line = stream.readLine().split("=");

      if ( line[0] == "SYMBOL" ) ui->cbSymbol->setCurrentText(line[1]);
      else if ( line[0] == "SR_THRESHOLD" ) ui->cbThreshold->setCurrentText(line[1]);
      else if ( line[0] == "MACD_THRESHOLD" ) tid_macd = line[1].toInt();
      else if ( line[0] == "RSI_THRESHOLD" ) tid_rsi = line[1].toInt();
      else if ( line[0] == "SLOWK_THRESHOLD" ) tid_slowk = line[1].toInt();
      else if ( line[0] == "SLOWD_THRESHOLD" ) tid_slowd = line[1].toInt();
      else if ( line[0] == "CLF_THRESHOLD" ) tid_clf = line[1].toInt();
      else if ( line[0] == "CGF_THRESHOLD" ) tid_cgf = line[1].toInt();
      else if ( line[0] == "CLS_THRESHOLD" ) tid_cls = line[1].toInt();
      else if ( line[0] == "CGS_THRESHOLD" ) tid_cgs = line[1].toInt();
      else if ( line[0] == "CONDITION" ) {

        // refresh once - before parse CONDITION
        if ( b ) {
          refresh_threshold_component();
          b = false;
        }

        // append new row if less than configured condition
        if ( filter->length() - 1 < i ) {
          filter->insertAt(i);
        }

        // override row condition
        filter->setConditionString(i++, line[1].split(","));
      }
    }

    // remove surplus rows
    while ( i < filter->length() ) {
      filter->removeAt( filter->length() - 1 );
    }
  }

  file.close();
}

void MainWindow::saveSearchConditions(const QString &filename) {
  QString condition_text = "";
  condition_text += "SYMBOL=" + ui->cbSymbol->currentText() + "\r\n";
  condition_text += "SR_THRESHOLD=" + ui->cbThreshold->currentText() + "\r\n";
  condition_text += "MACD_THRESHOLD=" + QString::number(tid_macd) + "\r\n";
  condition_text += "RSI_THRESHOLD=" + QString::number(tid_rsi) + "\r\n";
  condition_text += "SLOWK_THRESHOLD=" + QString::number(tid_slowk) + "\r\n";
  condition_text += "SLOWD_THRESHOLD=" + QString::number(tid_slowd) + "\r\n";
  condition_text += "CLF_THRESHOLD=" + QString::number(tid_clf) + "\r\n";
  condition_text += "CGF_THRESHOLD=" + QString::number(tid_cgf) + "\r\n";
  condition_text += "CLS_THRESHOLD=" + QString::number(tid_cls) + "\r\n";
  condition_text += "CGS_THRESHOLD=" + QString::number(tid_cgs) + "\r\n";

  int N = filter->length();
  for (int i = 0; i < N; ++i) {
    condition_text += "CONDITION=" + filter->getConditionString(i) + "\r\n";
  }

  QFile file(filename);

  if (file.open(QFile::WriteOnly)) {
    QTextStream stream(&file);
    stream << condition_text;
  }

  file.close();
}

//
// Util function
//
void MainWindow::clear_model() {
  if (srmodel != NULL) {
    delete model_query;
    delete model_query_monthly;
    delete model_query_weekly;
    delete model_query_daily;
    delete model_query_60min;
    delete model_query_5min;
    model_query = NULL;
    model_query_monthly = NULL;
    model_query_weekly = NULL;
    model_query_daily = NULL;
    model_query_60min = NULL;
    model_query_5min = NULL;

    delete srmodel;
    delete srmodel_monthly;
    delete srmodel_weekly;
    delete srmodel_daily;
    delete srmodel_60min;
    delete srmodel_5min;
    srmodel = NULL;
    srmodel_monthly = NULL;
    srmodel_weekly = NULL;
    srmodel_daily = NULL;
    srmodel_60min = NULL;
    srmodel_5min = NULL;

    ui->tblData->setModel(NULL);
    ui->tblData_Daily->setModel(NULL);
    ui->tblData_Weekly->setModel(NULL);
    ui->tblData_Monthly->setModel(NULL);
    ui->tblData_60min->setModel(NULL);
    ui->tblData_5min->setModel(NULL);

    thread_pool->clear();
  }

  SQLiteHandler::removeDatabase(&model_database);
}

void MainWindow::clear_program_result(const QString &symbol) {
  QDir dir = result_dir;
  QStringList nameFilter("*." + QString::number(SBAR_ID) + ".sbar");
  QStringList files = dir.entryList(nameFilter);
  QString f;

  for (int i = 0; i < files.length(); ++i) {
    f = result_dir + "/" + files[i];
    if (f.contains(symbol))
      QFile::remove(f);
  }
}

void MainWindow::export_result_to_file(const QString &symbol, const QString &interval, QSqlQuery *query,
                                       const QSqlDatabase &dbQuery, const bool &delete_flag) {
  QString filename = result_dir + "/" + symbol + "_" + interval + "." + QString::number(SBAR_ID)+ ".sbar";
  mutex.lock();
  thread_pool->start(new SearchResultWriter(query, filename, dbQuery, delete_flag), QThread::NormalPriority);
  mutex.unlock();
}

QString MainWindow::parse_filename(const QString &filename, QString *out_symbol = NULL, IntervalWeight *out_interval = NULL) {
  QStringList s = filename.split(".");
  QStringList symbol_interval = s[0].split("_");

  if (!list_symbol.contains(symbol_interval[0])) {
    list_symbol.append(symbol_interval[0]);
  }

  if (out_symbol != NULL) {
    *out_symbol = symbol_interval[0];
  }

  if (out_interval != NULL) {
    *out_interval = SQLiteHandler::get_interval_weight(symbol_interval[1]);
  }

  return s[0] + ".db";
}

void MainWindow::prepare_database_last_rowid() {
  database_last_rowid.clear();

  QString database_name;
  IntervalWeight w;
  int last_rowid;
  int symbol_length = list_symbol.length();

  for (int i = 0; i < symbol_length; ++i) {
    w = WEIGHT_MONTHLY;
    while (w != WEIGHT_INVALID) {
      database_name = sqlitedb.getDatabaseName(list_symbol[i], w);
      last_rowid = BarDataAdapter::getLastRowid(sqlitedb.getDatabase(database_name));
      database_last_rowid.insert(database_name, last_rowid);
      w = SQLiteHandler::get_child_interval(w);
    }
  }
}

void MainWindow::prepare_database_update_flag() {
  QMap<QString,int>::const_iterator it = database_last_rowid.constBegin();
  int last_rowid_old;
  int last_rowid_new;
  database_update_flag.clear();

  while (it != database_last_rowid.constEnd()) {
    last_rowid_old = it.value();
    last_rowid_new = BarDataAdapter::getLastRowid(sqlitedb.getDatabase(it.key()));
    database_update_flag.insert(it.key(), (last_rowid_old != last_rowid_new));
    ++it;
  }
}

inline QString concat_threshold_value(const QString &op, const QString &value) {
  if (value.toDouble() == 0) return "";
  return op + BarDataAdapter::remove_trailing_zero(value);
}

void MainWindow::refresh_threshold_component() {
  QString symbol = ui->cbSymbol->currentText();
  QString macd_v1 = QString::number( t_macd[ tid_macd - 1 ].value1 );
  QString macd_v2 = QString::number(t_macd[ tid_macd - 1 ].value2);
  QString rsi_v1 = QString::number(t_rsi[ tid_rsi - 1 ].value1);
  QString rsi_v2 = QString::number(t_rsi[ tid_rsi - 1 ].value2);
  QString slowk_v1 = QString::number(t_slowk[ tid_slowk - 1 ].value1);
  QString slowk_v2 = QString::number(t_slowk[ tid_slowk - 1 ].value2);
  QString slowd_v1 = QString::number(t_slowd[ tid_slowd - 1 ].value1);
  QString slowd_v2 = QString::number(t_slowd[ tid_slowd - 1 ].value2);
  QString cgf = "C > F" + concat_threshold_value( "-" , t_cgf[symbol][ tid_cgf ].value );
  QString clf = "C < F" + concat_threshold_value( "+" , t_clf[symbol][ tid_clf ].value );
  QString cgs = "C > S" + concat_threshold_value( "-" , t_cgs[symbol][ tid_cgs ].value );
  QString cls = "C < S" + concat_threshold_value( "+" , t_cls[symbol][ tid_cls ].value );

  filter->set_macd_threshold( tid_macd ); // pass non-zero based
  filter->set_rsi_threshold( tid_rsi );
  filter->set_slowk_threshold( tid_slowk );
  filter->set_slowd_threshold( tid_slowd );
  filter->set_cgf_threshold( tid_cgf ); // zero-based
  filter->set_clf_threshold( tid_clf );
  filter->set_cgs_threshold( tid_cgs );
  filter->set_cls_threshold( tid_cls );
  filter->refresh_column_name(symbol);

  label_threshold_params.setText(
    "(MACD: " + macd_v1 + "/" + macd_v2 + ") "
    "(RSI: " + rsi_v1 + "/" + rsi_v2 + ") "
    "(SlowK: " + slowk_v1 + "/" + slowk_v2 + ") "
    "(SlowD: " + slowd_v1 + "/" + slowd_v2 + ") " +
    ((clf.length() > 5)? "(" + clf + ") " : "") +
    ((cgf.length() > 5)? "(" + cgf + ") " : "") +
    ((cls.length() > 5)? "(" + cls + ") " : "") +
    ((cgs.length() > 5)? "(" + cgs + ") " : "") );
}

void MainWindow::set_threshold_parameters(const QString &symbol) {
  QString _symbol = symbol;

  if (_symbol.isEmpty()) {
    _symbol = ui->cbSymbol->currentText();
  }

  QString display_text;
  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  list_threshold = config->get_list_threshold();
  list_interval_weight = config->get_interval_weight();
  list_interval_threshold = config->get_interval_threshold();
  QVector<XmlConfigHandler::t_sr_threshold> vec = list_threshold[_symbol];

  ui->cbThreshold->clear();
  for (int i = 0; i < vec.length(); ++i) {
    if (vec[i].test_point != 0 && vec[i].break_threshold != 0) {
      display_text = QString::number(vec[i].break_threshold);
      ui->cbThreshold->addItem(display_text, vec[i].t_id);
    }
  }
}

void MainWindow::set_data_range_label() {
  QString symbol = ui->cbSymbol->currentData().toString();
  QDateTime dt;

  if (!symbol.isEmpty()) {
    ui->groupBox->setTitle(symbol + " Data Range");
    QString empty_data_message = "<span style=color:#aa0000;>No data</span>";
    ui->lblTo1->setText("To");
    ui->lblTo2->setText("To");
    ui->lblTo3->setText("To");
    ui->lblTo4->setText("To");
    ui->lblTo5->setText("To");
    ui->lblTo6->setText("To");

    dt = BarDataAdapter::selectMinDate(sqlitedb.getDatabase(symbol,"1min"));
    if (!dt.isNull()) {
      ui->lbl1minFirst->setText(dt.toString("MMM/dd/yyyy hh:mm"));
      dt = BarDataAdapter::selectMaxDate(sqlitedb.getDatabase(symbol,"1min"));
      ui->lbl1minLast->setText(dt.toString("MMM/dd/yyyy hh:mm"));
    } else {
      ui->lbl1minFirst->setText(empty_data_message);
      ui->lbl1minLast->setText("");
      ui->lblTo1->setText("");
    }

    dt = BarDataAdapter::selectMinDate(sqlitedb.getDatabase(symbol,"5min"));
    if (!dt.isNull()) {
      ui->lbl5minFirst->setText(dt.toString("MMM/dd/yyyy hh:mm"));
      dt = BarDataAdapter::selectMaxDate(sqlitedb.getDatabase(symbol,"5min"));
      ui->lbl5minLast->setText(dt.toString("MMM/dd/yyyy hh:mm"));
    } else {
      ui->lbl5minFirst->setText(empty_data_message);
      ui->lbl5minLast->setText("");
      ui->lblTo2->setText("");
    }

    dt = BarDataAdapter::selectMinDate(sqlitedb.getDatabase(symbol,"60min"));
    if (!dt.isNull()) {
      ui->lbl60minFirst->setText(dt.toString("MMM/dd/yyyy hh:mm"));
      dt = BarDataAdapter::selectMaxDate(sqlitedb.getDatabase(symbol,"60min"));
      ui->lbl60minLast->setText(dt.toString("MMM/dd/yyyy hh:mm"));
    } else {
      ui->lbl60minFirst->setText(empty_data_message);
      ui->lbl60minLast->setText("");
      ui->lblTo3->setText("");
    }

    dt = BarDataAdapter::selectMinDate(sqlitedb.getDatabase(symbol,"Daily"));
    if (!dt.isNull()) {
      ui->lblDailyFirst->setText(dt.toString("MMM/dd/yyyy"));
      dt = BarDataAdapter::selectMaxDate(sqlitedb.getDatabase(symbol,"Daily"));
      ui->lblDailyLast->setText(dt.toString("MMM/dd/yyyy"));
    } else {
      ui->lblDailyFirst->setText(empty_data_message);
      ui->lblDailyLast->setText("");
      ui->lblTo4->setText("");
    }

    dt = BarDataAdapter::selectMinDate(sqlitedb.getDatabase(symbol,"Weekly"));
    if (!dt.isNull()) {
      ui->lblWeeklyFirst->setText(dt.toString("MMM/dd/yyyy"));
      dt = BarDataAdapter::selectMaxDate(sqlitedb.getDatabase(symbol,"Weekly"));
      ui->lblWeeklyLast->setText(dt.toString("MMM/dd/yyyy"));
    } else {
      ui->lblWeeklyFirst->setText(empty_data_message);
      ui->lblWeeklyLast->setText("");
      ui->lblTo5->setText("");
    }

    dt = BarDataAdapter::selectMinDate(sqlitedb.getDatabase(symbol,"Monthly"));
    if (!dt.isNull()) {
      ui->lblMonthlyFirst->setText(dt.toString("MMM/dd/yyyy"));
      dt = BarDataAdapter::selectMaxDate(sqlitedb.getDatabase(symbol,"Monthly"));
      ui->lblMonthlyLast->setText(dt.toString("MMM/dd/yyyy"));
    } else {
      ui->lblMonthlyFirst->setText(empty_data_message);
      ui->lblMonthlyLast->setText("");
      ui->lblTo6->setText("");
    }
  }
}

void MainWindow::set_table_view(QTableView *table, const IntervalWeight &base_interval, const IntervalWeight &projection_interval) {
  static const int MAX_WIDTH_1 = 56;
  static const int MAX_WIDTH_2A = 60;
  static const int MAX_WIDTH_2B = 65;
  static const int MAX_WIDTH_2C = 70;
  static const int MAX_WIDTH_3 = 80;
  static const int MAX_WIDTH_4 = 85;
  static const int MAX_WIDTH_4B = 90;
  static const int MAX_WIDTH_5 = 100;
  static const int MAX_WIDTH_6 = 110;
  int m_index = 0;

  table->setColumnWidth(m_index++, 70); // date
  table->setColumnWidth(m_index++, 40); // time
  table->setColumnWidth(m_index++, MAX_WIDTH_1);
  table->setColumnWidth(m_index++, MAX_WIDTH_1);
  table->setColumnWidth(m_index++, MAX_WIDTH_1);
  table->setColumnWidth(m_index++, MAX_WIDTH_1);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2A); // Volume
  table->setColumnWidth(m_index++, MAX_WIDTH_2C); // MACD
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  // 10
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // MACD < 0
  table->setColumnWidth(m_index++, MAX_WIDTH_3);  // MACD < 0 (R)
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // MACD > 0
  table->setColumnWidth(m_index++, MAX_WIDTH_3);  // MACD > 0 (R)
  table->setColumnWidth(m_index++, MAX_WIDTH_2C); // MACDAvg
  table->setColumnWidth(m_index++, MAX_WIDTH_2C); // MACDDiff
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // RSI < 30
  table->setColumnWidth(m_index++, MAX_WIDTH_2C);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // RSI > 70
  // 20
  table->setColumnWidth(m_index++, MAX_WIDTH_2C);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowK
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowK < 20
  table->setColumnWidth(m_index++, MAX_WIDTH_4B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowK > 80
  table->setColumnWidth(m_index++, MAX_WIDTH_4B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowD
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowD < 20
  table->setColumnWidth(m_index++, MAX_WIDTH_4B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowD > 80
  table->setColumnWidth(m_index++, MAX_WIDTH_4B);

  if (base_interval < WEIGHT_DAILY && projection_interval == WEIGHT_DAILY) {
    table->setColumnWidth(m_index++, MAX_WIDTH_2B); // PrevDailyATR
  }
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // ATR
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // ATR (R)
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // FastAvg
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // SlowAvg

  if (base_interval < WEIGHT_DAILY && projection_interval == WEIGHT_DAILY) {
    table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Day-10 (F)
    table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Day-50 (F)
  }

  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // F-Slope
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // S-Slope
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(O-F)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(H-F)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(L-F)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(C-F)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(O-S)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(H-S)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(L-S)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(C-S)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Dist(F-S)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_4 +5); // Dist(F-S) Cross
  table->setColumnWidth(m_index++, MAX_WIDTH_6 +5); // Dist(F-S) Cross (ATR)
  table->setColumnWidth(m_index++, MAX_WIDTH_5 +5); // Dist(F-S) Cross (R)
  table->setColumnWidth(m_index++, 50);           // F < S
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // F < S (R)
  table->setColumnWidth(m_index++, 50);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // F > S (R)
  table->setColumnWidth(m_index++, 70);
  table->setColumnWidth(m_index++, MAX_WIDTH_4);
  table->setColumnWidth(m_index++, 70);
  table->setColumnWidth(m_index++, MAX_WIDTH_4);
  table->setColumnWidth(m_index++, 70);
  table->setColumnWidth(m_index++, MAX_WIDTH_4); // C < S (R)
  table->setColumnWidth(m_index++, 70);          // C > S
  table->setColumnWidth(m_index++, MAX_WIDTH_4); // C > S (R)
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Uptail
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  table->setColumnWidth(m_index++, MAX_WIDTH_3); // Downtail
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  // 70
  table->setColumnWidth(m_index++, MAX_WIDTH_2B); // Body
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  table->setColumnWidth(m_index++, MAX_WIDTH_2B);
  table->setColumnWidth(m_index++, MAX_WIDTH_3); // TotalLength
  table->setColumnWidth(m_index++, MAX_WIDTH_4);
  table->setColumnWidth(m_index++, MAX_WIDTH_4);
  table->setColumnWidth(m_index++, MAX_WIDTH_4); // Resistance (D)
  table->setColumnWidth(m_index++, MAX_WIDTH_4);
  table->setColumnWidth(m_index++, MAX_WIDTH_6);
  table->setColumnWidth(m_index++, MAX_WIDTH_5);
  // 80
  table->setColumnWidth(m_index++, MAX_WIDTH_3); // Support (D)
  table->setColumnWidth(m_index++, MAX_WIDTH_3);
  table->setColumnWidth(m_index++, MAX_WIDTH_5);
  table->setColumnWidth(m_index++, MAX_WIDTH_4);

  if ( table->model()->headerData(m_index, Qt::Horizontal) == "DateRange" ) {
    table->setColumnWidth(m_index++, MAX_WIDTH_2C); // DateRange
  }

  table->setColumnWidth(m_index++, MAX_WIDTH_1); // F-Cross
  table->setColumnWidth(m_index++, MAX_WIDTH_1); // S-Cross
  table->setColumnWidth(m_index++, MAX_WIDTH_2C +5); // Zone

  QString column_name;
  int N = table->model()->columnCount();

  for (int i = m_index; i < N; ++i) {
    column_name = table->model()->headerData(i, Qt::Horizontal).toString();

    if (!column_name.contains("Zone") && !column_name.endsWith("(#)")) break;

    if (column_name.endsWith("(#)")) {
      table->setColumnWidth(i, MAX_WIDTH_3); // Resistance (#)/ Support (#)
    } else {
      table->setColumnWidth(i, MAX_WIDTH_4); // Zone
    }
  }
}

void MainWindow::set_tab_widget_interval(const IntervalWeight &fromInterval) {
  while (ui->tabWidget->count() > 1) {
    ui->tabWidget->removeTab(1);
  }

  IntervalWeight result_interval = SQLiteHandler::get_child_interval(fromInterval);
  if (result_interval == WEIGHT_INVALID) {
    result_interval = WEIGHT_LARGEST;
  }

  ui->tabWidget->setTabText(0, SQLiteHandler::get_interval_name(result_interval));
  ui->tblData_5min->setModel(NULL);
  ui->tblData_60min->setModel(NULL);
  ui->tblData_Daily->setModel(NULL);
  ui->tblData_Weekly->setModel(NULL);
  ui->tblData_Monthly->setModel(NULL);

  if (fromInterval != WEIGHT_INVALID) {
    QString interval_name;
    IntervalWeight w = WEIGHT_LARGEST;
    while (w >= fromInterval) {
      interval_name = SQLiteHandler::get_interval_name(w);
      switch (w) {
        case WEIGHT_5MIN: ui->tabWidget->insertTab(1, ui->tab_5min, interval_name); break;
        case WEIGHT_60MIN: ui->tabWidget->insertTab(1, ui->tab_60min, interval_name); break;
        case WEIGHT_DAILY: ui->tabWidget->insertTab(1, ui->tab_Daily, interval_name); break;
        case WEIGHT_WEEKLY: ui->tabWidget->insertTab(1, ui->tab_Weekly, interval_name); break;
        case WEIGHT_MONTHLY: ui->tabWidget->insertTab(1, ui->tab_Monthly, interval_name); break;
        default: break;
      }
      w = SQLiteHandler::get_child_interval(w);
    }
  }
  vec_label_string.resize(ui->tabWidget->count());
  prevtab_idx = 0;
}

void MainWindow::enable_components(const bool &flag) {
  is_enable_components = flag;
  ui->btnClearResult->setEnabled(flag);
  ui->cbSymbol->setEnabled(flag);
  ui->cbThreshold->setEnabled(flag);
  ui->chkPlotMatcher->setEnabled(flag);
  ui->btnRefresh->setEnabled(flag);
  ui->btnSearch->setEnabled(flag);
  ui->btnSearch->blockSignals(!flag);
  ui->btnExport->setEnabled(flag);
  ui->tableWidget->setEnabled(flag);
  ui->tabWidget->setEnabled(flag);
  if (!flag) {
    clear_model();
    ui->tblData->setModel(NULL);
  }
}

QString MainWindow::getSymbol() const {
  return ui->cbSymbol->currentText();
}

//
// Intersect with hanci's Matcher Result
//
void MainWindow::on_chkPlotMatcher_stateChanged(const int &value) {
  matcher_rowid.clear();
  if (value == Qt::Checked) {
    matcher_rowid.append("0");
    QString symbol = ui->cbSymbol->currentData().toString();
    QString interval = filter->get_smallest_interval();
    QString base_filename = result_dir + "/" + symbol + "_" + interval;
    QString filename;
    for (int i = 0; i < 4; ++i) {
      filename = base_filename + "_" + QString::number(i) + ".txt";
      read_matcher_file_v2(filename, symbol, interval);
    }
    matcher_rowid.removeDuplicates();
  }
}

void MainWindow::read_matcher_file_v2(const QString &filename, const QString &symbol,const QString &interval) {
  QFile file(filename);

  if (!file.exists()) return;

  if (file.open(QFile::ReadOnly)) {
    QStringList _date;
    QStringList _time;
    QStringList line;
    QTextStream stream(&file);

    stream.readLine(); // number of rows (discard)
    int series_length = stream.readLine().toInt(); // series length
    while (!stream.atEnd()) {
      line = stream.readLine().split(" ");
      _date.push_front(line[0].replace("/","-"));
      _time.push_front(line[1]);
    }

    file.close();
//    qDebug() << _date.length() << _time.length() << _date[0] << _time[0];
    for (int i = 0; i < _date.length(); ++i) {
      matcher_rowid += BarDataAdapter::select_rowid_from_series(sqlitedb.getDatabase(symbol,interval), _date[i], _time[i], series_length);
    }
  }
}
