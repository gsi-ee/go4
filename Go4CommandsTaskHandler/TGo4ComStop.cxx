#include "TGo4ComStop.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComStop::TGo4ComStop() :
   TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THStop");
   SetDescription("stops example client's main loop");
}

TGo4ComStop::~TGo4ComStop()
{
}

Int_t TGo4ComStop::ExeCom()
{
   TGo4Task* cli=dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli)
      {
         cli->SendStatusMessage(1,kFALSE,"Client %s working function is stopped...",cli->GetName());
         cli->Stop();
      }
   else
      {
         TGo4Log::Debug(" !!! ComStop ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;

}
