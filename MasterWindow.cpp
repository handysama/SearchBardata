#include "MasterWindow.h"
#include "ui_MasterWindow.h"
#define MAX_SBAR_TAB 5

MasterWindow::MasterWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MasterWindow) {
  ui->setupUi(this);
  setWindowTitle("Search Bardata v"+QString(VERSION));
  setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QWidget::window()->size(),qApp->desktop()->availableGeometry()));

//  ui->menuOption->hide();
//  ui->menuOption->menuAction()->setVisible(false);
  ui->actionUpdate_Database->setVisible(false);

  ui->tabWidget->tabBar()->setStyleSheet(
    "QTabBar::tab:selected {background:#3C3C3C; border: 2px solid #3C3C3C; "
      "border-top-left-radius: 5px;"
      "border-top-right-radius: 5px; "
      "min-width: 18ex; padding: 2px; } "
    "QTabBar::tab:!selected {background:#878787; border: 2px solid #878787; "
      "border-top-left-radius: 5px;"
      "border-top-right-radius: 5px; "
      "min-width: 18ex; padding: 2px; }");

  QColor text_color[MAX_SBAR_TAB] = {Qt::yellow, Qt::cyan, Qt::green, Qt::magenta, Qt::red};

  // initialize
  for (int i = 0; i < MAX_SBAR_TAB; ++i) {
    MainWindow *w = new MainWindow(i);
    mainwindow_list.append(w);
    ui->tabWidget->addTab(mainwindow_list[i], "Tab " + QString::number(i+1));
    ui->tabWidget->tabBar()->setTabTextColor(i, text_color[i]);
    connect(w, SIGNAL(start_updating_database()), this, SLOT(start_updatingDatabaseEvent()));
    connect(w, SIGNAL(finish_updating_database()), this, SLOT(finish_updatingDatabaseEvent()));
    tid_macd.append(0);
    tid_rsi.append(0);
    tid_slowk.append(0);
    tid_slowd.append(0);
    tid_cgf.append(0);
    tid_clf.append(0);
    tid_cgs.append(0);
    tid_cls.append(0);
  }

  // dialog
  change_threshold_dialog = new ChangeThresholdDialog(this);
  set_threshold_dialog = new ThresholdParameters(this);

  // shortcut key
  new QShortcut(QKeySequence(Qt::ALT + Qt::Key_1), this, SLOT(setSmallWindow()));
  new QShortcut(QKeySequence(Qt::ALT + Qt::Key_0), this, SLOT(setNormalWindow()));
}

MasterWindow::~MasterWindow() {
  delete change_threshold_dialog;
  delete set_threshold_dialog;
  delete ui;
}

void MasterWindow::resizeEvent(QResizeEvent *e) {
  int t_width = ui->centralwidget->width() + 2;
  int t_height = ui->centralwidget->height();
  ui->tabWidget->setGeometry(0, 0, t_width, t_height);
  QMainWindow::resizeEvent(e);
}

void MasterWindow::start_updatingDatabaseEvent() {
  ui->tabWidget->setEnabled(false);
  ui->menubar->setEnabled(false);
}

void MasterWindow::finish_updatingDatabaseEvent() {
  ui->tabWidget->setEnabled(true);
  ui->menubar->setEnabled(true);
}

void MasterWindow::on_actionOpen_triggered() {
  QString filename = QFileDialog::getOpenFileName(this, "Open Search Conditions",
    QCoreApplication::applicationDirPath(), "Text files (*.txt)");

  if (!filename.isEmpty()) {
    int current_tab = ui->tabWidget->currentIndex();
    mainwindow_list[current_tab]->openSearchConditions(filename);
  }
}

void MasterWindow::on_actionSave_Conditions_triggered() {
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Search Conditions"),
    QCoreApplication::applicationDirPath(), "Text File (*.txt)");

  if (!filename.isEmpty()) {
    int current_tab = ui->tabWidget->currentIndex();
    mainwindow_list[current_tab]->saveSearchConditions(filename);
  }
}

void MasterWindow::on_actionChange_Threshold_triggered() {
  change_threshold_dialog->initialize();
  change_threshold_dialog->set_current_tab(ui->tabWidget->currentIndex());

  //
  // read threshold value from each window tab
  //
  for (int i = 0; i < MAX_SBAR_TAB; ++i) {
    change_threshold_dialog->set_symbol(i, mainwindow_list[i]->getSymbol());
    change_threshold_dialog->set_macd_threshold(i, mainwindow_list[i]->get_macd_threshold() -1);
    change_threshold_dialog->set_rsi_threshold(i, mainwindow_list[i]->get_rsi_threshold() -1);
    change_threshold_dialog->set_slowk_threshold(i, mainwindow_list[i]->get_slowk_threshold() -1);
    change_threshold_dialog->set_slowd_threshold(i, mainwindow_list[i]->get_slowd_threshold() -1);
    change_threshold_dialog->set_cgf_threshold(i, mainwindow_list[i]->get_cgf_threshold());
    change_threshold_dialog->set_clf_threshold(i, mainwindow_list[i]->get_clf_threshold());
    change_threshold_dialog->set_cgs_threshold(i, mainwindow_list[i]->get_cgs_threshold());
    change_threshold_dialog->set_cls_threshold(i, mainwindow_list[i]->get_cls_threshold());
  }

  if (change_threshold_dialog->exec() == QDialog::Accepted) {
    //
    // overwrite threshold with user new preferance
    //
    for (int i = 0; i < MAX_SBAR_TAB; ++i) {
      tid_macd[i] = change_threshold_dialog->get_macd_threshold(i);
      tid_rsi[i] = change_threshold_dialog->get_rsi_threshold(i);
      tid_slowk[i] = change_threshold_dialog->get_slowk_threshold(i);
      tid_slowd[i] = change_threshold_dialog->get_slowd_threshold(i);
      tid_cgf[i] = change_threshold_dialog->get_cgf_threshold(i);
      tid_clf[i] = change_threshold_dialog->get_clf_threshold(i);
      tid_cgs[i] = change_threshold_dialog->get_cgs_threshold(i);
      tid_cls[i] = change_threshold_dialog->get_cls_threshold(i);

      mainwindow_list[i]->set_macd_threshold_id( tid_macd[i] );
      mainwindow_list[i]->set_rsi_threshold_id( tid_rsi[i] );
      mainwindow_list[i]->set_slowk_threshold_id( tid_slowk[i] );
      mainwindow_list[i]->set_slowd_threshold_id( tid_slowd[i] );
      mainwindow_list[i]->set_cgf_threshold_id( tid_cgf[i] );
      mainwindow_list[i]->set_clf_threshold_id( tid_clf[i] );
      mainwindow_list[i]->set_cgs_threshold_id( tid_cgs[i] );
      mainwindow_list[i]->set_cls_threshold_id( tid_cls[i] );
      mainwindow_list[i]->refresh_threshold_component();
    }
  }
}
