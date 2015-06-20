#include <QApplication>
#include "MainWindow.h"
#include "MasterWindow.h"

//Q_IMPORT_PLUGIN();

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MasterWindow m;
  m.show();
  return a.exec();
}
