#include "QGo4BrowserListView.h"
//Added by qt3to4:
#include <QDropEvent>

QGo4BrowserListView::QGo4BrowserListView(QWidget* parent, const char* name, Qt::WFlags f) :
  Q3ListView(parent, name, f)
{
}

QGo4BrowserListView::~QGo4BrowserListView()
{
}

Q3DragObject* QGo4BrowserListView::dragObject()
{
   Q3DragObject* res = 0;
   emit RequestDragObject(&res);
   return res;
}

bool QGo4BrowserListView::IsAcceptItemDrop(const Q3ListViewItem* item, const QMimeSource * mime)
{
   bool res = false;
   emit ItemDropAccept((void*)item, (void*)mime, &res);
   return res;
}

void QGo4BrowserListView::ProcessItemDrop(Q3ListViewItem* item, QDropEvent * e)
{
   emit ItemDropProcess((void*)item, (void*)e);
}
