PLUGIN4_DIR = qt4/Go4plugin

ifdef DOPACKAGE
DISTRFILES         += $(PLUGIN4_DIR)/Module.mk
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.pro
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.h
DISTRFILES         += $(PLUGIN4_DIR)/go4plugin.qrc
DISTRFILES         += $(PLUGIN4_DIR)/QGo4WidgetPlugin.cpp 
DISTRFILES         += $(PLUGIN4_DIR)/QGo4LineEditPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4BrowserTreeWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QFitWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QFitNamedWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QFitModelWidgetPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4CommandsHistoryPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QRootCanvasPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QRootWindowPlugin.cpp
DISTRFILES         += $(PLUGIN4_DIR)/QGo4WidgetPlugin.h 
DISTRFILES         += $(PLUGIN4_DIR)/QGo4LineEditPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QGo4BrowserTreeWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QFitWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QFitNamedWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QFitModelWidgetPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QGo4CommandsHistoryPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QRootCanvasPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/QRootWindowPlugin.h
DISTRFILES         += $(PLUGIN4_DIR)/TGo4LockGuard.h
DISTRFILES         += $(PLUGIN4_DIR)/go4logo2.png
DISTRFILES         += $(PLUGIN4_DIR)/go4logo2_big.png
DISTRFILES         += $(PLUGIN4_DIR)/root.png
DISTRFILES         += $(PLUGIN4_DIR)/scale.png
endif

all-qt4plugin:
	cp -f qt4/Go4GUI/QGo4LineEdit.h $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4LineEdit.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4BrowserTreeWidget.h $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4BrowserTreeWidget.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4Widget.h $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4Widget.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitItem.h $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/TGo4FitPanel.h $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/ui_TGo4FitPanel.h $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitWidget.h $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitWidget.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitNamedWidget.h $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitNamedWidget.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitModelWidget.h $(PLUGIN4_DIR)
	cp -f qt4/Go4FitGUI/QFitModelWidget.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4CommandsHistory.h $(PLUGIN4_DIR)
	cp -f qt4/Go4GUI/QGo4CommandsHistory.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4QtRoot/QRootCanvas.h $(PLUGIN4_DIR)
	cp -f qt4/Go4QtRoot/QRootCanvas.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4QtRoot/QRootDialog.h $(PLUGIN4_DIR)
	cp -f qt4/Go4QtRoot/QRootDialog.cpp $(PLUGIN4_DIR)
	cp -f qt4/Go4QtRoot/QRootWindow.h $(PLUGIN4_DIR)
	cp -f qt4/Go4QtRoot/QRootWindow.cpp $(PLUGIN4_DIR)

#	cd $(PLUGIN4_DIR); $(QMAKE) go4plugin.pro; make all
	cd $(PLUGIN4_DIR); $(QMAKE) go4plugin.pro -o Makefile.qt $(QMAKEOPTFLAG) $(QMAKEFLAGS) $(QMAKELIBFLAGS) "LIBS+=$(ROOTGLIBS)" "INCLUDEPATH += $(ROOTINCPATH)" "GO4SYS = ../.."; $(MAKE) -f Makefile.qt

	@echo ""
	@echo "==========================================="
	@echo "Go4 plugin for Qt designer compiled"
	@echo "Now copy  $(PLUGIN4_DIR)/libgo4plugin.so  to"
	@echo "$(QTDIR)/plugins/designer directory"

clean-qt4plugin:
ifneq ($(wildcard $(PLUGIN4_DIR)/Makefile.qt),)
	cd $(PLUGIN4_DIR); $(MAKE) -f Makefile.qt distclean 
endif
	@rm -f $(PLUGIN4_DIR)/*.o $(PLUGIN4_DIR)/*.so $(PLUGIN4_DIR)/Makefile.qt
	@rm -fr $(PLUGIN4_DIR)/debug $(PLUGIN4_DIR)/release
	@rm -f $(PLUGIN4_DIR)/QGo4LineEdit.*
	@rm -f $(PLUGIN4_DIR)/QGo4BrowserTreeWidget.*
	@rm -f $(PLUGIN4_DIR)/QGo4Widget.*
	@rm -f $(PLUGIN4_DIR)/QFitItem.h $(PLUGIN4_DIR)/TGo4FitPanel.h $(PLUGIN4_DIR)/ui_TGo4FitPanel.h
	@rm -f $(PLUGIN4_DIR)/QFitWidget.*
	@rm -f $(PLUGIN4_DIR)/QFitNamedWidget.*
	@rm -f $(PLUGIN4_DIR)/QFitModelWidget.*
	@rm -f $(PLUGIN4_DIR)/QGo4CommandsHistory.*
	@rm -f $(PLUGIN4_DIR)/QRootCanvas.*
	@rm -f $(PLUGIN4_DIR)/QRootDialog.*
	@rm -f $(PLUGIN4_DIR)/QRootWindow.*

clean-qt4:: clean-qt4plugin
