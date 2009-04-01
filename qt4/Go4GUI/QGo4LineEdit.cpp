#include "QGo4LineEdit.h"

#include <QDropEvent>
#include <QDragEnterEvent>

QGo4LineEdit::QGo4LineEdit(QWidget* parent, const char* name) :
   QLineEdit(parent, name)
{
   setAcceptDrops( TRUE );
}

QGo4LineEdit::~QGo4LineEdit()
{
}

void QGo4LineEdit::dragEnterEvent(QDragEnterEvent* event)
{
   if (event->mimeData()->hasText())
      event->acceptProposedAction();
}

void QGo4LineEdit::dropEvent(QDropEvent* event)
{
   setText(event->mimeData()->text());
   emit textDropped();
}
