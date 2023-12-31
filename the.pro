QT += core gui widgets multimedia multimediawidgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(stylesheet/stylesheet.pri)

SOURCES += \
        playsetting.cpp \
        the_button.cpp \
        the_player.cpp \
        tomeo.cpp \
        videoplayer.cpp

HEADERS += \
    playsetting.h \
    the_button.h \
    the_player.h \
    videoplayer.h

TRANSLATIONS += \
    translations/the_en.ts \
    translations/the_zh.ts \
    translations/the_fr.ts \
    translations/the_de.ts \
    translations/the_es.ts \
    translations/the_ar.ts \
    translations/the_ja.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    playsetting.ui \
    videoplayer.ui

RESOURCES += \
    ico/image.qrc

