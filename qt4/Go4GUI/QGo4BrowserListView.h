#ifndef QGOBROWSERLISTVIEW_H
#define QGOBROWSERLISTVIEW_H

#include "q3listview.h"
#include <QtDesigner/QDesignerExportWidget>

#include <QDropEvent>

class Q3DragObject;

class QDESIGNER_WIDGET_EXPORT QGo4BrowserListView : public Q3ListView {
   Q_OBJECT

   public:
      QGo4BrowserListView(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
      virtual ~QGo4BrowserListView();

      bool IsAcceptItemDrop(const Q3ListViewItem* item, const QMimeSource * mime);
      void ProcessItemDrop(Q3ListViewItem* item, QDropEvent * e);

   signals:

      void RequestDragObject(Q3DragObject**);
      void ItemDropAccept(void*, void*, bool*);
      void ItemDropProcess(void*, void*);

   protected:

      virtual Q3DragObject* dragObject();
};

#endif

