#include "TGo4ComClearObject.h"

#include "TGo4Log.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"

TGo4ComClearObject::TGo4ComClearObject()
:TGo4AnalysisObjectCommand("ANClearObject",
                           "clears object by name",
                           "his")
{
   TRACE((12,"TGo4ComClearObject::TGo4ComClearObject() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // to send back status message
   SetProtection(kGo4ComModeController);
}

TGo4ComClearObject::TGo4ComClearObject(const char* name)
:TGo4AnalysisObjectCommand("ANClearObject",
                           "clears object by name",
                           name)
{
   TRACE((12,"TGo4ComClearObject::TGo4ComClearObject(Text_t*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                      // to send back status message
   SetProtection(kGo4ComModeController);
}


TGo4ComClearObject::~TGo4ComClearObject()
{
   TRACE((12,"TGo4ComClearObject::~TGo4ComClearObject() dtor",__LINE__, __FILE__));
}



Int_t TGo4ComClearObject::ExeCom()
{
   TRACE((12,"TGo4ComClearObject::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli)
      {
         TGo4Analysis* ana=TGo4Analysis::Instance();
         if(ana)
            {
               const Text_t* obname = GetObjectName();
               Bool_t ok=ana->ClearObjects(obname);
               if(ok)
                  {
                    cli->SendStatusMessage(1,kTRUE,"Object %s was cleared.", obname);
                  }
               else
                  {
                     cli->SendStatusMessage(2,kTRUE,"Could not clear object %s", obname);
                  } // if(ob)
            }
         else
            {
               cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ", GetName());
            } // if(ana)
      }
   else
      {
      TRACE((11,"TGo4ComClearObject::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! %s: NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
