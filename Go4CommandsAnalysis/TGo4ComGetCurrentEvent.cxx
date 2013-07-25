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

#include "TGo4ComGetCurrentEvent.h"

#include <stdio.h>

#include "TTree.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisImp.h"
#include "TGo4EventElement.h"
#include "TGo4RemoteCommand.h"

TGo4ComGetCurrentEvent::TGo4ComGetCurrentEvent(const char* obname) :
   TGo4AnalysisObjectCommand("ANGetEvent","Get current event from analysis",obname),
   fbOutputEvent(kTRUE),
   fbPrintoutOnly(kFALSE),
   fbTreeMode(kTRUE)
{
   GO4TRACE((12,"TGo4ComGetCurrentEvent::TGo4ComGetCurrentEvent(const char*) ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComGetCurrentEvent::TGo4ComGetCurrentEvent() :
   TGo4AnalysisObjectCommand("ANGetEvent","Get current event from analysis","his"),
   fbOutputEvent(kTRUE),
   fbPrintoutOnly(kFALSE),
   fbTreeMode(kTRUE)
 {
   GO4TRACE((12,"TGo4ComGetCurrentEvent::TGo4ComGetCurrentEvent() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
}

TGo4ComGetCurrentEvent::~TGo4ComGetCurrentEvent()
{
   GO4TRACE((12,"TGo4ComGetCurrentEvent::~TGo4ComGetCurrentEvent() dtor",__LINE__, __FILE__));
}


Int_t TGo4ComGetCurrentEvent::ExeCom()
{
   GO4TRACE((12,"TGo4ComGetCurrentEvent::ExeCom()",__LINE__, __FILE__));

   TGo4Log::Debug(" Executing %s : Event %s requested, isoutput=%d, printoutonly=%d ",
         GetName(), GetObjectName(), IsOutputEvent(), IsPrintoutOnly() );

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);

   if (cli==0) {
      GO4TRACE((11,"TGo4ComGetCurrentEvent::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! %s : NO or WRONG RECEIVER ERROR!!!",GetName());
      return 1;
   }

   TGo4Analysis* ana = TGo4Analysis::Instance();
   if (ana==0) {
      // never come here
      cli->SendStatusMessage(3, kTRUE,TString::Format(" %s ERROR no analysis ",GetName()));
      return -2;
   }

   if(IsTreeMode()) {
      // tree mode:
      if(IsPrintoutOnly()) {
         // only display event content on terminal
         ana->ShowEvent(GetObjectName(), IsOutputEvent());
      } else {
         TTree* sampletree = ana->CreateSingleEventTree(GetObjectName(), IsOutputEvent());
         if(sampletree) {
            cli->SendObject(sampletree, GetTaskName());
            delete sampletree;
         } else {
            cli->SendStatusMessage(3, kTRUE,TString::Format(
                  "GetCurrentEvent %s - ERROR:  no such event ", GetObjectName()));
         }
      }// if(IsPrintoutOnly())
   }// if(IsTreeMode())
   else
   {
      // event mode:
      TGo4EventElement* event=0;
      if(IsOutputEvent())
         event=ana->GetOutputEvent(GetObjectName());
      else
         event=ana->GetInputEvent(GetObjectName());
      if(event==0)
         // event step of name does not exists, we search event in folder:
         event = ana->GetEventStructure(GetObjectName());
      if(event) {
         if (IsPrintoutOnly())
            event->PrintEvent();
         else
            cli->SendObject(event, GetTaskName());
      }
      else
      {
         cli->SendStatusMessage(3, kTRUE,TString::Format(
               "GetCurrentEvent %s - ERROR:  no such event ", GetObjectName()));
      } // if(event)
   } // if(IsTreeMode())

   if (IsPrintoutOnly()) {
      fflush(stderr);
      fflush(stdout);
   }

   return -1;
}

void TGo4ComGetCurrentEvent::Set(TGo4RemoteCommand* remcom)
{
   if(remcom==0) return;
   TGo4AnalysisObjectCommand::Set(remcom);
   fbOutputEvent = remcom->GetValue(0);
   fbPrintoutOnly = remcom->GetValue(1);
   fbTreeMode = remcom->GetValue(2);
}
