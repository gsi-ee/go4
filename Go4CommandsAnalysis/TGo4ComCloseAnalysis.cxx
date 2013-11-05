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

#include "TGo4ComCloseAnalysis.h"

#include "TGo4Log.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"

TGo4ComCloseAnalysis::TGo4ComCloseAnalysis()
:TGo4AnalysisCommand("ANClose","closes analysis event classes")
{
   GO4TRACE((12,"TGo4ComCloseAnalysis::TGo4ComCloseAnalysis() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetProtection(kGo4ComModeController);
}

TGo4ComCloseAnalysis::~TGo4ComCloseAnalysis()
{
   GO4TRACE((12,"TGo4ComCloseAnalysis::~TGo4ComCloseAnalysis() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComCloseAnalysis::ExeCom()
{
   GO4TRACE((12,"TGo4ComCloseAnalysis::ExeCom() dtor",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli==0) {
      GO4TRACE((11,"TGo4ComCloseAnalysis::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! ComCloseAnalysis ''%s'': NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   GO4TRACE((11,"TGo4ComCloseAnalysis::ExeCom() - found valid receiver",__LINE__, __FILE__));
   TGo4Analysis* ana = TGo4Analysis::Instance();
   if(ana) {
      if(cli->MainIsRunning()) ana->PostLoop(); // if close is done on running analysis,
      // execute postloop before closing previous objects

      ana->CloseAnalysis();
      cli->SendStatusMessage(1, kFALSE,TString::Format("Analysis %s was closed.",ana->GetName()));
   } else {
      cli->SendStatusMessage(3, kTRUE,TString::Format(" %s ERROR no analysis ",GetName()));
   } // if(ana)


   return -1;
}
