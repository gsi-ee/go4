#include "TGo4ExampleClient.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4TaskHandler.h"
#include "TGo4ThreadHandler.h"
#include "snprintf.h"

#include "TGo4ExampleClientStatus.h"
#include "TGo4ExampleApplication.h"
#include "TGo4MainRunnable.h"
#include "TGo4WatchRunnable.h"

const Text_t TGo4ExampleClient::fgcWATCHTHREAD[]="WATCH-";
const Text_t TGo4ExampleClient::fgcMAINTHREAD[]="MAIN-";


TGo4ExampleClient::TGo4ExampleClient(const char* name,
                                     const char* host,
                                     UInt_t negport,
                                     Bool_t blockingmode)
: TGo4ClientTask(name, host, negport , blockingmode, kFALSE)
{
   TRACE((15,"TGo4ExampleClient::TGo4ExampleClient(Text_t*, Bool_t) constructor",__LINE__, __FILE__));
   SetMaster(kFALSE);
   TGo4Log::Debug(" ExampleClient ''%s'' started ",GetName());
   fcMainName= new Text_t[TGo4ThreadManager::fguTEXTLENGTH];
   fcWatchName= new Text_t[TGo4ThreadManager::fguTEXTLENGTH];
   Text_t namebuffer[TGo4ThreadManager::fguTEXTLENGTH];
   fxApplication= new TGo4ExampleApplication( (TGo4BufferQueue*) GetTaskHandler()->GetDataQueue());
   snprintf(namebuffer,TGo4ThreadManager::fguTEXTLENGTH-1,"MainRunnable of %s",GetName());
   TGo4MainRunnable* mainrun= new TGo4MainRunnable(namebuffer, this);
   snprintf(namebuffer,TGo4ThreadManager::fguTEXTLENGTH-1,"WatchRunnable of %s",GetName());
   TGo4WatchRunnable* watchrun= new TGo4WatchRunnable(namebuffer, this);
      // adding runnables to thread handler who takes over the responsibility...:
   snprintf(fcMainName,TGo4ThreadManager::fguTEXTLENGTH-1,"%s%s",fgcMAINTHREAD,GetName());
   fxWorkHandler->NewThread(fcMainName,mainrun);
   snprintf(fcWatchName,TGo4ThreadManager::fguTEXTLENGTH-1,"%s%s",fgcWATCHTHREAD,GetName());
   fxWorkHandler->NewThread(fcWatchName,watchrun);
   Launch();
}

TGo4ExampleClient::~TGo4ExampleClient()
{
   TRACE((15,"TGo4ExampleClient::~TGo4ExampleClient() destructor",__LINE__, __FILE__));
   delete [] fcMainName;
   delete [] fcWatchName;
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
   exstate->SetNames(fcMainName, fcWatchName);

}

void TGo4ExampleClient::Stop()
{
   TRACE((12,"TGo4ExampleClient::Stop()",__LINE__, __FILE__));
    cout << "Stop of example client!"<<endl;

   TGo4Log::Debug(" ExampleClient ''%s'' executing Stop(): stop main thread",GetName());
   fxWorkHandler->Stop(fcMainName);
}
void TGo4ExampleClient::Start()
{
   TRACE((12,"TGo4ExampleClient::Start()",__LINE__, __FILE__));
    cout << "Start of example client!"<<endl;

   TGo4Log::Debug(" ExampleClient ''%s'' executing Start(): start main thread",GetName());
   fxWorkHandler->Start(fcMainName);
}
TGo4ExampleApplication* TGo4ExampleClient::GetApplication()
{
  TRACE((12,"TGo4ExampleClient::GetApplication()",__LINE__, __FILE__));
   return fxApplication;
}
