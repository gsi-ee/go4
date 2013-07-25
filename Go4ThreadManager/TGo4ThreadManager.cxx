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

#include "TGo4ThreadManager.h"

#include "TApplication.h"
#include "TSystem.h"
#include "TCondition.h"

#include "TGo4Log.h"
#include "TGo4ThreadHandler.h"
#include "TGo4AppControlTimer.h"

const Long_t TGo4ThreadManager::fglTIMERPERIOD = 200; // 2000

TGo4ThreadManager::TGo4ThreadManager(const TGo4ThreadManager &right)
  :TNamed(right),
   fbInitDone(kFALSE),
   fbTerminating(kFALSE),
   fbTerminateApplication(kFALSE)
{
   GO4TRACE((15,"TGo4ThreadManager::TGo4ThreadManager copy ctor",__LINE__, __FILE__));
   fxWorkHandler=right.fxWorkHandler;
   fxBlocker = right.fxBlocker;
}

TGo4ThreadManager::TGo4ThreadManager (const char* name, Bool_t blockingmode, Bool_t autostart, Bool_t autocreate)
   :TNamed(name,"This is a TGo4ThreadManager"),
      fbInitDone(kFALSE),
      fbTerminating(kFALSE),
      fbTerminateApplication(kFALSE),
      fbBeingQuit(kFALSE)
{
   GO4TRACE((15,"TGo4ThreadManager::TGo4ThreadManager (const char*, Bool_t, Bool_t, Bool_t) constructor",__LINE__, __FILE__));

   TString myname=GetName();
   myname+="-";
   myname+=gSystem->HostName(); // add hostname
   myname+="-";
   myname+=gSystem->GetPid(); // add pid to threadmanager name
   SetName(myname.Data());    // be unique for multiple client connections!
   TString nomen("ThreadHandler of "); nomen+=GetName();
   fxWorkHandler=new TGo4ThreadHandler(nomen.Data(), this);
   fxBlocker=new TGo4AppControlTimer(this,fglTIMERPERIOD);
   fbAppBlocking=blockingmode;
   fbAutoCreate=autocreate;
   fbAutoStart=autostart;
   if(fbAppBlocking)
      // manager in blocking mode (analysis without gui)
      {
         TGo4Log::Debug(" ThreadManager -- Starting in application blocking mode ");
         fxBlocker->SetApplicationRun(kFALSE);
      }
   else
      // manager in non blocking mode (for gui)
      {
         TGo4Log::Debug(" ThreadManager --  Starting in application non-blocking mode ");
         fxBlocker->SetApplicationRun(kTRUE);
      }
}


TGo4ThreadManager::~TGo4ThreadManager()
{
   GO4TRACE((15,"TGo4ThreadManager::~TGo4ThreadManager destructor",__LINE__, __FILE__));
   delete fxWorkHandler; // this will cancel all threads and delete the internal instances
   //delete fxBlocker; // dtor is called from fxBlocker, may not delete it!
   //gApplication->Terminate(0);
}

