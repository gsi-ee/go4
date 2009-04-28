#include "QFitNamedWidget.h"
#include "QFitNamedWidgetPlugin.h"
#include <QtPlugin>


QFitNamedWidgetPlugin::QFitNamedWidgetPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QFitNamedWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QFitNamedWidgetPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QFitNamedWidgetPlugin::createWidget(QWidget *parent)
 {
     return new QFitNamedWidget(parent);
 }

 QString QFitNamedWidgetPlugin::name() const
 {
     return "QFitNamedWidget";
 }

 QString QFitNamedWidgetPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QFitNamedWidgetPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QFitNamedWidgetPlugin::toolTip() const
 {
     return "Widget for named items in Go4 Fit gui";
 }

QString QFitNamedWidgetPlugin::whatsThis() const
 {
     return "Widget, connected with named items in expert mode";
 }

bool QFitNamedWidgetPlugin::isContainer() const
 {
     return true;
 }

 QString QFitNamedWidgetPlugin::domXml() const
 {
     return "<widget class=\"QFitNamedWidget\" name=\"QFitNamedWidget\">\n"
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

 QString QFitNamedWidgetPlugin::includeFile() const
 {
     return "QFitNamedWidget.h";
 }


