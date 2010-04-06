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

#include "TGo4Master.h"

#include "TSystem.h"

#include "TGo4Log.h"
#include "TGo4Task.h"
#include "TGo4ServerTask.h"
#include "TGo4ClientTask.h"
#include "TGo4CommandInvoker.h"
#include "TGo4TaskHandlerCommandList.h"

TGo4Master::TGo4Master() :
   TGo4TaskOwner()
{
}

TGo4Master::TGo4Master(const char* name, Bool_t isserver, const char* serverhost, UInt_t negotport)
: TGo4TaskOwner(isserver)
{
   TGo4CommandInvoker::Instance(); // make sure a command invoker exists
   TGo4CommandInvoker::Register("MasterTask", this);
   TGo4Task* task(0);
   if(IsServer())
      task = new TGo4ServerTask(name, negotport,
                                kFALSE, //blockingmode
                                kFALSE, //standalone
                                kTRUE,  //autostart
                                kTRUE,  //autocreate
                                kTRUE);   //ismaster
   else
      task = new TGo4ClientTask(name, serverhost, negotport,
                                kFALSE, //blockingmode
                                kFALSE, //standalone
                                kFALSE, //autostart
                                kTRUE,  //autocreate
                                kTRUE, //ismaster
                                kFALSE); // autoconnect
   SetTask(task);
}

TGo4Master::~TGo4Master()
{
   TGo4CommandInvoker::UnRegister(this);
}

TGo4TaskHandlerCommandList* TGo4Master::CreateCommandList()
{
   return new TGo4TaskHandlerCommandList("Go4ServerTaskDefaultCommandList");
}

Bool_t TGo4Master::SubmitCommand(const char* name)
{
   if (GetTask()==0) return kFALSE;
   return GetTask()->SubmitCommand(name);
}

Bool_t TGo4Master::SubmitEmergencyCommand(Go4EmergencyCommand_t val)
{
   if (GetTask()==0) return kFALSE;
   return GetTask()->SubmitEmergencyCommand(val);
}

Bool_t TGo4Master::SubmitCommand(TGo4Command* com)
{
   if (GetTask()==0) return kFALSE;
   return GetTask()->SubmitCommand(com);
}

Bool_t TGo4Master::DisconnectSlave(const char* name, Bool_t waitforslave)
{
   Bool_t rev=kTRUE;
   if(IsServer()) {
      TGo4ServerTask* server = dynamic_cast<TGo4ServerTask*> (GetTask());
      if(server)
         rev=server->RemoveClient(name, waitforslave); //waitforclient
      else
         rev=kFALSE;
   } else {
      TGo4ClientTask* client = dynamic_cast<TGo4ClientTask*> (GetTask());
      if(client) {
         rev=client->DisconnectServer();
         client->Terminate(kFALSE); // terminate taskowner (TGo4Display), but not application
      } else
         rev=kFALSE;
   }
   return rev;
}
