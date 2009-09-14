#include "TGo4ServerTask.h"

#include "Riostream.h"

#include "TMutex.h"
#include "TApplication.h"
#include "TObjArray.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Socket.h"
#include "TGo4BufferQueue.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Status.h"
#include "TGo4CommandInvoker.h"
#include "TGo4TaskManager.h"
#include "TGo4TaskHandler.h"
#include "TGo4ConnectorRunnable.h"
#include "TGo4Master.h"
#include "TGo4Slave.h"
#include "TGo4TaskConnectorTimer.h"


const Int_t TGo4ServerTask::fgiOPENWAITCYCLES=100; // wait cycles (100)
const UInt_t TGo4ServerTask::fguOPENWAITCYCLETIME=100; // time in ms (20)
const Int_t TGo4ServerTask::fgiCLOSEWAITCYCLES=100; // wait cycles (100)
const UInt_t TGo4ServerTask::fguCLOSEWAITCYCLETIME=100; // time in ms (20)
const Int_t TGo4ServerTask::fgiCONNECTWAITCYCLES=20; // wait cycles (20)
const UInt_t TGo4ServerTask::fguCONNECTWAITCYCLETIME=200; // time in ms (200)
const UInt_t TGo4ServerTask::fguCONNECTTIMERPERIOD=100; // time in ms (50)


const char* TGo4ServerTask::fgcLAUNCHPREFSFILE = "etc/Go4LaunchPrefs.txt";


TGo4ServerTask::TGo4ServerTask(const char* name,
                               UInt_t negotiationport,
                               Bool_t blockingmode,
                               Bool_t standalone,
                               Bool_t autostart,
                               Bool_t autocreate,
                               Bool_t ismaster)
   : TGo4Task(name,blockingmode, autostart,autocreate,ismaster),
   fxTaskManager(0),fxCurrentTaskHandler(0),
   fxConnectTransport(0), fxDisConnectTransport(0),
   fuConnectPort(0), fbKeepServerSocket(kFALSE),
   fbConnectRequest(kFALSE), fbDisConnectRequest(kFALSE),
   fbConnectIsOpen(kFALSE),fbConnectIsDone(kFALSE), fbConnectIsClose(kFALSE),
   fxConnectorTimer(0)
{
   TString nomen("TaskManager of "); nomen += name;
   fxTaskManager= new TGo4TaskManager(nomen.Data(), this, negotiationport);

   if(negotiationport!=42)
      {
         // connector runnable:
         nomen.Form("ConnectorRunnable of %s", name);
         TGo4ConnectorRunnable* conny = new TGo4ConnectorRunnable(nomen.Data(),this);
         nomen.Form("CONNECTOR-%s", name);
         fxConnectorName=nomen;
         fxWorkHandler->NewThread(GetConnectorName(), conny);
      }
      else  {} // do not start connector port in local mode <- nameservice problems without wan!

   TGo4CommandInvoker::Instance(); // make sure a command invoker exists
   TGo4CommandInvoker::Register("ServerTask", this);
   fxConnectorTimer= new TGo4TaskConnectorTimer(this,fguCONNECTTIMERPERIOD);
   fxConnectorTimer->TurnOn();
   if(standalone)
      {
         Launch(); // create threads, start application control timer
      }
   else
      {
         // subclass must call Launch at end of its ctor
      }
}


TGo4ServerTask::~TGo4ServerTask()
{
   if (GetWorkHandler()) GetWorkHandler()->CancelAll();
   delete fxConnectorTimer;
   delete fxTaskManager;
   TGo4CommandInvoker::UnRegister(this);
}

