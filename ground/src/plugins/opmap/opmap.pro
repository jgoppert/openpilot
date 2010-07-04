TEMPLATE = lib
TARGET = OPMapGadget
include(../../openpilotgcsplugin.pri)
include(../../plugins/coreplugin/coreplugin.pri)
include(../../libs/opmapcontrol/opmapcontrol.pri)
include(../../plugins/uavobjects/uavobjects.pri)
HEADERS += opmapplugin.h \
    opmapgadgetoptionspage.h \
    opmapgadgetfactory.h \
    opmapgadgetconfiguration.h \
    opmapgadget.h \
    opmapgadgetwidget.h \
    opmap_waypointeditor_dialog.h \
    opmap_edit_waypoint_dialog.h \
    opmap_zoom_slider_widget.h \
    opmap_statusbar_widget.h \
    opmap_overlay_widget.h
SOURCES += opmapplugin.cpp \
    opmapgadgetwidget.cpp \
    opmapgadgetoptionspage.cpp \
    opmapgadgetfactory.cpp \
    opmapgadgetconfiguration.cpp \
    opmapgadget.cpp \
    opmap_waypointeditor_dialog.cpp \
    opmap_edit_waypoint_dialog.cpp \
    opmap_zoom_slider_widget.cpp \
    opmap_statusbar_widget.cpp \
    opmap_overlay_widget.cpp
OTHER_FILES += OPMapGadget.pluginspec
FORMS += opmapgadgetoptionspage.ui \
    opmap_widget.ui \
    opmap_waypointeditor_dialog.ui \
    opmap_edit_waypoint_dialog.ui \
    opmap_zoom_slider_widget.ui \
    opmap_statusbar_widget.ui \
    opmap_overlay_widget.ui
RESOURCES += opmap.qrc
