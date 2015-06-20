#ifndef MASTERWINDOW_H
#define MASTERWINDOW_H

#include <QDesktopWidget>
#include <QMainWindow>
#include <QShortcut>
#include <QWidget>

#include "MainWindow.h"
#include "ThresholdParameters.h"
#include "XmlConfigHandler.h"

namespace Ui {
  class MasterWindow;
}

class MasterWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MasterWindow(QWidget *parent = 0);
    ~MasterWindow();

  private slots:
    void resizeEvent(QResizeEvent *e);
    void start_updatingDatabaseEvent();
    void finish_updatingDatabaseEvent();
    void on_actionOpen_triggered();
    void on_actionSave_Conditions_triggered();

    void on_actionQuit_triggered() {
      close();
    }

    void on_actionUpdate_Database_triggered() {
    }

    void on_actionThresholdParameters_triggered() {
      threshold_dialog->show();
    }

    void setNormalWindow() {
      resize(1082, this->height());
      setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QWidget::window()->size(),qApp->desktop()->availableGeometry()));
    }

    void setSmallWindow() {
      this->resize(704, this->height());
    }

  private:
    Ui::MasterWindow *ui;
    QList<MainWindow*> mainwindow_list;
    ThresholdParameters *threshold_dialog = NULL;

    void initialize();
};

#endif // MASTERWINDOW_H
