#ifndef CHANGETHRESHOLDDIALOG_H
#define CHANGETHRESHOLDDIALOG_H

#include <QComboBox>
#include <QDialog>
#include "BarDataAdapter.h"
#include "XmlConfigHandler.h"

namespace Ui {
  class ChangeThresholdDialog;
}

class ChangeThresholdDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ChangeThresholdDialog(QWidget *parent = 0);
    ~ChangeThresholdDialog();
    void initialize();
    void set_current_tab(const int &index);
    void set_symbol(const int &tab_id, const QString &s);

    //
    // Getter Function
    //
    int get_macd_threshold(const int &tab_id) const {
      return ptr_macd[tab_id]->currentData().toInt();
    }

    int get_rsi_threshold(const int &tab_id) const {
      return ptr_rsi[tab_id]->currentData().toInt();
    }

    int get_slowk_threshold(const int &tab_id) const {
      return ptr_slowk[tab_id]->currentData().toInt();
    }

    int get_slowd_threshold(const int &tab_id) const {
      return ptr_slowd[tab_id]->currentData().toInt();
    }

    int get_cgf_threshold(const int &tab_id) const {
      return ptr_cgf[tab_id]->currentData().toInt();
    }

    int get_clf_threshold(const int &tab_id) const {
      return ptr_clf[tab_id]->currentData().toInt();
    }

    int get_cgs_threshold(const int &tab_id) const {
      return ptr_cgs[tab_id]->currentData().toInt();
    }

    int get_cls_threshold(const int &tab_id) const {
      return ptr_cls[tab_id]->currentData().toInt();
    }

    //
    // Setter Function
    //
    void set_macd_threshold(const int &tab_id, const int &index) {
      ptr_macd[tab_id]->setCurrentIndex(index);
    }

    void set_rsi_threshold(const int &tab_id, const int &index) {
      ptr_rsi[tab_id]->setCurrentIndex(index);
    }

    void set_slowk_threshold(const int &tab_id, const int &index) {
      ptr_slowk[tab_id]->setCurrentIndex(index);
    }

    void set_slowd_threshold(const int &tab_id, const int &index) {
      ptr_slowd[tab_id]->setCurrentIndex(index);
    }

    void set_cgf_threshold(const int &tab_id, const int &index) {
      ptr_cgf[tab_id]->setCurrentIndex(index);
    }

    void set_clf_threshold(const int &tab_id, const int &index) {
      ptr_clf[tab_id]->setCurrentIndex(index);
    }

    void set_cgs_threshold(const int &tab_id, const int &index) {
      ptr_cgs[tab_id]->setCurrentIndex(index);
    }

    void set_cls_threshold(const int &tab_id, const int &index) {
      ptr_cls[tab_id]->setCurrentIndex(index);
    }

  private slots:
    void on_btnSave_clicked() {
      this->accept();
    }

    void on_btnCancel_clicked() {
      this->reject();
    }

  private:
    Ui::ChangeThresholdDialog *ui;

    // Pointer to combobox to make it iterable
    QList<QComboBox*> ptr_macd;
    QList<QComboBox*> ptr_rsi;
    QList<QComboBox*> ptr_slowk;
    QList<QComboBox*> ptr_slowd;
    QList<QComboBox*> ptr_cgf;
    QList<QComboBox*> ptr_clf;
    QList<QComboBox*> ptr_cgs;
    QList<QComboBox*> ptr_cls;
    QVector<XmlConfigHandler::t_threshold_1> t_macd;
    QVector<XmlConfigHandler::t_threshold_1> t_rsi;
    QVector<XmlConfigHandler::t_threshold_1> t_slowk;
    QVector<XmlConfigHandler::t_threshold_1> t_slowd;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cgf;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_clf;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cgs;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cls;

    QVector<QString> symbol; // track symbol in each tab
    QVector<int> m_macd;
    QVector<int> m_rsi;
    QVector<int> m_slowk;
    QVector<int> m_slowd;
    QVector<int> m_cgf;
    QVector<int> m_clf;
    QVector<int> m_cgs;
    QVector<int> m_cls;
};

#endif // CHANGETHRESHOLDDIALOG_H
