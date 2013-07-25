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

#include "TGo4AnalysisWatchRunnable.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Thread.h"
#include "TGo4Task.h"

#include "TGo4ClientStatus.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"

const UInt_t TGo4AnalysisWatchRunnable::fguWATCHINTERVAL=1000;
//const UInt_t TGo4AnalysisWatchRunnable::fguWATCHINTERVAL=50;

TGo4AnalysisWatchRunnable::TGo4AnalysisWatchRunnable(const char* name, TGo4AnalysisClient* cli) :
   TGo4Runnable(name,cli->GetTask()), fxAnalysisClient(cli)
{
   fxAnalysis = cli->GetAnalysis();
}

TGo4AnalysisWatchRunnable::~TGo4AnalysisWatchRunnable()
{

}

Int_t TGo4AnalysisWatchRunnable::Run(void*)
{
   // check if ratemeter was updated
   if (fxAnalysis && fxAnalysis->IsInitDone())
   {
      if(!fxAnalysisClient->TestRatemeter())
      {
         // no, we have to update it ourselves and fill the status buffer
         if(fxAnalysisClient->MainIsRunning())
            fxAnalysisClient->UpdateRate(0); // update with calculation of time and average rate
         else
            fxAnalysisClient->UpdateRate(-1); // stopped: keep latest values for time and average rate
         {
            //TGo4LockGuard mainguard; // in UpdateStatusBuffer now JA
            //std::cout << "watch runnable updated status buffer itself!!!"<< std::endl;
            fxAnalysisClient->UpdateStatusBuffer();
         } // lock guard
      }
      else
      { // yes, main thread is running on that, we do nothing

      } // if(!fxAnalysisClient->TestRatemeter())
      fxAnalysisClient->SendAnalysisClientStatus();
   }
   else
   { // analysis not initialized, no status to update

   } // if (ana && ana->fbInitIsDone)

   TGo4Thread::Sleep(fguWATCHINTERVAL);
   return 0;
}
