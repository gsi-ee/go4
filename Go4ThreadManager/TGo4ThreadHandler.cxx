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

#include "TGo4ThreadHandler.h"

#include "TGo4Thread.h"
#include "TIterator.h"
#include "TObjArray.h"
#include "TMutex.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4ThreadManager.h"

TGo4ThreadHandler::TGo4ThreadHandler(const TGo4ThreadHandler &right)
  : TNamed(right)
{
   GO4TRACE((15,"TGo4ThreadHandler::TGo4ThreadHandler() copy constructor",__LINE__, __FILE__));
   fxManager = right.fxManager;
   fxListMutex=new TMutex;
   fxOperMutex=new TMutex;
   fxArray= (TObjArray*) ((right.fxArray)->Clone());
   fxIterator=fxArray->MakeIterator();
}

TGo4ThreadHandler::TGo4ThreadHandler (const char* name, TGo4ThreadManager* parent)
   :TNamed(name,"This is a TGo4ThreadHandler"),fbIsOperating(kFALSE)
{
   GO4TRACE((15,"TGo4ThreadHandler::TGo4ThreadHandler(const char*, TGo4ThreadManager*) constructor",__LINE__, __FILE__));
   fxManager=parent;
   fxListMutex=new TMutex;
   fxOperMutex=new TMutex;
   fxArray=new TObjArray;
   fxIterator=fxArray->MakeIterator();
}


TGo4ThreadHandler::~TGo4ThreadHandler()
{
   GO4TRACE((15,"TGo4ThreadHandler::~TGo4ThreadHandler() destructor",__LINE__, __FILE__));
   StopAll();
   CancelAll();
   TGo4Thread* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);
      fxIterator->Reset();
      while((th= (TGo4Thread*) fxIterator->Next())!=0)
         {
            if(th->IsInternal())
               // internal mode? then delete thread object
               {
                  GO4TRACE((14,"TGo4ThreadHandler::~TGo4ThreadHandler() Removing internal mode thread",__LINE__, __FILE__));
                  fxArray->Remove(th);
                  fxArray->Compress();
                  fxArray->Expand(fxArray->GetLast()+1);
                  delete th;
               }
            else
               {
                  // do nothing
                  GO4TRACE((14,"TGo4ThreadHandler::~TGo4ThreadHandler() Non internal mode thread",__LINE__, __FILE__));
               }
         }// while(th=...)
      delete fxIterator;
      fxArray->Clear(); // remove objects from array, but do not delete them
      delete fxArray;
   }
   delete fxListMutex;
   delete fxOperMutex;
}


