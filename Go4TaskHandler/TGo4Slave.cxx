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

#include "TGo4Slave.h"

#include "TROOT.h"
#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4TaskStatus.h"
#include "TGo4ServerTask.h"
#include "TGo4ClientTask.h"
#include "TGo4CommandInvoker.h"
#include "TGo4TaskHandlerCommandList.h"

TGo4Slave::TGo4Slave(const char* name, Bool_t isserver, const char* serverhost, UInt_t negotport)
:TGo4TaskOwner(isserver),
 fbMainIsRunning(kTRUE)
{
   TGo4CommandInvoker::Instance(); // make sure a command invoker exists
   TGo4CommandInvoker::SetCommandList(new TGo4TaskHandlerCommandList("ListOfTaskHandlerCommands"));
   TGo4CommandInvoker::Register("SlaveTask", this);
   TGo4Task* task=0;
   if(IsServer())
      {
         task=new TGo4ServerTask(name, negotport,
               kFALSE, //blockingmode
               kFALSE, //standalone
               kTRUE,  //autostart
               kTRUE,  //autocreate
               kFALSE   //ismaster
               );
      }
   else
      {
         task=new TGo4ClientTask(name, serverhost, negotport,
                     kFALSE, //blockingmode
                     kFALSE, //standalone
                     kFALSE, //autostart
                     kTRUE,  //autocreate
                     kFALSE, //ismaster
                     kTRUE );//autoconnect
      }
   SetTask(task);
}

TGo4Slave::TGo4Slave():fbMainIsRunning(kTRUE)
{

}

TGo4Slave::~TGo4Slave()
{
    TGo4CommandInvoker::UnRegister(this);
}


void TGo4Slave::Start()
{
   TGo4Log::Debug(" Slave ''%s'' default Start method, please override",GetName());
   fbMainIsRunning=kTRUE;
}
void TGo4Slave::Stop()
{
   TGo4Log::Debug(" Slave ''%s'' default Stop method, please override",GetName());
   fbMainIsRunning=kFALSE;
}

void TGo4Slave::KillMain()
{
// may be overridden by concrete slave implementation

}
void TGo4Slave::RestartMain()
{
// may be overridden by concrete slave implementation
}

void TGo4Slave::Terminate (Bool_t termapp)
{
   if(GetTask())
      GetTask()->TGo4ThreadManager::Terminate(termapp);
}

void TGo4Slave::TerminateFast ()
{
   Terminate();
}


TGo4TaskStatus* TGo4Slave::CreateStatus()
{
   TGo4TaskStatus* stat= new TGo4TaskStatus(GetName());
   UpdateStatus(stat); // set the internals
   return stat;
}

void TGo4Slave::UpdateStatus(TGo4TaskStatus* state)
{
   if(GetTask())
      GetTask()->UpdateStatus(state);
}

TGo4Command* TGo4Slave::NextCommand()
{
   if(GetTask())
      return (GetTask()->NextCommand());
   else
      return 0;
}



void TGo4Slave::SendObject(TObject * obj, const char* receiver)
{
   if(GetTask())
      GetTask()->SendObject(obj, receiver);
}

void TGo4Slave::SendStatus(TGo4Status * stat, const char* receiver)
{
   if(GetTask())
      GetTask()->SendStatus(stat, receiver);
}

Int_t TGo4Slave::Initialization()
{
// may be overridden by concrete slave implementation
    return 0;
}

TGo4ThreadHandler* TGo4Slave::GetThreadHandler()
{
   if(GetTask())
      return(GetTask()->GetWorkHandler());
   else
      return 0;
}


void TGo4Slave::SendStatusBuffer()
{
   if(GetTask()) GetTask()->SendStatusBuffer();
}


void TGo4Slave::SendStatusMessage(Int_t level, Bool_t printout, const TString& text)
{
   if(GetTask())
      GetTask()->SendStatusMessage(level, printout, text.Data());
}


void TGo4Slave::UpdateStatusBuffer()
{
   if(GetTask()) GetTask()->UpdateStatusBuffer();
}

void TGo4Slave::ExecuteString(const char* command)
{
   gROOT->ProcessLineSync(command);
}

