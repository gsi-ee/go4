#include "TGo4TaskOwner.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Task.h"
#include "TGo4ServerTask.h"
#include "TGo4TaskManager.h"
#include "TGo4ClientTask.h"


TGo4TaskOwner::TGo4TaskOwner() :
   TGo4CommandReceiver(),
   fxTask(0),
   fbServerMode(kTRUE)
{
}

TGo4TaskOwner::TGo4TaskOwner(Bool_t isserver) :
   TGo4CommandReceiver(),
   fxTask(0),
   fbServerMode(isserver)
{
}

TGo4TaskOwner::~TGo4TaskOwner()
{
   if(fxTask!=0)
      fxTask->SetOwner(0); // avoid the task to delete us again in dtor
   delete fxTask;
}

const char* TGo4TaskOwner::GetName() const
{
   return (fxTask!=0) ? fxTask->GetName() : 0;
}

void TGo4TaskOwner::SetTask(TGo4Task* task, Bool_t delold)
{
   if(delold) delete fxTask;
   fxTask=task;
   if(fxTask) fxTask->SetOwner(this);
}

void TGo4TaskOwner::Quit()
{
   fxTask->Quit();
}

Int_t TGo4TaskOwner::StartWorkThreads()
{
   TGo4Log::Debug(" Task Owner: default StartWorkThreads() ");
   fxTask->SetWorkIsStopped(kFALSE);
   return 0;
}
Int_t TGo4TaskOwner::StopWorkThreads()
{
   TGo4Log::Debug(" Task Owner: default StopWorkThreads() ");
   fxTask->SetWorkIsStopped(kTRUE);
   return 0;
}


Bool_t TGo4TaskOwner::IsConnected()
{
if(IsServer())
   {
      TGo4ServerTask* server=dynamic_cast<TGo4ServerTask*>(GetTask());
      if(server==0)
         {
            cerr <<"NEVER COME HERE: servermode without servertask in taskowner "<< GetName() << endl;
            return kFALSE;
         }
      return (server->GetCurrentTaskHandler()!=0);
   }
else
   {
      TGo4ClientTask* client=dynamic_cast<TGo4ClientTask*>(GetTask());
      if(client==0)
         {
            cerr <<"NEVER COME HERE: clientmode without clienttask in taskowner "<< GetName() << endl;
            return kFALSE;
         }
      return (client->IsConnected());
   }

}

TMutex* TGo4TaskOwner::GetTaskManagerMutex()
{
if(IsServer())
   {
   TGo4ServerTask* serv=dynamic_cast<TGo4ServerTask*>(GetTask());
   if(serv)
         return (serv->GetTaskManager()->GetMutex());
   else
         return 0;
   }
else
{
  return 0;
}
}

