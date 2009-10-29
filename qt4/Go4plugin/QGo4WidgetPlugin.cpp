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

#include "QGo4Widget.h"
#include "QGo4WidgetPlugin.h"

#include <QtPlugin>



QGo4WidgetPlugin::QGo4WidgetPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QGo4WidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QGo4WidgetPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QGo4WidgetPlugin::createWidget(QWidget *parent)
 {
     return new QGo4Widget(parent);
 }

 QString QGo4WidgetPlugin::name() const
 {
     return "QGo4Widget";
 }

 QString QGo4WidgetPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QGo4WidgetPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QGo4WidgetPlugin::toolTip() const
 {
     return "Go4 Editor Widget";
 }

QString QGo4WidgetPlugin::whatsThis() const
 {
     return "Basic widget to communicate with Go4 GUI enviroment";
 }

bool QGo4WidgetPlugin::isContainer() const
 {
     return true;
 }

 QString QGo4WidgetPlugin::domXml() const
 {
     return "<widget class=\"QGo4Widget\" name=\"Go4Widget\">\n"
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

 QString QGo4WidgetPlugin::includeFile() const
 {
     return "QGo4Widget.h";
 }



