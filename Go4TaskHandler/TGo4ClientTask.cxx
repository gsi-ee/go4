#include "TGo4ClientTask.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"

#include "TGo4BufferQueue.h"
#include "TGo4Command.h"
#include "TGo4ClientStatus.h"
#include "TGo4ThreadHandler.h"
#include "TGo4TaskHandler.h"

TGo4ClientTask::TGo4ClientTask(const char* name,
                               const char* serverhost,
                               UInt_t negotiationport,
                               Bool_t blockingmode,
                               Bool_t standalone,
                               Bool_t autostart,
                               Bool_t autocreate,
                               Bool_t ismaster,
                               Bool_t autoconnect)
   : TGo4Task(name,blockingmode,autostart,autocreate,ismaster),
            fxTaskHandler(0),fbAutoConnect(autoconnect),fbServerConnected(kFALSE)
{
   fxServerHostname=serverhost;
   TString nomen("TaskHandler of "); nomen+=name;
   fxTaskHandler=new TGo4TaskHandler(nomen.Data(),this,kTRUE, IsMaster(),negotiationport);
   fxCommandQ=dynamic_cast<TGo4BufferQueue*> (GetTaskHandler()->GetCommandQueue());
   fxStatusQ=dynamic_cast<TGo4BufferQueue*> (GetTaskHandler()->GetStatusQueue());
   fxDataQ=dynamic_cast<TGo4BufferQueue*> (GetTaskHandler()->GetDataQueue());
   if(standalone)
      {
         Launch(); // create threads, start application control timer
      }
   else
      {
         // subclass must call Launch at end of its ctor
      }
}

TGo4ClientTask::~TGo4ClientTask()

{
   DisconnectServer();
   delete fxTaskHandler;
}

TGo4TaskHandler* TGo4ClientTask::GetTaskHandler()
{
   return fxTaskHandler;
}

Int_t TGo4ClientTask::Initialization()
{
   Int_t rev=-1;
   if(fbInitDone)
      // already initialized, return ok value
      {
         return 0;
      } //// if(fbInitDone)
   else
      {
         rev=TGo4Task::Initialization(); // this will launch threads, etc.
         if(rev==0)
            {
               // success: then try to connect (all threads are up now)
             if(fbAutoConnect)
               {
                  if(!ConnectServer(GetServerHostName(),0))
                     Terminate();
               }
            }
         else
            {
               // init failed: tell timer return value
               // return rev; // "rev=rev"
            }
      }// else if(fbInitDone)
    return rev;
}

Bool_t TGo4ClientTask::ConnectServer(const char* node, UInt_t negport,
                            Go4CommandMode_t role,
                            const char* passwd)
{
if(fbServerConnected)
   {
      TGo4Log::Warn(" ClientTask::ConnectServer ''%s'': ServerTask already connected",
               GetName());
      return kTRUE;
   }

if(negport)
   fxTaskHandler->SetNegotiationPort(negport); // null negport will use the default port
if(IsMaster())
   fxTaskHandler->ClearQueues(); // do not clear queues on slave side,
                                 // because analysis status might be already in it!

//cout <<"TGo4ClientTask::ConnectServer with role "<<role<<" and passwd "<<passwd << endl;
if(passwd)
   {
      fxTaskHandler->SetRole(role);
      if(role==kGo4ComModeObserver)
         {
            TGo4TaskHandler::SetObservAccount(0,passwd);
         }
      else if (role==kGo4ComModeController)
         {
            TGo4TaskHandler::SetCtrlAccount(0,passwd);
         }
      else if (role==kGo4ComModeAdministrator)
         {
            TGo4TaskHandler::SetAdminAccount(0,passwd);
         }

   }
if(fxTaskHandler->Connect(node,0))
   {
//      TGo4Log::Info(" ClientTask ''%s'': connected successfully to ServerTask at node %s (port %d) ",
//               GetName(),node, negport);
      fbServerConnected=kTRUE;
      fxWorkHandler->StartAll(); // all transports are there, then start waiting threads
      SendStatusMessage(1,kTRUE,"Client %s connected successfully to Server task at node %s",
         GetName(),node); // note that negport does not contain actual portnumber here
      return kTRUE;
    }
else
   {
      TGo4Log::Error(" ClientTask ''%s'': FAILED connection to ServerTask at node %s",
               GetName(),node);
      return kFALSE;
   }


}

Bool_t TGo4ClientTask::DisconnectServer(Bool_t isterminating)
{
Bool_t rev=kTRUE;
if(fbServerConnected)
   {
     StopWorkThreads();
     if(IsMaster())
      {
//         fxTaskHandler->ClearQueues(); // prevent monitor commands to interfere with disconnect
           TGo4Queue* cq=GetCommandQueue();
           if(cq) cq->Clear(); // only clear command queue on master side,
                               // otherwise we lose status messages from server
      }
      ////////UNDER CONSTRUCTION
      WakeCommandQueue(TGo4Task::Get_fgiTERMID()); // will stop local command thread, and remote
      SendStopBuffers(); // note: this should only be done after disconnect login was succesfull JA
      rev=fxTaskHandler->DisConnect();
      /////////
      if(rev) fbServerConnected=kFALSE;
      if(!isterminating) StartWorkThreads();
   }
return rev;
}

void TGo4ClientTask::Quit()
{
   TGo4Log::Debug(" ClientTask''%s'' is quitting... ",GetName());
   SendStatusMessage(2,kTRUE,"ClientTask %s  is terminating...",GetName());
   DisconnectServer(kTRUE);
   Terminate(!IsMaster()); // never terminate master process
}

TGo4TaskStatus* TGo4ClientTask::CreateStatus()
{
   TGo4ClientStatus* stat= new TGo4ClientStatus(GetName());
   UpdateStatus(stat); // set the internals
   return stat;
}


void TGo4ClientTask::UpdateStatus(TGo4TaskStatus* state)
{
   TGo4Task::UpdateStatus(state);
   TGo4ClientStatus* clstate=dynamic_cast<TGo4ClientStatus*>(state);
   if(clstate)
      clstate->SetNames(GetServerHostName());
}

void TGo4ClientTask::AddLocalCommand(TGo4Command * com)
{
if(com==0) return;
if(fxCommandQ)
   {
      fxCommandQ->AddBufferFromObject(com);
   }
else
   {
      TGo4Log::Debug(" !!! ClientTask - ERROR adding local command - no command queue !!! ");
   }
}

TGo4BufferQueue* TGo4ClientTask::GetCommandQueue(const char*)
{
   return fxCommandQ;
}

TGo4BufferQueue * TGo4ClientTask::GetStatusQueue(const char*)
{
  return fxStatusQ;
}
TGo4BufferQueue * TGo4ClientTask::GetDataQueue(const char*)
{
   return fxDataQ;
}
