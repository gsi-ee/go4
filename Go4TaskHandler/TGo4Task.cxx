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

#include "TGo4Task.h"

#include <stdarg.h>

#include "TFile.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TBufferFile.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4CommandInvoker.h"
#include "TGo4RemoteCommand.h"
#include "TGo4LocalCommandRunnable.h"
#include "TGo4ObjectQueue.h"
#include "TGo4BufferQueue.h"
#include "TGo4ThreadHandler.h"
#include "TGo4TaskStatus.h"
#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerCommandList.h"
#include "TGo4Master.h"
#include "TGo4Slave.h"

const Int_t TGo4Task::fgiTERMID=999;

TGo4Task::TGo4Task(const char *name, Bool_t blockingmode,
                               Bool_t autostart,
                               Bool_t autocreate,
                               Bool_t ismaster) :
   TGo4ThreadManager(name,blockingmode, autostart,autocreate),
   fbCommandMaster(ismaster), fxMaster(nullptr), fxSlave(nullptr), fxOwner(nullptr),
   fbWorkIsStopped(kFALSE), fxStopBuffer(nullptr), fxQuitBuffer(nullptr)
{
   fxStatusBuffer = new TBufferFile(TBuffer::kWrite);
   fxStatusMutex = new TMutex(kTRUE);
   fxStopBuffer = TGo4BufferQueue::CreateValueBuffer((UInt_t) kComCloseInput);
   fxQuitBuffer = TGo4BufferQueue::CreateValueBuffer((UInt_t) kComQuit);
   fxAbortBuffer = TGo4BufferQueue::CreateValueBuffer((UInt_t) kComAbortTask);

   TGo4CommandInvoker::Instance(); // make sure we have an invoker instance!
   TGo4CommandInvoker::SetCommandList(CreateCommandList());

   TGo4CommandInvoker::Register("NoReceiver", this); // for simple command test
   TGo4CommandInvoker::Register("Task",this); // register as command receiver at the global invoker


   // local command queue:
   fxLocalCommandQueue = new TGo4ObjectQueue("localcommands");

   // local command runnable:
   TString nomen("LocalCommandRunnable of "); nomen+=name;
   TGo4LocalCommandRunnable* commander = new TGo4LocalCommandRunnable(nomen.Data(), this);

   fxCommanderName = "COMMANDER-"; fxCommanderName += name;
   GetWorkHandler()->NewThread(GetCommanderName(), commander);
}

TGo4Task::~TGo4Task()
{
   if(fxOwner) {
      fxOwner->SetTask(nullptr, kFALSE); // on termination from threadmanager, we take over responsibility for cleanup
      delete fxOwner;
   }
   delete fxLocalCommandQueue;
   delete fxCommandPrototype;
   delete fxQuitBuffer;
   delete fxStopBuffer;
   delete fxStatusBuffer;
   delete fxStatusMutex;
   TGo4CommandInvoker::UnRegister(this);
}

void TGo4Task::SetMaster(TGo4Master *m)
{
   if(m) SetMaster(kTRUE);
   fxMaster=m;
}

void TGo4Task::SetSlave(TGo4Slave *s)
{
   if(s) SetMaster(kFALSE);
   fxSlave=s;
}

void TGo4Task::SetOwner(TGo4TaskOwner* owner)
{
   fxOwner=owner;
   SetMaster(dynamic_cast<TGo4Master *>(owner));
   SetSlave(dynamic_cast<TGo4Slave *>(owner));
}

void TGo4Task::Start()
{
   if(fxSlave) fxSlave->Start();
}

void TGo4Task::Stop()
{
   if(fxSlave) fxSlave->Stop();
}

void TGo4Task::Quit()
{
   if(fxSlave) fxSlave->Quit();
}

void TGo4Task::KillMain()
{
   if(fxSlave) fxSlave->KillMain();

}
void TGo4Task::RestartMain()
{
   if(fxSlave) fxSlave->RestartMain();
}

void TGo4Task::Terminate (Bool_t termapp)
{
   if(fxSlave)
      fxSlave->Terminate(termapp);
   else
      TGo4ThreadManager::Terminate(termapp);
}

void TGo4Task::TerminateFast()
{
   if(fxSlave)
      fxSlave->TerminateFast();
   else
      TGo4ThreadManager::TerminateFast();
}

void TGo4Task::ExecuteString(const char *command)
{
   if(fxSlave)
      fxSlave->ExecuteString(command);
   else
      gROOT->ProcessLineSync(command);
}

TGo4TaskHandlerCommandList *TGo4Task::GetPrototype()
{
   // keep this method for compatibility reasons, user should not need access to list
   return fxCommandPrototype;
}

