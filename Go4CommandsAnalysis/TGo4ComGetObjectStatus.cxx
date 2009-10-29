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

#include "TGo4ComGetObjectStatus.h"

#include "TGo4Log.h"
#include "TGo4ObjectStatus.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"

TGo4ComGetObjectStatus::TGo4ComGetObjectStatus(const char* obname)
:TGo4AnalysisObjectCommand("ANObjectStatus","Get an object status from analysis",obname)
{
   TRACE((12,"TGo4ComGetObjectStatus::TGo4ComGetObjectStatus(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComGetObjectStatus::~TGo4ComGetObjectStatus()
{
   TRACE((12,"TGo4ComGetObjectStatus::~TGo4ComGetObjectStatus() dtor",__LINE__, __FILE__));
}

TGo4ComGetObjectStatus::TGo4ComGetObjectStatus()
:TGo4AnalysisObjectCommand("ANObjectStatus","Get an object status from analysis","his")

 {
   TRACE((12,"TGo4ComGetObjectStatus::TGo4ComGetObjectStatus() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

Int_t TGo4ComGetObjectStatus::ExeCom()
{
   TRACE((12,"TGo4ComGetObjectStatus::ExeCom()",__LINE__, __FILE__));

   if (fxReceiverBase!=0)
      {
      TRACE((11,"TGo4ComGetObjectStatus::ExeCom() - found valid receiver",__LINE__, __FILE__));
         TGo4Log::Debug(" Executing %s : Object %s Status requested ",
            GetName(), GetObjectName());
         TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
         TGo4Analysis* ana=TGo4Analysis::Instance();
         TGo4ObjectStatus* state= ana->CreateObjectStatus(GetObjectName(), GetFolderName());
         if(state)
            {
               TString name=state->GetName();      // workaround for the case that
               name.Append("_status");      // histogram is in monitoring list
               state->SetName(name.Data());
               cli->SendObject(state, GetTaskName());
               delete state; // SendObject will stream state, but not delete it!
            }
         else
            {
               cli->SendStatusMessage(3, kTRUE,"GetObjectStatus - ERROR:  no such object %s in folder %s",
                                                GetObjectName(), GetFolderName());
            }

      }

   else

       {
      TRACE((11,"TGo4ComGetObjectStatus::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
         TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
         return 1;
      }
   return -1;
}
