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

#include "QFitWidget.h"
#include "QFitWidgetPlugin.h"
#include <QtPlugin>


QFitWidgetPlugin::QFitWidgetPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QFitWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QFitWidgetPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QFitWidgetPlugin::createWidget(QWidget *parent)
 {
     return new QFitWidget(parent);
 }

 QString QFitWidgetPlugin::name() const
 {
     return "QFitWidget";
 }

 QString QFitWidgetPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QFitWidgetPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QFitWidgetPlugin::toolTip() const
 {
     return "Basic widget in Go4 Fit gui";
 }

 QString QFitWidgetPlugin::whatsThis() const
 {
     return "Fit Widget, connected with items in expert mode";
 }

 bool QFitWidgetPlugin::isContainer() const
 {
     return true;
 }

 QString QFitWidgetPlugin::domXml() const
 {
     return "<widget class=\"QFitWidget\" name=\"QFitWidget\">\n"
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

 QString QFitWidgetPlugin::includeFile() const
 {
     return "QFitWidget.h";
 }



