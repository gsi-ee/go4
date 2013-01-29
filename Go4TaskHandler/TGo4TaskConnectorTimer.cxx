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

#include "TGo4TaskConnectorTimer.h"

#include "TGo4Log.h"
#include "TGo4ServerTask.h"

TGo4TaskConnectorTimer::TGo4TaskConnectorTimer (TGo4ServerTask* server, Long_t msperiod) :
   TTimer(msperiod,kTRUE),
   fxServer(server)
{
}

TGo4TaskConnectorTimer::~TGo4TaskConnectorTimer()
{
}

Bool_t TGo4TaskConnectorTimer::Notify ()
{
  // cout << "TGo4TaskConnectorTimer::Notify ()" << endl;
//   TurnOff();
   Int_t action=0;
   action=fxServer->TimerConnect();

//   TGo4Log::Debug(" Task Connector Timer --  starting new notify loop after action %d! ",
//      action);
   Reset();
//   TurnOn();

   return kFALSE;
}
