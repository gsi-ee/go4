#ifndef GO4PLUGIN_H
#define GO4PLUGIN_H


#include <QtDesigner/QtDesigner>
#include <QtCore/qplugin.h>

class Go4WidgetPlugins: public QObject, public QDesignerCustomWidgetCollectionInterface
 {
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

 public:
     Go4WidgetPlugins(QObject *parent = 0);

     virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

 private:
     QList<QDesignerCustomWidgetInterface*> widgets;
 };
 
 
#endif

