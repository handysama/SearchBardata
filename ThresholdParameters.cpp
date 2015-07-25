#include "ThresholdParameters.h"
#include "ui_ThresholdParameters.h"

#define DSPINBOX_MIN_VALUE -10000.00
#define DSPINBOX_MAX_VALUE  99999.99
#define INTERSECT_MAX_VALUE 10000
#define MAX_THRESHOLD_COUNT 3

ThresholdParameters::ThresholdParameters(QWidget *parent):QDialog(parent), ui(new Ui::ThresholdParameters) {
  ui->setupUi(this);
  setFixedSize(width(), height());

  _parent = parent;
  ui->sbValueB1->setRange(DSPINBOX_MIN_VALUE, DSPINBOX_MAX_VALUE);
  ui->sbValueB2->setRange(DSPINBOX_MIN_VALUE, DSPINBOX_MAX_VALUE);
  ui->sbValueB3->setRange(DSPINBOX_MIN_VALUE, DSPINBOX_MAX_VALUE);
  ui->sbValueB4->setRange(DSPINBOX_MIN_VALUE, DSPINBOX_MAX_VALUE);
  ui->sbValueB5->setRange(DSPINBOX_MIN_VALUE, DSPINBOX_MAX_VALUE);
  ui->sbCGF_1->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCGF_2->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCGF_3->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCLF_1->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCLF_2->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCLF_3->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCGS_1->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCGS_2->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCGS_3->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCLS_1->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCLS_2->setRange(0, DSPINBOX_MAX_VALUE);
  ui->sbCLS_3->setRange(0, DSPINBOX_MAX_VALUE);
  ui->vMonthly->setMaximum(INTERSECT_MAX_VALUE);
  ui->vWeekly->setMaximum(INTERSECT_MAX_VALUE);
  ui->vDaily->setMaximum(INTERSECT_MAX_VALUE);
  ui->v60min->setMaximum(INTERSECT_MAX_VALUE);
  ui->v5min->setMaximum(INTERSECT_MAX_VALUE);

  // widget list pointer
  list_b_value.append(ui->sbValueB1);
  list_b_value.append(ui->sbValueB2);
  list_b_value.append(ui->sbValueB3);
  list_b_value.append(ui->sbValueB4);
  list_b_value.append(ui->sbValueB5);

  MACD_operator1.append(ui->cbMACD_operator1_1);
  MACD_operator1.append(ui->cbMACD_operator1_2);
  MACD_operator1.append(ui->cbMACD_operator1_3);
  MACD_value1.append(ui->sbMACD_value1_1);
  MACD_value1.append(ui->sbMACD_value1_2);
  MACD_value1.append(ui->sbMACD_value1_3);
  MACD_operator2.append(ui->cbMACD_operator2_1);
  MACD_operator2.append(ui->cbMACD_operator2_2);
  MACD_operator2.append(ui->cbMACD_operator2_3);
  MACD_value2.append(ui->sbMACD_value2_1);
  MACD_value2.append(ui->sbMACD_value2_2);
  MACD_value2.append(ui->sbMACD_value2_3);

  RSI_operator1.append(ui->cbRSI_operator1_1);
  RSI_operator1.append(ui->cbRSI_operator1_2);
  RSI_operator1.append(ui->cbRSI_operator1_3);
  RSI_value1.append(ui->sbRSI_value1_1);
  RSI_value1.append(ui->sbRSI_value1_2);
  RSI_value1.append(ui->sbRSI_value1_3);
  RSI_operator2.append(ui->cbRSI_operator2_1);
  RSI_operator2.append(ui->cbRSI_operator2_2);
  RSI_operator2.append(ui->cbRSI_operator2_3);
  RSI_value2.append(ui->sbRSI_value2_1);
  RSI_value2.append(ui->sbRSI_value2_2);
  RSI_value2.append(ui->sbRSI_value2_3);

  SlowK_operator1.append(ui->cbSlowK_operator1_1);
  SlowK_operator1.append(ui->cbSlowK_operator1_2);
  SlowK_operator1.append(ui->cbSlowK_operator1_3);
  SlowK_value1.append(ui->sbSlowK_value1_1);
  SlowK_value1.append(ui->sbSlowK_value1_2);
  SlowK_value1.append(ui->sbSlowK_value1_3);
  SlowK_operator2.append(ui->cbSlowK_operator2_1);
  SlowK_operator2.append(ui->cbSlowK_operator2_2);
  SlowK_operator2.append(ui->cbSlowK_operator2_3);
  SlowK_value2.append(ui->sbSlowK_value2_1);
  SlowK_value2.append(ui->sbSlowK_value2_2);
  SlowK_value2.append(ui->sbSlowK_value2_3);

  SlowD_operator1.append(ui->cbSlowD_operator1_1);
  SlowD_operator1.append(ui->cbSlowD_operator1_2);
  SlowD_operator1.append(ui->cbSlowD_operator1_3);
  SlowD_value1.append(ui->sbSlowD_value1_1);
  SlowD_value1.append(ui->sbSlowD_value1_2);
  SlowD_value1.append(ui->sbSlowD_value1_3);
  SlowD_operator2.append(ui->cbSlowD_operator2_1);
  SlowD_operator2.append(ui->cbSlowD_operator2_2);
  SlowD_operator2.append(ui->cbSlowD_operator2_3);
  SlowD_value2.append(ui->sbSlowD_value2_1);
  SlowD_value2.append(ui->sbSlowD_value2_2);
  SlowD_value2.append(ui->sbSlowD_value2_3);

  CGF_value.append( ui->sbCGF_1 );
  CGF_value.append( ui->sbCGF_2 );
  CGF_value.append( ui->sbCGF_3 );
  CGF_disable_flag.append( ui->chkCGF_1 );
  CGF_disable_flag.append( ui->chkCGF_2 );
  CGF_disable_flag.append( ui->chkCGF_3 );

  CLF_value.append( ui->sbCLF_1 );
  CLF_value.append( ui->sbCLF_2 );
  CLF_value.append( ui->sbCLF_3 );
  CLF_disable_flag.append( ui->chkCLF_1 );
  CLF_disable_flag.append( ui->chkCLF_2 );
  CLF_disable_flag.append( ui->chkCLF_3 );

  CGS_value.append( ui->sbCGS_1 );
  CGS_value.append( ui->sbCGS_2 );
  CGS_value.append( ui->sbCGS_3 );
  CGS_disable_flag.append( ui->chkCGS_1 );
  CGS_disable_flag.append( ui->chkCGS_2 );
  CGS_disable_flag.append( ui->chkCGS_3 );

  CLS_value.append( ui->sbCLS_1 );
  CLS_value.append( ui->sbCLS_2 );
  CLS_value.append( ui->sbCLS_3 );
  CLS_disable_flag.append( ui->chkCLS_1 );
  CLS_disable_flag.append( ui->chkCLS_2 );
  CLS_disable_flag.append( ui->chkCLS_3 );

  initialize();
}

