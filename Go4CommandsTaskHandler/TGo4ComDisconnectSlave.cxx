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

#include "TGo4ComDisconnectSlave.h"

#include "TGo4Log.h"
#include "TGo4Master.h"

TGo4ComDisconnectSlave::TGo4ComDisconnectSlave()
:TGo4TaskHandlerCommand("MRDisconnectSlave","disconnect the specified slave")
{
   SetReceiverName("MasterTask"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   fbIsLocal = kTRUE;
   fxSlaveName = "currentslave";
   fbWaitForSlave = kTRUE;
}
TGo4ComDisconnectSlave::~TGo4ComDisconnectSlave()
{

}


Int_t TGo4ComDisconnectSlave::ExeCom()
{
TGo4Master* master=dynamic_cast<TGo4Master*>(fxReceiverBase);
if(master)
   {
      TGo4Log::Debug(" removing slave %s",fxSlaveName.Data());
      if(!master->DisconnectSlave(fxSlaveName.Data(), fbWaitForSlave))
         {
            TGo4Log::Debug(" ComDisconnectSlave: Sorry, the specified slave is not connected ");
            return 1;
         }
      else
         {
            TGo4Log::Debug(" ComDisconnectSlave: Slave %s is removed ",fxSlaveName.Data());
            return 0;
         }
      }
   else
      {
         TGo4Log::Debug(" !!! ComRemoveClient ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
