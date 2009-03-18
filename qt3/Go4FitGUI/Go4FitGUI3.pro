MOC_DIR=.moc
OBJECTS_DIR=.obj

SOURCES	+= QFitItem.cpp \
            QFitWidget.cpp \
            QFitNamedWidget.cpp \
            QFitModelWidget.cpp

HEADERS	+= QFitItem.h \
            QFitWidget.h \
            QFitNamedWidget.h \
            QFitModelWidget.h

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

TEMPLATE = lib
CONFIG += qt warn_off thread
INCLUDEPATH += $(ROOTSYS)/include $(GO4SYS)/include
DEPENDPATH += $(ROOTSYS)/include $(GO4SYS)/include
LANGUAGE = C++
