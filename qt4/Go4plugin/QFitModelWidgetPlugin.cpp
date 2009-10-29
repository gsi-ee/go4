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

#include "QFitModelWidget.h"
#include "QFitModelWidgetPlugin.h"
#include <QtPlugin>


QFitModelWidgetPlugin::QFitModelWidgetPlugin(QObject *parent)
     : QObject(parent)
 {
     initialized = false;
 }

 void QFitModelWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
 {
     if (initialized)
         return;

     initialized = true;
 }

 bool QFitModelWidgetPlugin::isInitialized() const
 {
     return initialized;
 }

 QWidget *QFitModelWidgetPlugin::createWidget(QWidget *parent)
 {
     return new QFitModelWidget(parent);
 }

 QString QFitModelWidgetPlugin::name() const
 {
     return "QFitModelWidget";
 }

 QString QFitModelWidgetPlugin::group() const
 {
     return "Go4 Custom Widgets";
 }

 QIcon QFitModelWidgetPlugin::icon() const
 {
     return QIcon(":/go4logo2.png");
 }

 QString QFitModelWidgetPlugin::toolTip() const
 {
     return "Widget for model items in Go4 Fit gui";
 }

QString QFitModelWidgetPlugin::whatsThis() const
 {
     return "Widget, connected with model items in expert mode";
 }

bool QFitModelWidgetPlugin::isContainer() const
 {
     return true;
 }

 QString QFitModelWidgetPlugin::domXml() const
 {
     return "<widget class=\"QFitModelWidget\" name=\"QFitModelWidget\">\n"
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

 QString QFitModelWidgetPlugin::includeFile() const
 {
     return "QFitModelWidget.h";
 }


