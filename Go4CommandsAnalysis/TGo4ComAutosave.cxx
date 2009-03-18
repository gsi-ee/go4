#include "TGo4ComAutosave.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComAutosave::TGo4ComAutosave() :
   TGo4AnalysisCommand("ANAutosave","Do immediate autosave"),
   fiAutoSaveCompression(5), fbAutoSaveOverwrite(1)
{
   TRACE((12,"TGo4ComAutosave::TGo4ComAutosave() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   SetProtection(kGo4ComModeController);
}

TGo4ComAutosave::~TGo4ComAutosave()
{
   TRACE((12,"TGo4ComAutosave::~TGo4ComAutosave() dtor",__LINE__, __FILE__));
}

void TGo4ComAutosave::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
SetAutoSaveCompression(remcom->GetValue(0));
SetAutoSaveOverwrite(remcom->GetValue(1));
SetAutoFileName(remcom->GetString(0));
}

Int_t TGo4ComAutosave::ExeCom()
{
   TRACE((12,"TGo4ComAutosave::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
         TGo4Analysis* ana = TGo4Analysis::Instance();
         if(ana)
            {
               Bool_t autoenab=ana->IsAutoSaveOn();
               cli->SendStatusMessage(1, kTRUE,"Autosaving now...");
               ana->SetAutoSave(kTRUE);
               ana->SetAutoSaveFile(GetAutoFileName(),
                                    fbAutoSaveOverwrite,
                                    fiAutoSaveCompression);
               ana->AutoSave();
               ana->SetAutoSave(autoenab); // recover old autosave state
            }

      }
   else
      {
      TRACE((11,"TGo4ComAutosave::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComAutosave ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}

