TEMPLATE	= app 
LANGUAGE	= C++
MOC_DIR     =.moc
OBJECTS_DIR =.obj

CONFIG	+= qt warn_off thread

INCLUDEPATH += ../../include ../Go4FitGUI ../../
DEPENDPATH  += ../../include ../Go4FitGUI ../../

TARGET      = go4
DESTDIR     = ../../bin
PROJECTNAME = Go4GUI

RESOURCES += go4icons.qrc

win32:QMAKE_LFLAGS  += /nodefaultlib:msvcprt.lib /nodefaultlib:libcmt.lib /verbose:lib

win32:LIBS += ../../Go4Log/TGo4Log.obj


HEADERS	+= \
	TGo4HServerConnection.h\
	TGo4QSettings.h \
	TGo4AnalysisWindow.h \
	TGo4WorkSpace.h \
	QGo4LineEdit.h \
	QGo4Widget.h \
	QGo4BrowserTreeWidget.h \
	TGo4ASImage.h \
	TGo4MainWindow.h \
	QGo4CommandsHistory.h \
	TGo4ViewPanel.h \
	TGo4StartClient.h \
	TGo4AnalysisStatusMonitor.h \
	TGo4AnalysisConfiguration.h \
	TGo4ConfigStep.h \
	TGo4CreateNewHistogram.h \
	TGo4CreateNewCondition.h \
	TGo4CreateNewDynEntry.h \
	TGo4LogInfo.h \
	TGo4LoadedLibraries.h \
	TGo4TreeViewer.h \
	TGo4HisDrawOptions.h \
	TGo4HisDrawOptionsShort.h \
	TGo4ScaleOptions.h \
	TGo4DividePad.h \
	TGo4Style.h \
	TGo4EditDynEntry.h \
	TGo4ParaEdit.h \
	TGo4ConditionEditor.h \
	TGo4PrintWidget.h \
	TGo4SetScaleValues.h \
	TGo4ConditionInfo.h \
	TGo4HistogramInfo.h \
	TGo4LogSettings.h \
	TGo4EventInfo.h \
	TGo4OpenRemoteDialog.h \
	TGo4Browser.h \
	TGo4BrowserOptions.h \
	TGo4ConnectServer.h \
	TGo4MBSViewer.h \
	TGo4CommandLine.h \
	TGo4MacroDialog.h \
	TGo4MarkerSettings.h \
	TGo4OptStatsSettings.h

SOURCES	+= \
	TGo4QSettings.cpp \
	TGo4AnalysisWindow.cpp \
	TGo4WorkSpace.cpp \
	QGo4LineEdit.cpp \
	QGo4Widget.cpp \
	QGo4BrowserTreeWidget.cpp \
	TGo4ASImage.cpp \
	TGo4MainWindow.cpp \
	MainGo4GUI.cpp \
	QGo4CommandsHistory.cpp \
	TGo4ViewPanel.cpp \
	TGo4StartClient.cpp \
	TGo4AnalysisStatusMonitor.cpp \
	TGo4AnalysisConfiguration.cpp \
	TGo4ConfigStep.cpp \
	TGo4CreateNewHistogram.cpp \
	TGo4CreateNewCondition.cpp \
	TGo4CreateNewDynEntry.cpp \
	TGo4LogInfo.cpp \
	TGo4LoadedLibraries.cpp \
	TGo4TreeViewer.cpp \
	TGo4HisDrawOptions.cpp \
	TGo4HisDrawOptionsShort.cpp \
	TGo4ScaleOptions.cpp \
	TGo4DividePad.cpp \
	TGo4Style.cpp \
	TGo4EditDynEntry.cpp \
	TGo4ParaEdit.cpp \
	TGo4ConditionEditor.cpp \
	TGo4PrintWidget.cpp \
	TGo4SetScaleValues.cpp \
	TGo4ConditionInfo.cpp \
	TGo4HistogramInfo.cpp \
	TGo4LogSettings.cpp \
	TGo4EventInfo.cpp \
	TGo4OpenRemoteDialog.cpp \
	TGo4Browser.cpp \
	TGo4BrowserOptions.cpp \
	TGo4ConnectServer.cpp \
	TGo4MBSViewer.cpp \
	TGo4CommandLine.cpp \
	TGo4MacroDialog.cpp \
	TGo4MarkerSettings.cpp \
	TGo4OptStatsSettings.cpp 

