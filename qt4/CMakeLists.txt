set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(qt4)
   set(CMAKE_INCLUDE_CURRENT_DIR ON)
   find_package(Qt4 4.7 REQUIRED QtGui QtXml QtNetwork)
   if(NOT Qt4_FOUND)
      message(FATAL_ERROR "Could NOT find Qt4")
   endif()
   set(qt_libs Qt4::QtCore Qt4::QtGui Qt4::QtNetwork)
else()
   if(ROOT_qt5web_FOUND)
      set(qt5comp WebEngine WebEngineWidgets)
   endif()
   find_package(Qt5 COMPONENTS Widgets Network ${qt5comp} REQUIRED)
   if(NOT Qt5_FOUND)
      message(FATAL_ERROR "Could NOT find Qt5")
   endif()
   set(qt_libs Qt5::Widgets Qt5::Network)
endif()


#if(ROOT_x11_FOUND)
#   add_subdirectory(Go4QtRoot)
#endif()

#if(ROOT_qt5web_FOUND)
#   add_subdirectory(Go4Web)
#endif()

set(Go4GUI_headers
   Go4GUI/TGo4HServerConnection.h
   Go4GUI/TGo4HttpProxy.h
   Go4GUI/TGo4QSettings.h
   Go4GUI/TGo4AnalysisWindow.h
   Go4GUI/TGo4MdiArea.h
   Go4GUI/QGo4LineEdit.h
   Go4GUI/QGo4Widget.h
   Go4GUI/QGo4BrowserTreeWidget.h
   Go4GUI/TGo4ASImage.h
   Go4GUI/TGo4MainWindow.h
   Go4GUI/QGo4CommandsHistory.h
   Go4GUI/TGo4ViewPanel.h
   Go4GUI/TGo4StartClient.h
   Go4GUI/TGo4AnalysisStatusMonitor.h
   Go4GUI/TGo4AnalysisConfiguration.h
   Go4GUI/TGo4ConfigStep.h
   Go4GUI/TGo4CreateNewHistogram.h
   Go4GUI/TGo4CreateNewCondition.h
   Go4GUI/TGo4CreateNewDynEntry.h
   Go4GUI/TGo4LogInfo.h
   Go4GUI/TGo4LoadedLibraries.h
   Go4GUI/TGo4TreeViewer.h
   Go4GUI/TGo4HisDrawOptions.h
   Go4GUI/TGo4HisDrawOptionsShort.h
   Go4GUI/TGo4ScaleOptions.h
   Go4GUI/TGo4DividePad.h
   Go4GUI/TGo4Style.h
   Go4GUI/TGo4EditDynEntry.h
   Go4GUI/TGo4ParaEdit.h
   Go4GUI/TGo4ConditionEditor.h
   Go4GUI/TGo4PrintWidget.h
   Go4GUI/TGo4SetScaleValues.h
   Go4GUI/TGo4ConditionInfo.h
   Go4GUI/TGo4HistogramInfo.h
   Go4GUI/TGo4LogSettings.h
   Go4GUI/TGo4EventInfo.h
   Go4GUI/TGo4OpenRemoteDialog.h
   Go4GUI/TGo4Browser.h
   Go4GUI/TGo4BrowserOptions.h
   Go4GUI/TGo4ConnectServer.h
   Go4GUI/TGo4MBSViewer.h
   Go4GUI/TGo4CommandLine.h
   Go4GUI/TGo4MacroDialog.h
   Go4GUI/TGo4MarkerSettings.h
   Go4GUI/TGo4OptStatsSettings.h
   Go4GUI/TGo4UserCommands.h
   Go4GUI/TGo4UserCommandsDialog.h
)