TGo4Status *TGo4Task::NextStatus(Bool_t wait)
{
   if (!IsMaster())
      return nullptr;
   TObject *obj = nullptr;
   TGo4Status *stat = nullptr;
   TGo4BufferQueue *statqueue = dynamic_cast<TGo4BufferQueue *>(GetStatusQueue());
   if (statqueue) {
      // polling mode for timer: we do not go into condition wait!
      if (!wait && statqueue->IsEmpty())
         return nullptr;
      obj = statqueue->WaitObjectFromBuffer();
      if (obj) {
         if (obj->InheritsFrom(TGo4Status::Class())) {
            stat = dynamic_cast<TGo4Status *>(obj);
         } else {
            TGo4Log::Debug(" !!! Master Task: NextStatus ERROR, unknown object %s from status queue!!! ",
                           obj->GetName());
            delete obj;
         }
      } else {
         TGo4Log::Debug(" !!! Master Task NextStatus ERROR -- NULL object from data queue!!! ");
      }
   } else {
      stat = nullptr;
   }
   return stat;
}

TObject *TGo4Task::NextObject(Bool_t wait)
{
   if (!IsMaster())
      return nullptr;
   TObject *obj = nullptr;
   TGo4BufferQueue *dataqueue = dynamic_cast<TGo4BufferQueue *>(GetDataQueue());
   if (dataqueue) {
      if (!wait && dataqueue->IsEmpty())
         return nullptr;                       // polling mode for timer: we do not go into condition wait!
      obj = dataqueue->WaitObjectFromBuffer(); // wait for buffer and stream object

   } else {
      obj = nullptr;
   }
   return obj;
}

void TGo4Task::AddUserCommand(TGo4Command *com)
{
   fxCommandPrototype->AddCommand(com);
}

void TGo4Task::AddUserCommandList(TGo4CommandProtoList *comlist)
{
   if(comlist)
   {
      *fxCommandPrototype += *comlist;
      // operator+= of TGo4CommandProtolist puts new commands into old list
      delete comlist;
      comlist = nullptr;
   }
}

void TGo4Task::SendObject(TObject *obj, const char *receiver)
{
   if(IsMaster()) return;
   if(obj) {
      // object exists, put it into data queue
      TGo4BufferQueue * dataq=GetDataQueue(receiver);
      if(dataq)
      {
         //               TGo4Log::Debug(" Task - sending object: %s ",obj->GetName());
         //SendStatusMessage(1, kTRUE,"Task - sending object: %s ",obj->GetName());
         dataq->AddBufferFromObject(obj);
      }
      else
      {
         TGo4Log::Debug(" !!! Task - ERROR sending object - no data queue !!! ");
      }
   }
   else
   {
      // object not found, send error message through status
      SendStatusMessage(2, kTRUE, "Task - object not found");
   }
}

void TGo4Task::SendStatus(TGo4Status *stat, const char *receiver)
{
   if(IsMaster()) return;
   if(stat) {
      // object exists, put it into status queue
      TGo4BufferQueue * statq=GetStatusQueue(receiver);
      if(statq) {
         TGo4Log::Debug(" Task - sending status %s ", stat->ClassName());
         statq->AddBufferFromObject(stat);
      } else {
         TGo4Log::Debug(" !!! Task - ERROR sending status: no status queue !!! ");
      }
   } else {
      // TGo4Log::Debug(" !!! Task - ERROR sending status: no such object!!! ");
   }
}

void TGo4Task::SendStatusBuffer()
{
   if(IsMaster()) return;
   TGo4LockGuard statguard(fxStatusMutex); // do not send during buffer update
   TGo4Log::Debug(" Task - sending status buffer ");
   TGo4BufferQueue * statq=GetStatusQueue();
   if(statq) statq->AddBuffer(fxStatusBuffer,kTRUE);
}

void TGo4Task::SendStatusMessage(Int_t level, Bool_t printout, const char *text,...)
{
   if(IsMaster()) return;
   Int_t lbuflen=256;
   // put potential printf arguments in text:
   char txtbuf[256];
   va_list args;
   va_start(args, text);
   vsnprintf(txtbuf, lbuflen, text, args);
   va_end(args);
   // figure out here possible destination for message in string:
   const char *dest;
   char *curs = txtbuf;
   TString receiver = txtbuf;
   Ssiz_t pos = receiver.Index("::",2,0,TString::kExact);
   if(pos != kNPOS) {
      // before this we have receiver:
      receiver.Resize(pos);
      dest = receiver.Data();
      curs += ((size_t) pos);
      curs +=2; // skip separator
   } else {
      dest = nullptr;
   }
   Bool_t previousmode = TGo4Log::IsOutputEnabled();
   TGo4Log::OutputEnable(printout); // override the messaging state
   const char *go4mess = TGo4Log::Message(level, "%s", curs);
   TGo4Log::OutputEnable(previousmode); // restore old state of messaging
   if((level > 0) && go4mess)  {
      // do not send debug-level output to gui, and do not send suppressed messages as empty string!
      auto message = new TGo4Status(go4mess);
      SendStatus(message, dest);
      delete message;
   }
}