Bool_t TGo4ServerTask::RemoveClient(const char* name, Bool_t clientwait, Bool_t isterminating)
{
   Bool_t rev=kTRUE;
   TGo4TaskHandler* taskhandler=0;
   if(name && strstr(name,"current"))
      taskhandler=GetCurrentTaskHandler();
   else
      taskhandler=GetTaskHandler(name);
   if(taskhandler==0)
      {
         // no such taskhandler for name
         TGo4Log::Debug(" ServerTask -- RemoveClient FAILED, no client %s !!! ",
                  name);
         rev=kFALSE;
      }
   else // if(taskhandler==0)
      {
       TGo4Log::Debug(" ServerTask -- removing client task %s ",name);
       // first stop all user threads waiting on or writing into the data queues
      StopWorkThreads();
      if(clientwait)
         {
         // normal mode: handshake with client to be removed

            // send quit command to client, client will send dummy objects back
            // to release waiting sockets
            // then client will request a disconnect action from the connector runnable
            if(IsMaster())
               SubmitEmergencyCommand(kComQuit); // master quits client when removing
            else
               SubmitEmergencyData(kComQuit, taskhandler->GetName()); // client data runnable must handle quit request of server!
            TGo4Log::Debug(" Server Task --  Waiting for client %s disconnection...",taskhandler->GetName());
            Int_t removeresult=fxTaskManager->WaitForClientRemoved();
                           // disconnection is done by connector
                           // thread using task manager methods
            switch(removeresult)
               {
               case -1:
                  // timeout
                  TGo4Log::Debug(" !!! Server Task -- client remove wait TIMEOUT !!!  ");
                  rev=fxTaskManager->DisConnectClient(taskhandler->GetName(),kFALSE); // do not wait for client ok
                  break;

               case -2:
                  // we are terminating the server
                  TGo4Log::Debug(" !!! Server Task -- client remove aborted for TERMINATION MODE !!!  ");
                  rev=kFALSE;
                  break;

               default:
                  // all right
                  TGo4Log::Debug(" Server Task -- waited %d cycles until client was removed. ",removeresult);
                  rev=kTRUE;
                  break;

                     }
         } // if(clientwait)
      else
         {
         // no communication with client, just disconnect without waiting
         TGo4Log::Debug(" !!! Server Task -- removing client %s without waiting...  ",
                                    taskhandler->GetName());
         SendStopBuffers(taskhandler->GetName());
         rev= (fxTaskManager->DisConnectClient(taskhandler->GetName(),kFALSE)==0); // do not wait
         }
      if(!isterminating) StartWorkThreads();
   } // else if(taskhandler==0)
   return rev;
}

Int_t TGo4ServerTask::RemoveAllClients(Bool_t force)
{
   Int_t rev=0; // return value is number of removed clients
   //cout <<"TTTTTTTT TGo4ServerTask::RemoveAllClients" << endl;
//// new: first figure out all existing names, then remove one by one:
   TGo4TaskHandler* taskhandler=0;
   TObjArray names;
   Bool_t reset=kTRUE;
   while((taskhandler=fxTaskManager->NextTaskHandler(reset)) !=0)
      {
         reset=kFALSE;
         //cout <<"adding name "<<taskhandler->GetName() << endl;
         names.AddLast(new TNamed(taskhandler->GetName(), "title"));
      }
   TIter niter(&names);
   TObject* nomen=0;
   while((nomen =niter.Next()) !=0)
      {
         //cout <<"removing th "<<nomen->GetName() << endl;
         RemoveClient(nomen->GetName(),!force,kTRUE);
         rev++;
      }
   names.Delete();
   // end iteration
   return rev;
}


Bool_t TGo4ServerTask::RemoveCurrentClient()
{
   Bool_t rev=kTRUE;
   TGo4TaskHandler* taskhandler=GetCurrentTaskHandler();
   if(taskhandler!=0)
      {
         // we have a current client, remove it
         TGo4Log::Debug(" Server task -- removing current client %s ",taskhandler->GetName());
         rev = RemoveClient(taskhandler->GetName());
      }
   else
      {
         rev=kFALSE;
      }
   return rev;
}

