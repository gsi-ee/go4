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

#include "QGo4Table.h"
#include <iostream>

QGo4Table::QGo4Table(QWidget* parent, const char* name) :
   QTable(parent, name)
{
}

QGo4Table::~QGo4Table()
{
}


void QGo4Table::sortColumn ( int col, bool ascending, bool wholeRows)
{
   //std::cout<<"QGo4Table::sortColumn("<<col<<","<<ascending <<", "<<wholeRows<<") "<<std::endl;
   QTable::sortColumn(col,ascending, true);

}


