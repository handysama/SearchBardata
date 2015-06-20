#include "MasterWindow.h"
#include "ui_MasterWindow.h"

MasterWindow::MasterWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MasterWindow) {
  ui->setupUi(this);
  setWindowTitle("Search Bardata v"+QString(VERSION));
  setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QWidget::window()->size(),qApp->desktop()->availableGeometry()));

  ui->menuOption->hide();
  ui->menuOption->menuAction()->setVisible(false);
  ui->actionUpdate_Database->setVisible(false);
  ui->tabWidget->tabBar()->setStyleSheet(
    "QTabBar::tab:selected {background:#3C3C3C; border: 2px solid #3C3C3C; "
      "border-top-left-radius: 5px;"
      "border-top-right-radius: 5px; "
      "min-width: 18ex; padding: 2px; } "
    "QTabBar::tab:!selected {background:#878787; border: 2px solid #878787; "
      "border-top-left-radius: 5px;"
      "border-top-right-radius: 5px; "
      "min-width: 18ex; padding: 2px; }"
        );

  QColor text_color[5] = {Qt::yellow, Qt::cyan, Qt::green, Qt::magenta, Qt::red};

  // initialize
  for (int i = 0; i < 5; ++i) {
    MainWindow *w = new MainWindow(i);
    mainwindow_list.append(w);
    ui->tabWidget->addTab(mainwindow_list[i], "Tab " + QString::number(i+1));
    ui->tabWidget->tabBar()->setTabTextColor(i, text_color[i]);
    connect(w, SIGNAL(start_updating_database()), this, SLOT(start_updatingDatabaseEvent()));
    connect(w, SIGNAL(finish_updating_database()), this, SLOT(finish_updatingDatabaseEvent()));
  }

  // dialog
  threshold_dialog = new ThresholdParameters();

  // shortcut key
  new QShortcut(QKeySequence(Qt::ALT + Qt::Key_1), this, SLOT(setSmallWindow()));
  new QShortcut(QKeySequence(Qt::ALT + Qt::Key_0), this, SLOT(setNormalWindow()));
}

MasterWindow::~MasterWindow() {
  delete threshold_dialog;
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
