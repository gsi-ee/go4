#ifndef QGOBROWSERLISTVIEW_H
#define QGOBROWSERLISTVIEW_H

#include <QTreeWidget>
#include <QtDesigner/QDesignerExportWidget>

#include <QDropEvent>

class QDESIGNER_WIDGET_EXPORT QGo4BrowserListView : public QTreeWidget {
   Q_OBJECT

   public:
      QGo4BrowserListView(QWidget* parent = 0);
      virtual ~QGo4BrowserListView();

      virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
      virtual QStringList mimeTypes() const;
      virtual Qt::DropActions supportedDropActions () const;

      virtual void mouseMoveEvent(QMouseEvent *event);

   signals:

      void RequestDragObject(QDrag**);
      void ItemDropProcess(void*, void*);

};

#endif

