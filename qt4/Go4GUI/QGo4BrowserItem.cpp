#include "QGo4BrowserItem.h"

#include "QGo4BrowserListView.h"
//Added by qt3to4:
#include <QDropEvent>

QGo4BrowserItem::QGo4BrowserItem(Q3ListView* parent, Q3ListViewItem* prev, const char* name) :
   Q3ListViewItem(parent, prev, name)
{
}

QGo4BrowserItem::QGo4BrowserItem(Q3ListViewItem* parent, Q3ListViewItem* prev, const char* name) :
   Q3ListViewItem(parent, prev, name)
{
}

bool QGo4BrowserItem::acceptDrop ( const QMimeSource * mime ) const
{
   if (!dropEnabled()) return false;

   QGo4BrowserListView* br = dynamic_cast<QGo4BrowserListView*> (listView());

   return (br==0) ? false : br->IsAcceptItemDrop(this, mime);
}

void QGo4BrowserItem::dropped ( QDropEvent * e )
{
   QGo4BrowserListView* br = dynamic_cast<QGo4BrowserListView*> (listView());
   if (br!=0) br->ProcessItemDrop(this, e);
}

