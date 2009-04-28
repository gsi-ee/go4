#include "QGo4CommandsHistory.h"
#include "QGo4CommandsHistoryPlugin.h"
#include <QtPlugin>


QGo4CommandsHistoryPlugin::QGo4CommandsHistoryPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QGo4CommandsHistoryPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QGo4CommandsHistoryPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QGo4CommandsHistoryPlugin::createWidget(QWidget *parent)
 {
     return new QGo4CommandsHistory(parent);
 }

 QString QGo4CommandsHistoryPlugin::name() const
 {
     return "QGo4CommandsHistory";
 }

 QString QGo4CommandsHistoryPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QGo4CommandsHistoryPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QGo4CommandsHistoryPlugin::toolTip() const
 {
     return "Command line combo box with history";
 }

QString QGo4CommandsHistoryPlugin::whatsThis() const
 {
     return "ComboBox with command line functionality: enter key will execute command, history of commands is available";;
 }

bool QGo4CommandsHistoryPlugin::isContainer() const
 {
     return false;
 }

 QString QGo4CommandsHistoryPlugin::domXml() const
 {
     return "<widget class=\"QGo4CommandsHistory\" name=\"QGo4CommandsHistory\">\n"
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

 QString QGo4CommandsHistoryPlugin::includeFile() const
 {
     return "QGo4CommandsHistory.h";
 }


