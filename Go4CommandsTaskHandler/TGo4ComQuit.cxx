#include "TGo4ComQuit.h"

#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComQuit::TGo4ComQuit()
:TGo4TaskHandlerCommand("THQuit","tells client taskhandler application to quit")
{
   fbIsSynchron=kFALSE;
}

TGo4ComQuit::~TGo4ComQuit()
{
}


Int_t TGo4ComQuit::ExeCom()
{
   TGo4Task* cli=dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli)
      {
      cout << "Quit command executes on receiver:"<< ((TGo4Task*) fxReceiverBase)->GetName() <<endl;
//      cli->SendStatusMessage(2,kFALSE,"Quit command executes on client %s ...",cli->GetName());
      cli->Quit();
      }
   else
      {
         TGo4Log::Debug(" !!! ComQuit ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
