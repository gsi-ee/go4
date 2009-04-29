#include "QRootWindow.h"
#include "QRootWindowPlugin.h"

#include <QtPlugin>


QRootWindowPlugin::QRootWindowPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QRootWindowPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;
     initialized = true;
 }

 bool QRootWindowPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QRootWindowPlugin::createWidget(QWidget *parent)
 {
   QRootWindow* widg=new QRootWindow(parent);
   QPalette* npal= new QPalette(widg->palette());
   npal->setBrush(QPalette::Window,QPixmap(":/root.png"));
   widg->setPalette(*npal);
   widg->setAutoFillBackground(true);
   return widg;
 }

 QString QRootWindowPlugin::name() const
 {
     return "QRootWindow";
 }

 QString QRootWindowPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QRootWindowPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QRootWindowPlugin::toolTip() const
 {
     return "Qwidget to embed a ROOT TGCompositeFrame";
 }

QString QRootWindowPlugin::whatsThis() const
 {
     return "ROOT TGFrame embedded into a QWidget ";
 }

bool QRootWindowPlugin::isContainer() const
 {
     return false;
 }

 QString QRootWindowPlugin::domXml() const
 {
     return "<widget class=\"QRootWindow\" name=\"QRootWindow\">\n"
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

 QString QRootWindowPlugin::includeFile() const
 {
     return "QRootWindow.h";
 }


