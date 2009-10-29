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

#include "go4plugin.h"

#include "QGo4WidgetPlugin.h"
#include "QGo4LineEditPlugin.h"
#include "QGo4CommandsHistoryPlugin.h"
#include "QGo4BrowserTreeWidgetPlugin.h"
#include "QFitWidgetPlugin.h"
#include "QFitNamedWidgetPlugin.h"
#include "QFitModelWidgetPlugin.h"
#include "QRootCanvasPlugin.h"
#include "QRootWindowPlugin.h"


Go4WidgetPlugins::Go4WidgetPlugins(QObject *parent)
         : QObject(parent)
{
   Q_INIT_RESOURCE(go4plugin);
   widgets.append(new QGo4WidgetPlugin(this));
   widgets.append(new QGo4LineEditPlugin(this));
   widgets.append(new QGo4CommandsHistoryPlugin(this));
   widgets.append(new QGo4BrowserTreeWidgetPlugin(this));
   widgets.append(new QFitWidgetPlugin(this));
   widgets.append(new QFitNamedWidgetPlugin(this));
   widgets.append(new QFitModelWidgetPlugin(this));
   widgets.append(new QRootCanvasPlugin(this));
   widgets.append(new QRootWindowPlugin(this));
}

 QList<QDesignerCustomWidgetInterface*> Go4WidgetPlugins::customWidgets() const
 {
     return widgets;
 }

Q_EXPORT_PLUGIN2(go4plugin, Go4WidgetPlugins)



