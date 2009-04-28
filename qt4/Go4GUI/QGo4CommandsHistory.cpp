#include "QGo4CommandsHistory.h"
#include <QKeyEvent>

QGo4CommandsHistory::QGo4CommandsHistory(QWidget* w, const char* name) :
    QComboBox(w)
{
   setObjectName(name ? name : "Go4CommandsHistory");
   setMaxCount(50);
   setInsertPolicy(InsertAtTop);
   setDuplicatesEnabled(FALSE);
   setAutoCompletion(TRUE);
   setEditable(true);
   fiEnterPressed = 0;
}

QGo4CommandsHistory::~QGo4CommandsHistory()
{

}

void QGo4CommandsHistory::keyPressEvent(QKeyEvent* e)
{
   if (e->key()==Qt::Key_Return)
     fiEnterPressed = (fiEnterPressed+1) % 2;
   QComboBox::keyPressEvent(e);
}
