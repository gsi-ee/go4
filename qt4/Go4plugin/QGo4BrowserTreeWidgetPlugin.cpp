#include "QGo4BrowserTreeWidget.h"
#include "QGo4BrowserTreeWidgetPlugin.h"
#include <QtPlugin>



QGo4BrowserTreeWidgetPlugin::QGo4BrowserTreeWidgetPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QGo4BrowserTreeWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QGo4BrowserTreeWidgetPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QGo4BrowserTreeWidgetPlugin::createWidget(QWidget *parent)
 {
     return new QGo4BrowserTreeWidget(parent);
 }

 QString QGo4BrowserTreeWidgetPlugin::name() const
 {
     return "QGo4BrowserTreeWidget";
 }

 QString QGo4BrowserTreeWidgetPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QGo4BrowserTreeWidgetPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QGo4BrowserTreeWidgetPlugin::toolTip() const
 {
     return "Go4browser list view";

 }

 QString QGo4BrowserTreeWidgetPlugin::whatsThis() const
 {
     return "List view, used in browser to handle start drag event of its items";
 }

 bool QGo4BrowserTreeWidgetPlugin::isContainer() const
 {
     return false;
 }

 QString QGo4BrowserTreeWidgetPlugin::domXml() const
 {
     return "<widget class=\"QGo4BrowserTreeWidget\" name=\"Go4BrowserListView widget\">\n"
            " <property name=\"geometry\">\n"
            "  <rect>\n"
            "   <x>0</x>\n"
            "   <y>0</y>\n"
            "   <width>100</width>\n"
            "   <height>100</height>\n"
            "  </rect>\n"
            " </property>\n"
            "</widget>\n";
 }

 QString QGo4BrowserTreeWidgetPlugin::includeFile() const
 {
     return "QGo4BrowserTreeWidget.h";
 }


