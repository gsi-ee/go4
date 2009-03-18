#include "TGo4ComDisconnectSlave.h"

#include "TGo4Log.h"
#include "TGo4Master.h"

TGo4ComDisconnectSlave::TGo4ComDisconnectSlave()
:TGo4TaskHandlerCommand("MRDisconnectSlave","disconnect the specified slave")
{
   SetReceiverName("MasterTask"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   fbIsLocal = kTRUE;
   fxSlaveName = "currentslave";
   fbWaitForSlave = kTRUE;
}
TGo4ComDisconnectSlave::~TGo4ComDisconnectSlave()
{

}


Int_t TGo4ComDisconnectSlave::ExeCom()
{
TGo4Master* master=dynamic_cast<TGo4Master*>(fxReceiverBase);
if(master)
   {
      TGo4Log::Debug(" removing slave %s",fxSlaveName.Data());
      if(!master->DisconnectSlave(fxSlaveName.Data(), fbWaitForSlave))
         {
            TGo4Log::Debug(" ComDisconnectSlave: Sorry, the specified slave is not connected ");
            return 1;
         }
      else
         {
            TGo4Log::Debug(" ComDisconnectSlave: Slave %s is removed ",fxSlaveName.Data());
            return 0;
         }
      }
   else
      {
         TGo4Log::Debug(" !!! ComRemoveClient ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
