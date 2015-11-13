// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "QGo4CommandsHistory.h"

#include <QKeyEvent>

#define GO4_COMHIS_MAX 50


QGo4CommandsHistory::QGo4CommandsHistory(QWidget* w, const char* name) :
    QComboBox(w)
{
   setObjectName(name ? name : "Go4CommandsHistory");
   setMaxCount(GO4_COMHIS_MAX);
   setInsertPolicy(InsertAtTop);
   setDuplicatesEnabled(false);
   setAutoCompletion(true);
   setEditable(true);
}

QGo4CommandsHistory::~QGo4CommandsHistory()
{
}

void QGo4CommandsHistory::keyPressEvent(QKeyEvent* e)
{
   if (e->key()==Qt::Key_Return){
     if(count()>=GO4_COMHIS_MAX)  setInsertPolicy(InsertAtCurrent); // overwrite last command
     emit enterPressedSingal();
   }

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
