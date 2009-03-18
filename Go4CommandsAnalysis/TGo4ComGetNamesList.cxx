#include "TGo4ComGetNamesList.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4AnalysisImp.h"

TGo4ComGetNamesList::TGo4ComGetNamesList() :TGo4AnalysisCommand()
{
   TRACE((12,"TGo4ComGetNamesList::TGo4ComGetNamesList() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   SetName("ANNames");
   SetDescription("Requests list of names and folders");

}

TGo4ComGetNamesList::~TGo4ComGetNamesList()
{
   TRACE((12,"TGo4ComGetNamesList::~TGo4ComGetNamesList() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComGetNamesList::ExeCom()
{
   TRACE((12,"TGo4ComGetNamesList::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli)
      {
         cli->SendStatusMessage(1,kFALSE,"%s::Analysis nameslist was requested from client %s", GetTaskName(), GetTaskName());
         //cli->SendNamesList();
         TGo4Analysis* ana=TGo4Analysis::Instance();
         ana->UpdateNamesList();
         TGo4AnalysisObjectNames* state= ana->GetNamesList();
         cli->SendObject(state, GetTaskName());
      }
   else
      {
      TRACE((11,"TGo4ComGetNamesList::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComGetNamesList ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
