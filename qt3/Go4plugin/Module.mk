PLUGIN_DIR = $(GO4SYS)/qt3/Go4plugin

ifdef DOPACKAGE
DISTRFILES         += $(PLUGIN_DIR)/go4plugin.pro
DISTRFILES         += $(PLUGIN_DIR)/TGo4QtDesignerPlugin.h
DISTRFILES         += $(PLUGIN_DIR)/TGo4QtDesignerPlugin.cpp
endif

plugin:
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4LineEdit.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4LineEdit.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Table.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Table.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4BrowserListView.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4BrowserListView.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Widget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4Widget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitWidget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitWidget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitNamedWidget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitNamedWidget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitModelWidget.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4FitGUI/QFitModelWidget.cpp $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4CommandsHistory.h $(PLUGIN_DIR)
	cp -f $(GO4SYS)/qt3/Go4GUI/QGo4CommandsHistory.cpp $(PLUGIN_DIR)
	cd $(PLUGIN_DIR); $(QMAKE) go4plugin.pro; make all
	@echo ""
	@echo "==========================================="
	@echo "Go4 plugin for Qt designer compiled"
	@echo "Now copy $(PLUGIN_DIR)/libgo4plugin.so file to"
	@echo "$(QTDIR)/plugins/designer directory"

clean-plugin:
	@cd $(PLUGIN_DIR); rm -f QGo4*.* QFit*.* moc_*.* *.o *.so Makefile
