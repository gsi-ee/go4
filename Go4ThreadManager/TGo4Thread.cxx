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

#include "TGo4Thread.h"

#include "TThread.h"
#include "TSystem.h"
#include "TCondition.h"

#include "TGo4Runnable.h"
#include "TGo4Log.h"
#include "Go4Exceptions.h"

TGo4Thread::TGo4Thread(const TGo4Thread &right)
   :TNamed(right)
{
   GO4TRACE((14,"TGo4Thread::TGo4Thread(const TGo4Thread &)",__LINE__, __FILE__));
   fxRunnable = right.fxRunnable;
   fbIsInternal = right.fbIsInternal;
   fxCondition = new TCondition();
}

TGo4Thread::TGo4Thread (const char *name, TGo4Runnable *runnable, Bool_t internal)
   :TNamed(name,"This is a TGo4Thread"),
   fbIsCreated(kFALSE),
   fbIsRunning(kFALSE),
   fiThreadPID(0),
   fiThreadSelfID(0),
   fbIsWaiting(kFALSE),
   fxThread(nullptr)
{
   GO4TRACE((14,"TGo4Thread::TGo4Thread(const char *, TGo4Runnable *, Bool_t)",__LINE__, __FILE__));

   TGo4Log::Debug(" New Go4Thread ``%s'' created ",name);
   fxRunnable = runnable;
   fbIsInternal = internal;
   fxCondition = new TCondition();
   fxRunnable->SetThread(this);
}


TGo4Thread::~TGo4Thread()
{
   GO4TRACE((14,"TGo4Thread::~TGo4Thread()",__LINE__, __FILE__));

   Stop();
   Cancel();
   delete fxCondition;
   fxCondition = nullptr;
   if (fbIsInternal) {
      GO4TRACE((14, "TGo4Thread::~TGo4Thread() internal mode", __LINE__, __FILE__));
      TGo4Log::Debug(" Go4Thread ``%s'' deleting runnable ``%s'' ", GetName(), fxRunnable->GetName());
      delete fxRunnable; // internal mode: go4thread responsible for runnable
   } else {
      GO4TRACE((14, "TGo4Thread::~TGo4Thread() non internal mode", __LINE__, __FILE__));
      // do nothing
   }
}

void TGo4Thread::Threadfunc(void *arg)
{
   GO4TRACE((2,"TGo4Thread::Threadfunc(void *)",__LINE__, __FILE__));
   TGo4Thread *go4th = (TGo4Thread *) arg; // we need reference to calling class instance
   TGo4Runnable *runnable = go4th->GetRunnable();

  // this is the function running as pthread...
  // initializations:
   go4th->SetPID();
   go4th->SetSelfID();
//   fiThreadPID=gSystem->GetPid(); // set ids
//   fiThreadSelfID=TThread::SelfId();
   TGo4Log::Debug(" TThread %ld (PID:%d) of Go4Thread ``%s''  started... \n",
            go4th->GetSelfID(), go4th->GetPID(), go4th->GetName());

for(;;) // loop keeps thread alive after exception has occurred...
   {
   try
      {
      GO4TRACE((1,"TGo4Thread::Threadfunc(void *) try block",__LINE__, __FILE__));
      while(1)
         {
            TThread::CancelPoint();
            if( !(go4th->IsRunning()) )
               {
                  GO4TRACE((1,"TGo4Thread::Threadfunc(void *) waiting mode",__LINE__, __FILE__));
                  TGo4Log::Debug(" Go4Thread ``%s'' (PID:%d) waiting...\n",
                           go4th->GetName(), go4th->GetPID());
                   go4th->SetWaiting(kTRUE);
                   go4th->GetCondition()->Wait(); //wait for funcrun condition
                   go4th->SetWaiting(kFALSE);
                   TGo4Log::Debug(" Go4Thread ``%s'' (PID:%d) running...\n",
                           go4th->GetName(), go4th->GetPID() );
                }
            else
               {
               // Do Nothing, just continue
               }

            if(go4th->IsCreated())
               // normal  mode: enter runnable
               {
               GO4TRACE((1,"TGo4Thread::Threadfunc(void *) entering running mode",__LINE__, __FILE__));
                  // call runnable prerun method before running:
               runnable->PreRun((void *) 0); // we are runnable's friend, can call protected method...
               while(go4th->IsRunning())
                  {
                     TThread::CancelPoint();
                     runnable->Run((void *) 0); // we are runnable's friend, can call protected method...
                  }
                   // call runnable postrun method before stopping
               runnable->PostRun((void *) 0); // we are runnable's friend, can call protected method...

               }
            else
               // aborting mode after condition release: loop cancel point
               {
                  GO4TRACE((1,"TGo4Thread::Threadfunc(void *) entering Cancel loop",__LINE__, __FILE__));
                  TGo4Log::Debug(" Go4Thread ``%s''  entering Cancel loop\n ", go4th->GetName());
                  while(!(go4th->IsCreated()))
                     {
                        {
                        #ifdef _MSC_VER
                           return; // cancel point does not work on windows; escape to root thread framework...
                        #else
                           GO4TRACE((1,"TGo4Thread::Threadfunc(void *) in Cancel loop",__LINE__, __FILE__));
                           Sleep(500);
                           TThread::CancelPoint();
                        #endif
                        }
                     }
               }
        } // while(1)
    }// try
  catch(TGo4Exception &ex)
    {
      GO4TRACE((1,"TGo4Thread::Threadfunc(void *) Go4Exception Catch",__LINE__, __FILE__));
      runnable->ThreadCatch(ex);
      TThread::CancelPoint();
    }

  catch(...)
    {
      GO4TRACE((1,"TGo4Thread::Threadfunc(void *) Unexpected Catch",__LINE__, __FILE__));
      //runnable->UnexpectedCatch(); // do not handle alien exceptions!
      TThread::CancelPoint();
      throw; // otherwise, we have trouble with newer pthread
             // implementations that use one pid for all threads, e.g. Fedora2
    }

   } // for(;;) loop keeps thread alive
}

