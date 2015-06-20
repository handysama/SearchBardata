#include "ThresholdParameters.h"
#include "ui_ThresholdParameters.h"

ThresholdParameters::ThresholdParameters(QWidget *parent) :
  QDialog(parent), ui(new Ui::ThresholdParameters) {
  ui->setupUi(this);
  setFixedSize(width(), height());

  ui->label_6->hide();
  ui->label_7->hide();
  ui->sbValueB4->hide();
  ui->sbValueB5->hide();

  _parent = parent;
  double _max = std::numeric_limits<double>::max();
  double _min = -100000.00;

  ui->sbValueB1->setRange(_min, _max);
  ui->sbValueB2->setRange(_min, _max);
  ui->sbValueB3->setRange(_min, _max);
  ui->sbValueB4->setRange(_min, _max);
  ui->sbValueB5->setRange(_min, _max);

  ui->vMonthly->setMaximum(10000);
  ui->vWeekly->setMaximum(10000);
  ui->vDaily->setMaximum(10000);
  ui->v60min->setMaximum(10000);
  ui->v5min->setMaximum(10000);

  // widget list pointer
  list_b_value.append(ui->sbValueB1);
  list_b_value.append(ui->sbValueB2);
  list_b_value.append(ui->sbValueB3);
  list_b_value.append(ui->sbValueB4);
  list_b_value.append(ui->sbValueB5);

  initialize();
}

ThresholdParameters::~ThresholdParameters() {
  delete ui;
}

void ThresholdParameters::initialize() {
  XmlConfigHandler *config = XmlConfigHandler::get_instance();

  list_symbol = config->get_list_symbol();
  sr_threshold = config->get_list_threshold();
  sr_intersect_threshold = config->get_interval_threshold();

  for (int i = 0; i < list_symbol.length(); ++i)
    ui->cbSymbol->addItem(list_symbol[i], list_symbol[i]);

  ui->vMonthly->setValue(sr_intersect_threshold[WEIGHT_MONTHLY]);
  ui->vWeekly->setValue(sr_intersect_threshold[WEIGHT_WEEKLY]);
  ui->vDaily->setValue(sr_intersect_threshold[WEIGHT_DAILY]);
  ui->v60min->setValue(sr_intersect_threshold[WEIGHT_60MIN]);
  ui->v5min->setValue(sr_intersect_threshold[WEIGHT_5MIN]);
}

void ThresholdParameters::on_cbSymbol_currentIndexChanged(const int &index) {
  QString symbol = ui->cbSymbol->itemData(index).toString();
  QVector<XmlConfigHandler::t_threshold> vec = sr_threshold[symbol];

  for (int i = 0; i < 5; ++i)
    list_b_value[i]->setValue(vec[i].b);
}

void ThresholdParameters::on_btnSave_clicked() {
  QString symbol = ui->cbSymbol->currentData().toString();
  QVector<XmlConfigHandler::t_threshold> vec_old = sr_threshold[symbol];
  QVector<XmlConfigHandler::t_threshold> vec;
  XmlConfigHandler::t_threshold data;
  double threshold = 2.0; // ??

  for (int i = 0; i < 5; ++i) {
    int j = 0;
    while (vec_old[j].index < i)++j;
    data.index = i;
    data.threshold = threshold;
    data.b = list_b_value[i]->value();
    data.r = list_b_value[i]->value();
    if (!vec_old[j].recalculate) {
      data.recalculate = (data.b >= 0 && data.index == vec_old[j].index && data.b != vec_old[j].b);
    } else {
      data.recalculate = (data.b >= 0);
    }
    vec.append(data);
  }

  sr_threshold[symbol] = vec;
  sr_intersect_threshold[WEIGHT_MONTHLY] = ui->vMonthly->value();
  sr_intersect_threshold[WEIGHT_WEEKLY] = ui->vWeekly->value();
  sr_intersect_threshold[WEIGHT_DAILY] = ui->vDaily->value();
  sr_intersect_threshold[WEIGHT_60MIN] = ui->v60min->value();
  sr_intersect_threshold[WEIGHT_5MIN] = ui->v5min->value();

  // rewrite config
  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  config->set_list_threshold(sr_threshold);
  config->set_interval_threshold(sr_intersect_threshold);
  config->write_config();

  // refresh threshold in MainWindow/MasterWindow
//  qobject_cast<MainWindow*>(_parent)->set_threshold_parameters("");

  QMessageBox::information(this, "Information", "Data saved.\nPlease perform Refresh to recalcute new threshold.");
}
