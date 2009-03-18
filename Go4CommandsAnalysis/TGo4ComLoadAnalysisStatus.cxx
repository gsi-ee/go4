#include "TGo4ComLoadAnalysisStatus.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4RuntimeException.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComLoadAnalysisStatus::TGo4ComLoadAnalysisStatus(const char* filename)
:TGo4AnalysisCommand("ANLoad","Load Analysis Settings from file")
{
   TRACE((12,"TGo4ComLoadAnalysisStatus::TGo4ComLoadAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   SetProtection(kGo4ComModeController);
   SetFileName(filename);
}

TGo4ComLoadAnalysisStatus::TGo4ComLoadAnalysisStatus()
:TGo4AnalysisCommand("ANLoad","Load Analysis Settings from file")
{
   TRACE((12,"TGo4ComLoadAnalysisStatus::TGo4ComLoadAnalysisStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   SetFileName(TGo4Analysis::fgcDEFAULTSTATUSFILENAME);
   SetProtection(kGo4ComModeController);
}

TGo4ComLoadAnalysisStatus::~TGo4ComLoadAnalysisStatus()
{
   TRACE((12,"TGo4ComLoadAnalysisStatus::~TGo4ComLoadAnalysisStatus() dtor",__LINE__, __FILE__));
}

void TGo4ComLoadAnalysisStatus::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
SetFileName(remcom->GetString(0));
}

Int_t TGo4ComLoadAnalysisStatus::ExeCom()
{
   TRACE((12,"TGo4ComLoadAnalysisStatus::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
      TRACE((11,"TGo4ComLoadAnalysisStatus::ExeCom() - found valid receiver",__LINE__, __FILE__));
//         TGo4Log::Debug(" Executing ComLoadAnalysisStatus...  ");
         TGo4Analysis* ana= TGo4Analysis::Instance();
         if(ana)
            {
               Bool_t ok=ana->LoadStatus( GetFileName() );
               if(ok)
                  {
                    cli->SendStatusMessage(1, kFALSE,"Loaded analysis status from file %s.",
                                                GetFileName());
                  }
               else
                  {
                     cli->SendStatusMessage(3, kFALSE,"ERROR on Loading analysis status from file %s",
                                                 GetFileName());
                  } // if(ok)

            }
         else
            {
                     cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis",
                                                 GetName());
            } // if(ana)

      }
   else
      {
      TRACE((11,"TGo4ComLoadAnalysisStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! ComLoadAnalysisStatus ''%s'': NO RECEIVER ERROR!!!",GetName());
         throw TGo4RuntimeException(); // never come here!
         return 1;
      }

   return -1;
}

