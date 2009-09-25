#include "TGo4ComGetEnvelope.h"

#include "TH1.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4ObjEnvelope.h"

TGo4ComGetEnvelope::TGo4ComGetEnvelope(const char* obname, const char* foldername)
:TGo4AnalysisObjectCommand("ANGetEnvelope","Get an arbitrary object from analysis",obname)
{
   TRACE((12,"TGo4ComGetEnvelope::TGo4ComGetEnvelope(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
   SetFolderName(foldername);
}

TGo4ComGetEnvelope::TGo4ComGetEnvelope()
:TGo4AnalysisObjectCommand()
{
   TRACE((12,"TGo4ComGetEnvelope::TGo4ComGetEnvelope() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
   SetName("ANGetEnvelope");
   SetDescription("Get an arbitrary object from analysis");
}

TGo4ComGetEnvelope::~TGo4ComGetEnvelope()
{
   TRACE((12,"TGo4ComGetEnvelope::~TGo4ComGetEnvelope() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComGetEnvelope::ExeCom()
{
   TRACE((12,"TGo4ComGetEnvelope::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli!=0) {
      TRACE((11,"TGo4ComGetEnvelope::ExeCom() - found valid receiver",__LINE__, __FILE__));
      TNamed* obj = TGo4Analysis::Instance()->GetObject( GetObjectName(), GetFolderName());
      TGo4ObjEnvelope* envelope = new TGo4ObjEnvelope(obj, GetObjectName(), GetFolderName());
      cli->SendObject(envelope, GetTaskName());
      delete envelope;
   }
   else

       {
      TRACE((11,"TGo4ComGetEnvelope::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
