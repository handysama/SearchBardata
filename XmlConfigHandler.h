#ifndef XMLCONFIGHANDLER
#define XMLCONFIGHANDLER

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

class XmlConfigHandler {
  public:
    // Support / Resistance
    typedef struct {
      int t_id;
      QString symbol;
      double test_point; // 2
      double break_threshold;
      double react_threshold;
      bool recalculate;
    } t_sr_threshold;

    // MACD / RSI / SlowK / SlowD
    typedef struct {
      int t_id;
      QString operator1;
      QString operator2;
      double value1;
      double value2;
    } t_threshold_1;

    // CGF / CLF / CGS / CLS
    typedef struct {
      int t_id;
      QString value; // allow to be null (not set)
    } t_threshold_2;

  private:
    static XmlConfigHandler *_instance;
    const QString application_path = QCoreApplication::applicationDirPath();
    const QString config_path = application_path + "/config.xml";

    // config parameters
    QString database_dir;
    QString input_dir;
    QString process_dir;
    QString result_dir;
    QStringList list_symbol;
    QStringList list_database;
    QMap<int,QString> interval_weight;
    QMap<int,int> interval_threshold;
    QMap< QString, QVector<t_sr_threshold> > list_sr_threshold; // Symbol, Thresholds
    QVector<t_threshold_1> t_rsi;
    QVector<t_threshold_1> t_macd;
    QVector<t_threshold_1> t_slowk;
    QVector<t_threshold_1> t_slowd;
    QMap< QString, QVector<t_threshold_2> > t_cgf; // Symbol, Thresholds
    QMap< QString, QVector<t_threshold_2> > t_clf;
    QMap< QString, QVector<t_threshold_2> > t_cgs;
    QMap< QString, QVector<t_threshold_2> > t_cls;
    QDateTime last_update;
    bool enable_developer_mode;
    bool enable_parent_indexing;
    bool enable_update_support_resistance;
    bool enable_update_histogram;

    XmlConfigHandler(){}
    XmlConfigHandler(XmlConfigHandler const&){}
    XmlConfigHandler& operator=(XmlConfigHandler const&){return *this;}

    void initialize_variable();
    void xml_parse_database(QXmlStreamReader &xml);
    void xml_parse_interval(QXmlStreamReader &xml);
    void xml_parse_threshold(QXmlStreamReader &xml);
    void xml_parse_element(QXmlStreamReader &xml, const QString &name, QStringList *out);
    void xml_parse_macd(QXmlStreamReader &xml);
    void xml_parse_rsi(QXmlStreamReader &xml);
    void xml_parse_slowk(QXmlStreamReader &xml);
    void xml_parse_slowd(QXmlStreamReader &xml);
    void xml_parse_close_fs(QXmlStreamReader &xml, const QString &symbol, const QString &column_name); // CGF, CLF, CGS, CLS
    void read_config();
    void write_config_default();

  public:
    void write_config();
    void reset_recalculate_threshold();

    ~XmlConfigHandler() {
      delete _instance;
    }

    static XmlConfigHandler *get_instance() {
      if (_instance == NULL) {
        _instance = new XmlConfigHandler();
      }
      _instance->read_config();
      return _instance;
    }

    QString get_input_dir() const {
      return input_dir;
    }

    QString get_result_dir() const {
      return result_dir;
    }

    QString get_database_dir() const {
      return database_dir;
    }

    QDateTime get_last_updated() const {
      return last_update;
    }

    QStringList get_list_symbol() const {
      return list_symbol;
    }

    QMap<int,QString> get_interval_weight() const {
      return interval_weight;
    }

    QMap<int,int> get_interval_threshold() const {
      return interval_threshold;
    }

    QMap<QString,QVector<t_sr_threshold>> get_list_threshold() const {
      return list_sr_threshold;
    }

    QVector<t_threshold_1> get_macd_threshold() const {
      return t_macd;
    }

    QVector<t_threshold_1> get_rsi_threshold() const {
      return t_rsi;
    }

    QVector<t_threshold_1> get_slowk_threshold() const {
      return t_slowk;
    }

    QVector<t_threshold_1> get_slowd_threshold() const {
      return t_slowd;
    }

    QVector<t_threshold_2> get_cgf_threshold(const QString &symbol) const {
      return t_cgf[symbol];
    }

    QVector<t_threshold_2> get_clf_threshold(const QString &symbol) const {
      return t_clf[symbol];
    }

    QVector<t_threshold_2> get_cgs_threshold(const QString &symbol) const {
      return t_cgs[symbol];
    }

    QVector<t_threshold_2> get_cls_threshold(const QString &symbol) const {
      return t_cls[symbol];
    }


    QMap<QString,QVector<t_threshold_2>> get_cgf_threshold(void) const {
      return t_cgf;
    }

    QMap<QString,QVector<t_threshold_2>> get_clf_threshold(void) const {
      return t_clf;
    }

    QMap<QString,QVector<t_threshold_2>> get_cgs_threshold(void) const {
      return t_cgs;
    }

    QMap<QString,QVector<t_threshold_2>> get_cls_threshold(void) const {
      return t_cls;
    }



    bool is_enable_developer_mode() const {
      return enable_developer_mode;
    }

    bool is_enable_parent_indexing() const {
      return enable_parent_indexing;
    }

    bool is_enable_update_support_resistance() const {
      return enable_update_support_resistance;
    }

    bool is_enable_update_histogram() const {
      return enable_update_histogram;
    }

    void set_last_updated(const QDateTime &value) {
      last_update = value;
    }

    void set_input_dir(const QString &value) {
      input_dir = value;
    }

    void set_result_dir(const QString &value) {
      result_dir = value;
    }

    void set_database_dir(const QString &value) {
      database_dir = value;
    }

    void set_list_symbol(const QStringList &symbols) {
      list_symbol = symbols;
    }

    void set_list_threshold(const QMap<QString,QVector<t_sr_threshold>> &threshold) {
      list_sr_threshold = threshold;
    }

    void set_interval_threshold(const QMap<int,int> &threshold) {
      interval_threshold = threshold;
    }

    void set_macd_threshold(const QVector<t_threshold_1> &t) {
      t_macd = t;
    }

    void set_rsi_threshold(const QVector<t_threshold_1> &t) {
      t_rsi = t;
    }

    void set_slowk_threshold(const QVector<t_threshold_1> &t) {
      t_slowk = t;
    }

    void set_slowd_threshold(const QVector<t_threshold_1> &t) {
      t_slowd = t;
    }

    void set_cgf_threshold(const QString &symbol, const QVector<t_threshold_2> &t) {
      t_cgf[symbol] = t;
    }

    void set_clf_threshold(const QString &symbol, const QVector<t_threshold_2> &t) {
      t_clf[symbol] = t;
    }

    void set_cgs_threshold(const QString &symbol, const QVector<t_threshold_2> &t) {
      t_cgs[symbol] = t;
    }

    void set_cls_threshold(const QString &symbol, const QVector<t_threshold_2> &t) {
      t_cls[symbol] = t;
    }
};

#endif // XMLCONFIGHANDLER
