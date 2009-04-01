TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off thread

INCLUDEPATH	+= $(ROOTSYS)/include $(GO4SYS)/include

HEADERS	+= \
	TGo4QSettings.h \
	TGo4AnalysisWindow.h \
	QGo4RootCanvas.h \
	TGo4WorkSpace.h \
	QGo4LineEdit.h \
	QGo4Table.h \
	QGo4Widget.h \
	QGo4BrowserListView.h \
	QGo4BrowserItem.h \
	TGo4ASImage.h \
	TGo4MainWindow.h \
	QGo4CommandsHistory.h

SOURCES	+= \
	TGo4QSettings.cpp \
	TGo4AnalysisWindow.cpp \
	QGo4RootCanvas.cpp \
	TGo4WorkSpace.cpp \
	QGo4LineEdit.cpp \
	QGo4Table.cpp \
	QGo4Widget.cpp \
	QGo4BrowserListView.cpp \
	QGo4BrowserItem.cpp \
	TGo4ASImage.cpp \
	TGo4MainWindow.cpp \
	MainGo4GUI.cpp \
	QGo4CommandsHistory.cpp

FORMS	+= TGo4ViewPanel.ui \
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

IMAGES	= ../../icons/branch_t.png \
	../../icons/editcopy.png \
	../../icons/editcut.png \
	../../icons/editpaste.png \
	../../icons/filenew.png \
	../../icons/fileopen.png \
	../../icons/filesave.png \
	../../icons/folder_t.png \
	../../icons/go4logo2_big.png \
	../../icons/h1_t.png \
	../../icons/h2_t.png \
	../../icons/leaf_t.png \
	../../icons/object_folder_t.png \
	../../icons/print.png \
	../../icons/redo.png \
	../../icons/right.png \
	../../icons/root_t.png \
	../../icons/rootdb_t.png \
	../../icons/searchfind.png \
	../../icons/tree_t.png \
	../../icons/undo.png \
	../../icons/branch-cl_t.png \
	../../icons/branch-ob_t.png \
	../../icons/branch_folder_s.png \
	../../icons/branch_folder_t_1.png \
	../../icons/branch_t_1.png \
	../../icons/leaf_s.png \
	../../icons/leaf_t_1.png \
	../../icons/h2_t_1.png \
	../../icons/x_pic.png \
	../../icons/y_pic.png \
	../../icons/z_pic.png \
	../../icons/cut_t.png \
	../../icons/editcut_1.png \
	../../icons/refresh.png \
	../../icons/superimpose.png \
	../../icons/h3_t.png \
	../../icons/sizeh1.png \
	../../icons/downarrow.png \
	../../icons/leftarrow.png \
	../../icons/rightarrow.png \
	../../icons/sizev1.png \
	../../icons/sizeh.png \
	../../icons/sizev.png \
	../../icons/sizeh.png \
	../../icons/uparrow.png \
	../../icons/zoom.png \
	../../icons/cont_bw.png \
	../../icons/asimage.png \
	../../icons/lego1.png \
	../../icons/lego2.png \
	../../icons/lego.png \
	../../icons/surf.png \
	../../icons/surf2.png \
	../../icons/surf_wire.png \
	../../icons/cont_2.png \
	../../icons/macro_t.png \
	../../icons/warn.png \
	../../icons/info.png \
	../../icons/start.png \
	../../icons/info1.png \
	../../icons/ObjBrowser.png \
	../../icons/ObjBrowserD.png \
	../../icons/ObjBrowserL.png \
	../../icons/ObjBrowserR.png \
	../../icons/chart.png \
	../../icons/control.png \
	../../icons/restart.png \
	../../icons/user.png \
	../../icons/draw_t.png \
	../../icons/tree_s.png \
	../../icons/launchanal.png \
	../../icons/connect.png \
	../../icons/disconnect.png \
	../../icons/killanal.png \
	../../icons/killanalysis.png \
	../../icons/shutanal.png \
	../../icons/savewin.png \
	../../icons/saveall.png \
	../../icons/Stop.png \
	../../icons/dllicon.png \
	../../icons/monitor.png \
	../../icons/close1.png \
	../../icons/log.png \
	../../icons/savelog.png \
	../../icons/exit.png \
	../../icons/delete.png \
	../../icons/refresh.png \
	../../icons/error.png \
	../../icons/drawoptions.png \
	../../icons/scale.png \
	../../icons/changelincol.png \
	../../icons/changefillcol.png \
	../../icons/changemarcol.png \
	../../icons/divide.png \
	../../icons/polycond.png \
	../../icons/polycondarray.png \
	../../icons/windcond.png \
	../../icons/windcondarray.png \
	../../icons/findfile.png \
	../../icons/mb_question_s.png \
	../../icons/sizeall.png \
	../../icons/surf5.png \
	../../icons/color.png \
	../../icons/profile_t.png \
	../../icons/canvas.png \
	../../icons/update.png \
	../../icons/startselected.png \
	../../icons/dynentryx.png \
	../../icons/dyntreex.png \
	../../icons/zoomxin.png \
	../../icons/zoomxout.png \
	../../icons/zoomyin.png \
	../../icons/zoomyout.png \
	../../icons/zoomzin.png \
	../../icons/zoomzout.png \
	../../icons/zoomoff.png \
	../../icons/zoomicon.png \
	../../icons/shiftdown.png \
	../../icons/shiftleft.png \
	../../icons/shiftright.png \
	../../icons/shiftup.png \
	../../icons/shiftzup.png \
	../../icons/shiftzdown.png \
	../../icons/analysiswin.png \
	../../icons/fitpanel.png \
	../../icons/clear.png \
	../../icons/style.png \
	../../icons/marble.png \
	../../icons/metal.png \
	../../icons/stone1.png \
	../../icons/stonebright.png \
	../../icons/stonedark.png \
	../../icons/info.png \
	../../icons/root.png \
	../../icons/fitter.png \
	../../icons/parameter.png \
	../../icons/dynlist.png \
	../../icons/left.png \
	../../icons/closewindow.png \
	../../icons/cursormarker.png \
	../../icons/cursormarkersmall.png \
	../../icons/zoomlim.png \
	../../icons/adjustsize.png \
	../../icons/back.xpm \
	../../icons/close.png \
	../../icons/condcre.png \
	../../icons/condedit.png \
	../../icons/condlist.png \
	../../icons/del.png \
	../../icons/forward.xpm \
	../../icons/go4logo2.png \
	../../icons/go4logo_t.png \
	../../icons/hiscre.png \
	../../icons/hislist.png \
	../../icons/histserv.png \
	../../icons/home.xpm \
	../../icons/icons.png \
	../../icons/infob.png \
	../../icons/marble.xpm \
	../../icons/metal.xpm \
	../../icons/rootdb_t_1.png \
	../../icons/stone1.xpm \
	../../icons/stonebright.xpm \
	../../icons/stonedark.xpm \
	../../icons/cont0.png \
	../../icons/colz_new.png \
	../../icons/colz.png \
	../../icons/cont2.png \
	../../icons/cont3.png \
	../../icons/cont1.png \
	../../icons/col.png \
	../../icons/pixz_new.png \
	../../icons/super_new.png \
	../../icons/surf1.png \
	../../icons/surf3.png \
	../../icons/surf4.png \
	../../icons/dynlistlist.png \
	../../icons/picture.png \
	../../icons/tgraph.png \
	../../icons/network.png \
	../../icons/fileclose.png \
	../../icons/clear_ok.png \
	../../icons/clear_nok.png \
	../../icons/parabutton.png \
	../../icons/mbslogo.png \
	../../icons/mbslogorun.gif \
	../../icons/mbsbutton.png \
	../../icons/mbsrun.gif \
	../../icons/eventitem.png \
	../../icons/eventobj.png \
	../../icons/folder.png \
	../../icons/rename.png \
	../../icons/export.png \
	../../icons/copyws.png \
	../../icons/crefolder.png \
	../../icons/open.png \
	../../icons/cursor.png \
	../../icons/scalexlin.png \
	../../icons/scalexlog.png \
	../../icons/scaleylin.png \
	../../icons/scaleylog.png \
	../../icons/scalezlin.png \
	../../icons/scalezlog.png \
	../../icons/draw1dhis.png \
	../../icons/draw1dline.png

MOC_DIR=.moc
OBJECTS_DIR=.obj

TARGET = ../../bin/go4
MOC = $(SYSCONF_MOC)
IMAGEFILE = images.cpp
PROJECTNAME = Go4GUI

DEPENDPATH	+= $(ROOTSYS)/include $(GO4SYS)/include
