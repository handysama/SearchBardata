#include "ChangeThresholdDialog.h"
#include "ui_ChangeThresholdDialog.h"
#define MAX_SBAR_TAB 5

ChangeThresholdDialog::ChangeThresholdDialog(QWidget *parent): QDialog(parent), ui(new Ui::ChangeThresholdDialog) {
  ui->setupUi(this);
  setFixedHeight(this->height());
  setFixedWidth(this->width());

  for (int i = 0; i < MAX_SBAR_TAB; ++i) {
    symbol.push_back("");
  }

  ptr_macd.append(ui->cbMACD_threshold_1);
  ptr_macd.append(ui->cbMACD_threshold_2);
  ptr_macd.append(ui->cbMACD_threshold_3);
  ptr_macd.append(ui->cbMACD_threshold_4);
  ptr_macd.append(ui->cbMACD_threshold_5);

  ptr_rsi.append(ui->cbRSI_threshold_1);
  ptr_rsi.append(ui->cbRSI_threshold_2);
  ptr_rsi.append(ui->cbRSI_threshold_3);
  ptr_rsi.append(ui->cbRSI_threshold_4);
  ptr_rsi.append(ui->cbRSI_threshold_5);

  ptr_slowk.append(ui->cbSlowK_threshold_1);
  ptr_slowk.append(ui->cbSlowK_threshold_2);
  ptr_slowk.append(ui->cbSlowK_threshold_3);
  ptr_slowk.append(ui->cbSlowK_threshold_4);
  ptr_slowk.append(ui->cbSlowK_threshold_5);

  ptr_slowd.append(ui->cbSlowD_threshold_1);
  ptr_slowd.append(ui->cbSlowD_threshold_2);
  ptr_slowd.append(ui->cbSlowD_threshold_3);
  ptr_slowd.append(ui->cbSlowD_threshold_4);
  ptr_slowd.append(ui->cbSlowD_threshold_5);

  ptr_cgf.append(ui->cbCGF_threshold_1);
  ptr_cgf.append(ui->cbCGF_threshold_2);
  ptr_cgf.append(ui->cbCGF_threshold_3);
  ptr_cgf.append(ui->cbCGF_threshold_4);
  ptr_cgf.append(ui->cbCGF_threshold_5);

  ptr_clf.append(ui->cbCLF_threshold_1);
  ptr_clf.append(ui->cbCLF_threshold_2);
  ptr_clf.append(ui->cbCLF_threshold_3);
  ptr_clf.append(ui->cbCLF_threshold_4);
  ptr_clf.append(ui->cbCLF_threshold_5);

  ptr_cgs.append(ui->cbCGS_threshold_1);
  ptr_cgs.append(ui->cbCGS_threshold_2);
  ptr_cgs.append(ui->cbCGS_threshold_3);
  ptr_cgs.append(ui->cbCGS_threshold_4);
  ptr_cgs.append(ui->cbCGS_threshold_5);

  ptr_cls.append(ui->cbCLS_threshold_1);
  ptr_cls.append(ui->cbCLS_threshold_2);
  ptr_cls.append(ui->cbCLS_threshold_3);
  ptr_cls.append(ui->cbCLS_threshold_4);
  ptr_cls.append(ui->cbCLS_threshold_5);

  initialize();
}

ChangeThresholdDialog::~ChangeThresholdDialog() {
  delete ui;
}