void TGo4ServerTask::SetCurrentTask(const char* name)
{
   //cout <<"server task setting current task to "<<name << endl;
   TGo4TaskHandler* han=0;
   if(fxTaskManager==0)
      {
         TGo4Log::Debug(" TGo4ServerTask ''%s'' ERROR- task manager not existing!!! ");
      }
   else
      {
         // first stop all user threads waiting on or writing into the data queues
         if(IsWorkStopped())
            {
               // Working threads have already been stopped
               // maybe by RemoveClient method.
               // We do nothing, since the current taskhandler is not
               // reset yet and any access to current task in the derived
               // _user_ method would crash the program!
            }
         else // if(IsWorkStopped())
            {
               // work has not been stopped, we do that
               StopWorkThreads();
            } // else if(fbWorkIsStopped)
      {
           if(name==0)
               {
                  // zero name given, set pointer to last handler still in list
                     fxCurrentTaskHandler=fxTaskManager->GetLastTaskHandler();
                     //cout << "**** set current th from get lastth:"<< fxCurrentTaskHandler <<endl;
               }
            else // if(name==0)
               {
               // name specified, search for it
               han=fxTaskManager->GetTaskHandler(name);
               if(han)
                  {
                     fxCurrentTaskHandler=han;
                     //cout << "**** set current th from name:"<< fxCurrentTaskHandler <<endl;
                  }
               else // if(han)
                  {
                      TGo4Log::Debug(" ServerTask: FAILED setting current task to %s-- no such client! ",name);
                  } // else if(han)
               } // else if(name==0)

      } // TGo4LockGuard
       // finally, start user threads again
         StartWorkThreads();
      } // else if(fxTaskManager==0)
}


TGo4TaskHandler* TGo4ServerTask::GetTaskHandler(const char* name)
{
   return (fxTaskManager->GetTaskHandler(name));
}

TGo4TaskHandler* TGo4ServerTask::GetTaskHandler()
{
   return (GetCurrentTaskHandler());
}

TGo4TaskHandler* TGo4ServerTask::GetCurrentTaskHandler()
{
   return fxCurrentTaskHandler;
}

TGo4TaskManager* TGo4ServerTask::GetTaskManager()
{
   return fxTaskManager;
}

void TGo4ServerTask::SetConnect(TGo4Socket * trans, const char* host, UInt_t port, Bool_t keepserv)
{
   fxConnectTransport=trans;
   fxConnectHost=host;
   fuConnectPort=port;
   fbConnectRequest=kTRUE;
   fbKeepServerSocket=keepserv;
}

void TGo4ServerTask::SetDisConnect(TGo4Socket * trans)
{
   fxDisConnectTransport=trans;
   fbDisConnectRequest=kTRUE;
}

