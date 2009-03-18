#ifndef QGO4BROWSERITEM_H
#define QGO4BROWSERITEM_H

#include "q3listview.h"
//Added by qt3to4:
#include <QDropEvent>

class QGo4BrowserItem : public Q3ListViewItem {
   public:
      QGo4BrowserItem(Q3ListView* parent, Q3ListViewItem* prev, const char* name);
      QGo4BrowserItem(Q3ListViewItem* parent, Q3ListViewItem* prev, const char* name);

      virtual bool acceptDrop ( const QMimeSource * mime ) const;
      virtual void dropped ( QDropEvent * e );
};

#endif
