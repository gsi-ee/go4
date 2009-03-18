#ifndef QGO4BROWSERITEM_H
#define QGO4BROWSERITEM_H

#include "qlistview.h"

class QGo4BrowserItem : public QListViewItem {
   public:
      QGo4BrowserItem(QListView* parent, QListViewItem* prev, const char* name);
      QGo4BrowserItem(QListViewItem* parent, QListViewItem* prev, const char* name);

      virtual bool acceptDrop ( const QMimeSource * mime ) const;
      virtual void dropped ( QDropEvent * e );
};

#endif
