#include "TGo4ComRemoveClient.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4ServerTask.h"

// later second ctor with client name as parameter + advanced command list make!

TGo4ComRemoveClient::TGo4ComRemoveClient()
:TGo4TaskHandlerCommand("SVRemoveClient","remove the specified client")
{
   SetReceiverName("ServerTask"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   fbIsLocal = kTRUE;
   fxClientName = "currentclient";
   fbWaitForClient = kTRUE;
}

TGo4ComRemoveClient::~TGo4ComRemoveClient()
{
}

void TGo4ComRemoveClient::SetClient(const char* name)
{
   fxClientName = name;
}

void TGo4ComRemoveClient::SetWaitForClient(Bool_t wait)
{
   fbWaitForClient=wait;
}

Int_t TGo4ComRemoveClient::ExeCom()
{
Bool_t ret=0;
TGo4ServerTask* serv=dynamic_cast<TGo4ServerTask*>(fxReceiverBase);
if(serv)
   {
      TGo4Log::Debug(" removing client %s",fxClientName.Data());
      ret= serv->RemoveClient(fxClientName.Data(), fbWaitForClient);
      if(!ret)
         {
            TGo4Log::Debug(" ComRemoveClient: Sorry, the specified client is not connected ");
            return 1;
         }
      else
         {
            TGo4Log::Debug(" ComRemoveClient: Client %s is removed ",fxClientName.Data());
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
