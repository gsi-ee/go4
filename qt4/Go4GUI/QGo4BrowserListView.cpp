#include "QGo4BrowserListView.h"

#include <QDropEvent>

QGo4BrowserListView::QGo4BrowserListView(QWidget* parent) :
  QTreeWidget(parent)
{
}

QGo4BrowserListView::~QGo4BrowserListView()
{
}

QStringList QGo4BrowserListView::mimeTypes () const
{
   QStringList qstrList;
   qstrList.append("text");
   return qstrList;
}


Qt::DropActions QGo4BrowserListView::supportedDropActions () const
{
    // returns what actions are supported when dropping
    return Qt::CopyAction | Qt::MoveAction;
}


bool QGo4BrowserListView::dropMimeData(QTreeWidgetItem *item, int index, const QMimeData *data, Qt::DropAction action)
{
	emit ItemDropProcess((void*)item, (void*)data);

   return true;
}

void QGo4BrowserListView::mouseMoveEvent(QMouseEvent *event)
{
	// if not left button - return
	if (!(event->buttons() & Qt::LeftButton)) return;

	QDrag *drag = 0;

	// request from browser drag

	emit RequestDragObject(&drag);

	if (drag)
   	drag->start(Qt::CopyAction | Qt::MoveAction);
}
