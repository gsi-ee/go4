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

#include "TGo4ComGetNamesList.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4AnalysisImp.h"

TGo4ComGetNamesList::TGo4ComGetNamesList() :TGo4AnalysisCommand()
{
   GO4TRACE((12,"TGo4ComGetNamesList::TGo4ComGetNamesList() ctor",__LINE__, __FILE__));
   SetReceiverName("AnalysisClient");  // this command needs client as receiver
                                       // override default receiver
   SetName("ANNames");
   SetDescription("Requests list of names and folders");

}

TGo4ComGetNamesList::~TGo4ComGetNamesList()
{
   GO4TRACE((12,"TGo4ComGetNamesList::~TGo4ComGetNamesList() dtor",__LINE__, __FILE__));
}

Int_t TGo4ComGetNamesList::ExeCom()
{
   GO4TRACE((12,"TGo4ComGetNamesList::ExeCom()",__LINE__, __FILE__));

   TGo4AnalysisClient* cli = dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli == 0) {
      GO4TRACE((11,"TGo4ComGetNamesList::ExeCom() - no receiver specified ERROR!",__LINE__, __FILE__));
      TGo4Log::Debug(" !!! ComGetNamesList ''%s'': NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   cli->SendStatusMessage(1,kFALSE, TString::Format(
         "%s::Analysis nameslist was requested from client %s", GetTaskName(), GetTaskName()));
   //cli->SendNamesList();
   TGo4Analysis* ana = TGo4Analysis::Instance();
   ana->UpdateNamesList();
   TGo4AnalysisObjectNames* state = ana->GetNamesList();
   cli->SendObject(state, GetTaskName());
   return -1;

}
