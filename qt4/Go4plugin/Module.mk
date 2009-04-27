PLUGIN4_DIR = $(GO4SYS)/qt4/Go4plugin

ifdef DOPACKAGE
DISTRFILES         += $(PLUGIN4_DIR)/Module.mk
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.pro
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.h
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.qrc
DISTRFILES         += $(PLUGIN4_DIR)/QGo4WidgetPlugin.cpp 
DISTRFILES         += $(PLUGIN4_DIR)/QGo4LineEditPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4BrowserListViewPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QFitWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QFitNamedWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QFitModelWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4CommandsHistoryPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4RootCanvasPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/TQRootCanvasPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/TQRootWindowPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4WidgetPlugin.h 
DISTRFILES         += $(PLUGIN4_DIR)/QGo4LineEditPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QGo4BrowserListViewPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QFitWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QFitNamedWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QFitModelWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QGo4CommandsHistoryPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QGo4RootCanvasPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/TQRootCanvasPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/TQRootWindowPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/lockguard.h
DISTRFILES         += $(PLUGIN4_DIR)/go4logo2.png
DISTRFILES         += $(PLUGIN4_DIR)/go4logo2_big.png
DISTRFILES         += $(PLUGIN4_DIR)/root.png
DISTRFILES         += $(PLUGIN4_DIR)/scale.png
endif

all-qt4plugin:
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4LineEdit.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4LineEdit.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4BrowserListView.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4BrowserListView.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4Widget.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4Widget.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitItem.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/TGo4FitPanel.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/ui_TGo4FitPanel.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitWidget.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitWidget.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitNamedWidget.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitNamedWidget.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitModelWidget.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4FitGUI/QFitModelWidget.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4CommandsHistory.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4CommandsHistory.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4RootCanvas.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4GUI/QGo4RootCanvas.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootcanvas.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootcanvas.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootdialog.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootdialog.cpp $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootwindow.h $(PLUGIN4_DIR)
	cp -f $(GO4SYS)/qt4/Go4QtRoot/tqrootwindow.cpp $(PLUGIN4_DIR)

#	cd $(PLUGIN4_DIR); $(QMAKE) go4plugin.pro; make all
	cd $(PLUGIN4_DIR); $(QMAKE) go4plugin.pro $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(ROOTGLIBS) " ; make all

	@echo ""
	@echo "==========================================="
	@echo "Go4 plugin for Qt designer compiled"
	@echo "Now copy  $(PLUGIN4_DIR)/libgo4plugin.so  to"
	@echo "$(QTDIR)/plugins/designer directory"

clean-qt4plugin:
	cd $(PLUGIN4_DIR); $(QMAKE) go4plugin.pro $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(ROOTGLIBS) " ; make distclean; rm -f *.o *.so Makefile
	rm -fr $(PLUGIN4_DIR)/debug $(PLUGIN4_DIR)/release
	rm -f $(PLUGIN4_DIR)/QGo4LineEdit.*
	rm -f $(PLUGIN4_DIR)/QGo4BrowserListView.*
	rm -f $(PLUGIN4_DIR)/QGo4Widget.*
	rm -f $(PLUGIN4_DIR)/QFitItem.h $(PLUGIN4_DIR)/TGo4FitPanel.h $(PLUGIN4_DIR)/ui_TGo4FitPanel.h
	rm -f $(PLUGIN4_DIR)/QFitWidget.*
	rm -f $(PLUGIN4_DIR)/QFitNamedWidget.*
	rm -f $(PLUGIN4_DIR)/QFitModelWidget.*
	rm -f $(PLUGIN4_DIR)/QGo4CommandsHistory.*
	rm -f $(PLUGIN4_DIR)/QGo4RootCanvas.*
	rm -f $(PLUGIN4_DIR)/tqrootcanvas.*
	rm -f $(PLUGIN4_DIR)/tqrootdialog.*
	rm -f $(PLUGIN4_DIR)/tqrootwindow.*
