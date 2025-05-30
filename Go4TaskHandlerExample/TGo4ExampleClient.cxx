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

#include "TGo4ExampleClient.h"

#include <iostream>

#include "TGo4Log.h"
#include "TGo4TaskHandler.h"
#include "TGo4ThreadHandler.h"

#include "TGo4ExampleClientStatus.h"
#include "TGo4MainRunnable.h"
#include "TGo4WatchRunnable.h"

const char *TGo4ExampleClient::fgcWATCHTHREAD="WATCH-";
const char *TGo4ExampleClient::fgcMAINTHREAD="MAIN-";


TGo4ExampleClient::TGo4ExampleClient(const char *name,
                                     const char *host,
                                     UInt_t negport,
                                     Bool_t blockingmode)
: TGo4ClientTask(name, host, negport, blockingmode, kFALSE)
{
   GO4TRACE((15,"TGo4ExampleClient::TGo4ExampleClient(const char *, const char *, Uint_t, Bool_t)",__LINE__, __FILE__));
   SetMaster(kFALSE);
   TGo4Log::Debug(" ExampleClient ''%s'' started ",GetName());
   fxApplication= new TGo4ExampleApplication( (TGo4BufferQueue*) GetTaskHandler()->GetDataQueue());
   TGo4MainRunnable* mainrun = new TGo4MainRunnable(TString::Format("MainRunnable of %s",GetName()).Data(), this);
   TGo4WatchRunnable* watchrun = new TGo4WatchRunnable(TString::Format("WatchRunnable of %s",GetName()).Data(), this);
      // adding runnables to thread handler who takes over the responsibility...:
   fcMainName.Form("%s%s", fgcMAINTHREAD, GetName());
   fxWorkHandler->NewThread(fcMainName.Data(), mainrun);
   fcWatchName.Form("%s%s", fgcWATCHTHREAD, GetName());
   fxWorkHandler->NewThread(fcWatchName.Data(), watchrun);
   Launch();
}

TGo4ExampleClient::~TGo4ExampleClient()
{
   GO4TRACE((15,"TGo4ExampleClient::~TGo4ExampleClient()",__LINE__, __FILE__));
   fxWorkHandler->CancelAll(); // make sure threads wont work on application when its deleted
   delete fxApplication;
}

TGo4TaskStatus * TGo4ExampleClient::CreateStatus()
{
   GO4TRACE((12,"TGo4ExampleClient::CreateStatus()",__LINE__, __FILE__));
   TGo4ExampleClientStatus* stat= new TGo4ExampleClientStatus(GetName());
   UpdateStatus(stat); // set the internals
   return stat;

}
void TGo4ExampleClient::UpdateStatus(TGo4TaskStatus *state)
{
   GO4TRACE((12,"TGo4ExampleClient::UpdateStatus(TGo4ClientStatus*)",__LINE__, __FILE__));
   TGo4ClientTask::UpdateStatus(state); // fill superclass attributes
   TGo4ExampleClientStatus* exstate= dynamic_cast<TGo4ExampleClientStatus*> (state);
   if (exstate) {
      exstate->SetHistoStatus(GetApplication()->GetHistogram());
      exstate->SetNames(fcMainName.Data(), fcWatchName.Data());
   }
}

void TGo4ExampleClient::Stop()
{
   GO4TRACE((12,"TGo4ExampleClient::Stop()",__LINE__, __FILE__));
   std::cout << "Stop of example client!" << std::endl;

   TGo4Log::Debug(" ExampleClient ''%s'' executing Stop(): stop main thread",GetName());
   fxWorkHandler->Stop(fcMainName.Data());
}

void TGo4ExampleClient::Start()
{
   GO4TRACE((12,"TGo4ExampleClient::Start()",__LINE__, __FILE__));
   std::cout << "Start of example client!" << std::endl;

   TGo4Log::Debug(" ExampleClient ''%s'' executing Start(): start main thread",GetName());
   fxWorkHandler->Start(fcMainName.Data());
}

TGo4ExampleApplication* TGo4ExampleClient::GetApplication()
{
   GO4TRACE((12,"TGo4ExampleClient::GetApplication()",__LINE__, __FILE__));
   return fxApplication;
}
