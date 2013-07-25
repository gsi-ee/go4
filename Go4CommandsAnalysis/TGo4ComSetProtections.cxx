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

#include "TGo4ComSetProtections.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4RemoteCommand.h"

TGo4ComSetProtections::TGo4ComSetProtections(const char* obname, const char* flags)
:TGo4AnalysisObjectCommand("ANSetProtect","Set protection properties for object",obname)
{
   GO4TRACE((12,"TGo4ComSetProtections::TGo4ComSetProtections(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                      // override default receiver
   SetFlags(flags);
   SetProtection(kGo4ComModeController);
}

TGo4ComSetProtections::TGo4ComSetProtections()
:TGo4AnalysisObjectCommand()
{
   GO4TRACE((12,"TGo4ComSetProtections::TGo4ComSetProtections() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
   SetName("ANSetProtect");
   SetDescription("Set protection properties for object");
   SetProtection(kGo4ComModeController);
}

TGo4ComSetProtections::~TGo4ComSetProtections()
{
   GO4TRACE((12,"TGo4ComSetProtections::~TGo4ComSetProtections() dtor",__LINE__, __FILE__));
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
               cli->SendStatusMessage(1, kFALSE,TString::Format(
                     "Changed object or folder %s protections: %s ",
                        GetObjectName(),fxFlags.Data()));
            }
         else
            {
                cli->SendStatusMessage(2, kFALSE,TString::Format(
                      "Could not change object/folder %s protections, no such object",
                        GetObjectName()));
            }
      }
   else

       {
         TGo4Log::Debug(" !!! %s : NO RECEIVER ERROR!!!",GetName());
         return 1;
      }

   return -1;
}
