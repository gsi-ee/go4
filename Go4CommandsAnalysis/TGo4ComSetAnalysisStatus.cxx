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

#include "TGo4ComSetAnalysisStatus.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4RuntimeException.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComSetAnalysisStatus::TGo4ComSetAnalysisStatus(TGo4AnalysisStatus* settings)
:TGo4AnalysisCommand("ANSetStatus","Set Analysis to given Status ")
{
   TRACE((12,"TGo4ComSetAnalysisStatus::TGo4ComSetAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   fxAnalysisStatus=settings;
   SetProtection(kGo4ComModeController);

}

TGo4ComSetAnalysisStatus::TGo4ComSetAnalysisStatus()
:TGo4AnalysisCommand("ANSetStatus","Set Analysis to given Status ")
{
   TRACE((12,"TGo4ComSetAnalysisStatus::TGo4ComSetAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   fxAnalysisStatus=0;
   SetProtection(kGo4ComModeController);

}

TGo4ComSetAnalysisStatus::~TGo4ComSetAnalysisStatus()
{
   TRACE((12,"TGo4ComSetAnalysisStatus::~TGo4ComSetAnalysisStatus() dtor",__LINE__, __FILE__));
   delete fxAnalysisStatus;
}

void TGo4ComSetAnalysisStatus::SetStatusObject(TGo4AnalysisStatus* settings)
{
  fxAnalysisStatus=settings;
}

void TGo4ComSetAnalysisStatus::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
TGo4AnalysisStatus* stat=dynamic_cast<TGo4AnalysisStatus* >(remcom->GetAggregate());

if(stat)
   {
      delete fxAnalysisStatus;
      fxAnalysisStatus=stat;
   }
}


Int_t TGo4ComSetAnalysisStatus::ExeCom()
{
   TRACE((12,"TGo4ComSetAnalysisStatus::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      TRACE((11,"TGo4ComSetAnalysisStatus::ExeCom() - found valid receiver",__LINE__, __FILE__));
         //TGo4Log::Debug(" Executing SetAnalysisStatus...  ");
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               if(cli->MainIsRunning()) ana->PostLoop(); // if submit is done on running analysis,
                                   // execute postloop before closing previous objects
               ana->SetStatus(fxAnalysisStatus);
               cli->SendStatusMessage(1, kFALSE,"New analysis status was set.");
            }
         else
            {
               cli->SendStatusMessage(3, kTRUE,"%s ERROR no analysis",
                                                 GetName());
            }
      }
   else
      {
      TRACE((11,"TGo4ComSetAnalysisStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ''%s'': NO RECEIVER ERROR!!!",GetName());
         throw TGo4RuntimeException();
         return 1;
      }

   return -1;
}