set(Go4GUI_sources
   Go4GUI/TGo4QSettings.cpp
   Go4GUI/TGo4HttpProxy.cpp
   Go4GUI/TGo4AnalysisWindow.cpp
   Go4GUI/TGo4MdiArea.cpp
   Go4GUI/QGo4LineEdit.cpp
   Go4GUI/QGo4Widget.cpp
   Go4GUI/QGo4BrowserTreeWidget.cpp
   Go4GUI/TGo4ASImage.cpp
   Go4GUI/TGo4MainWindow.cpp
   Go4GUI/QGo4CommandsHistory.cpp
   Go4GUI/TGo4ViewPanel.cpp
   Go4GUI/TGo4StartClient.cpp
   Go4GUI/TGo4AnalysisStatusMonitor.cpp
   Go4GUI/TGo4AnalysisConfiguration.cpp
   Go4GUI/TGo4ConfigStep.cpp
   Go4GUI/TGo4CreateNewHistogram.cpp
   Go4GUI/TGo4CreateNewCondition.cpp
   Go4GUI/TGo4CreateNewDynEntry.cpp
   Go4GUI/TGo4LogInfo.cpp
   Go4GUI/TGo4LoadedLibraries.cpp
   Go4GUI/TGo4TreeViewer.cpp
   Go4GUI/TGo4HisDrawOptions.cpp
   Go4GUI/TGo4HisDrawOptionsShort.cpp
   Go4GUI/TGo4ScaleOptions.cpp
   Go4GUI/TGo4DividePad.cpp
   Go4GUI/TGo4Style.cpp
   Go4GUI/TGo4EditDynEntry.cpp
   Go4GUI/TGo4ParaEdit.cpp
   Go4GUI/TGo4ConditionEditor.cpp
   Go4GUI/TGo4PrintWidget.cpp
   Go4GUI/TGo4SetScaleValues.cpp
   Go4GUI/TGo4ConditionInfo.cpp
   Go4GUI/TGo4HistogramInfo.cpp
   Go4GUI/TGo4LogSettings.cpp
   Go4GUI/TGo4EventInfo.cpp
   Go4GUI/TGo4OpenRemoteDialog.cpp
   Go4GUI/TGo4Browser.cpp
   Go4GUI/TGo4BrowserOptions.cpp
   Go4GUI/TGo4HServerConnection.cpp
   Go4GUI/TGo4ConnectServer.cpp
   Go4GUI/TGo4MBSViewer.cpp
   Go4GUI/TGo4CommandLine.cpp
   Go4GUI/TGo4MacroDialog.cpp
   Go4GUI/TGo4MarkerSettings.cpp
   Go4GUI/TGo4OptStatsSettings.cpp
   Go4GUI/TGo4UserCommands.cpp
   Go4GUI/TGo4UserCommandsDialog.cpp
)

set(Go4GUI_forms
   Go4GUI/TGo4ViewPanel.ui
   Go4GUI/TGo4StartClient.ui
   Go4GUI/TGo4AnalysisStatusMonitor.ui
   Go4GUI/TGo4AnalysisConfiguration.ui
   Go4GUI/TGo4ConfigStep.ui
   Go4GUI/TGo4CreateNewHistogram.ui
   Go4GUI/TGo4CreateNewCondition.ui
   Go4GUI/TGo4CreateNewDynEntry.ui
   Go4GUI/TGo4LogInfo.ui
   Go4GUI/TGo4LoadedLibraries.ui
   Go4GUI/TGo4TreeViewer.ui
   Go4GUI/TGo4HisDrawOptions.ui
   Go4GUI/TGo4HisDrawOptionsShort.ui
   Go4GUI/TGo4ScaleOptions.ui
   Go4GUI/TGo4DividePad.ui
   Go4GUI/TGo4Style.ui
   Go4GUI/TGo4EditDynEntry.ui
   Go4GUI/TGo4ParaEdit.ui
   Go4GUI/TGo4ConditionEditor.ui
   Go4GUI/TGo4PrintWidget.ui
   Go4GUI/TGo4SetScaleValues.ui
   Go4GUI/TGo4ConditionInfo.ui
   Go4GUI/TGo4HistogramInfo.ui
   Go4GUI/TGo4LogSettings.ui
   Go4GUI/TGo4EventInfo.ui
   Go4GUI/TGo4OpenRemoteDialog.ui
   Go4GUI/TGo4Browser.ui
   Go4GUI/TGo4BrowserOptions.ui
   Go4GUI/TGo4HServerConnection.ui
   Go4GUI/TGo4ConnectServer.ui
   Go4GUI/TGo4MBSViewer.ui
   Go4GUI/TGo4CommandLine.ui
   Go4GUI/TGo4MacroDialog.ui
   Go4GUI/TGo4MarkerSettings.ui
   Go4GUI/TGo4OptStatsSettings.ui
   Go4GUI/TGo4UserCommands.ui
   Go4GUI/TGo4UserCommandsDialog.ui
)

