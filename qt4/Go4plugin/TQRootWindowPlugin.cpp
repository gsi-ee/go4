#include "tqrootwindow.h"
#include "TQRootWindowPlugin.h"

#include <QtPlugin>


TQRootWindowPlugin::TQRootWindowPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void TQRootWindowPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;
     initialized = true;
 }

 bool TQRootWindowPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *TQRootWindowPlugin::createWidget(QWidget *parent)
 {
   TQRootWindow* widg=new TQRootWindow(parent,"TQRootWindow",true);
   QPalette* npal= new QPalette(widg->palette());
   npal->setBrush(QPalette::Window,QPixmap(":/root.png"));
   widg->setPalette(*npal);
   widg->setAutoFillBackground(true);
   return widg;
 }

 QString TQRootWindowPlugin::name() const
 {
     return "TQRootWindow";
 }

 QString TQRootWindowPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon TQRootWindowPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString TQRootWindowPlugin::toolTip() const
 {
     return "Qwidget to embed a ROOT TGCompositeFrame";
 }

QString TQRootWindowPlugin::whatsThis() const
 {
     return "ROOT TGFrame embedded into a QWidget ";
 }

bool TQRootWindowPlugin::isContainer() const
 {
     return false;
 }

 QString TQRootWindowPlugin::domXml() const
 {
     return "<widget class=\"TQRootWindow\" name=\"TQRootWindow\">\n"
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

 QString TQRootWindowPlugin::includeFile() const
 {
     return "tqrootwindow.h";
 }


