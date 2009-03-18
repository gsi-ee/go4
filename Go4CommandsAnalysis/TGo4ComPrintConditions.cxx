#include "TGo4ComPrintConditions.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"

TGo4ComPrintConditions::TGo4ComPrintConditions()
:TGo4AnalysisCommand("ANPrcon","Printout of condition counters")
{
   TRACE((12,"TGo4ComPrintConditions::TGo4ComPrintConditions() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComPrintConditions::~TGo4ComPrintConditions()
{
   TRACE((12,"TGo4ComPrintConditions::~TGo4ComPrintConditions() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComPrintConditions::ExeCom()
{
   TRACE((12,"TGo4ComPrintConditions::ExeCom() dtor",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      TRACE((11,"TGo4ComPrintConditions::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               ana->PrintConditions();
               cli->SendStatusMessage(1, kFALSE,"Analysis %s prints out condition counters",ana->GetName());
            }
         else
            {
                    cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",GetName());
            } // if(ana)
      }
   else
      {
      TRACE((11,"TGo4ComPrintConditions::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
