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

#include "TGo4AnalysisCommand.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClientImp.h"

TGo4AnalysisCommand::TGo4AnalysisCommand(const char* name, const char* desc) :
   TGo4Command(name, desc, 7)
{
   GO4TRACE((12,"TGo4AnalysisCommand::TGo4AnalysisCommand(const char*, const char*) ctor",__LINE__, __FILE__));

   fbIsEnabled=kTRUE;
   fbIsSynchron=kTRUE;
   SetReceiverName("AnalysisClient");  // sets the receiver name checked by command invoker
                                 // receiver name neednt be class name!
}

TGo4AnalysisCommand::TGo4AnalysisCommand() :
   TGo4Command()

{
   GO4TRACE((12,"TGo4AnalysisCommand::TGo4AnalysisCommand() ctor",__LINE__, __FILE__));

   fbIsEnabled=kTRUE;
   fbIsSynchron=kTRUE;
   SetReceiverName("AnalysisClient");  // sets the receiver name checked by command invoker
                                 // receiver name neednt be class name!
}


TGo4AnalysisCommand::~TGo4AnalysisCommand()
{
  GO4TRACE((12,"TGo4AnalysisCommand::~TGo4AnalysisCommand() dtor",__LINE__, __FILE__));
}


TGo4AnalysisCommand::TGo4AnalysisCommand(const TGo4AnalysisCommand &right)
  :TGo4Command(right)
{
   GO4TRACE((12,"TGo4AnalysisCommand::TGo4AnalysisCommand() copy constructor",__LINE__, __FILE__));
   //fxReceiver=right.fxReceiver;
}

Int_t TGo4AnalysisCommand::RefuseCom()
{
   TGo4AnalysisClient* cli=dynamic_cast<TGo4AnalysisClient*> (fxReceiverBase);
   if(cli!=0)
      cli->SendStatusMessage(2,kTRUE, TString::Format(
            "%s::Command %s from GUI %s not allowed for %s (mode %d)",
            GetTaskName(), GetName(), GetTaskName(),GetModeDescription(GetMode()), GetMode()));
   else
      TGo4Command::RefuseCom();
   return 0;
}
