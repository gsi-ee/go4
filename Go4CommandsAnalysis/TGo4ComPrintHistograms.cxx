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

#include "TGo4ComPrintHistograms.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"

TGo4ComPrintHistograms::TGo4ComPrintHistograms()
:TGo4AnalysisCommand("ANPrhis","Printout of Histogram statistics")
{
   TRACE((12,"TGo4ComPrintHistograms::TGo4ComPrintHistograms() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComPrintHistograms::~TGo4ComPrintHistograms()
{
   TRACE((12,"TGo4ComPrintHistograms::~TGo4ComPrintHistograms() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComPrintHistograms::ExeCom()
{
   TRACE((12,"TGo4ComPrintHistograms::ExeCom() dtor",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      TRACE((11,"TGo4ComPrintHistograms::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               ana->PrintHistograms();
               cli->SendStatusMessage(1, kFALSE,"Analysis %s prints out histogram statistics.",ana->GetName());
            }
         else
            {
                    cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",GetName());
            } // if(ana)



      }
   else
      {
      TRACE((11,"TGo4ComPrintHistograms::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}