set(Go4FitGUI_sources
   Go4FitGUI/TGo4FitPanel.cpp
   Go4FitGUI/QFitItem.cpp
   Go4FitGUI/QFitWidget.cpp
   Go4FitGUI/QFitNamedWidget.cpp
   Go4FitGUI/QFitModelWidget.cpp
   Go4FitGUI/QFitModelPolynomWidget.cpp
   Go4FitGUI/QFitModelGauss1Widget.cpp
   Go4FitGUI/QFitModelGauss2Widget.cpp
   Go4FitGUI/QFitModelGaussNWidget.cpp
   Go4FitGUI/QFitModelFormulaWidget.cpp
   Go4FitGUI/QFitModelFunctionWidget.cpp
   Go4FitGUI/QFitDataWidget.cpp
   Go4FitGUI/QFitRangeWidget.cpp
   Go4FitGUI/QFitParWidget.cpp
   Go4FitGUI/QFitAmplEstimWidget.cpp
   Go4FitGUI/QFitOutputActionWidget.cpp
   Go4FitGUI/QFitDependencyWidget.cpp
   Go4FitGUI/QFitParCfgWidget.cpp
   Go4FitGUI/QFitMinuitWidget.cpp
   Go4FitGUI/QFitMinuitResWidget.cpp
   Go4FitGUI/QFitSlotWidget.cpp
   Go4FitGUI/QFitterWidget.cpp
   Go4FitGUI/QFitTableWidget.cpp
   Go4FitGUI/QFitPrintWidget.cpp
   Go4FitGUI/QFitLinearTransWidget.cpp
   Go4FitGUI/QFitMatrixTransWidget.cpp
   Go4FitGUI/QFitPeakFinderWidget.cpp
   Go4FitGUI/QFitRangeCutWidget.cpp
)

set(Go4FitGUI_headers
   Go4FitGUI/TGo4FitPanel.h
   Go4FitGUI/QFitItem.h
   Go4FitGUI/QFitWidget.h
   Go4FitGUI/QFitNamedWidget.h
   Go4FitGUI/QFitModelWidget.h
   Go4FitGUI/QFitModelPolynomWidget.h
   Go4FitGUI/QFitModelGauss1Widget.h
   Go4FitGUI/QFitModelGauss2Widget.h
   Go4FitGUI/QFitModelGaussNWidget.h
   Go4FitGUI/QFitModelFormulaWidget.h
   Go4FitGUI/QFitModelFunctionWidget.h
   Go4FitGUI/QFitDataWidget.h
   Go4FitGUI/QFitRangeWidget.h
   Go4FitGUI/QFitParWidget.h
   Go4FitGUI/QFitAmplEstimWidget.h
   Go4FitGUI/QFitOutputActionWidget.h
   Go4FitGUI/QFitDependencyWidget.h
   Go4FitGUI/QFitParCfgWidget.h
   Go4FitGUI/QFitMinuitWidget.h
   Go4FitGUI/QFitMinuitResWidget.h
   Go4FitGUI/QFitSlotWidget.h
   Go4FitGUI/QFitterWidget.h
   Go4FitGUI/QFitTableWidget.h
   Go4FitGUI/QFitPrintWidget.h
   Go4FitGUI/QFitLinearTransWidget.h
   Go4FitGUI/QFitMatrixTransWidget.h
   Go4FitGUI/QFitPeakFinderWidget.h
   Go4FitGUI/QFitRangeCutWidget.h
)

