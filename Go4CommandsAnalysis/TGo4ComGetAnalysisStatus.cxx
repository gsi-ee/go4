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

#include "TGo4ComGetAnalysisStatus.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisImp.h"

TGo4ComGetAnalysisStatus::TGo4ComGetAnalysisStatus() :
   TGo4AnalysisCommand("ANGetStatus","Requests Analysis Status object")
{
   GO4TRACE((12,"TGo4ComGetAnalysisStatus::TGo4ComGetAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComGetAnalysisStatus::~TGo4ComGetAnalysisStatus()
{
   GO4TRACE((12,"TGo4ComGetAnalysisStatus::~TGo4ComGetAnalysisStatus() dtor",__LINE__, __FILE__));
}


Int_t TGo4ComGetAnalysisStatus::ExeCom()
{
   GO4TRACE((12,"TGo4ComGetAnalysisStatus::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli == 0) {
      GO4TRACE((11,"TGo4ComGetAnalysisStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! ComGetAnalysisStatus ''%s'': NO RECEIVER ERROR!!!",GetName());
      return 1;
   }
   cli->SendStatusMessage(1,kFALSE,"Analysis status was requested from client...");
   //cli->SendAnalysisStatus(); // we avoid using analysisclient method
   // just use analysis and taskhandler classes
   TGo4Analysis* ana = TGo4Analysis::Instance();
   if (ana) {
      TGo4AnalysisStatus* state = ana->CreateStatus();
      cli->SendStatus(state, GetTaskName());
      delete state;
   }

   return -1;
}
