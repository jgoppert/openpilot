TEMPLATE = lib
TARGET = RawHID
include(../../openpilotgcsplugin.pri)
include(rawhid_dependencies.pri)
HEADERS += rawhid_global.h \
    rawhidplugin.h \
    rawhid.h \
    pjrc_rawhid.h \
    rawhid_const.h
SOURCES += rawhidplugin.cpp \
    rawhid.cpp
FORMS += 
RESOURCES += 
DEFINES += RAWHID_LIBRARY
OTHER_FILES += RawHID.pluginspec

# Platform Specific USB HID Stuff
win32 { 
    SOURCES += pjrc_rawhid_win.cpp
    LIBS += -lhid \
        -lsetupapi
}
macx { 
    SOURCES += pjrc_rawhid_mac.cpp
    SDK = /Developer/SDKs/MacOSX10.5.sdk
    ARCH = -mmacosx-version-min=10.5 \
        -arch \
        ppc \
        -arch \
        i386
    LIBS += $(ARCH) \
        -Wl,-syslibroot,$(SDK) \
        -framework \
        IOKit \
        -framework \
        CoreFoundation
}
linux-g++ {
    SOURCES += pjrc_rawhid_unix.cpp
    LIBS += -lusb
}
linux-g++-64 {
    SOURCES += pjrc_rawhid_unix.cpp
    LIBS += -lusb
}
