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

#include "TGo4ComClearObject.h"

#include "TGo4Log.h"
#include "TGo4AnalysisImp.h"
#include "TGo4AnalysisClientImp.h"

TGo4ComClearObject::TGo4ComClearObject()
:TGo4AnalysisObjectCommand("ANClearObject",
                           "clears object by name",
                           "his")
{
   GO4TRACE((12,"TGo4ComClearObject::TGo4ComClearObject() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // to send back status message
   SetProtection(kGo4ComModeController);
}

TGo4ComClearObject::TGo4ComClearObject(const char* name)
:TGo4AnalysisObjectCommand("ANClearObject",
                           "clears object by name",
                           name)
{
   GO4TRACE((12,"TGo4ComClearObject::TGo4ComClearObject(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                      // to send back status message
   SetProtection(kGo4ComModeController);
}


TGo4ComClearObject::~TGo4ComClearObject()
{
   GO4TRACE((12,"TGo4ComClearObject::~TGo4ComClearObject() dtor",__LINE__, __FILE__));
}



Int_t TGo4ComClearObject::ExeCom()
{
   GO4TRACE((12,"TGo4ComClearObject::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if (cli==0) {
      GO4TRACE((11,"TGo4ComClearObject::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! %s: NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   TGo4Analysis* ana = TGo4Analysis::Instance();
   if(ana==0) {
      cli->SendStatusMessage(3, kTRUE,TString::Format(" %s ERROR no analysis ", GetName()));
      return -1;
   }

   const char* obname = GetObjectName();
   Bool_t ok = ana->ClearObjects(obname);
   if(ok) {
      cli->SendStatusMessage(1,kTRUE,TString::Format("Object %s was cleared.", obname));
   } else {
      cli->SendStatusMessage(2,kTRUE,TString::Format("Could not clear object %s", obname));
   } // if(ob)

   return -1;
}