Bool_t TGo4ThreadHandler::AddThread (TGo4Thread* gthr)
{
   GO4TRACE((14,"TGo4ThreadHandler::AddThread(TGo4Thread*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
      {
      TGo4LockGuard listguard(fxListMutex);
         if(fxArray->FindObject(gthr)==0)
         // is thread already in list?
            {
               //no, add new thread
               GO4TRACE((13,"TGo4ThreadHandler::AddThread(TGo4Thread*) Adding new go4 thread to array",__LINE__, __FILE__));
               fxArray->AddLast(gthr);
               rev=kTRUE;
            }
         else
            {
               // yes, do nothing
               GO4TRACE((13,"TGo4ThreadHandler::AddThread(TGo4Thread*) Thread was already in array",__LINE__, __FILE__));
               rev=kFALSE;
            }
      }
   return rev;
}

Bool_t TGo4ThreadHandler::RemoveThread (const char* name)
{
   GO4TRACE((14,"TGo4ThreadHandler::RemoveThread(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   {
      TGo4LockGuard listguard(fxListMutex);
      TObject* obj=fxArray->FindObject(name);
      TGo4Thread* th= (TGo4Thread*) obj;
      if(obj!=0)
         // thread found, then remove it
         {
            GO4TRACE((13,"TGo4ThreadHandler::RemoveThread(const char*) Removing thread from array",__LINE__, __FILE__));
           fxArray->Remove(obj);
            fxArray->Compress();
            fxArray->Expand(fxArray->GetLast()+1);
            if(th->IsInternal())
               // internal mode? then delete thread
               {
               GO4TRACE((13,"TGo4ThreadHandler::RemoveThread(const char*) Deleting internal mode thread",__LINE__, __FILE__));
               TGo4LockGuard operguard(fxOperMutex); // protect operating flag
                  fbIsOperating=kTRUE;
                  fxManager->UnBlockApp(); // tell blocking timer to enable system
                  delete th;
                  fbIsOperating=kFALSE;
                  fxManager->BlockApp();  // blocking again
               }
            else
               {
               // do nothing
               GO4TRACE((13,"TGo4ThreadHandler::RemoveThread(const char*) Non internal mode thread is not deleted",__LINE__, __FILE__));
               }
            rev=kTRUE;
         }
      else
         {
            GO4TRACE((13,"TGo4ThreadHandler::RemoveThread(const char*) Thread not found in array",__LINE__, __FILE__));
            rev=kFALSE;
         }
   }
   return rev;
}

Bool_t TGo4ThreadHandler::NewThread(const char* name, TGo4Runnable* runnable)
{
   GO4TRACE((14,"TGo4ThreadHandler::NewThread(const char*,TGo4Runnable*)",__LINE__, __FILE__));
   TGo4Runnable* nrun=0;
   TGo4Thread* nthread=0;
   if(runnable==0)
      {
         GO4TRACE((13,"TGo4ThreadHandler::NewThread(const char*,TGo4Runnable*) No runnable specified error",__LINE__, __FILE__));
         return kFALSE;
         //throw TGo4RuntimeException();
      }
  else
     {
        GO4TRACE((13,"TGo4ThreadHandler::NewThread(const char*,TGo4Runnable*) Assigning external runnable to new internal thread",__LINE__, __FILE__));
        nrun=runnable;
     }
  nthread= new TGo4Thread(name,nrun,kTRUE);
  return AddThread(nthread);
}

Int_t TGo4ThreadHandler::CreateAll ()
{
   GO4TRACE((15,"TGo4ThreadHandler::CreateAll()",__LINE__, __FILE__));
   Int_t createdthreads=0; // return value: number of creation successes
   TGo4Thread* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);  // protect thread list array
   TGo4LockGuard operguard(fxOperMutex); // protect operating flag
      fbIsOperating=kTRUE;
      fxManager->UnBlockApp(); // tell blocking timer to enable system
      fxIterator->Reset();
      while((th= (TGo4Thread*) fxIterator->Next())!=0)
         {
            if(th->Create())
               {
                  GO4TRACE((13,"TGo4ThreadHandler::CreateAll() Thread creation success",__LINE__, __FILE__));
                  ++createdthreads; // increment success counter
               }
            else
               {
                  GO4TRACE((13,"TGo4ThreadHandler::CreateAll() Thread not created",__LINE__, __FILE__));
                  // thread was already up, no new thread created
               }
         }
      fxManager->BlockApp(); // tell blocking timer to enable system
      fbIsOperating=kFALSE;
   }
   return createdthreads;
}

Bool_t TGo4ThreadHandler::Create (const char* thname)
{
   GO4TRACE((14,"TGo4ThreadHandler::Create(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   TGo4Thread* th=GetThread(thname);
   if(th!=0)
     // thread was found in list
      {
      GO4TRACE((13,"TGo4ThreadHandler::Create(const char*) Creating new TThread for Go4Thread",__LINE__, __FILE__));
      TGo4LockGuard operguard(fxOperMutex); // protect operating flag
         fbIsOperating=kTRUE;
         fxManager->UnBlockApp(); // tell blocking timer to enable system
         rev=th->Create();
         fbIsOperating=kFALSE;
         fxManager->BlockApp(); // block again afterwards
      }
   else
      // no such thread
      {
      GO4TRACE((13,"TGo4ThreadHandler::Create(const char*) Go4Thread was not found in thread array!",__LINE__, __FILE__));
        rev=kFALSE;
      }
   return rev;
}

Int_t TGo4ThreadHandler::CancelAll ()
{
   GO4TRACE((15,"TGo4ThreadHandler::CancelAll()",__LINE__, __FILE__));
   Int_t cancelledthreads=0; // return value: number of successes
   TGo4Thread* th=0;
      {
      TGo4LockGuard listguard(fxListMutex);
      TGo4LockGuard operguard(fxOperMutex); // protect operating flag
         fbIsOperating=kTRUE;
         fxIterator->Reset();
         while((th= (TGo4Thread*) fxIterator->Next())!=0)
            {
               if(th->Cancel())
                  {
                     GO4TRACE((13,"TGo4ThreadHandler::CancelAll() Thread Cancel success",__LINE__, __FILE__));
                     ++cancelledthreads; // increment success counter
                  }
               else
                {
                     // thread was already down, not cancelled
                     GO4TRACE((13,"TGo4ThreadHandler::CancelAll() Thread was not canceled",__LINE__, __FILE__));

                }
            }
         fbIsOperating=kFALSE;
      }
   return cancelledthreads;
}

Bool_t TGo4ThreadHandler::Cancel (const char* thname)
{
   GO4TRACE((14,"TGo4ThreadHandler::Cancel(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   TGo4Thread* th=GetThread(thname);
   if(th!=0)
        // go4thread was found in list
      {
      GO4TRACE((13,"TGo4ThreadHandler::Cancel(const char*) Canceling TThread",__LINE__, __FILE__));
      TGo4LockGuard operguard(fxOperMutex); // protect operating flag
         fbIsOperating=kTRUE;
         fxManager->UnBlockApp(); // tell blocking timer to enable system
         rev=th->Cancel();
         fbIsOperating=kFALSE;
         fxManager->BlockApp();  // blocking again
      }
   else
      // no such go4thread
      {
         GO4TRACE((13,"TGo4ThreadHandler::Cancel(const char*) Go4Thread was not found in thread array!",__LINE__, __FILE__));
         rev=kFALSE;
      }
   return rev;
}

Int_t TGo4ThreadHandler::ReCreateAll ()
{
   GO4TRACE((15,"TGo4ThreadHandler::ReCreateAll()",__LINE__, __FILE__));
   Int_t recreatedthreads=0; // return value: number of creation successes
   TGo4Thread* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);
   TGo4LockGuard operguard(fxOperMutex); // protect operating flag
      fbIsOperating=kTRUE;
      fxIterator->Reset();
      while((th= (TGo4Thread*) fxIterator->Next())!=0)
         {
            if(th->ReCreate())
               {
                  GO4TRACE((13,"TGo4ThreadHandler::ReCreateAll() Thread ReCreate success",__LINE__, __FILE__));
                  ++recreatedthreads; // increment success counter
               }
            else
               {
                  // thread was already up, no new thread created
                  GO4TRACE((13,"TGo4ThreadHandler::ReCreateAll() Thread was not recreated",__LINE__, __FILE__));

               }
         }
      fbIsOperating=kFALSE;
   }
   return recreatedthreads;
}

Bool_t TGo4ThreadHandler::ReCreate (const char* thname)
{
   GO4TRACE((14,"TGo4ThreadHandler::ReCreate(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   TGo4Thread* th=GetThread(thname);
   if(th!=0)
      // thread was found in list
      {
      GO4TRACE((13,"TGo4ThreadHandler::ReCreate(const char*) ReCreating TThread for Go4Thread",__LINE__, __FILE__));
      TGo4LockGuard operguard(fxOperMutex); // protect operating flag
         fbIsOperating=kTRUE;
         fxManager->UnBlockApp(); // tell blocking timer to enable system
         rev=th->ReCreate();
         fbIsOperating=kFALSE;
         fxManager->BlockApp(); // block again afterwards
      }
   else
     // no such thread
      {
      GO4TRACE((13,"TGo4ThreadHandler::ReCreate(const char*) Go4Thread was not found in thread array!",__LINE__, __FILE__));
         rev=kFALSE;
      }

  return rev;
}

Int_t TGo4ThreadHandler::StartAll ()
{
   GO4TRACE((15,"TGo4ThreadHandler::StartAll()",__LINE__, __FILE__));
   Int_t startedfuncs=0; // return value: number of successes
   TGo4Thread* th=0;
      {
      TGo4LockGuard listguard(fxListMutex);
      TGo4LockGuard operguard(fxOperMutex); // protect operating flag
         fbIsOperating=kTRUE;     // Start has Create option, so we need gSystem
         fxManager->UnBlockApp(); // tell blocking timer to enable system
         fxIterator->Reset();
         while((th= (TGo4Thread*) fxIterator->Next())!=0)
            {
               if(!th->Start())
                  // StartWork returns fbFuncrun status _before_ call
                  // false: Workfunc was started from stopped status
                  {
                     // false: Workfunc was started from stopped status
                     GO4TRACE((13,"TGo4ThreadHandler::StartAll() Thread Start success",__LINE__, __FILE__));
                     ++startedfuncs; // increment success counter
                  }
               else
                  {
                     // true:
                     // function was already running before
                     GO4TRACE((13,"TGo4ThreadHandler::StartAll() Thread was already running",__LINE__, __FILE__));
                  }
            }
         fbIsOperating=kFALSE;
         fxManager->BlockApp();
      }
   return startedfuncs;
}

Bool_t TGo4ThreadHandler::Start (const char* thname)
{
   GO4TRACE((14,"TGo4ThreadHandler::Start(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   TGo4Thread* th=GetThread(thname);
   if(th!=0)
      // thread was found in list
      {
      GO4TRACE((13,"TGo4ThreadHandler::Start(const char*) Starting Thread",__LINE__, __FILE__));
      TGo4LockGuard operguard(fxOperMutex); // protect operating flag
         fbIsOperating=kTRUE;     // Start has Create option, so we need gSystem
         fxManager->UnBlockApp(); // tell blocking timer to enable system
         rev=th->Start();
         fbIsOperating=kFALSE;
         fxManager->BlockApp(); // tell blocking timer to block system
      }
   else
      // no such thread
      {
      GO4TRACE((13,"TGo4ThreadHandler::Start(const char*) Go4Thread was not found in thread array!",__LINE__, __FILE__));
         rev=kFALSE;
      }
   return rev;
}

Int_t TGo4ThreadHandler::StopAll ()
{
   GO4TRACE((15,"TGo4ThreadHandler::StopAll()",__LINE__, __FILE__));
   Int_t stoppedfuncs=0; // return value: number of successes
   TGo4Thread* th=0;
      {
      TGo4LockGuard listguard(fxListMutex);
         fxIterator->Reset();
         while((th= (TGo4Thread*) fxIterator->Next())!=0)
            {
               if(th->Stop())
                  // StopWork returns fbFuncrun status _before_ call
                  {
                     // true: Workfunc was stopped from running status
                     GO4TRACE((13,"TGo4ThreadHandler::StopAll() Go4Thread Stop success",__LINE__, __FILE__));
                     ++stoppedfuncs; // increment success counter
                  }
               else
                  {
                     // false:
                     // function was already stopped  before
                     GO4TRACE((13,"TGo4ThreadHandler::StopAll() Go4Thread was already stopped",__LINE__, __FILE__));
                  }
            }
      }
   return stoppedfuncs;
}

Bool_t TGo4ThreadHandler::Stop (const char* thname)
{
   GO4TRACE((14,"TGo4ThreadHandler::Stop(const char*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   TGo4Thread* th=GetThread(thname);
   if(th!=0)
      // thread was found in list
      {
         GO4TRACE((13,"TGo4ThreadHandler::Stop(const char*) Stopping Go4Runnable",__LINE__, __FILE__));
         rev=th->Stop();
      }
   else
      // no such thread
      {
         GO4TRACE((13,"TGo4ThreadHandler::Stop(const char*) Go4Thread was not found in thread array!",__LINE__, __FILE__));
         rev=kFALSE;
      }
   return rev;
}

Int_t TGo4ThreadHandler::DumpThreads (Int_t mode)
{
   GO4TRACE((15,"TGo4ThreadHandler::DumpThreads(Int_t)",__LINE__, __FILE__));
   if(TGo4Log::GetIgnoreLevel()>0) return 2; // only write threaddumpfile for debug mode
   Int_t retval=0;
   TGo4Thread* th=0;
   FILE* fp;
   Int_t i=0;
   char Filename[80];
   switch(mode)
      {
         case 0:
            {
            GO4TRACE((14,"TGo4ThreadHandler::DumpThreads(Int_t) mode 0: File output",__LINE__, __FILE__));
            strcpy(Filename,"threaddump.txt");
            fp = fopen(Filename,"w");
            if (!fp)
               {
               GO4TRACE((13,"TGo4ThreadHandler::DumpThreads(Int_t) fopen failed!!",__LINE__, __FILE__));
                  TGo4Log::Debug(" ThreadHandler -- Error, Could not open thread dump file!");
                  retval=1;
                  break;
               }
            else
               {
               GO4TRACE((13,"TGo4ThreadHandler::DumpThreads(Int_t) writing into opened file",__LINE__, __FILE__));
                fprintf(fp,"\nTGo4ThreadHandler thread information dump file:\n");
                     {
                     TGo4LockGuard listguard(fxListMutex);
                        fxIterator->Reset();
                        while((th= (TGo4Thread*) fxIterator->Next())!=0)
                           {
                              fprintf(fp,"TGo4Thread %d: \tPID:%d \tSelfID: %d",
                                 i++,th->GetPID(),(int) th->GetSelfID());
                              fprintf(fp,"\t name: %s\n",th->GetName());
                           }
                     }
                  if (fclose(fp))
                     {
                     GO4TRACE((13,"TGo4ThreadHandler::DumpThreads(Int_t) fclose failed!!",__LINE__, __FILE__));
                        TGo4Log::Debug(" ThreadHandlerr -- Error, Could not close thread dump file!");
                        retval=1;
                     }
               } // if(!fp)/else
            }//case 0:
            break;
      default:
         {
         GO4TRACE((14,"TGo4ThreadHandler::DumpThreads(Int_t) mode: default ",__LINE__, __FILE__));
            TGo4Log::Debug(" ThreadHandlerr -- Error: DumpThreads mode unknown ");
         }
         break;
      }// switch(mode)
   return retval;
}

TGo4Thread* TGo4ThreadHandler::GetThread (const char* name)
{
   GO4TRACE((12,"TGo4ThreadHandler::GetThread(const char*)",__LINE__, __FILE__));
   TGo4Thread* thread=0;
   {
      TGo4LockGuard listguard(fxListMutex);
      thread = (TGo4Thread*) fxArray->FindObject(name);
   }
   return thread;
}

Int_t TGo4ThreadHandler::GetEntries ()
{
   GO4TRACE((12,"TGo4ThreadHandler::GetEntries()",__LINE__, __FILE__));
   Int_t entries=0;
   {
   TGo4LockGuard listguard(fxListMutex);
      entries=fxArray->GetEntries();
   }
   return entries;
}

Bool_t TGo4ThreadHandler::AllCreated ()
{
   GO4TRACE((14,"TGo4ThreadHandler::AllCreated()",__LINE__, __FILE__));
   Bool_t rev=kTRUE; // return value: false if one thread is not there
   TGo4Thread* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);
      fxIterator->Reset();
      while((th= (TGo4Thread*) fxIterator->Next())!=0)
         {
            if((th->GetPID())==0)
               {
               GO4TRACE((11,"TGo4ThreadHandler::AllCreated() TThread is _not_ existing",__LINE__, __FILE__));
               rev=kFALSE; // this thread is not there
               break;
               }
            else
               {
               GO4TRACE((11,"TGo4ThreadHandler::AllCreated() TThread is existing",__LINE__, __FILE__));
                  // thread PID found
               }
         } // while(th=...)
   }
   return rev;
}

Bool_t TGo4ThreadHandler::AllRunning ()
{
   GO4TRACE((14,"TGo4ThreadHandler::AllRunning()",__LINE__, __FILE__));
   Bool_t rev=kTRUE; // return value: false if one thread is not there
   TGo4Thread* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);
      fxIterator->Reset();
      while((th= (TGo4Thread*) fxIterator->Next())!=0)
         {
         if(!(th->IsRunning()))
            {
            GO4TRACE((11,"TGo4ThreadHandler::AllCreated() TGo4Thread is _not_ running",__LINE__, __FILE__));
               rev=kFALSE; // this thread is not working
               break;
            }
         else
            {
            GO4TRACE((11,"TGo4ThreadHandler::AllCreated() TGo4Thread is running",__LINE__, __FILE__));
               // thread is working
            }
         }
   }
   return rev;
}

Bool_t TGo4ThreadHandler::AllWaiting ()
{
   GO4TRACE((14,"TGo4ThreadHandler::AllWaiting()",__LINE__, __FILE__));
   Bool_t rev=kTRUE; // return value: false if one runnable is still running
   TGo4Thread* th=0;
   {
   TGo4LockGuard listguard(fxListMutex);
      fxIterator->Reset();
      while((th= (TGo4Thread*) fxIterator->Next())!=0)
         {
         if(!(th->IsWaiting()))
            {
            GO4TRACE((11,"TGo4ThreadHandler::AllCreated() TGo4Thread is still running",__LINE__, __FILE__));
               //std::cout <<"-------Thread "<<th->GetName()<<" is still running..." << std::endl;
               rev=kFALSE; // this runnable is still doing
               break;
            }
         else
            {
            GO4TRACE((11,"TGo4ThreadHandler::AllCreated() TGo4Thread is waiting",__LINE__, __FILE__));
               // runnable is waiting
            }
         }
   }
   return rev;
}