ThresholdParameters::~ThresholdParameters() {
  delete ui;
}

void set_operator_option(QComboBox *cb) {
  if (cb != NULL) {
    cb->blockSignals(true);
    cb->clear();
    cb->addItem("<", "<");
    cb->addItem(">", ">");
    cb->addItem("=", "=");
    cb->addItem("Not set", "");
    cb->setCurrentText("Not set");
    cb->blockSignals(false);
  }
}

void ThresholdParameters::initialize() {
  XmlConfigHandler *config = XmlConfigHandler::get_instance();

  list_symbol = config->get_list_symbol();
  sr_threshold = config->get_list_threshold();
  sr_intersect_threshold = config->get_interval_threshold();
  t_macd = config->get_macd_threshold();
  t_rsi = config->get_rsi_threshold();
  t_slowk = config->get_slowk_threshold();
  t_slowd = config->get_slowd_threshold();
  t_cgf = config->get_cgf_threshold();
  t_clf = config->get_clf_threshold();
  t_cgs = config->get_cgs_threshold();
  t_cls = config->get_cls_threshold();

  for (int i = 0; i < list_symbol.length(); ++i)
    ui->cbSymbol->addItem(list_symbol[i], list_symbol[i]);

  ui->vMonthly->setValue(sr_intersect_threshold[WEIGHT_MONTHLY]);
  ui->vWeekly->setValue(sr_intersect_threshold[WEIGHT_WEEKLY]);
  ui->vDaily->setValue(sr_intersect_threshold[WEIGHT_DAILY]);
  ui->v60min->setValue(sr_intersect_threshold[WEIGHT_60MIN]);
  ui->v5min->setValue(sr_intersect_threshold[WEIGHT_5MIN]);

  for (int i = 0; i < t_macd.size(); ++i) {
    set_operator_option(MACD_operator1[i]);
    set_operator_option(MACD_operator2[i]);
    MACD_value1[i]->setValue(t_macd[i].value1);
    MACD_value2[i]->setValue(t_macd[i].value2);
    MACD_operator1[i]->setCurrentText(t_macd[i].operator1);
    MACD_operator2[i]->setCurrentText(t_macd[i].operator2);
  }

  for (int i = 0; i < t_macd.size(); ++i) {
    set_operator_option(RSI_operator1[i]);
    set_operator_option(RSI_operator2[i]);
    RSI_value1[i]->setValue(t_rsi[i].value1);
    RSI_value2[i]->setValue(t_rsi[i].value2);
    RSI_operator1[i]->setCurrentText(t_rsi[i].operator1);
    RSI_operator2[i]->setCurrentText(t_rsi[i].operator2);
  }

  for (int i = 0; i < t_slowk.size(); ++i) {
    set_operator_option(SlowK_operator1[i]);
    set_operator_option(SlowK_operator2[i]);
    SlowK_value1[i]->setValue(t_slowk[i].value1);
    SlowK_value2[i]->setValue(t_slowk[i].value2);
    SlowK_operator1[i]->setCurrentText(t_slowk[i].operator1);
    SlowK_operator2[i]->setCurrentText(t_slowk[i].operator2);
  }

  for (int i = 0; i < t_slowd.size(); ++i) {
    set_operator_option(SlowD_operator1[i]);
    set_operator_option(SlowD_operator2[i]);
    SlowD_value1[i]->setValue(t_slowd[i].value1);
    SlowD_value2[i]->setValue(t_slowd[i].value2);
    SlowD_operator1[i]->setCurrentText(t_slowd[i].operator1);
    SlowD_operator2[i]->setCurrentText(t_slowd[i].operator2);
  }

//  for (int i = 0; i < t_cgf.size(); ++i) {
//    CGF_disable_flag[i]->setChecked( t_cgf[i].value.isEmpty() );
//    CGF_value[i]->setValue( t_cgf[i].value.toDouble() );
//  }

//  for (int i = 0; i < t_clf.size(); ++i) {
//    CLF_disable_flag[i]->setChecked( t_clf[i].value.isEmpty() );
//    CLF_value[i]->setValue( t_clf[i].value.toDouble() );
//  }

//  for (int i = 0; i < t_cgs.size(); ++i) {
//    CGS_disable_flag[i]->setChecked( t_cgs[i].value.isEmpty() );
//    CGS_value[i]->setValue( t_cgs[i].value.toDouble() );
//  }

//  for (int i = 0; i < t_cls.size(); ++i) {
//    CLS_disable_flag[i]->setChecked( t_cls[i].value.isEmpty() );
//    CLS_value[i]->setValue( t_cls[i].value.toDouble() );
//  }
}

