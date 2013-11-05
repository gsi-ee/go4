// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ComDeleteObject.h"

#include "TGo4Log.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"

TGo4ComDeleteObject::TGo4ComDeleteObject(const char* obname)
:TGo4AnalysisObjectCommand("ANDelete","delete object by name", obname)
{
   GO4TRACE((12,"TGo4ComDeleteObject::TGo4ComDeleteObject(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // to send back status message

   SetProtection(kGo4ComModeController);
}

TGo4ComDeleteObject::TGo4ComDeleteObject()
:TGo4AnalysisObjectCommand("ANDelete","delete object by name", "dummy")
{
   GO4TRACE((12,"TGo4ComDeleteObject::TGo4ComDeleteObject() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // to send back status message
   SetProtection(kGo4ComModeController);
}

TGo4ComDeleteObject::~TGo4ComDeleteObject()
{
   GO4TRACE((12,"TGo4ComDeleteObject::~TGo4ComDeleteObject() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComDeleteObject::ExeCom()
{
   GO4TRACE((12,"TGo4ComDeleteObject::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli==0) {
      GO4TRACE((11,"TGo4ComDeleteObject::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   TGo4Analysis* ana =TGo4Analysis::Instance();
   if(ana)
   {
      const char* obname = GetObjectName();
      if(ana->DeleteObjects(obname))
      {
         cli->SendStatusMessage(1,kTRUE,TString::Format(
               "Deleted Object %s (all deletables of this folder, resp.)", obname));
      }
      else
      {
         cli->SendStatusMessage(2,kTRUE,TString::Format(
               "Could not delete object %s !", obname));
      }
   }
   else
   {
      cli->SendStatusMessage(3, kTRUE,TString::Format(
            " %s ERROR no analysis ", GetName()));
   } // if(ana)

   return -1;
}
