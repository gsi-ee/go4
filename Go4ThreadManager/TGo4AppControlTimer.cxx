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

#include "TGo4AppControlTimer.h"

#include "go4iostream.h"

#include "TCondition.h"
#include "TApplication.h"
#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4ThreadManager.h"
#include "TGo4ThreadHandler.h"

UInt_t TGo4AppControlTimer::fguTERMWAIT = 3000;

Int_t TGo4AppControlTimer::fgiTERMWAITCYCLES = 5;

TGo4AppControlTimer::TGo4AppControlTimer (TGo4ThreadManager* manager, Long_t msperiod, Bool_t mode)
   :TTimer(msperiod,mode),
   fxManager(0),
   fbApplicationRun(kFALSE)
{
   TRACE((15,"TGo4AppControlTimer::TGo4AppControlTimer(TGo4ThreadManager*, Long_t, Bool_t) ctor",__LINE__, __FILE__));
   fxManager=manager;
   fxCondition=new TCondition();
}


TGo4AppControlTimer::~TGo4AppControlTimer()
{
TRACE((15,"TGo4AppControlTimer::~TGo4AppControlTimer() dtor",__LINE__, __FILE__));
//cout <<"!!!!!!!!!!!!!!! Destructor of TGo4AppControlTimer" << endl;
     if(fxCondition)
         {
            delete fxCondition;
            fxCondition=0;
         }
}

Bool_t TGo4AppControlTimer::Notify ()
{
   //TRACE((12,"TGo4AppControlTimer::Notify()",__LINE__, __FILE__));

//   TurnOff();
   if(fxManager==0)
      {
         cerr<<"TGo4AppControlTimer::Notify () NEVER COME HERE!!!" << endl;
         cerr<<"TGo4AppControlTimer::Notify () with  zero threadmanager!!!" << endl;

      }
   if(fxManager->Initialization()==0)
      // init successfull?
      {
         TRACE((11,"TGo4AppControlTimer: Successful Initialization",__LINE__, __FILE__));
         if(!fbApplicationRun)
            // root TApplication shall be blocked
            {
            TRACE((11,"TGo4AppControlTimer: Application shall be blocked!",__LINE__, __FILE__));
            if(TThread::Exists())
               // necessary to avoid gThreadImp bug in condition wait...
               {
               // normal mode:
                  TRACE((10,"TGo4AppControlTimer: blocking ROOT gSystem!",__LINE__, __FILE__));
                  TGo4Log::Debug(" Application Control Timer --  Blocking ROOT gSystem!! ");
                  fxCondition->Wait();  // wait for running signal
                  TGo4Log::Debug(" Application Control Timer --  Releasing ROOT gSystem!! ");
               }
            else
               {
                  // no thread up, cannot wait for condition
                  TRACE((10,"TGo4AppControlTimer: no TThread existing, not blocking !",__LINE__, __FILE__));
               }
          }
        else
          {
            // do not block :)
            TRACE((11,"TGo4AppControlTimer: Application shall not be blocked!",__LINE__, __FILE__));
          }

        if(fxManager->IsTerminating())
          // check if Timer was woken for program end
          {
             TRACE((11,"TGo4AppControlTimer: Terminating mode",__LINE__, __FILE__));
             Int_t t=0;
             while(!(fxManager->GetWorkHandler()->AllWaiting()) && (t++<fgiTERMWAITCYCLES) )
            {
               TGo4Log::Debug(" Application Control Timer --  waiting for Runnables to stop... ");
               //cout << "TGo4AppControlTimer waiting for Runnables to stop..."<<t<<endl;
               gSystem->Sleep(fguTERMWAIT);
               fxManager->GetWorkHandler()->StopAll();
            }
            TGo4Log::Debug(" Application Control Timer --  deleting TGo4ThreadManager! ");
            Bool_t pleaseterminate=fxManager->IsTerminateApp();
            delete fxManager;      // executes dtors
            fxManager=0;
            TurnOff(); // do not call this Notify again in Terminate
            if(pleaseterminate)
               {
                  TGo4Log::Debug(" Application Control Timer --  Terminating Application now! Goodbye...");
                  gApplication->Terminate(0);
               }
            else
               {
                  TGo4Log::Debug(" Application Control Timer --  Finished Threadmanager deletion, killing myself now...");
                  delete this;
               }
            return kFALSE;
          } // if(pleaseterminate)
        else
          {
            // prepare for another timer cycle
            TRACE((11,"TGo4AppControlTimer: Resetting for new cycle",__LINE__, __FILE__));
            Reset();
  //          TurnOn();
          }
      } //if(fxManager->Initialization()==0)
   else
      // init not successful
      {
         TRACE((11,"TGo4AppControlTimer: Initialization failed",__LINE__, __FILE__));
         TGo4Log::Debug(" Application Control Timer --  ThreadManager Init not successful, retrying! ");
         Reset();
//         TurnOn();
      }
    return kFALSE;
}

Bool_t TGo4AppControlTimer::GetApplicationRun ()
{
  TRACE((12,"TGo4AppControlTimer::GetApplicationRun ()",__LINE__, __FILE__));
  return fbApplicationRun;
}