Int_t TGo4ServerTask::TimerConnect()
{
   Int_t rev=0;
   ///////////////////////////////////////
   //// handle the disconnection first:
   if(fbDisConnectRequest)
      {
         if(fxDisConnectTransport!=0)
            {
               // we have a transport instance to disconnect
               fxDisConnectTransport->Close();
               //delete fxDisConnectTransport; // new
               //cout << "++++++++Timer closed transport"<<endl;
               fbConnectIsClose=kTRUE;
               fbDisConnectRequest=kFALSE; // we served the request, reset it
            rev+=1;
            }
         else
            {
               // error, zero pointer given
               rev+=32;
            }
      }
   else //// if(fbDisConnectRequest)
      {
         // no open request, continue
         rev+=2;
      }

   /////////////////////////////////////
   //// then handle the connection:
   if(fbConnectRequest)
      {
         // timer shall open a transport as server
      if(fxConnectTransport!=0)
         {

            if(!fxConnectTransport->IsOpen())
               {
                  // transport is not open, so do it
//                  cout << "++++++++Timer will open transport"<<endl;
                  fbConnectIsOpen=kTRUE; // tell connector thread that we try to open
                  Int_t result=fxConnectTransport->Open(GetConnectHost(), fuConnectPort, fbKeepServerSocket);
                  if(result==0)
                     {
                        fbConnectIsDone=kTRUE; // tell connector thread we returned from open
                        fbConnectRequest=kFALSE; // we served the request, reset it
                        fbKeepServerSocket=kFALSE; // reset keep server socket flag
                        rev+=4;
                     }
                  else
                     {
                        rev=-4;
                        // open was not finished, we poll once again...
                     }
               }
            else
               {
                  // transport was already open
//                  cout <<"OOOOOOOOOOOO TimerConnect transport already open!" << endl;
                  fbConnectIsOpen=kTRUE;
                  fbConnectIsDone=kTRUE; // tell connector thread we returned from open
                  fbConnectRequest=kFALSE; // we served the request, reset it
                  fbKeepServerSocket=kFALSE; // reset keep server socket flag
                  rev+=8;
               }
         } // if(fxConnectTransport!=0)
      else
         {
               rev+=64;
               // no Transport specified: create raw server for negotiation port
               //fxConnectTransport=new TGo4Socket("Server",3);
               //cout << "(((((( timer created new raw server transport"<<endl;
         }
      } //// if(fbConnectRequest)
   else
      {
         // no open request, continue
         rev+=16;
      }
   return rev;
}

Int_t TGo4ServerTask::WaitForOpen()
{
   Int_t count=0;
   while(!fbConnectIsOpen)
      {
         if(count>TGo4ServerTask::fgiOPENWAITCYCLES)
            {
               count = -1; // timeout
               break;
            }
         else
            {
            TGo4Thread::Sleep(TGo4ServerTask::fguOPENWAITCYCLETIME);
            ++count;
            }
//      cout << "*****WaitForOpen()"<<endl;
      }
   fbConnectIsOpen=kFALSE; //  reset for next time
   return count;
}


Int_t TGo4ServerTask::WaitForClose()
{
   Int_t count=0;
   while(!fbConnectIsClose)
      {
          if(count>TGo4ServerTask::fgiCLOSEWAITCYCLES)
            {
               count = -1; // timeout
               break;
            }
         else
            {
               TGo4Thread::Sleep(TGo4ServerTask::fguCLOSEWAITCYCLETIME);
               ++count;
         }
      //cout << "*****WaitForClose() "<<count<< endl;
      }
   fbConnectIsClose=kFALSE; //  reset for next time
   return count;
}

Int_t TGo4ServerTask::WaitForConnection()
{
   Int_t count=0;
   while(!fbConnectIsDone)
      {
         if(IsTerminating())
            {
               count = -2; // termination mode
               break;
            }
// timeout would affect the permanent connector port Open , we skip this
//         else if(count>TGo4ServerTask::fgiCONNECTWAITCYCLES)
//            {
//               count = -1; // timeout
//               break;
//            }
         else
            {
               TGo4Thread::Sleep(TGo4ServerTask::fguCONNECTWAITCYCLETIME);
               ++count;
            }
//         cout << "*****WaitForConnection()"<<endl;
      }
   fbConnectIsDone=kFALSE; //  reset for next time
   return count;
}

TGo4Socket* TGo4ServerTask::GetConnectTransport()
{
   return fxConnectTransport;
}

TGo4BufferQueue* TGo4ServerTask::GetCommandQueue(const char* name)
{
   TGo4BufferQueue* queue=0;
   TGo4TaskHandler* currenttask=0;
   if(name==0 || strstr(name,"current"))
      currenttask=GetCurrentTaskHandler(); // find out the current client
   else
      currenttask=GetTaskHandler(name); // find out destination client by name
   if(currenttask)
         queue=dynamic_cast<TGo4BufferQueue*> (currenttask->GetCommandQueue());
   return queue;
}

