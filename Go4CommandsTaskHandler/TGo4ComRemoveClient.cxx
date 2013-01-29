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

#include "TGo4ComRemoveClient.h"

#include "TGo4Log.h"
#include "TGo4ServerTask.h"

// later second ctor with client name as parameter + advanced command list make!

TGo4ComRemoveClient::TGo4ComRemoveClient() :
   TGo4TaskHandlerCommand("SVRemoveClient","remove the specified client")
{
   SetReceiverName("ServerTask"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   fbIsLocal = kTRUE;
   fxClientName = "currentclient";
   fbWaitForClient = kTRUE;
}

TGo4ComRemoveClient::~TGo4ComRemoveClient()
{
}

void TGo4ComRemoveClient::SetClient(const char* name)
{
   fxClientName = name;
}

void TGo4ComRemoveClient::SetWaitForClient(Bool_t wait)
{
   fbWaitForClient=wait;
}

Int_t TGo4ComRemoveClient::ExeCom()
{
   TGo4ServerTask* serv=dynamic_cast<TGo4ServerTask*>(fxReceiverBase);
   if(serv)
   {
      TGo4Log::Debug(" removing client %s",fxClientName.Data());
      Bool_t ret = serv->RemoveClient(fxClientName.Data(), fbWaitForClient);
      if(!ret)
      {
         TGo4Log::Debug(" ComRemoveClient: Sorry, the specified client is not connected ");
         return 1;
      }
      else
      {
         TGo4Log::Debug(" ComRemoveClient: Client %s is removed ",fxClientName.Data());
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
