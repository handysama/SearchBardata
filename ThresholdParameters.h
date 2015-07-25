#ifndef THRESHOLDPARAMETERS_H
#define THRESHOLDPARAMETERS_H

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>
#include "XmlConfigHandler.h"

namespace Ui {
  class ThresholdParameters;
}

class ThresholdParameters : public QDialog {
  Q_OBJECT

  public:
    explicit ThresholdParameters(QWidget *parent = 0);
    ~ThresholdParameters();

  private slots:
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_cbSymbol_currentIndexChanged(const int &index);
    void on_chkCGF_1_stateChanged(const int &state);
    void on_chkCGF_2_stateChanged(const int &state);
    void on_chkCGF_3_stateChanged(const int &state);
    void on_chkCLF_1_stateChanged(const int &state);
    void on_chkCLF_2_stateChanged(const int &state);
    void on_chkCLF_3_stateChanged(const int &state);
    void on_chkCGS_1_stateChanged(const int &state);
    void on_chkCGS_2_stateChanged(const int &state);
    void on_chkCGS_3_stateChanged(const int &state);
    void on_chkCLS_1_stateChanged(const int &state);
    void on_chkCLS_2_stateChanged(const int &state);
    void on_chkCLS_3_stateChanged(const int &state);

  private:
    Ui::ThresholdParameters *ui;
    QWidget *_parent = NULL;
    QStringList list_symbol;
    QList<QDoubleSpinBox *> list_b_value;
    QList<QComboBox *> MACD_operator1;
    QList<QComboBox *> MACD_operator2;
    QList<QComboBox *> RSI_operator1;
    QList<QComboBox *> RSI_operator2;
    QList<QComboBox *> SlowK_operator1;
    QList<QComboBox *> SlowK_operator2;
    QList<QComboBox *> SlowD_operator1;
    QList<QComboBox *> SlowD_operator2;
    QList<QDoubleSpinBox *> MACD_value1;
    QList<QDoubleSpinBox *> MACD_value2;
    QList<QDoubleSpinBox *> RSI_value1;
    QList<QDoubleSpinBox *> RSI_value2;
    QList<QDoubleSpinBox *> SlowK_value1;
    QList<QDoubleSpinBox *> SlowK_value2;
    QList<QDoubleSpinBox *> SlowD_value1;
    QList<QDoubleSpinBox *> SlowD_value2;
    QList<QDoubleSpinBox *> CGF_value;
    QList<QDoubleSpinBox *> CLF_value;
    QList<QDoubleSpinBox *> CGS_value;
    QList<QDoubleSpinBox *> CLS_value;
    QList<QCheckBox *> CGF_disable_flag;
    QList<QCheckBox *> CLF_disable_flag;
    QList<QCheckBox *> CGS_disable_flag;
    QList<QCheckBox *> CLS_disable_flag;
    QMap<QString,QVector<XmlConfigHandler::t_sr_threshold>> sr_threshold;
    QMap<int,int> sr_intersect_threshold;
    QVector<XmlConfigHandler::t_threshold_1> t_macd;
    QVector<XmlConfigHandler::t_threshold_1> t_rsi;
    QVector<XmlConfigHandler::t_threshold_1> t_slowk;
    QVector<XmlConfigHandler::t_threshold_1> t_slowd;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cgf;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_clf;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cgs;
    QMap<QString,QVector<XmlConfigHandler::t_threshold_2>> t_cls;
    QString config_path;

    void initialize();
};

#endif // THRESHOLDPARAMETERS_H
