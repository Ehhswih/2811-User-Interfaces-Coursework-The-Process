QT += gui
QT += core
QT += network
QT += widgets

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/stylesheeteditor.cpp 

HEADERS += \
    $$PWD/stylesheeteditor.h
 

FORMS += $$PWD/stylesheeteditor.ui

RESOURCES += \
    $$PWD/stylesheet.qrc

