#include "QGo4RootCanvas.h"
#include "QGo4RootCanvasPlugin.h"
#include <QtPlugin>




QGo4RootCanvasPlugin::QGo4RootCanvasPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QGo4RootCanvasPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QGo4RootCanvasPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QGo4RootCanvasPlugin::createWidget(QWidget *parent)
 {
    QGo4RootCanvas* widg=new QGo4RootCanvas(parent);
    QPalette* npal= new QPalette(widg->palette());
    npal->setBrush(QPalette::Window, QPixmap(":/go4logo2.png"));
    widg->setPalette(*npal);
    widg->setAutoFillBackground(true);
    return widg;
 }

 QString QGo4RootCanvasPlugin::name() const
 {
     return "QGo4RootCanvas";
 }

 QString QGo4RootCanvasPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QGo4RootCanvasPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QGo4RootCanvasPlugin::toolTip() const
 {
     return "Go4 QtROOT canvas";
 }

QString QGo4RootCanvasPlugin::whatsThis() const
 {
     return "Go4 extension of standard QtROOT canvas";
 }

bool QGo4RootCanvasPlugin::isContainer() const
 {
     return false;
 }

 QString QGo4RootCanvasPlugin::domXml() const
 {
     return "<widget class=\"QGo4RootCanvas\" name=\"QGo4RootCanvas\">\n"
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

 QString QGo4RootCanvasPlugin::includeFile() const
 {
     return "QGo4RootCanvas.h";
 }


