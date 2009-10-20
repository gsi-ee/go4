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
}

QGo4CommandsHistory::~QGo4CommandsHistory()
{
}

void QGo4CommandsHistory::keyPressEvent(QKeyEvent* e)
{
   if (e->key()==Qt::Key_Return)
      emit enterPressedSingal();

   QComboBox::keyPressEvent(e);
}

QStringList QGo4CommandsHistory::getHistory(int max)
{
   QStringList lst;
   if (max>count()) max = count();

   for(int i=0; i<max; i++)
      lst.append(itemText(i));

    return lst;
}
