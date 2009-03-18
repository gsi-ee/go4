#include "QGo4LineEdit.h"

#include "q3dragobject.h"
//Added by qt3to4:
#include <QDropEvent>
#include <QDragEnterEvent>

#include <iostream>
using namespace std;
 

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
   if (!Q3TextDrag::decode(event,str)) event->ignore();
   event->accept(); // necessary for Qt4
}

void QGo4LineEdit::dropEvent(QDropEvent* event)
{
   QString str;
   if (!Q3TextDrag::decode(event, str)) return;
   setText(str);
   emit textDropped();

//   clear();
//   insert(str);
//   e->acceptAction();
}
