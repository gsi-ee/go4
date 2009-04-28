#include "tqrootcanvas.h"
#include "TQRootCanvasPlugin.h"

#include <QtPlugin>
//#include <iostream>


TQRootCanvasPlugin::TQRootCanvasPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void TQRootCanvasPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;
     initialized = true;
 }

 bool TQRootCanvasPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *TQRootCanvasPlugin::createWidget(QWidget *parent)
 {
     TQRootCanvas* widg=new TQRootCanvas(parent,"TQRootCanvas");
     QPalette* npal= new QPalette(widg->palette());
     //npal->setColor(QPalette::Window,Qt::red);
     npal->setBrush(QPalette::Window,QPixmap(":/scale.png"));
     widg->setPalette(*npal);
     widg->setAutoFillBackground(true);
     return widg;

 }

 QString TQRootCanvasPlugin::name() const
 {
     return "TQRootCanvas";
 }

 QString TQRootCanvasPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon TQRootCanvasPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString TQRootCanvasPlugin::toolTip() const
 {
     return "Standard QtROOT canvas";
 }

QString TQRootCanvasPlugin::whatsThis() const
 {
     return "ROOT canvas embedded into QWidget";
 }

bool TQRootCanvasPlugin::isContainer() const
 {
     return false;
 }

 QString TQRootCanvasPlugin::domXml() const
 {
     return "<widget class=\"TQRootCanvas\" name=\"TQRootCanvas\">\n"
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

 QString TQRootCanvasPlugin::includeFile() const
 {
     return "tqrootcanvas.h";
 }


