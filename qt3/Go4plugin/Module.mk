PLUGIN3_DIR = $(GO4SYS)/qt3/Go4plugin

ifdef DOPACKAGE
DISTRFILES         += $(PLUGIN3_DIR)/go4plugin.pro
DISTRFILES         += $(PLUGIN3_DIR)/TGo4QtDesignerPlugin.h
DISTRFILES         += $(PLUGIN3_DIR)/TGo4QtDesignerPlugin.cpp
endif

all-qt3plugin:
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4LineEdit.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4LineEdit.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Table.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Table.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4BrowserListView.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4BrowserListView.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Widget.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Widget.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitWidget.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitWidget.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitNamedWidget.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitNamedWidget.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitModelWidget.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitModelWidget.cpp $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4CommandsHistory.h $(PLUGIN3_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4CommandsHistory.cpp $(PLUGIN3_DIR)
	cd $(PLUGIN3_DIR); $(QMAKE) go4plugin.pro; make all
	@echo ""
	@echo "==========================================="
	@echo "Go4 plugin for Qt designer compiled"
	@echo "Now copy $(PLUGIN3_DIR)/libgo4plugin.so file to"
	@echo "$(QTDIR)/plugins/designer directory"

clean-qt3plugin:
	@cd $(PLUGIN3_DIR); rm -f QGo4*.* QFit*.* moc_*.* *.o *.so Makefile
