#include "go4plugin.h"

#include "QGo4WidgetPlugin.h"
#include "QGo4LineEditPlugin.h"
#include "QGo4CommandsHistoryPlugin.h"
#include "QGo4BrowserListViewPlugin.h"
#include "QFitWidgetPlugin.h"
#include "QFitNamedWidgetPlugin.h"
#include "QFitModelWidgetPlugin.h"
#include "TQRootCanvasPlugin.h"
#include "QGo4RootCanvasPlugin.h"
#include "TQRootWindowPlugin.h"


Go4WidgetPlugins::Go4WidgetPlugins(QObject *parent)
         : QObject(parent)
{
   Q_INIT_RESOURCE(go4plugin);
   widgets.append(new QGo4WidgetPlugin(this));
   widgets.append(new QGo4LineEditPlugin(this));
   widgets.append(new QGo4CommandsHistoryPlugin(this));
   widgets.append(new QGo4BrowserListViewPlugin(this));
   widgets.append(new QFitWidgetPlugin(this));
   widgets.append(new QFitNamedWidgetPlugin(this));
   widgets.append(new QFitModelWidgetPlugin(this));
   widgets.append(new TQRootCanvasPlugin(this));
   widgets.append(new QGo4RootCanvasPlugin(this));
   widgets.append(new TQRootWindowPlugin(this));
}

 QList<QDesignerCustomWidgetInterface*> Go4WidgetPlugins::customWidgets() const
 {
     return widgets;
 }

Q_EXPORT_PLUGIN2(go4plugin, Go4WidgetPlugins)