void ThresholdParameters::on_cbSymbol_currentIndexChanged(const int &index) {
  QString symbol = ui->cbSymbol->itemData(index).toString();
  QVector<XmlConfigHandler::t_sr_threshold> vec = sr_threshold[symbol];

  for (int i = 0; i < MAX_THRESHOLD_COUNT; ++i) {
    list_b_value[i]->setValue(vec[i].break_threshold);
  }
}

void ThresholdParameters::on_btnCancel_clicked() {
  this->reject();
}

// TODO: update CGF, CLF
void ThresholdParameters::on_btnSave_clicked() {
  QString symbol = ui->cbSymbol->currentData().toString();
  QVector<XmlConfigHandler::t_sr_threshold> vec_old = sr_threshold[symbol];
  QVector<XmlConfigHandler::t_sr_threshold> vec;
  XmlConfigHandler::t_sr_threshold data;
  double threshold = 2.0; // ??

  //
  // S/R Refresh Threshold and MACD/RSI/SlowK/SlowD
  //
  for (int i = 0; i < MAX_THRESHOLD_COUNT; ++i) {
    int j = 0;
    while (vec_old[j].t_id < i) ++j;
    data.t_id = i;
    data.test_point = threshold;
    data.break_threshold = list_b_value[i]->value();
    data.react_threshold = list_b_value[i]->value();

    if (!vec_old[j].recalculate) {
      data.recalculate = (data.break_threshold >= 0 && data.t_id == vec_old[j].t_id && data.break_threshold != vec_old[j].break_threshold);
    } else {
      data.recalculate = (data.break_threshold >= 0);
    }

    vec.append(data);

    t_macd[i].operator1 = MACD_operator1[i]->currentData().toString();
    t_macd[i].operator2 = MACD_operator2[i]->currentData().toString();
    t_macd[i].value1 = MACD_value1[i]->value();
    t_macd[i].value2 = MACD_value2[i]->value();
//    t_macd[i].is_rank = MACD_rank[i]->isChecked();

    t_rsi[i].operator1 = RSI_operator1[i]->currentData().toString();
    t_rsi[i].operator2 = RSI_operator2[i]->currentData().toString();
    t_rsi[i].value1 = RSI_value1[i]->value();
    t_rsi[i].value2 = RSI_value2[i]->value();

    t_slowk[i].operator1 = SlowK_operator1[i]->currentData().toString();
    t_slowk[i].operator2 = SlowK_operator2[i]->currentData().toString();
    t_slowk[i].value1 = SlowK_value1[i]->value();
    t_slowk[i].value2 = SlowK_value2[i]->value();

    t_slowd[i].operator1 = SlowD_operator1[i]->currentData().toString();
    t_slowd[i].operator2 = SlowD_operator2[i]->currentData().toString();
    t_slowd[i].value1 = SlowD_value1[i]->value();
    t_slowd[i].value2 = SlowD_value2[i]->value();
  }


  //
  // Intersect Search Threshold
  //
  sr_threshold[symbol] = vec;
  sr_intersect_threshold[WEIGHT_MONTHLY] = ui->vMonthly->value();
  sr_intersect_threshold[WEIGHT_WEEKLY] = ui->vWeekly->value();
  sr_intersect_threshold[WEIGHT_DAILY] = ui->vDaily->value();
  sr_intersect_threshold[WEIGHT_60MIN] = ui->v60min->value();
  sr_intersect_threshold[WEIGHT_5MIN] = ui->v5min->value();


  //
  // Overwrite config
  //
  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  config->set_list_threshold(sr_threshold);
  config->set_interval_threshold(sr_intersect_threshold);
  config->set_macd_threshold(t_macd);
  config->set_rsi_threshold(t_rsi);
  config->set_slowk_threshold(t_slowk);
  config->set_slowd_threshold(t_slowd);
//  config->set_cgf_threshold(ui->cbSymbol_CF->currentText(), t_cgf);
//  config->set_clf_threshold(ui->cbSymbol_CF->currentText(), t_clf);
//  config->set_cgs_threshold(ui->cbSymbol_CS->currentText(), t_cgs);
//  config->set_cls_threshold(ui->cbSymbol_CS->currentText(), t_cls);
  config->write_config();

  // refresh threshold in MainWindow/MasterWindow
//  qobject_cast<MainWindow*>(_parent)->set_threshold_parameters("");

  QMessageBox::information(this, "Information", "Data saved.\nPlease perform Refresh to recalcute new threshold.");
  this->accept();
}

