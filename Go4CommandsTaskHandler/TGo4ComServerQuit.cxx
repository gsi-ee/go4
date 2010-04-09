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

#include "TGo4ComServerQuit.h"

#include "TGo4Log.h"
#include "TGo4ServerTask.h"

TGo4ComServerQuit::TGo4ComServerQuit() :
   TGo4TaskHandlerCommand("SVQuit","quits the server and the whole application")
{
   SetReceiverName("ServerTask"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   fbIsSynchron=kFALSE;
   fbIsLocal=kFALSE; // we misuse this old command to send it to analysis server for shutdown
   SetProtection(kGo4ComModeAdministrator);
}

TGo4ComServerQuit::~TGo4ComServerQuit()
{
}

Int_t TGo4ComServerQuit::ExeCom()
{
   TGo4ServerTask* server=dynamic_cast<TGo4ServerTask*>(fxReceiverBase);
   if (server!=0)
      {
         server->Quit(); // Quit will disconnect clients one by one
         //server->Shutdown(); // Shutdown will terminate without waiting for clients removed properly
         return 0;
      }
   else
      {
         TGo4Log::Debug(" !!! ComServerQuit ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
