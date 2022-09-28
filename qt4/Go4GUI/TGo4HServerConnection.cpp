// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4HServerConnection.h"

TGo4HServerConnection::TGo4HServerConnection( QWidget* parent, const char *name, bool modal, Qt::WindowFlags fl)
   : QDialog( parent, fl )
{
   setObjectName(name ? name : "Go4HServConnection");
   setModal(modal);
   setupUi(this);
}