Bool_t TGo4Thread::Create ()
{
   GO4TRACE((14,"TGo4Thread::Create()",__LINE__, __FILE__));
   // thread instance already there...
   if(fbIsCreated)
      return kFALSE;

   fxThread= new TThread(GetName(),
                         (void(*) (void *)) &Threadfunc ,
                         (void *) this);
   fxThread->Run();
   fbIsCreated = kTRUE;
   return kTRUE;
}

Bool_t TGo4Thread::Cancel ()
{
    GO4TRACE((14,"TGo4Thread::Cancel()",__LINE__, __FILE__));

    if(fbIsCreated) {
      // thread existing, then cancel
      TGo4Log::Debug(" Go4Thread ``%s'' --  Canceling TThread %ld (PID:%d) ",
               GetName(), fiThreadSelfID, fiThreadPID);
      if(fxThread) {
         GO4TRACE((13,"TGo4Thread::Cancel() -- canceling existing TThread",__LINE__, __FILE__));
         fbIsCreated = kFALSE;
         fbIsRunning = kTRUE; // these settings let Threadfunc enter Cancel loop
         fxCondition->Signal();  // unlock condition mutex before deleting it
         Sleep(500); // waitstate for Threadfunc to enter cancel loop after condition

         // JAM2018 bugfix for crashes with ROOT 6 concerning Thread CleanUpPop
         TThread::Delete(fxThread);

         fxThread = nullptr;
         fiThreadPID = 0;
         fiThreadSelfID = 0;
      } else {
         GO4TRACE((13,"TGo4Thread::Cancel() -- Error: TThread pointer is zero!",__LINE__, __FILE__));
         TGo4Log::Debug(" Go4Thread ``%s'' Cancel -- Internal inconsistency error! ",GetName());
         throw TGo4RuntimeException();
      }
      return kTRUE;
  }
  // no such thread...
  return kFALSE;
}

Bool_t TGo4Thread::ReCreate ()
{
   GO4TRACE((14,"TGo4Thread::ReCreate()",__LINE__, __FILE__));

   TThread* oldthread;
   if(fbIsCreated)
    // thread existing, then recreate possible

    {
    GO4TRACE((13,"TGo4Thread::ReCreate() -- old TThread existing",__LINE__, __FILE__));
    TGo4Log::Debug(" Recreating Go4Thread ``%s'' --  old TThread %ld (PID:%d) ",
               GetName(), fiThreadSelfID, fiThreadPID);
      if(fxThread)
         {
            GO4TRACE((13,"TGo4Thread::ReCreate() -- recreating existing TThread",__LINE__, __FILE__));
            Stop();// halt runnable
            oldthread=fxThread; // remember old TThread
            // first start new TThread of same Threadfunc and runnable:
            fxThread= new TThread(GetName(),
                               (void(*) (void *)) &Threadfunc ,
                               (void *) this);

            // then kill the old TThread, so Threadfunc may ReCreate() itself
            fbIsCreated=kFALSE;
            if(fbIsWaiting)
            {
               fxCondition->Signal();  // unlock condition mutex before deleting it
                                       // this will not start the runnable, fbIsRunning still false
               //fbIsWaiting=kFALSE;    // keep flag consistent with Threadfunc state..
            }
            TThread::Delete(oldthread);
            //delete oldthread;
            fbIsCreated=kTRUE;
            fxThread->Run(); // does this work if thread recreates itself?
            Start(); // start runnable in new TThread

         }
      else
         {
            GO4TRACE((13,"TGo4Thread::ReCreate() -- Error: old TThread pointer is zero!",__LINE__, __FILE__));
            TGo4Log::Debug(" Go4Thread ``%s'' ReCreate -- Internal inconsistency error! ",GetName());
            throw TGo4RuntimeException();
         }
      return kTRUE;
    }
  else //if(fbIsCreated)
    // no old thread, then create new one
    {
      GO4TRACE((13,"TGo4Thread::ReCreate() -- old TThread existing",__LINE__, __FILE__));
      Create();
      return kFALSE;
    }
}

Bool_t TGo4Thread::Start ()
{
   GO4TRACE((12,"TGo4Thread::Start()",__LINE__, __FILE__));
   if (!fbIsCreated) {
      // check if thread is up
      // not yet created, then do so
      GO4TRACE((11,"TGo4Thread::Start() -- creating new TThread",__LINE__, __FILE__));
      Create();
   } else {
      // do nothing, thread already there
      GO4TRACE((11,"TGo4Thread::Start() -- TThread already existing",__LINE__, __FILE__));
   }
   Bool_t old = fbIsRunning;
   fbIsRunning = kTRUE;
   fxCondition->Signal(); // wake up waiting threadfunction
   return old; // old status of workfunc requested by thread list
}

Bool_t TGo4Thread::Stop ()
{
   GO4TRACE((12,"TGo4Thread::Stop()",__LINE__, __FILE__));
   Bool_t old = fbIsRunning;
   fbIsRunning = kFALSE;
   return old; // old status of workfunc requested by thread list
}

void TGo4Thread::Sleep (UInt_t millisecs)
{
   GO4TRACE((12,"TGo4Thread::Sleep()",__LINE__, __FILE__));
   gSystem->Sleep(millisecs);
   TThread::CancelPoint();
}

void TGo4Thread::SetSelfID()
{
   fiThreadSelfID = TThread::SelfId();
}

void TGo4Thread::SetPID()
{
   fiThreadPID = gSystem->GetPid();
}
