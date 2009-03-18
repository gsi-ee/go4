#include "TGo4ComSetProtections.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComSetProtections::TGo4ComSetProtections(const char* obname, const char* flags)
:TGo4AnalysisObjectCommand("ANSetProtect","Set protection properties for object",obname)
{
   TRACE((12,"TGo4ComSetProtections::TGo4ComSetProtections(const Text_t*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                      // override default receiver
   SetFlags(flags);
   SetProtection(kGo4ComModeController);
}

TGo4ComSetProtections::TGo4ComSetProtections()
:TGo4AnalysisObjectCommand()
{
   TRACE((12,"TGo4ComSetProtections::TGo4ComSetProtections() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
   SetName("ANSetProtect");
   SetDescription("Set protection properties for object");
   SetProtection(kGo4ComModeController);
}

TGo4ComSetProtections::~TGo4ComSetProtections()
{
   TRACE((12,"TGo4ComSetProtections::~TGo4ComSetProtections() dtor",__LINE__, __FILE__));
}

void TGo4ComSetProtections::Set(TGo4RemoteCommand* remcom)
{
if(remcom==0) return;
TGo4AnalysisObjectCommand::Set(remcom);
SetFlags(remcom->GetString(2));
}



Int_t TGo4ComSetProtections::ExeCom()
{
   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0)
      {
         TGo4Analysis* ana=TGo4Analysis::Instance();
         if(ana->ProtectObjects(GetObjectName(),(const Option_t*) fxFlags.Data()))
            {
               cli->SendStatusMessage(1, kFALSE,"Changed object or folder %s protections: %s ",
                                               GetObjectName(),fxFlags.Data());
            }
         else
            {
                cli->SendStatusMessage(2, kFALSE,"Could not change object/folder %s protections, no such object",
                                               GetObjectName());
            }
      }
   else

       {
         TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
