#ifndef QGOBROWSERLISTVIEW_H
#define QGOBROWSERLISTVIEW_H

#include "qlistview.h"
#include "qwidgetplugin.h"

class QDragObject;

class QT_WIDGET_PLUGIN_EXPORT QGo4BrowserListView : public QListView {
   Q_OBJECT

   public:
      QGo4BrowserListView(QWidget* parent = 0, const char* name = 0, WFlags f = 0);
      virtual ~QGo4BrowserListView();

      bool IsAcceptItemDrop(const QListViewItem* item, const QMimeSource * mime);
      void ProcessItemDrop(QListViewItem* item, QDropEvent * e);

   signals:

      void RequestDragObject(QDragObject**);
      void ItemDropAccept(void*, void*, bool*);
      void ItemDropProcess(void*, void*);

   protected:

      virtual QDragObject* dragObject();
};

#endif