void TGo4Task::UpdateStatusBuffer()
{
   if(IsMaster()) return;
   TGo4LockGuard statguard(fxStatusMutex); // do not update during sending
   TGo4LockGuard main; // protect root streaming
   TFile *filsav = gFile;
   gFile = nullptr;

   TGo4TaskStatus *state = fxSlave ? fxSlave->CreateStatus() : CreateStatus();

   fxStatusBuffer->Reset();
   fxStatusBuffer->InitMap();
   fxStatusBuffer->WriteObject(state);
   gFile = filsav;
   delete state; // avoid memory leak!!
}

TGo4Command *TGo4Task::NextCommand()
{
   if(IsMaster()) return nullptr;
   TGo4BufferQueue *comq = GetCommandQueue();
   if(!comq) return nullptr;

   TGo4Command *com = nullptr;
   if(!comq->IsEmpty() || (fxSlave && !fxSlave->MainIsRunning() ) )
   {
      // put new command out of queue
      // or wait for command if analysis is stopped
      TObject *obj = comq->WaitObjectFromBuffer();
      if(obj)
      {
         if(obj->InheritsFrom(TGo4Command::Class()))
         {
            com = dynamic_cast<TGo4Command *>(obj);
            com->SetTaskName("current");
            com->SetMode(kGo4ComModeController);
         }
         else
         {
            delete obj;
         }
      }
      else
      {
         //TGo4Log::Debug(" !!! Slave ERROR -- NULL object from command queue!!! ");
      }
   }
   else //if(!fxCommandQ->IsEmpty() || !AnalysisIsRunning())
   {
      com = nullptr;
   }
   return com;
}

Int_t TGo4Task::Initialization()
{
   // this method will be called by the application control timer every timerperiod
   Int_t rev = -1;
   if(fbInitDone)
      // already initialized
   {
      rev = 0;
   } //// if(fbInitDone)
   else
   {
      if(!fxCommandPrototype)
      {
         if(fxMaster)
         {
            fxCommandPrototype = fxMaster->CreateCommandList(); // use factory method
            TGo4Log::Debug(" Task --  command list is created from Master factory");
         }
         else
         {
            fxCommandPrototype = CreateCommandList();
            TGo4Log::Debug(" Task --  command list is created from Task factory");
         }
      }
      rev = TGo4ThreadManager::Initialization(); // this will launch threads, etc.
      fxWorkHandler->Start(GetCommanderName()); // for non autostart mode
      if(fxSlave) fxSlave->Initialization();
   }// else if(fbInitDone)
   return rev;
}

void TGo4Task::UpdateStatus(TGo4TaskStatus *state)
{
   TGo4TaskHandlerStatus *taskhandlerstatus = nullptr;
   TGo4TaskHandler *th = GetTaskHandler();
   if(th) taskhandlerstatus = th->CreateStatus();
   state->SetTaskHandlerStatus(taskhandlerstatus);
   state->SetFlags(fbAppBlocking, fbAutoCreate, fbAutoStart, fbTerminating, fbInitDone);
}

TGo4TaskStatus *TGo4Task::CreateStatus()
{
   TGo4TaskStatus *stat = new TGo4TaskStatus(GetName());
   UpdateStatus(stat); // set the internals
   return stat;
}

Bool_t TGo4Task::SubmitCommand(const char *name)
{
   if(!strcmp(name,"THEMQuit"))
   {
      return SubmitEmergencyCommand(kComQuit);
   }
   else if (!strcmp(name,"THEMKill"))
   {
      return SubmitEmergencyCommand(kComKillMain);
   }
   else if (!strcmp(name,"THEMRestart"))
   {
      return SubmitEmergencyCommand(kComRestartMain);
   }

   TGo4Command *com = MakeCommand(name);
   if (!com) { // only encapsulate commands that are not known here
      // try simple command with remote command envelope:
      TGo4LockGuard mainlock;
      com = new TGo4RemoteCommand(name);
   }
   return SubmitCommand(com);
}

Bool_t TGo4Task::SubmitEmergencyCommand(Go4EmergencyCommand_t val)
{
   TGo4BufferQueue *queue = GetCommandQueue();
   if(queue)
   {
      // we have an active command queue...
      if(val==kComQuit)
      {
         // quit command might be send from thread. use preallocated buffer!
         queue->AddBuffer(fxQuitBuffer,kTRUE);
      }
      else
      {
         TBuffer *commandbuffer = TGo4BufferQueue::CreateValueBuffer((UInt_t) val);
         queue->AddBuffer(commandbuffer); // put command into queue
      }

      return kTRUE;
   }
   return kFALSE;
}

