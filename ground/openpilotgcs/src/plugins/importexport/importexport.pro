TEMPLATE = lib
TARGET = ImportExportGadget
DEFINES += IMPORTEXPORT_LIBRARY
QT += xml
include(../../openpilotgcsplugin.pri)
include(importexport_dependencies.pri)
HEADERS += importexportplugin.h \
    importexportgadgetwidget.h \
    importexportdialog.h \
    xmlconfig.h
HEADERS += importexportgadget.h
HEADERS += importexportgadgetfactory.h
HEADERS += importexportgadgetconfiguration.h
HEADERS += importexportgadgetoptionspage.h
SOURCES += importexportplugin.cpp \
    importexportgadgetwidget.cpp \
    importexportdialog.cpp \
    xmlconfig.cpp
SOURCES += importexportgadget.cpp
SOURCES += importexportgadgetfactory.cpp
SOURCES += importexportgadgetconfiguration.cpp
SOURCES += importexportgadgetoptionspage.cpp
OTHER_FILES += ImportExportGadget.pluginspec
FORMS += importexportgadgetoptionspage.ui \
    importexportgadgetwidget.ui \
    importexportdialog.ui
