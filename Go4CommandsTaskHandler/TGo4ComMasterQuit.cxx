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

#include "TGo4ComMasterQuit.h"

#include "TGo4Log.h"
#include "TGo4Master.h"

TGo4ComMasterQuit::TGo4ComMasterQuit() :
   TGo4TaskHandlerCommand("MRQuit","quits the master and the whole application")
{
   SetReceiverName("MasterTask"); // sets the receiver name checked by command invoker
   // receiver name need to be class name!
   fbIsSynchron = kFALSE;
   fbIsLocal = kTRUE;
}

Int_t TGo4ComMasterQuit::ExeCom()
{
   auto master = dynamic_cast<TGo4Master *>(fxReceiverBase);

   if (master) {
      master->Quit();
      return 0;
   }

   TGo4Log::Debug(" !!! ComMasterQuit ''%s'': NO RECEIVER ERROR!!!",GetName());
   return 1;
}
