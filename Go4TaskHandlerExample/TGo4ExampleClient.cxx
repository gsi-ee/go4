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

#include "TGo4ExampleClient.h"

#include "go4iostream.h"

#include "TGo4Log.h"
#include "TGo4TaskHandler.h"
#include "TGo4ThreadHandler.h"

#include "TGo4ExampleClientStatus.h"
#include "TGo4ExampleApplication.h"
#include "TGo4MainRunnable.h"
#include "TGo4WatchRunnable.h"

const char* TGo4ExampleClient::fgcWATCHTHREAD="WATCH-";
const char* TGo4ExampleClient::fgcMAINTHREAD="MAIN-";


TGo4ExampleClient::TGo4ExampleClient(const char* name,
                                     const char* host,
                                     UInt_t negport,
                                     Bool_t blockingmode)
: TGo4ClientTask(name, host, negport , blockingmode, kFALSE)
{
   TRACE((15,"TGo4ExampleClient::TGo4ExampleClient(const char*, const char*, Uint_t, Bool_t) constructor",__LINE__, __FILE__));
   SetMaster(kFALSE);
   TGo4Log::Debug(" ExampleClient ''%s'' started ",GetName());
   fxApplication= new TGo4ExampleApplication( (TGo4BufferQueue*) GetTaskHandler()->GetDataQueue());
   TGo4MainRunnable* mainrun = new TGo4MainRunnable(Form("MainRunnable of %s",GetName()), this);
   TGo4WatchRunnable* watchrun = new TGo4WatchRunnable(Form("WatchRunnable of %s",GetName()), this);
      // adding runnables to thread handler who takes over the responsibility...:
   fcMainName.Form("%s%s", fgcMAINTHREAD, GetName());
   fxWorkHandler->NewThread(fcMainName.Data(), mainrun);
   fcWatchName.Form("%s%s", fgcWATCHTHREAD, GetName());
   fxWorkHandler->NewThread(fcWatchName.Data(), watchrun);
   Launch();
}

TGo4ExampleClient::~TGo4ExampleClient()
{
   TRACE((15,"TGo4ExampleClient::~TGo4ExampleClient() destructor",__LINE__, __FILE__));
   fxWorkHandler->CancelAll(); // make sure threads wont work on application when its deleted
   delete fxApplication;
}

TGo4TaskStatus * TGo4ExampleClient::CreateStatus()
{
   TRACE((12,"TGo4ExampleClient::CreateStatus()",__LINE__, __FILE__));
   TGo4ExampleClientStatus* stat= new TGo4ExampleClientStatus(GetName());
   UpdateStatus(stat); // set the internals
   return stat;

}
void TGo4ExampleClient::UpdateStatus(TGo4ClientStatus* state)
{
   TRACE((12,"TGo4ExampleClient::UpdateStatus(TGo4ClientStatus*)",__LINE__, __FILE__));
   TGo4ClientTask::UpdateStatus(state); // fill superclass attributes
   TGo4ExampleClientStatus* exstate= (TGo4ExampleClientStatus*) state;
   exstate->SetHistoStatus(GetApplication()->GetHistogram());
   exstate->SetNames(fcMainName.Data(), fcWatchName.Data());

}

void TGo4ExampleClient::Stop()
{
   TRACE((12,"TGo4ExampleClient::Stop()",__LINE__, __FILE__));
    cout << "Stop of example client!"<<endl;

   TGo4Log::Debug(" ExampleClient ''%s'' executing Stop(): stop main thread",GetName());
   fxWorkHandler->Stop(fcMainName.Data());
}
void TGo4ExampleClient::Start()
{
   TRACE((12,"TGo4ExampleClient::Start()",__LINE__, __FILE__));
    cout << "Start of example client!"<<endl;

   TGo4Log::Debug(" ExampleClient ''%s'' executing Start(): start main thread",GetName());
   fxWorkHandler->Start(fcMainName.Data());
}
TGo4ExampleApplication* TGo4ExampleClient::GetApplication()
{
  TRACE((12,"TGo4ExampleClient::GetApplication()",__LINE__, __FILE__));
   return fxApplication;
}
