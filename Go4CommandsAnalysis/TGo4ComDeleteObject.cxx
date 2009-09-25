#include "TGo4ComDeleteObject.h"

#include "TGo4Log.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"

TGo4ComDeleteObject::TGo4ComDeleteObject(const char* obname)
:TGo4AnalysisObjectCommand("ANDelete","delete object by name", obname)
{
   TRACE((12,"TGo4ComDeleteObject::TGo4ComDeleteObject(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // to send back status message

   SetProtection(kGo4ComModeController);
}

TGo4ComDeleteObject::TGo4ComDeleteObject()
:TGo4AnalysisObjectCommand("ANDelete","delete object by name", "dummy")
{
   TRACE((12,"TGo4ComDeleteObject::TGo4ComDeleteObject() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // to send back status message
   SetProtection(kGo4ComModeController);
}

TGo4ComDeleteObject::~TGo4ComDeleteObject()
{
   TRACE((12,"TGo4ComDeleteObject::~TGo4ComDeleteObject() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComDeleteObject::ExeCom()
{
   TRACE((12,"TGo4ComDeleteObject::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli)
      {
         TGo4Analysis* ana=TGo4Analysis::Instance();
         if(ana)
            {
               const char* obname = GetObjectName();
               if(ana->DeleteObjects(obname))
                  {
                     cli->SendStatusMessage(1,kTRUE,"Deleted Object %s (all deletables of this folder, resp.)",
                           obname);
                  }
               else
                  {
                     cli->SendStatusMessage(2,kTRUE,"Could not delete object %s !",
                           obname);
                  }
            }
         else
         {
            cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",
                                                 GetName());
         } // if(ana)
      }
   else
      {
      TRACE((11,"TGo4ComDeleteObject::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
         return 1;
      } // if(cli)

   return -1;
}
