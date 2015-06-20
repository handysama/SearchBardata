#ifndef THRESHOLDPARAMETERS_H
#define THRESHOLDPARAMETERS_H

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
    void on_cbSymbol_currentIndexChanged(const int &index);

  private:
    Ui::ThresholdParameters *ui;
    QWidget *_parent = NULL;
    QStringList list_symbol;
    QList<QDoubleSpinBox *> list_b_value;
    QMap<QString,QVector<XmlConfigHandler::t_threshold>> sr_threshold;
    QMap<int,int> sr_intersect_threshold;
    QString config_path;

    void initialize();
};

#endif // THRESHOLDPARAMETERS_H
