#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T09:23:40
#
#-------------------------------------------------

QT += core \
    gui \
    widgets \
    xml \
#    network \
    sql \
#    qml \
#    quick \
#    printsupport \
#    multimedia

TARGET = BibConv
TEMPLATE = app


SOURCES += main.cpp\
        bibconv.cpp \
    bible.cpp \
    song.cpp

HEADERS  += bibconv.h \
    bible.h \
    song.h

FORMS    += bibconv.ui