TGo4BufferQueue * TGo4ServerTask::GetStatusQueue(const char* name)
{
   TGo4BufferQueue* queue=0;
   TGo4TaskHandler* currenttask=0;
   if(name==0)
      currenttask=GetCurrentTaskHandler(); // find out the current client
   else
      currenttask=GetTaskHandler(name); // find out destination client by name
   if(currenttask)
         queue=dynamic_cast<TGo4BufferQueue*> (currenttask->GetStatusQueue());
   return queue;
}


TGo4BufferQueue * TGo4ServerTask::GetDataQueue(const char* name)
{
   TGo4BufferQueue* queue=0;
   TGo4TaskHandler* currenttask=0;
   if(name==0 || strstr(name,"current"))
      currenttask=GetCurrentTaskHandler(); // find out the current client
   else
      currenttask=GetTaskHandler(name); // find out destination client by name
   if(currenttask)
         queue=dynamic_cast<TGo4BufferQueue*> (currenttask->GetDataQueue());
   return queue;

}

TGo4Command* TGo4ServerTask::NextCommand()
{
if(IsMaster()) return 0;
TGo4Command* com=0;
TGo4TaskHandler* han=0;
Bool_t reset=kTRUE;
TGo4LockGuard taskmutex(fxTaskManager->GetMutex());
while((han=fxTaskManager->NextTaskHandler(reset))!=0)
   {
       reset=kFALSE;
       TGo4BufferQueue * comq=dynamic_cast<TGo4BufferQueue*> (han->GetCommandQueue());
       if(comq==0) continue; //NEVER COME HERE!
       if(!comq->IsEmpty()) // prevent waiting on queue
         {
             com= dynamic_cast<TGo4Command*>(comq->WaitObjectFromBuffer());
             if(com)
               {
                  com->SetTaskName(han->GetName());
                  com->SetMode(han->GetRole());
                  return com;
               }
         }
   } // while
return com;
}

void TGo4ServerTask::SendStatus(TGo4Status * stat, const char* receiver)
{
if(IsMaster()) return;
if(stat==0) return;
if(receiver!=0)
   {
      //cout <<"TGo4ServerTask::SendStatus to receiver "<<receiver << endl;
      TGo4Task::SendStatus(stat,receiver);
   }
else
   {
      // send status to all
      TGo4LockGuard taskmutex(fxTaskManager->GetMutex());
      TGo4TaskHandler* han=0;
      Bool_t reset=kTRUE;
      while((han=fxTaskManager->NextTaskHandler(reset))!=0)
         {
            reset=kFALSE;
            TGo4BufferQueue * statq=dynamic_cast<TGo4BufferQueue*> (han->GetStatusQueue());
            if(statq==0) continue; //NEVER COME HERE!
            TGo4Log::Debug(" Task - sending status %s to task %s", stat->ClassName(), han->GetName());
            statq->AddBufferFromObject(stat);
         }// while
   } // if(receiver)
}

void TGo4ServerTask::SendStatusBuffer()
{
if(IsMaster()) return;
//cout << "TGo4ServerTask::SendStatusBuffer() apply for fxStatusMutex"<< endl;
TGo4LockGuard statguard(fxStatusMutex); // do not send during buffer update
//cout << "TGo4ServerTask::SendStatusBuffer() apply for taskmanager mutex"<< endl;
TGo4LockGuard taskmutex(fxTaskManager->GetMutex()); // protect task list
TGo4TaskHandler* han=0;
Bool_t reset=kTRUE;
while((han=fxTaskManager->NextTaskHandler(reset))!=0)
   {
      reset=kFALSE;
      TGo4BufferQueue * statq=dynamic_cast<TGo4BufferQueue*> (han->GetStatusQueue());
      if(statq==0) continue; //NEVER COME HERE!
      TGo4Log::Debug(" Task - sending status buffer to task %s", han->GetName());
      statq->AddBuffer(fxStatusBuffer,kTRUE);
   }// while
}



