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

#include "TGo4ComPrintConditions.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"

TGo4ComPrintConditions::TGo4ComPrintConditions()
:TGo4AnalysisCommand("ANPrcon","Printout of condition counters")
{
   GO4TRACE((12,"TGo4ComPrintConditions::TGo4ComPrintConditions() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComPrintConditions::~TGo4ComPrintConditions()
{
   GO4TRACE((12,"TGo4ComPrintConditions::~TGo4ComPrintConditions() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComPrintConditions::ExeCom()
{
   GO4TRACE((12,"TGo4ComPrintConditions::ExeCom() dtor",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      GO4TRACE((11,"TGo4ComPrintConditions::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               ana->PrintConditions();
               cli->SendStatusMessage(1, kFALSE,TString::Format(
                     "Analysis %s prints out condition counters",ana->GetName()));
            }
         else
            {
                    cli->SendStatusMessage(3, kTRUE,TString::Format(
                          " %s ERROR no analysis ",GetName()));
            } // if(ana)
      }
   else
      {
      GO4TRACE((11,"TGo4ComPrintConditions::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
