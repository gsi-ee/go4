#include "TGo4ComMasterQuit.h"

#include "TGo4Log.h"
#include "TGo4Master.h"

TGo4ComMasterQuit::TGo4ComMasterQuit()
:TGo4TaskHandlerCommand("MRQuit","quits the master and the whole application")
{
   SetReceiverName("MasterTask"); // sets the receiver name checked by command invoker
                                  // receiver name neednt be class name!
   fbIsSynchron=kFALSE;
   fbIsLocal=kTRUE;
}
TGo4ComMasterQuit::~TGo4ComMasterQuit()
{

}
Int_t TGo4ComMasterQuit::ExeCom()
{
   TGo4Master* master=dynamic_cast<TGo4Master*>(fxReceiverBase);
   if (master!=0)
      {
         master->Quit();
         return 0;
      }
   else
      {
         TGo4Log::Debug(" !!! ComMasterQuit ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
