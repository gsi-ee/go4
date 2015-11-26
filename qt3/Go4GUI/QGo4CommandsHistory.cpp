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
   if ( (e->key()==Qt::Key_Return) || (e->key()==Qt::Key_Enter) )
     fiEnterPressed = (fiEnterPressed+1) % 2;
   QComboBox::keyPressEvent(e);
}
