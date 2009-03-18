#include "QGo4CommandsHistory.h"
//Added by qt3to4:
#include <QKeyEvent>

QGo4CommandsHistory::QGo4CommandsHistory(QWidget* w, const char* name) :
    QComboBox(TRUE, w, name)
{
   setMaxCount(50);
   setInsertionPolicy(AtTop);
   setDuplicatesEnabled(FALSE);
   setAutoCompletion(TRUE);
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