Bool_t TGo4ServerTask::StartConnectorThread()
{
   Bool_t rev=kTRUE;
   rev= ( GetWorkHandler()->Start( GetConnectorName() ) );
      //cout << "ServerTask started connector thread"<<endl;
   return rev;
}

Bool_t TGo4ServerTask::StopConnectorThread()
{
   Bool_t rev=kTRUE;
   rev= ( GetWorkHandler()->Stop( GetConnectorName() ) ); // unset running flag
   // now establish a dummy connection to our own server to release the listen socket:
   const char* host = gSystem->HostName();
   Int_t negotiationport=fxTaskManager->GetNegotiationPort();
   TGo4Socket* connector= new TGo4Socket(kTRUE); // raw socket transport
      //cout << "host:"<<host<<" , port:" << negotiationport <<endl;
   connector->Open(host,negotiationport); // open connection to server's connector runnable
   connector->Send(TGo4TaskHandler::Get_fgcERROR()); // force server to stop
   connector->Close();
   delete connector;
   return rev;
}


Bool_t TGo4ServerTask::ConnectorThreadIsStopped()
{
   Bool_t rev=kTRUE;
   TGo4Thread* conny= GetWorkHandler()->GetThread(GetConnectorName());
   rev= conny->IsWaiting();
   return rev;
}

void TGo4ServerTask::Quit()
{
   TGo4Log::Debug(" ServerTask Quit -- removing all connected clients ");
   SendStatusMessage(2,kTRUE,"ServerTask %s is shutting down now! All clients are removed...",GetName());
   RemoveAllClients();
   //StopWorkThreads(); // are re-started after last client is removed...
   WakeCommandQueue(TGo4Task::Get_fgiTERMID()); // will stop local command thread, and remote
   Terminate(!IsMaster()); // terminate only slave server here!
}


void TGo4ServerTask::Shutdown()
{
   TGo4Log::Debug(" ServerTask Shutdown without disconnect waiting");
   SendStatusMessage(2,kTRUE,"ServerTask %s is shutting down now! All clients are removed...",GetName());
   TGo4Thread::Sleep(10*fguCONNECTTIMERPERIOD); // wait 1 s to broadcast shutdown message before terminating...
   StopWorkThreads();
   WakeCommandQueue(TGo4Task::Get_fgiTERMID()); // will stop local command thread, and remote
   RemoveAllClients(true);
   TGo4Slave* slave=GetSlave();
   if(slave)
      {
         slave->Stop(); // to execute analysis postloop.
                        // We are within main thread here, i.e. it never stops before termination!
         slave->SetTask(0,kFALSE); // otherwise owner dtor will delete us...
         delete slave;   //call dtors of analysis framework
         //SetOwner(0);
      }
   gApplication->Terminate(); // do not wait until appctrl timer terminates us
}


void TGo4ServerTask::LockAll()
{
//cout <<"TGo4ServerTask::LockAll" << endl;

fxStatusMutex->Lock();
//cout <<"TGo4ServerTask::LockAll before taskmutex" << endl;
fxTaskManager->GetMutex()->Lock();
//cout <<"TGo4ServerTask::LockAll before mainmutex" << endl;
TGo4LockGuard::LockMainMutex();
//cout <<"TGo4ServerTask::LockAll leaving" << endl;

}

void TGo4ServerTask::UnLockAll()
{
//cout <<"TGo4ServerTask::UnLockAll" << endl;
TGo4LockGuard::UnLockMainMutex();
//cout <<"TGo4ServerTask::UnLockAll after mainmutex" << endl;
fxTaskManager->GetMutex()->UnLock();
//cout <<"TGo4ServerTask::UnLockAll after taskmutex" << endl;
fxStatusMutex->UnLock();
//cout <<"TGo4ServerTask::UnLockAll leaving" << endl;
}

const char* TGo4ServerTask::Get_fgcLAUNCHPREFSFILE()
{
   return fgcLAUNCHPREFSFILE;
}


