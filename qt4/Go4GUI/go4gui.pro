TEMPLATE	= app 
LANGUAGE	= C++

CONFIG	+= qt warn_off thread dll

INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include ../Go4FitGUI

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


MOC_DIR=.moc
OBJECTS_DIR=.obj

TARGET = ../../bin/go4
MOC = $(SYSCONF_MOC)

RESOURCES += go4icons.qrc
PROJECTNAME = Go4GUI

DEPENDPATH	+= $(ROOTSYS)/include $(GO4SYS)/include
