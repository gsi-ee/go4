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

#include "TGo4AnalysisMainRunnable.h"

#include "Riostream.h"
#include <stdexcept>

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Thread.h"
#include "TGo4Task.h"
#include "TGo4Command.h"

#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4CommandInvoker.h"
#include "TGo4EventErrorException.h"
#include "TGo4EventTimeoutException.h"
#include "TGo4AnalysisStepException.h"
#include "TGo4UserException.h"
#include "TGo4EventEndException.h"
#include "TGo4DynamicListException.h"

const UInt_t TGo4AnalysisMainRunnable::fguPOLLINTERVAL=1000;

TGo4AnalysisMainRunnable::TGo4AnalysisMainRunnable(const char* name, TGo4AnalysisClient* cli)
: TGo4Runnable(name,cli->GetTask()), fxAnalysisClient(cli)
{
   fxAnalysis=cli->GetAnalysis();
}

TGo4AnalysisMainRunnable::~TGo4AnalysisMainRunnable()
{

}

Int_t TGo4AnalysisMainRunnable::PostRun(void * )
{
   //cout << "test of main runnable postrun "<< endl;
   if (fxAnalysisClient && fxAnalysisClient->MainIsRunning() && !fxAnalysisClient->IsServer()) fxAnalysis->PostLoop();
      // only execute postloop here if client was quit without stopping before
      // otherwise, TGo4AnalysisClient::Stop() has already called PostLoop
      // note that server shutdown will call postloop explicitely
      // note that any client disconnecting from server should NOT call pre/postloop,
      // so server prerun/postrun have no op
   return 0;
}

Int_t TGo4AnalysisMainRunnable::PreRun(void * )
{
   TRACE((12,"TGo4AnalysisMainRunnable::PreRun()",__LINE__, __FILE__));
   //cout << "test of main runnable prerun "<< endl;
   //fxAnalysis->PreLoop();
      // this thread will never be stopped during analysis lifetime!
      // preloop execution is obsolete here now, because AnalysisClient::Start will do that for us!
   return 0;
}

Int_t TGo4AnalysisMainRunnable::Run(void*)
{
   //TRACE((12,"TGo4AnalysisMainRunnable::Run()",__LINE__, __FILE__));
   //
try
   {
   TGo4Command* com=fxAnalysisClient->NextCommand();
   //if(com== (TGo4Command*) -1) return 0; // for command memleak debug: no execute, no mainloop
   if(com)
     {
         if( com->GetCommandID() != TGo4Task::Get_fgiTERMID() )
            {
                // normal operation if we have not a terminate dummy command
               ////////////
               //// note JA: mutex order needs to be preserved in any thread:
               //// statusmutex,taskmanagermutex,queuemutex(locally in each queue),   mainmutex(root streamer etc.)
               TMutex* smutex=fxAnalysisClient->GetTask()->GetStatusBufferMutex();
               TGo4LockGuard buflock(smutex); // protect deadlocking status buffer
               TMutex* tmutex=fxAnalysisClient->GetTaskManagerMutex();
               TGo4LockGuard tasklock(tmutex); //  protect deadlocking taskmanger mutex, if we are server task
                  // if tmutex or smutex==0, these will be just a dummy mainmutex
               TGo4LockGuard mainlock; // protect command invocation!
               /////////////
                  TGo4CommandInvoker::Instance()->Invoke(com); // execute command by invoker
                  delete com;
            } // end mutexes scope
         else
            {
               // terminate dummy command: do not execute, but stop this thread
               //cout <<"Analysis main runnable got termid command" << endl;
               GetThread()->Stop();
            }

     }
   else  // if(com)
     {
        // zero object means proceed with analysis...
      // for analysis as server, we have to check running state again
      // (no command queue means no wait for next command)
      if(fxAnalysisClient->MainIsRunning())
         {
            //TGo4LockGuard mainguard; // global lock main actions inside MainCycle now
              if(!fxAnalysisClient->IsCintMode())
                 fxAnalysis->MainCycle();
         }
      else
         {
            //cout <<"main runnable: analysis is not running" << endl;
            TGo4Thread::Sleep(fguPOLLINTERVAL);
         }

      }  // if(com)
   return 0;
} // try

/////////////////////////////////////////////////////////////////////////////////
// begin catch block
catch(TGo4EventTimeoutException& ex)
{
   ex.Handle(); // display exception on terminal in case of debug
   if(TGo4Log::GetIgnoreLevel()<1)
      {
         // only display message if debug output enabled
         fxAnalysisClient->SendStatusMessage(2,kTRUE,"Analysis %s TIMEOUT for event source %s:%s.",
                  fxAnalysisClient->GetName(), ex.GetSourceClass(), ex.GetSourceName());
      } else{}
    return 0;
}
catch(TGo4EventEndException& ex)
{
fxAnalysisClient->SendStatusMessage(2,kTRUE,"End of event source %s:\n     %s - %s",
                                 ex.GetSourceClass(),
                                 ex.GetSourceName(),ex.GetErrMess());
if(fxAnalysis->IsErrorStopEnabled()) fxAnalysisClient->Stop();
}
catch(TGo4EventErrorException& ex)
{
   //ex.Handle();
   Int_t prio=ex.GetPriority();
   if(prio==0)
      {
         // only display message without stop
         fxAnalysisClient->SendStatusMessage(1,kTRUE,"Event source %s:\n     %s - %s",
                                 ex.GetSourceClass(),
                                 ex.GetSourceName(),ex.GetErrMess());
      }
   else
      {
      fxAnalysisClient->SendStatusMessage(3,kTRUE,"Analysis %s ERROR: %s from event source %s:%s",
                           fxAnalysisClient->GetName(),ex.GetErrMess(),
                           ex.GetSourceClass(), ex.GetSourceName());
      if(fxAnalysis->IsErrorStopEnabled()) fxAnalysisClient->Stop();
      }
   return 0;
}

catch(TGo4DynamicListException& ex)
{
   ex.Handle();
   fxAnalysisClient->SendStatusMessage(3,kTRUE,"Analysis %s ERROR: %s from dynamic list entry %s:%s",
                           fxAnalysisClient->GetName(),ex.GetStatusMessage(),
                           ex.GetEntryName(), ex.GetEntryClass());
   if(fxAnalysis->IsErrorStopEnabled())
      fxAnalysisClient->Stop();
   return 0;
}

catch(TGo4AnalysisStepException& ex)
{
   ex.Handle();
   fxAnalysisClient->SendStatusMessage(3,kTRUE,"Analysis %s ERROR: %s in Analysis Step %s",
                           fxAnalysisClient->GetName(), ex.GetStatusMessage(), ex.GetStepName());
   if(fxAnalysis->IsErrorStopEnabled())
         fxAnalysisClient->Stop();
   return 0;
}

catch(TGo4UserException& ex)
{
   //ex.Handle();
   fxAnalysisClient->SendStatusMessage(ex.GetPriority(),kTRUE,  ex.GetMessage() );
   if(fxAnalysis->IsErrorStopEnabled() && ex.GetPriority()>2)
         fxAnalysisClient->Stop(); // only stop for errors, warnings and infos continue loop!
   return 0;
}

catch(std::exception& ex) // treat standard library exceptions
{
   fxAnalysisClient->SendStatusMessage(3,kTRUE,"Analysis %s got standard exception %s",
                           fxAnalysisClient->GetName(), ex.what());
   if(fxAnalysis->IsErrorStopEnabled())
         fxAnalysisClient->Stop();
   return 0;
}
// end catch block
////////////////////////////////////////////////////////////////////////////////
return 0;
}
