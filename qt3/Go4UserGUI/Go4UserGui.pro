TEMPLATE = lib
LANGUAGE = C++

#suppress symbolic linking when generating library
QMAKE_LN_SHLIB = @echo suppress linking

CONFIG += qt warn_off thread

INCLUDEPATH += $(GO4SYS)/include

DEPENDPATH += $(GO4SYS)/include

FORMS += QUserPanel.ui
