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

#include "TGo4ComStop.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComStop::TGo4ComStop() :
   TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THStop");
   SetDescription("stops example client's main loop");
}

TGo4ComStop::~TGo4ComStop()
{
}

Int_t TGo4ComStop::ExeCom()
{
   TGo4Task* cli=dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli)
      {
         cli->SendStatusMessage(1,kFALSE,TString::Format("Client %s working function is stopped...",cli->GetName()));
         cli->Stop();
      }
   else
      {
         TGo4Log::Debug(" !!! ComStop ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;

}
