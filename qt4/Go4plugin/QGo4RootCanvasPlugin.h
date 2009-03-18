#ifndef QGO4ROOTCANVASPLUGIN_H
#define QGO4ROOTCANVASPLUGIN_H

#include <QDesignerCustomWidgetInterface>

class QGo4RootCanvasPlugin : public QObject, public QDesignerCustomWidgetInterface
 {
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetInterface)

 public:
     QGo4RootCanvasPlugin(QObject *parent = 0);

     bool isContainer() const;
     bool isInitialized() const;
     QIcon icon() const;
     QString domXml() const;
     QString group() const;
     QString includeFile() const;
     QString name() const;
     QString toolTip() const;
     QString whatsThis() const;
     QWidget *createWidget(QWidget *parent);
     void initialize(QDesignerFormEditorInterface *core);

 private:
     bool initialized;
 };

#endif
