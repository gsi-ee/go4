#include "TGo4ComStart.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComStart::TGo4ComStart() :TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THStart");
   SetDescription("starts example client's main loop");
}

TGo4ComStart::~TGo4ComStart()
{
}

Int_t TGo4ComStart::ExeCom()
{
   TGo4Task* cli=dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli)
      {
         cli->SendStatusMessage(1,kFALSE,"Client %s working function is started...",cli->GetName());
         cli->Start();
         return 0;
      }
   else
      {
         TGo4Log::Debug(" !!! ComStart ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