FORMS += TGo4ViewPanel.ui \
	TGo4StartClient.ui \
	TGo4AnalysisStatusMonitor.ui \
	TGo4AnalysisConfiguration.ui \
	TGo4ConfigStep.ui \
	TGo4CreateNewHistogram.ui \
	TGo4CreateNewCondition.ui \
	TGo4CreateNewDynEntry.ui \
	TGo4LogInfo.ui \
	TGo4LoadedLibraries.ui \
	TGo4TreeViewer.ui \
	TGo4HisDrawOptions.ui \
	TGo4HisDrawOptionsShort.ui \
	TGo4ScaleOptions.ui \
	TGo4DividePad.ui \
	TGo4Style.ui \
	TGo4EditDynEntry.ui \
	TGo4ParaEdit.ui \
	TGo4ConditionEditor.ui \
	TGo4PrintWidget.ui \
	TGo4SetScaleValues.ui \
	TGo4ConditionInfo.ui \
	TGo4HistogramInfo.ui \
	TGo4LogSettings.ui \
	TGo4EventInfo.ui \
	TGo4OpenRemoteDialog.ui \
	TGo4Browser.ui \
	TGo4BrowserOptions.ui \
	TGo4HServerConnection.ui \
	TGo4ConnectServer.ui \
	TGo4MBSViewer.ui \
	TGo4CommandLine.ui \
	TGo4MacroDialog.ui \
	TGo4MarkerSettings.ui \
	TGo4OptStatsSettings.ui

	

SOURCES	+= ../Go4FitGUI/TGo4FitPanel.cpp \
            ../Go4FitGUI/QFitItem.cpp \
            ../Go4FitGUI/QFitWidget.cpp \
            ../Go4FitGUI/QFitNamedWidget.cpp \
            ../Go4FitGUI/QFitModelWidget.cpp \
            ../Go4FitGUI/QFitModelPolynomWidget.cpp \
            ../Go4FitGUI/QFitModelGauss1Widget.cpp \
            ../Go4FitGUI/QFitModelGauss2Widget.cpp \
            ../Go4FitGUI/QFitModelGaussNWidget.cpp \
            ../Go4FitGUI/QFitModelFormulaWidget.cpp \
            ../Go4FitGUI/QFitModelFunctionWidget.cpp \
            ../Go4FitGUI/QFitDataWidget.cpp \
            ../Go4FitGUI/QFitRangeWidget.cpp \
            ../Go4FitGUI/QFitParWidget.cpp \
            ../Go4FitGUI/QFitAmplEstimWidget.cpp \
            ../Go4FitGUI/QFitOutputActionWidget.cpp \
            ../Go4FitGUI/QFitDependencyWidget.cpp \
            ../Go4FitGUI/QFitParCfgWidget.cpp \
            ../Go4FitGUI/QFitMinuitWidget.cpp \
            ../Go4FitGUI/QFitMinuitResWidget.cpp \
            ../Go4FitGUI/QFitSlotWidget.cpp \
            ../Go4FitGUI/QFitterWidget.cpp \
            ../Go4FitGUI/QFitTableWidget.cpp \
            ../Go4FitGUI/QFitPrintWidget.cpp \
            ../Go4FitGUI/QFitLinearTransWidget.cpp \
            ../Go4FitGUI/QFitMatrixTransWidget.cpp \
            ../Go4FitGUI/QFitPeakFinderWidget.cpp \
            ../Go4FitGUI/QFitRangeCutWidget.cpp

