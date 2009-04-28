#include "QGo4BrowserListView.h"
#include "QGo4BrowserListViewPlugin.h"
#include <QtPlugin>



QGo4BrowserListViewPlugin::QGo4BrowserListViewPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QGo4BrowserListViewPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QGo4BrowserListViewPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QGo4BrowserListViewPlugin::createWidget(QWidget *parent)
 {
     return new QGo4BrowserListView(parent);
 }

 QString QGo4BrowserListViewPlugin::name() const
 {
     return "QGo4BrowserListView";
 }

 QString QGo4BrowserListViewPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QGo4BrowserListViewPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QGo4BrowserListViewPlugin::toolTip() const
 {
     return "Go4browser list view";

 }

 QString QGo4BrowserListViewPlugin::whatsThis() const
 {
     return "List view, used in browser to handle start drag event of its items";
 }

 bool QGo4BrowserListViewPlugin::isContainer() const
 {
     return false;
 }

 QString QGo4BrowserListViewPlugin::domXml() const
 {
     return "<widget class=\"QGo4BrowserListView\" name=\"Go4BrowserListView widget\">\n"
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

 QString QGo4BrowserListViewPlugin::includeFile() const
 {
     return "QGo4BrowserListView.h";
 }


