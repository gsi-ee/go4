#include "QGo4BrowserListView.h"

QGo4BrowserListView::QGo4BrowserListView(QWidget* parent, const char* name, WFlags f) :
  QListView(parent, name, f)
{
}

QGo4BrowserListView::~QGo4BrowserListView()
{
}

QDragObject* QGo4BrowserListView::dragObject()
{
   QDragObject* res = 0;
   emit RequestDragObject(&res);
   return res;
}

bool QGo4BrowserListView::IsAcceptItemDrop(const QListViewItem* item, const QMimeSource * mime)
{
   bool res = false;
   emit ItemDropAccept((void*)item, (void*)mime, &res);
   return res;
}

void QGo4BrowserListView::ProcessItemDrop(QListViewItem* item, QDropEvent * e)
{
   emit ItemDropProcess((void*)item, (void*)e);
}