set(Go4FitGUI_forms
   Go4FitGUI/TGo4FitPanel.ui
   Go4FitGUI/QFitModelPolynomWidget.ui
   Go4FitGUI/QFitModelGauss1Widget.ui
   Go4FitGUI/QFitModelGauss2Widget.ui
   Go4FitGUI/QFitModelGaussNWidget.ui
   Go4FitGUI/QFitModelFormulaWidget.ui
   Go4FitGUI/QFitModelFunctionWidget.ui
   Go4FitGUI/QFitDataWidget.ui
   Go4FitGUI/QFitRangeWidget.ui
   Go4FitGUI/QFitParWidget.ui
   Go4FitGUI/QFitAmplEstimWidget.ui
   Go4FitGUI/QFitOutputActionWidget.ui
   Go4FitGUI/QFitDependencyWidget.ui
   Go4FitGUI/QFitParCfgWidget.ui
   Go4FitGUI/QFitMinuitWidget.ui
   Go4FitGUI/QFitMinuitResWidget.ui
   Go4FitGUI/QFitSlotWidget.ui
   Go4FitGUI/QFitterWidget.ui
   Go4FitGUI/QFitTableWidget.ui
   Go4FitGUI/QFitPrintWidget.ui
   Go4FitGUI/QFitLinearTransWidget.ui
   Go4FitGUI/QFitMatrixTransWidget.ui
   Go4FitGUI/QFitPeakFinderWidget.ui
   Go4FitGUI/QFitRangeCutWidget.ui
)

set(QtRoot_app_sources
   Go4QtRoot/QRootApplication.cpp
)

set(QtRoot_headers
   Go4QtRoot/QRootApplication.h
)

set(extra_libs Minuit Ged TreePlayer)
set(gui_defines)

if(ROOT_x11_FOUND)
   set(QtRoot_sources
      Go4QtRoot/QRootCanvas.cpp
      Go4QtRoot/QRootDialog.cpp
      Go4QtRoot/QRootWindow.cpp
   )
   list(APPEND extra_libs Gui X11)
   list(APPEND gui_defines -D__GO4X11__)
endif()

if(Qt5_FOUND AND ROOT_qt5web_FOUND)
   set(Go4Web_sources
      Go4Web/QWebCanvas.cpp
   )
   list(APPEND extra_libs RHTTP ROOTWebDisplay ROOTQt5WebDisplay WebGui6 WebGui6Full)
   list(APPEND qt_libs Qt5::WebEngine Qt5::WebEngineWidgets)
   list(APPEND gui_defines -D__GO4WEB__)
endif()

if(ROOT_asimage_FOUND)
   list(APPEND extra_libs ASImage)
else()
   list(APPEND gui_defines -D__NOGO4ASI__)
endif()

if(ROOT_spectrum_FOUND)
   list(APPEND extra_libs Spectrum)
endif()

if(HDF5_FOUND)
   list(APPEND gui_defines -D__GO4HDF5__)
endif()


set(Go4GUI_root_sources
   Go4GUI/TGo4DrawCloneProxy.cxx
   Go4GUI/TGo4DrawObjProxy.cxx
   Go4GUI/TGo4Script.cxx
   Go4GUI/TGo4WidgetProxy.cxx
)


add_executable(go4
    ${Go4FitGUI_forms}
    ${Go4GUI_forms}
    ${QtRoot_app_sources}
    ${QtRoot_sources}
    ${Go4Web_sources}
    ${Go4FitGUI_sources}
    ${Go4GUI_root_sources}
    ${Go4GUI_sources}
    Go4GUI/MainGo4GUI.cpp
    Go4GUI/go4icons.qrc
)

target_include_directories(go4 PRIVATE
   ${CMAKE_BINARY_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/Go4Web
   ${CMAKE_CURRENT_SOURCE_DIR}/Go4QtRoot
   ${CMAKE_CURRENT_SOURCE_DIR}/Go4FitGUI
   ${CMAKE_CURRENT_SOURCE_DIR}/Go4GUI
)

link_directories(${CMAKE_BINARY_DIR}/lib)

target_link_libraries(go4
   ${qt_libs}
   ${ROOT_LIBRARIES}
   ${extra_libs}
   Go4Fit Go4Base Go4ThreadManager Go4TaskHandler Go4AnalysisBase Go4Gui
)

target_compile_definitions(go4 PRIVATE ${GO4_DEFINITIONS} ${gui_defines})

add_dependencies(go4 move_headers)