HEADERS	+= ../Go4FitGUI/TGo4FitPanel.h \
            ../Go4FitGUI/QFitItem.h \
            ../Go4FitGUI/QFitWidget.h \
            ../Go4FitGUI/QFitNamedWidget.h \
            ../Go4FitGUI/QFitModelWidget.h \
            ../Go4FitGUI/QFitModelPolynomWidget.h \
            ../Go4FitGUI/QFitModelGauss1Widget.h \
            ../Go4FitGUI/QFitModelGauss2Widget.h \
            ../Go4FitGUI/QFitModelGaussNWidget.h \
            ../Go4FitGUI/QFitModelFormulaWidget.h \
            ../Go4FitGUI/QFitModelFunctionWidget.h \
            ../Go4FitGUI/QFitDataWidget.h \
            ../Go4FitGUI/QFitRangeWidget.h \
            ../Go4FitGUI/QFitParWidget.h \
            ../Go4FitGUI/QFitAmplEstimWidget.h \
            ../Go4FitGUI/QFitOutputActionWidget.h \
            ../Go4FitGUI/QFitDependencyWidget.h \
            ../Go4FitGUI/QFitParCfgWidget.h \
            ../Go4FitGUI/QFitMinuitWidget.h \
            ../Go4FitGUI/QFitMinuitResWidget.h \
            ../Go4FitGUI/QFitSlotWidget.h \
            ../Go4FitGUI/QFitterWidget.h \
            ../Go4FitGUI/QFitTableWidget.h \
            ../Go4FitGUI/QFitPrintWidget.h \
            ../Go4FitGUI/QFitLinearTransWidget.h \
            ../Go4FitGUI/QFitMatrixTransWidget.h \
            ../Go4FitGUI/QFitPeakFinderWidget.h \
            ../Go4FitGUI/QFitRangeCutWidget.h

FORMS	= ../Go4FitGUI/TGo4FitPanel.ui \
	../Go4FitGUI/QFitModelPolynomWidget.ui \
	../Go4FitGUI/QFitModelGauss1Widget.ui \
	../Go4FitGUI/QFitModelGauss2Widget.ui \
	../Go4FitGUI/QFitModelGaussNWidget.ui \
	../Go4FitGUI/QFitModelFormulaWidget.ui \
	../Go4FitGUI/QFitModelFunctionWidget.ui \
	../Go4FitGUI/QFitDataWidget.ui \
	../Go4FitGUI/QFitRangeWidget.ui \
	../Go4FitGUI/QFitParWidget.ui \
	../Go4FitGUI/QFitAmplEstimWidget.ui \
	../Go4FitGUI/QFitOutputActionWidget.ui \
	../Go4FitGUI/QFitDependencyWidget.ui \
	../Go4FitGUI/QFitParCfgWidget.ui \
	../Go4FitGUI/QFitMinuitWidget.ui \
	../Go4FitGUI/QFitMinuitResWidget.ui \
	../Go4FitGUI/QFitSlotWidget.ui \
	../Go4FitGUI/QFitterWidget.ui \
	../Go4FitGUI/QFitTableWidget.ui \
	../Go4FitGUI/QFitPrintWidget.ui \
	../Go4FitGUI/QFitLinearTransWidget.ui \
	../Go4FitGUI/QFitMatrixTransWidget.ui \
	../Go4FitGUI/QFitPeakFinderWidget.ui \
	../Go4FitGUI/QFitRangeCutWidget.ui

SOURCES += ../Go4QtRoot/QRootApplication.cpp \
           ../Go4QtRoot/QRootCanvas.cpp \
           ../Go4QtRoot/QRootDialog.cpp \
           ../Go4QtRoot/QRootWindow.cpp 

HEADERS += ../Go4QtRoot/QRootApplication.h \
           ../Go4QtRoot/QRootCanvas.h \
           ../Go4QtRoot/QRootDialog.h \
           ../Go4QtRoot/QRootWindow.h

SOURCES += TGo4DrawCloneProxy.cxx \
           TGo4DrawObjProxy.cxx \
           TGo4Script.cxx \
           TGo4WidgetProxy.cxx \
           G__Go4GUI.cxx

#HEADERS += TGo4DrawCloneProxy.h \
#           TGo4DrawObjProxy.h \
#           TGo4Script.h \
#           TGo4WidgetProxy.h
	