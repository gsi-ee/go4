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

#include "TGo4ExampleCommandList.h"

#include "TGo4ComAction1.h"
#include "TGo4ComAction2.h"

TGo4ExampleCommandList::~TGo4ExampleCommandList()
{
}

TGo4ExampleCommandList::TGo4ExampleCommandList()
:TGo4TaskHandlerCommandList("Go4 Example Application Command list")
{
// add all application specific commands here:
  AddCommand (new TGo4ComAction1);
  AddCommand (new TGo4ComAction2);
}