Bool_t TGo4Task::SubmitEmergencyData(Go4EmergencyCommand_t val, const char *receiver)
{
   TGo4BufferQueue *queue=GetDataQueue(receiver);
   if(queue)
   {
      // we have an active data queue...
      if(val==kComQuit)
      {
         // quit data is send from thread. use preallocated buffer!
         queue->AddBuffer(fxQuitBuffer,kTRUE);
      }
      else
      {
         TBuffer *commandbuffer=TGo4BufferQueue::CreateValueBuffer((UInt_t) val);
         queue->AddBuffer(commandbuffer); // put command into queue
      }
      return kTRUE;
   }
   return kFALSE;
}

Bool_t TGo4Task::SubmitCommand(TGo4Command *com)
{
   if (!com) return kFALSE;

   Bool_t rev=kTRUE;
   if(com->IsLocal())
      SubmitLocalCommand(com);
   else {
      // command for remote client, put into actual client queue
      TGo4BufferQueue *queue=GetCommandQueue();
      if(queue) {
         // we have an active command queue...
         TGo4LockGuard mainlock; // protect the streamer!
         queue->AddBufferFromObject(com); // put command into queue
      } else
         rev = kFALSE;
      delete com; // buffer queue does not adopt com, we delete it
   }
   return rev;
}

TGo4TaskHandlerCommandList* TGo4Task::CreateCommandList()
{
   return new TGo4TaskHandlerCommandList("Go4ServerTaskDefaultCommandList");
}

TGo4Command *TGo4Task::MakeCommand(const char *name)
{
   TGo4LockGuard mainlock;
   return fxCommandPrototype->MakeCommand(name);
}

Bool_t TGo4Task::SubmitLocalCommand(TGo4Command *com)
{
   if(!com) return kFALSE;
   com->SetMode(kGo4ComModeAdministrator); // everything is allowed here...
   fxWorkHandler->Start(GetCommanderName()); // for non autostart mode
   TGo4ObjectQueue* lqueue = GetLocalCommandQueue();
   if(!lqueue) {
      delete com;
      return kFALSE;
   }
   lqueue->AddObject(com); // object queue adopts command
   return kTRUE;
}

void TGo4Task::WakeCommandQueue(Int_t id)
{
   TGo4TaskHandler *th = GetTaskHandler();
   if(th && th->IsAborting()) return;

   // put dummy buffer to command queue. This will wake up the main thread from command wait.
   TGo4Command *com = new TGo4Command("dummy","this wakes up queue",id);
   SubmitCommand(com); // wake up main command queue (to taskhandler)
   com = new TGo4Command("dummy","this wakes up queue",id);
   SubmitLocalCommand(com); // wake up local command queue

   // note: command is owned by submit command after submit!
}

void TGo4Task::GetStatus()
{
   TGo4Log::Debug(" Task ''%s'' Send Status to Command Master ",GetName());
   TGo4BufferQueue *queue = GetStatusQueue();
   if(!queue) return;

   TGo4LockGuard mainguard;
   //         std::cout << "Mainlock acquired by clienttask: GetStatus"<< std::endl;
   TGo4TaskStatus *state = CreateStatus();
   queue->AddBufferFromObject(state);
}

Int_t TGo4Task::StartWorkThreads()
{
   fbWorkIsStopped = kFALSE;
   return fxOwner ? fxOwner->StartWorkThreads() : 0;
}

Int_t TGo4Task::StopWorkThreads()
{
   fbWorkIsStopped=kTRUE;
   return fxOwner ? fxOwner->StopWorkThreads() : 0;
}

void TGo4Task::SendStopBuffers(const char *taskname)
{
   TGo4TaskHandler *th = GetTaskHandler();
   if(!th || th->IsAborting()) return;

   if(IsMaster())
   {
      TGo4BufferQueue * comq=GetCommandQueue(taskname);
      if(comq)
      {
         comq->AddBuffer(fxStopBuffer,kTRUE);
      }
   }
   else
   {
      TGo4BufferQueue * dataq=GetDataQueue(taskname);
      if(dataq)
      {
         dataq->AddBuffer(fxStopBuffer,kTRUE);
      }
      TGo4BufferQueue * statq=GetStatusQueue(taskname);
      if(statq)
      {
         statq->AddBuffer(fxStopBuffer,kTRUE);
      }
   }//if(IsMaster())
}

Int_t TGo4Task::Get_fgiTERMID()
{
   return fgiTERMID;
}