void ChangeThresholdDialog::initialize() {
  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  QString item_text;

  t_macd = config->get_macd_threshold();
  t_rsi = config->get_rsi_threshold();
  t_slowk = config->get_slowk_threshold();
  t_slowd = config->get_slowd_threshold();

  for (int j = 0; j < MAX_SBAR_TAB; ++j) {
    ptr_macd[j]->clear();
    ptr_rsi[j]->clear();
    ptr_slowk[j]->clear();
    ptr_slowd[j]->clear();

    for (int i = 0; i < t_macd.size(); ++i) {
      if ( !t_macd[i].operator1.isEmpty() ) {
        item_text = t_macd[i].operator1 + " " + QString::number(t_macd[i].value1) + " / " +
                    t_macd[i].operator2 + " " + QString::number(t_macd[i].value2);
        ptr_macd[j]->addItem(item_text, t_macd[i].t_id);
      }
    }

    for (int i = 0; i < t_rsi.size(); ++i) {
      if ( !t_rsi[i].operator1.isEmpty() ) {
        item_text = t_rsi[i].operator1 + " " + QString::number(t_rsi[i].value1) + " / " +
                    t_rsi[i].operator2 + " " + QString::number(t_rsi[i].value2);
        ptr_rsi[j]->addItem(item_text, t_rsi[i].t_id);
      }
    }

    for (int i = 0; i < t_slowk.size(); ++i) {
      if ( !t_slowk[i].operator1.isEmpty() ) {
        item_text = t_slowk[i].operator1 + " " + QString::number(t_slowk[i].value1) + " / " +
                    t_slowk[i].operator2 + " " + QString::number(t_slowk[i].value2);
        ptr_slowk[j]->addItem(item_text, t_slowk[i].t_id);
      }
    }

    for (int i = 0; i < t_slowd.size(); ++i) {
      if ( !t_slowd[i].operator1.isEmpty() ) {
        item_text = t_slowd[i].operator1 + " " + QString::number(t_slowd[i].value1) + " / " +
                    t_slowd[i].operator2 + " " + QString::number(t_slowd[i].value2);
        ptr_slowd[j]->addItem(item_text, t_slowd[i].t_id);
      }
    }
  }
}

void ChangeThresholdDialog::set_current_tab(const int &index) {
  if (index < 0 || index > ui->tabWidget->count()) return;
  ui->tabWidget->setCurrentIndex(index);
}

void ChangeThresholdDialog::set_symbol(const int &tab_id, const QString &s) {
  symbol[tab_id] = s;
  QString _symbol = symbol[tab_id];

  XmlConfigHandler *config = XmlConfigHandler::get_instance();
  t_cgf = config->get_cgf_threshold();
  t_clf = config->get_clf_threshold();
  t_cgs = config->get_cgs_threshold();
  t_cls = config->get_cls_threshold();

  ptr_cgf[tab_id]->clear();
  ptr_clf[tab_id]->clear();
  ptr_cgs[tab_id]->clear();
  ptr_cls[tab_id]->clear();

  for (int i = 0; i < t_cgf[_symbol].size() ; ++i) {
    if ( !t_cgf[_symbol][i].value.isEmpty() ) {
      ptr_cgf[tab_id]->addItem( BarDataAdapter::remove_trailing_zero( t_cgf[_symbol][i].value ) , t_cgf[_symbol][i].t_id);
    }
  }

  for (int i = 0; i < t_clf[_symbol].size() ; ++i) {
    if ( !t_clf[_symbol][i].value.isEmpty() ) {
      ptr_clf[tab_id]->addItem( BarDataAdapter::remove_trailing_zero( t_clf[_symbol][i].value ) , t_clf[_symbol][i].t_id);
    }
  }

  for (int i = 0; i < t_cgs[_symbol].size() ; ++i) {
    if ( !t_cgs[_symbol][i].value.isEmpty() ) {
      ptr_cgs[tab_id]->addItem( BarDataAdapter::remove_trailing_zero( t_cgs[_symbol][i].value ) , t_cgs[_symbol][i].t_id);
    }
  }

  for (int i = 0; i < t_cls[_symbol].size() ; ++i) {
    if ( !t_cls[_symbol][i].value.isEmpty() ) {
      ptr_cls[tab_id]->addItem( BarDataAdapter::remove_trailing_zero( t_cls[_symbol][i].value ) , t_cls[_symbol][i].t_id);
    }
  }
}
