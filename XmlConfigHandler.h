#ifndef XMLCONFIGHANDLER
#define XMLCONFIGHANDLER

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

class XmlConfigHandler {
  public:
    typedef struct {
      double threshold; // 2
      double b;
      double r;
      int index; // id_threshold
      bool recalculate;
    } t_threshold;

    typedef struct {
      int t_id;
      double value1;
      double value2;
      QString _operator;
    } t_threshold_1;

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

    int rsi_operator;
    int rsi_lb;
    int rst_max;

    QMap<int,QString> interval_weight;
    QMap<int,int> interval_threshold;
    QMap<QString,QVector<t_threshold>> list_threshold;
    QDateTime last_update;
    bool enable_parent_indexing;
    bool enable_developer_mode;
    bool enable_test_point_update;
    bool enable_distfc_distsc_rank;
    bool enable_distfs_rank;

    XmlConfigHandler(){}
    XmlConfigHandler(XmlConfigHandler const&){}
    XmlConfigHandler& operator=(XmlConfigHandler const&){return *this;}

    void init_config_variable();
    void xml_parse_database(QXmlStreamReader &xml);
    void xml_parse_interval(QXmlStreamReader &xml);
    void xml_parse_threshold(QXmlStreamReader &xml);
    void xml_parse_element(QXmlStreamReader &xml, const QString &name, QStringList *out);
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

    QMap<QString,QVector<t_threshold>> get_list_threshold() const {
      return list_threshold;
    }

    bool is_enable_developer_mode() const {
      return enable_developer_mode;
    }

    bool is_enable_parent_indexing() const {
      return enable_parent_indexing;
    }

    bool is_enable_test_point_update() const {
      return enable_test_point_update;
    }

    bool is_enable_distf_dists_rank() const {
      return enable_distfc_distsc_rank;
    }

    bool is_enable_distfs_rank() const {
      return enable_distfs_rank;
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

    void set_list_threshold(const QMap<QString,QVector<t_threshold>> &threshold) {
      list_threshold = threshold;
    }

    void set_interval_threshold(const QMap<int,int> &threshold) {
      interval_threshold = threshold;
    }
};

#endif // XMLCONFIGHANDLER
