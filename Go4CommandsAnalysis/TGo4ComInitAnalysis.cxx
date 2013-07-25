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

#include "TGo4ComInitAnalysis.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"

TGo4ComInitAnalysis::TGo4ComInitAnalysis()
:TGo4AnalysisCommand("ANInit","executes InitEventClasses of Analysis")
{
   GO4TRACE((12,"TGo4ComInitAnalysis::TGo4ComInitAnalysis() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   SetProtection(kGo4ComModeController);
}

TGo4ComInitAnalysis::~TGo4ComInitAnalysis()
{
   GO4TRACE((12,"TGo4ComInitAnalysis::~TGo4ComInitAnalysis() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComInitAnalysis::ExeCom()
{
   GO4TRACE((12,"TGo4ComInitAnalysis::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      GO4TRACE((11,"TGo4ComInitAnalysis::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               cli->SendStatusMessage(2, kTRUE,"INITIALISING submitted settings, PLEASE WAIT...");
               TGo4Thread::Sleep(1000);
               if(ana->InitEventClasses())
                  {
                     cli->SendStatusMessage(1, kTRUE,TString::Format(
                           "Analysis %s event classes were initialized.", ana->GetName()));
                     if(cli->MainIsRunning())
                        ana->PreLoop(); // re-init userpointers when init was done on the fly
                  }
               else
                  {
                     cli->SendStatusMessage(3, kTRUE, TString::Format(
                           "Analysis %s initialization failed !!! ", ana->GetName()));
                  }
            }
         else
            {
               cli->SendStatusMessage(3, kTRUE, TString::Format(" %s ERROR no analysis", GetName()));
            }
      }
   else
      {
      GO4TRACE((11,"TGo4ComInitAnalysis::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComInitAnalysis ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
