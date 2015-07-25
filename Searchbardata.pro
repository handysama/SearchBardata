QT += core gui sql concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.10.8
TARGET = SearchBardata_v0.10
TEMPLATE = app
CONFIG += qt c++11 precompile_header release warn_on
DEFINES += VERSION=\\\"$$VERSION\\\"
PRECOMPILED_HEADER = Precompiled.h
win32:LIBS += libmysql.lib

HEADERS = Precompiled.h\
          MainWindow.h MasterWindow.h ThresholdParameters.h \
          BarBuffer.h BarDataAdapter.h BarDataList.h \
          ChangeThresholdDialog.h \
          FilterOptionFactory.h \
          SearchResultWriter.h \
          SRModel.h \
          SQLiteHandler.h \
          XmlConfigHandler.h \
          MySqlConnector.h
#          DailySupport.h

SOURCES = main.cpp MainWindow.cpp MasterWindow.cpp ThresholdParameters.cpp ChangeThresholdDialog.cpp \
          BarBuffer.cpp \
          SQLiteHandler.cpp \
          XmlConfigHandler.cpp

FORMS = MainWindow.ui MasterWindow.ui ChangeThresholdDialog.ui ThresholdParameters.ui

#RC_FILE = resources.rc
#DISTFILES += \
#    resources.rc \
#    SearchBardata.exe.manifest
