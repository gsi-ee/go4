PLUGIN_DIR = $(GO4SYS)/qt4/Go4plugin

ifdef DOPACKAGE
DISTRFILES         += $(PLUGIN_DIR)/go4plugin.pro
DISTRFILES         += $(PLUGIN_DIR)/go4plugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/go4plugin.h
DISTRFILES         += $(PLUGIN_DIR)/go4plugin.qrc
DISTRFILES         += $(PLUGIN_DIR)/QGo4WidgetPlugin.cpp 
DISTRFILES         += $(PLUGIN_DIR)/QGo4LineEditPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QGo4BrowserListViewPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QFitWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QFitNamedWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QFitModelWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QGo4CommandsHistoryPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QGo4RootCanvasPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/TQRootCanvasPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/TQRootWindowPlugin.cpp
DISTRFILES         += $(PLUGIN_DIR)/QGo4WidgetPlugin.h 
DISTRFILES         += $(PLUGIN_DIR)/QGo4LineEditPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/QGo4BrowserListViewPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/QFitWidgetPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/QFitNamedWidgetPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/QFitModelWidgetPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/QGo4CommandsHistoryPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/QGo4RootCanvasPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/TQRootCanvasPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/TQRootWindowPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/lockguard.h
DISTRFILES         += $(PLUGIN_DIR)/go4logo2.png
DISTRFILES         += $(PLUGIN_DIR)/go4logo2_big.png
DISTRFILES         += $(PLUGIN_DIR)/root.png
DISTRFILES         += $(PLUGIN_DIR)/scale.png
endif

plugin:
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4LineEdit.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4LineEdit.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4BrowserListView.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4BrowserListView.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4Widget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4Widget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitItem.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/TGo4FitPanel.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/ui_TGo4FitPanel.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitWidget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitWidget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitNamedWidget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitNamedWidget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitModelWidget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitModelWidget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4CommandsHistory.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4CommandsHistory.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4RootCanvas.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4RootCanvas.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4Table.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4Table.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootcanvas.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootcanvas.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqcanvasmenu.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqcanvasmenu.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootdialog.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootdialog.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootwindow.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootwindow.cpp $(PLUGIN_DIR)

#	cd $(PLUGIN_DIR); $(QMAKE) go4plugin.pro; make all
	cd $(PLUGIN_DIR); $(QMAKE) go4plugin.pro $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(ROOTGLIBS) " ; make all

	@echo ""
	@echo "==========================================="
	@echo "Go4 plugin for Qt designer compiled"
	@echo "Now copy  $(PLUGIN_DIR)/libgo4plugin.so  to"
	@echo "$(QTDIR)/plugins/designer directory"

clean-plugin:
	cd $(PLUGIN_DIR); $(QMAKE) go4plugin.pro $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(ROOTGLIBS) " ; make distclean; rm -f *.o *.so Makefile
	rm -fr $(PLUGIN_DIR)/debug $(PLUGIN_DIR)/release
	rm -f $(PLUGIN_DIR)/QGo4LineEdit.*
	rm -f $(PLUGIN_DIR)/QGo4BrowserListView.*
	rm -f $(PLUGIN_DIR)/QGo4Widget.*
	rm -f $(PLUGIN_DIR)/QFitItem.h $(PLUGIN_DIR)/TGo4FitPanel.h $(PLUGIN_DIR)/ui_TGo4FitPanel.h
	rm -f $(PLUGIN_DIR)/QFitWidget.*
	rm -f $(PLUGIN_DIR)/QFitNamedWidget.*
	rm -f $(PLUGIN_DIR)/QFitModelWidget.*
	rm -f $(PLUGIN_DIR)/QGo4CommandsHistory.*
	rm -f $(PLUGIN_DIR)/QGo4RootCanvas.*
	rm -f $(PLUGIN_DIR)/tqrootcanvas.*
	rm -f $(PLUGIN_DIR)/tqcanvasmenu.*
	rm -f $(PLUGIN_DIR)/tqrootdialog.*
	rm -f $(PLUGIN_DIR)/tqrootwindow.*
