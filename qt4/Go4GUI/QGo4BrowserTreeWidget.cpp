#include "QGo4BrowserTreeWidget.h"

#include <QDropEvent>

QGo4BrowserTreeWidget::QGo4BrowserTreeWidget(QWidget* parent) :
  QTreeWidget(parent)
{
}

QGo4BrowserTreeWidget::~QGo4BrowserTreeWidget()
{
}

QStringList QGo4BrowserTreeWidget::mimeTypes () const
{
   QStringList qstrList;
   qstrList.append("text");
   return qstrList;
}


Qt::DropActions QGo4BrowserTreeWidget::supportedDropActions () const
{
    // returns what actions are supported when dropping
    return Qt::CopyAction | Qt::MoveAction;
}


bool QGo4BrowserTreeWidget::dropMimeData(QTreeWidgetItem *item, int, const QMimeData *data, Qt::DropAction)
{
	emit ItemDropProcess((void*)item, (void*)data);

   return true;
}

void QGo4BrowserTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
	// if not left button - return
	if (!(event->buttons() & Qt::LeftButton)) return;

	QDrag *drag = 0;

	// request from browser drag

	emit RequestDragObject(&drag);

	if (drag)
   	drag->start(Qt::CopyAction | Qt::MoveAction);
}
