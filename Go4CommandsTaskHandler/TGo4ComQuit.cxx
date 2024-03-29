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

#include "TGo4ComQuit.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComQuit::TGo4ComQuit() :
   TGo4TaskHandlerCommand("THQuit","tells client taskhandler application to quit")
{
   fbIsSynchron=kFALSE;
}

Int_t TGo4ComQuit::ExeCom()
{
   TGo4Task* cli = dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli) {
      TGo4Log::Info("Quit command executes on receiver: %s", ((TGo4Task*) fxReceiverBase)->GetName());
//      cli->SendStatusMessage(2,kFALSE, "Quit command executes on client %s ...",cli->GetName());
      cli->Quit();
      return 0;
   }

   TGo4Log::Debug(" !!! ComQuit ''%s'': NO RECEIVER ERROR!!!",GetName());
   return 1;
}
