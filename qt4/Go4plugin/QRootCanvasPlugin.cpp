// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "QRootCanvas.h"
#include "QRootCanvasPlugin.h"

#include <QtPlugin>
//#include <iostream>


QRootCanvasPlugin::QRootCanvasPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QRootCanvasPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;
     initialized = true;
 }

 bool QRootCanvasPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QRootCanvasPlugin::createWidget(QWidget *parent)
 {
     QRootCanvas* widg=new QRootCanvas(parent);
     QPalette* npal= new QPalette(widg->palette());
     //npal->setColor(QPalette::Window,Qt::red);
     npal->setBrush(QPalette::Window,QPixmap(":/scale.png"));
     widg->setPalette(*npal);
     widg->setAutoFillBackground(true);
     return widg;

 }

 QString QRootCanvasPlugin::name() const
 {
     return "QRootCanvas";
 }

 QString QRootCanvasPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QRootCanvasPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QRootCanvasPlugin::toolTip() const
 {
     return "Standard QtROOT canvas";
 }

QString QRootCanvasPlugin::whatsThis() const
 {
     return "ROOT canvas embedded into QWidget";
 }

bool QRootCanvasPlugin::isContainer() const
 {
     return false;
 }

 QString QRootCanvasPlugin::domXml() const
 {
     return "<widget class=\"QRootCanvas\" name=\"QRootCanvas\">\n"
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

 QString QRootCanvasPlugin::includeFile() const
 {
     return "QRootCanvas.h";
 }


