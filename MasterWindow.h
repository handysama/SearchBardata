#ifndef MASTERWINDOW_H
#define MASTERWINDOW_H

#include <QDesktopWidget>
#include <QMainWindow>
#include <QShortcut>
#include <QWidget>

#include "ChangeThresholdDialog.h"
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
    void on_actionChange_Threshold_triggered();

    void on_actionQuit_triggered() {
      close();
    }

    void on_actionUpdate_Database_triggered() {
    }

    void on_actionThresholdParameters_triggered() {
      if (set_threshold_dialog->exec() == QDialog::Accepted) {
        // TODO: refresh threshold in MainWindow later

      }
    }

    void setNormalWindow() {
      resize(1153, this->height());
      setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QWidget::window()->size(),qApp->desktop()->availableGeometry()));
    }

    void setSmallWindow() {
      this->resize(809, this->height());
    }

  private:
    Ui::MasterWindow *ui;
    QList<MainWindow*> mainwindow_list;
    ChangeThresholdDialog *change_threshold_dialog = NULL;
    ThresholdParameters *set_threshold_dialog = NULL;
    QVector<int> tid_macd;
    QVector<int> tid_rsi;
    QVector<int> tid_slowk;
    QVector<int> tid_slowd;
    QVector<int> tid_cgf;
    QVector<int> tid_clf;
    QVector<int> tid_cgs;
    QVector<int> tid_cls;

    void initialize();
};

#endif // MASTERWINDOW_H
