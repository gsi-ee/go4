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

#include "TGo4ComPrintDynList.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"

TGo4ComPrintDynList::TGo4ComPrintDynList() :
   TGo4AnalysisCommand("ANPrdyn","Printout of dynamic list")
{
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComPrintDynList::~TGo4ComPrintDynList()
{
}

Int_t TGo4ComPrintDynList::ExeCom()
{
   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               ana->PrintDynamicList();
               cli->SendStatusMessage(1, kFALSE, TString::Format(
                     "Analysis %s prints out dynamic list status.",ana->GetName()));
            }
         else
            {
                    cli->SendStatusMessage(3, kTRUE, TString::Format(
                          " %s ERROR no analysis ",GetName()));
            } // if(ana)
      }
   else
      {
         TGo4Log::Debug(" !!! ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
