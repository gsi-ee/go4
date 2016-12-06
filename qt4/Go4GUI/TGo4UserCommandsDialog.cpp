// $Id: TGo4UserCommandsDialog.cpp 842 2012-05-30 14:23:51Z adamczew $
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

#include "TGo4UserCommandsDialog.h"

#include <QToolTip>

TGo4UserCommandsDialog::TGo4UserCommandsDialog( QWidget* parent )
    : QDialog( parent )
{
   setObjectName("Go4UserCommandsDialog");
   setupUi(this);
}




