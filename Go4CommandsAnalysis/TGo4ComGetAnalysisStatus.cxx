#include "TGo4ComGetAnalysisStatus.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4AnalysisImp.h"

TGo4ComGetAnalysisStatus::TGo4ComGetAnalysisStatus()
:TGo4AnalysisCommand("ANGetStatus","Requests Analysis Status object")
{
   TRACE((12,"TGo4ComGetAnalysisStatus::TGo4ComGetAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComGetAnalysisStatus::~TGo4ComGetAnalysisStatus()
{
   TRACE((12,"TGo4ComGetAnalysisStatus::~TGo4ComGetAnalysisStatus() dtor",__LINE__, __FILE__));
}


Int_t TGo4ComGetAnalysisStatus::ExeCom()
{
   TRACE((12,"TGo4ComGetAnalysisStatus::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli)
      {
       cli->SendStatusMessage(1,kFALSE,"Analysis status was requested from client...");
       //cli->SendAnalysisStatus(); // we avoid using analysisclient method
                                    // just use analysis and taskhandler classes
       TGo4Analysis* ana=TGo4Analysis::Instance();
       TGo4AnalysisStatus* state=ana->CreateStatus();
       cli->SendStatus(state, GetTaskName());
       delete state;
      }
   else
      {
      TRACE((11,"TGo4ComGetAnalysisStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComGetAnalysisStatus ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
