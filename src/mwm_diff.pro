
TARGET = mwm_diff

CONFIG += console
CONFIG -= app_bundle
CONFIG += warn_on c++11

TEMPLATE = app

SOURCES += \
    main.cpp \
    lcs.cpp \
    diff.cpp \
    processor.cpp \

HEADERS += \
    main.h \
    lcs.h \
    diff.h \
    processor.h \
