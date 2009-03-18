#include "TGo4ComSaveAnalysisStatus.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus(const char* filename)
:TGo4AnalysisCommand("ANSave","Save settings to file")
{
   TRACE((12,"TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetFileName(filename);
   SetProtection(kGo4ComModeController);
}


TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus()
:TGo4AnalysisCommand("ANSave","Save settings to file")
{
   TRACE((12,"TGo4ComSaveAnalysisStatus::TGo4ComSaveAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
   SetFileName(TGo4Analysis::fgcDEFAULTSTATUSFILENAME);
   SetProtection(kGo4ComModeController);
}


TGo4ComSaveAnalysisStatus::~TGo4ComSaveAnalysisStatus()
{
   TRACE((12,"TGo4ComSaveAnalysisStatus::~TGo4ComSaveAnalysisStatus() dtor",__LINE__, __FILE__));
}

void TGo4ComSaveAnalysisStatus::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
SetFileName(remcom->GetString(0));
}



Int_t TGo4ComSaveAnalysisStatus::ExeCom()
{
   TRACE((12,"TGo4ComSaveAnalysisStatus::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      TRACE((11,"TGo4ComSaveAnalysisStatus::ExeCom() - found valid receiver",__LINE__, __FILE__));
//         TGo4Log::Debug(" Executing ComSaveAnalysisStatus...  ");
         TGo4Analysis* ana=TGo4Analysis::Instance();
         if(ana)
            {
               Bool_t ok=ana->SaveStatus( GetFileName() );
               if(ok)
                  {
                   cli->SendStatusMessage(1, kFALSE,"Saved analysis status to file %s.",
                                                GetFileName());
                  }
               else
                  {
                   cli->SendStatusMessage(3, kFALSE,"ERROR on Saving analysis status to file %s.",
                                                GetFileName());
                  } // if(ok)
            }
         else
            {
                    cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",
                                                 GetName());
            } // if(ana)
      }
   else
      {
      TRACE((11,"TGo4ComSaveAnalysisStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComSaveAnalysisStatus ''%s'': NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