Bool_t TGo4ThreadManager::BlockApp ()
{
   GO4TRACE((12,"TGo4ThreadManager::BlockApp()",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(fbAppBlocking)
      {
      if( !fbTerminating && !( fxWorkHandler->IsOperating() ) )
         {
            GO4TRACE((11,"TGo4ThreadManager::BlockApp() blocking mode",__LINE__, __FILE__));
            fxBlocker->SetApplicationRun(kFALSE);
            rev=kTRUE;
         }
      else
         {
         // in case of Termination or threadhandler operation:
         // do not block app again, may deadlock control timer
            GO4TRACE((11,"TGo4ThreadManager::BlockApp() unblocking mode",__LINE__, __FILE__));
            rev=kFALSE;
         }
      }
   else
      {
      // manager is in non blocking mode (enables gui callback operations)
         rev=kFALSE;
      }
   return rev;
}

Bool_t TGo4ThreadManager::UnBlockApp (Int_t mode)
{
   GO4TRACE((12,"TGo4ThreadManager::UnBlockApp()",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   switch(mode)
      {
      case 0:
         {
         GO4TRACE((11,"TGo4ThreadManager::UnBlockApp() mode 0",__LINE__, __FILE__));
         if(!fxBlocker->GetApplicationRun())
            // only send condition if timer is really waiting
            {

               fxBlocker->SetApplicationRun(kTRUE);
               ((TCondition*) fxBlocker->GetCondition() )->Signal();
            }
         rev=kTRUE;
         }
         break;
      case 1:
         {
         GO4TRACE((11,"TGo4ThreadManager::UnBlockApp() mode 1",__LINE__, __FILE__));
         fxBlocker->SetApplicationRun(kTRUE);
         rev=kTRUE;
         }
         break;
      case 2:
         {
         GO4TRACE((11,"TGo4ThreadManager::UnBlockApp() mode 2",__LINE__, __FILE__));
         if(!fxBlocker->GetApplicationRun())
            // only send condition if timer is really waiting
            {
               ((TCondition*)  fxBlocker->GetCondition() )->Signal();
            }
         rev=kTRUE;
         }
         break;
      default:
         {
         GO4TRACE((16,"++TGo4ThreadManager::UnBlockApp() unknown mode"));
         //std::cerr << "TGo4ThreadManager::UnBlockApp() unknown mode"<< std::endl;
         rev=kFALSE;
         }
         break;
      }  // switch(mode)
  return rev;
}

Int_t TGo4ThreadManager::Initialization ()
{
  GO4TRACE((12,"TGo4ThreadManager::Initialization()",__LINE__, __FILE__));
  if(fbInitDone)
    // already initialized, return ok value
    {
       GO4TRACE((11,"TGo4ThreadManager::Initialization()--already init done, returning",__LINE__, __FILE__));
       //std::cout << "already init done, returning"<< std::endl;
      return 0;
    }
  else
    // first call of initialization: check threads
    {
      if(fbAutoCreate)
         // auto thread creation mode: wait for all threads being up
         {
            GO4TRACE((11,"TGo4ThreadManager::Initialization()--in AutoCreate mode",__LINE__, __FILE__));
            if( fxWorkHandler->AllCreated() )
               // test for threads, block timer and start work if they are up
               {
                  TGo4Log::Debug(" ThreadManager -- All threads are up, writing dump file ");
                  BlockApp();
                  fxWorkHandler->DumpThreads();
                  if(fbAutoStart)
                     {
                        // autostart mode of runnables
                        GO4TRACE((11,"TGo4ThreadManager::Initialization()--in AutoStart mode",__LINE__, __FILE__));
                         fxWorkHandler->StartAll();
                     }
                  else
                     {
                        GO4TRACE((11,"TGo4ThreadManager::Initialization()--in non-AutoStart mode",__LINE__, __FILE__));

                        // do not start runnables
                     }

                  fbInitDone=kTRUE;
                  return 0;
               }
            else
              // some threads are missing, suspend starting until next timer cycle
              {
                TGo4Log::Debug(" ThreadManager -- some threads are missing, re-doing Init ");

                //std::cout <<"TGo4ThreadManager waiting for threads"<< std::endl;
                return 1;
              }
         }
      else
         {
            // do not check or dump threads
            GO4TRACE((11,"TGo4ThreadManager::Initialization()--not in AutoCreate mode",__LINE__, __FILE__));
            fbInitDone=kTRUE;
            return 0;
         }
    }
}

void TGo4ThreadManager::Launch ()
{
   GO4TRACE((15,"TGo4ThreadManager::Launch()",__LINE__, __FILE__));
   if(fbAutoCreate)
      {
         // create all TThreads of TGo4Threads in threadhandler list
         GO4TRACE((13,"TGo4ThreadManager::Launch()-- executing AutoCreate mode",__LINE__, __FILE__));
         fxWorkHandler->CreateAll();
      }
   else
      {
         // do not create TThreads
         GO4TRACE((13,"TGo4ThreadManager::Launch()-- no AutoCreate mode",__LINE__, __FILE__));
      }
   fxBlocker->TurnOn();  // later in method which is called at the end of derived ctor?
}

void TGo4ThreadManager::Terminate (Bool_t termapp)
{
   GO4TRACE((15,"TGo4ThreadManager::Terminate()",__LINE__, __FILE__));
   TGo4Log::Debug(" ThreadManager -- Preparing Termination... ");
   fxWorkHandler->StopAll();
   //gSystem->Sleep(10000); // wait for workfunc to return
   fbTerminating=kTRUE;
   fbTerminateApplication=termapp;
   {
   GO4TRACE((13,"TGo4ThreadManager::Terminate()--waking up timer:",__LINE__, __FILE__));
   UnBlockApp(); // wake up blocking timer
   }
}

void TGo4ThreadManager::TerminateFast()
{
   Terminate();
}