void ThresholdParameters::on_chkCGF_1_stateChanged(const int &state) {
  ui->sbCGF_1->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCGF_1->setValue(0);
  }
}

void ThresholdParameters::on_chkCGF_2_stateChanged(const int &state) {
  ui->sbCGF_2->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCGF_2->setValue(0);
  }
}

void ThresholdParameters::on_chkCGF_3_stateChanged(const int &state) {
  ui->sbCGF_3->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCGF_3->setValue(0);
  }
}


void ThresholdParameters::on_chkCLF_1_stateChanged(const int &state) {
  ui->sbCLF_1->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCLF_1->setValue(0);
  }
}

void ThresholdParameters::on_chkCLF_2_stateChanged(const int &state) {
  ui->sbCLF_2->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCLF_2->setValue(0);
  }
}

void ThresholdParameters::on_chkCLF_3_stateChanged(const int &state) {
  ui->sbCLF_3->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCLF_3->setValue(0);
  }
}


void ThresholdParameters::on_chkCGS_1_stateChanged(const int &state) {
  ui->sbCGS_1->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCGS_1->setValue(0);
  }
}

void ThresholdParameters::on_chkCGS_2_stateChanged(const int &state) {
  ui->sbCGS_2->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCGS_2->setValue(0);
  }
}

void ThresholdParameters::on_chkCGS_3_stateChanged(const int &state) {
  ui->sbCGS_3->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCGS_3->setValue(0);
  }
}


void ThresholdParameters::on_chkCLS_1_stateChanged(const int &state) {
  ui->sbCLS_1->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCLS_1->setValue(0);
  }
}

void ThresholdParameters::on_chkCLS_2_stateChanged(const int &state) {
  ui->sbCLS_2->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCLS_2->setValue(0);
  }
}

void ThresholdParameters::on_chkCLS_3_stateChanged(const int &state) {
  ui->sbCLS_3->setEnabled( state == Qt::Unchecked );
  if ( state == Qt::Checked ) {
    ui->sbCLS_3->setValue(0);
  }
}
