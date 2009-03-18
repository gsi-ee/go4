#include "QGo4LineEdit.h"

#include "qdragobject.h"

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
   QString str;
   if (!QTextDrag::decode(event,str)) event->ignore();
}

void QGo4LineEdit::dropEvent(QDropEvent* event)
{
   QString str;
   if (!QTextDrag::decode(event, str)) return;
   setText(str);
   emit textDropped();

//   clear();
//   insert(str);
//   e->acceptAction();
}
