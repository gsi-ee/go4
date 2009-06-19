TEMPLATE = lib
LANGUAGE = C++
MOC_DIR=.moc
OBJECTS_DIR=.obj
CONFIG += qt warn_off thread
INCLUDEPATH += ../../include ../Go4GUI
DEPENDPATH  += ../../include
DESTDIR=../../lib


win32 {
   QMAKE_LFLAGS += /FORCE
   TARGET=libGo4FitGUI
}

SOURCES	+= TGo4FitPanel.cpp \
            QFitItem.cpp \
            QFitWidget.cpp \
            QFitNamedWidget.cpp \
            QFitModelWidget.cpp \
            QFitModelPolynomWidget.cpp \
            QFitModelGauss1Widget.cpp \
            QFitModelGauss2Widget.cpp \
            QFitModelGaussNWidget.cpp \
            QFitModelFormulaWidget.cpp \
            QFitModelFunctionWidget.cpp \
            QFitDataWidget.cpp \
            QFitRangeWidget.cpp \
            QFitParWidget.cpp \
            QFitAmplEstimWidget.cpp \
            QFitOutputActionWidget.cpp \
            QFitDependencyWidget.cpp \
            QFitParCfgWidget.cpp \
            QFitMinuitWidget.cpp \
            QFitMinuitResWidget.cpp \
            QFitSlotWidget.cpp \
            QFitterWidget.cpp \
            QFitTableWidget.cpp \
            QFitPrintWidget.cpp \
            QFitLinearTransWidget.cpp \
            QFitMatrixTransWidget.cpp \
            QFitPeakFinderWidget.cpp \
            QFitRangeCutWidget.cpp

HEADERS	+= TGo4FitPanel.h \
            QFitItem.h \
            QFitWidget.h \
            QFitNamedWidget.h \
            QFitModelWidget.h \
            QFitModelPolynomWidget.h \
            QFitModelGauss1Widget.h \
            QFitModelGauss2Widget.h \
            QFitModelGaussNWidget.h \
            QFitModelFormulaWidget.h \
            QFitModelFunctionWidget.h \
            QFitDataWidget.h \
            QFitRangeWidget.h \
            QFitParWidget.h \
            QFitAmplEstimWidget.h \
            QFitOutputActionWidget.h \
            QFitDependencyWidget.h \
            QFitParCfgWidget.h \
            QFitMinuitWidget.h \
            QFitMinuitResWidget.h \
            QFitSlotWidget.h \
            QFitterWidget.h \
            QFitTableWidget.h \
            QFitPrintWidget.h \
            QFitLinearTransWidget.h \
            QFitMatrixTransWidget.h \
            QFitPeakFinderWidget.h \
            QFitRangeCutWidget.h

FORMS	= TGo4FitPanel.ui \
	QFitModelPolynomWidget.ui \
	QFitModelGauss1Widget.ui \
	QFitModelGauss2Widget.ui \
	QFitModelGaussNWidget.ui \
	QFitModelFormulaWidget.ui \
	QFitModelFunctionWidget.ui \
	QFitDataWidget.ui \
	QFitRangeWidget.ui \
	QFitParWidget.ui \
	QFitAmplEstimWidget.ui \
	QFitOutputActionWidget.ui \
	QFitDependencyWidget.ui \
	QFitParCfgWidget.ui \
	QFitMinuitWidget.ui \
	QFitMinuitResWidget.ui \
	QFitSlotWidget.ui \
	QFitterWidget.ui \
	QFitTableWidget.ui \
	QFitPrintWidget.ui \
	QFitLinearTransWidget.ui \
	QFitMatrixTransWidget.ui \
	QFitPeakFinderWidget.ui \
	QFitRangeCutWidget.ui
