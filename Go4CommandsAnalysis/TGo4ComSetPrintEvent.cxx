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

#include "TGo4ComSetPrintEvent.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4EventElement.h"
#include "TGo4MbsSource.h"
#include "TGo4RemoteCommand.h"

TGo4ComSetPrintEvent::TGo4ComSetPrintEvent(const char* obname)
:TGo4AnalysisObjectCommand("ANSetPrintEvent","Set event printout for analysis",obname)
{
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
}
TGo4ComSetPrintEvent::TGo4ComSetPrintEvent()
:TGo4AnalysisObjectCommand("ANSetPrintEvent","Set event printout for analysis","dummy")
 {
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                                 // override default receiver
}

TGo4ComSetPrintEvent::~TGo4ComSetPrintEvent()
{
}

void TGo4ComSetPrintEvent::Set(TGo4RemoteCommand* remcom)
{
   if(remcom==0) return;
   TGo4AnalysisObjectCommand::Set(remcom);
   fxPrintPar.fiNum  = remcom->GetValue(0);
   fxPrintPar.fiSid  = remcom->GetValue(1);
   fxPrintPar.fiLong = remcom->GetValue(2);
   fxPrintPar.fiHex  = remcom->GetValue(3);
   fxPrintPar.fiData = remcom->GetValue(4);
}

Int_t TGo4ComSetPrintEvent::ExeCom()
{
   TGo4Log::Debug(" Executing %s : Printout for %s requested for %d events of subid %d, long:%d, hex:%d, data:%d",
         GetObjectName(), fxPrintPar.fiNum, fxPrintPar.fiSid, fxPrintPar.fiLong, fxPrintPar.fiHex, fxPrintPar.fiData);
   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);

   if (cli==0) {
      GO4TRACE((11,"TGo4ComSetPrintEvent::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! %s : NO or WRONG RECEIVER ERROR!!!",GetName());
      return 1;
   }

   TGo4Analysis* ana = TGo4Analysis::Instance();
   if(ana==0) {
      // never come here
      cli->SendStatusMessage(3, kTRUE," %s ERROR no analysis ",GetName());
      return -2;
   }

   // request for event by name from folder
   TGo4EventElement* eve = ana->GetEventStructure(GetObjectName());
   if (eve == 0) {
      cli->SendStatusMessage(2, kTRUE," Event %s was not found to set printout mode!",GetObjectName());
      return -1;
   }

   TGo4MbsSource* mbs_src = dynamic_cast<TGo4MbsSource*> (eve->GetEventSource());

   if (mbs_src==0) {
      cli->SendStatusMessage(2, kTRUE," Source of event %s of class %s is not yet supported for printevent mode!", GetObjectName(), eve->ClassName());
      return -1;
   }

   mbs_src->SetPrintEvent(fxPrintPar.fiNum,
                          fxPrintPar.fiSid,
                          fxPrintPar.fiLong,
                          fxPrintPar.fiHex,
                          fxPrintPar.fiData);
   return -1;
}
