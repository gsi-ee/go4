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

#include "TGo4ComExecLine.h"

#include "TGo4Log.h"
#include "TGo4Task.h"

TGo4ComExecLine::TGo4ComExecLine() : TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THExec");
   SetDescription("Execute line as CINT macro");
}

TGo4ComExecLine::TGo4ComExecLine(const char* macro) : TGo4TaskHandlerCommand()
{
   fbIsSynchron=kFALSE;
   SetName("THExec");
   SetDescription("Execute line as CINT macro");
   SetLine(macro);
}

TGo4ComExecLine::~TGo4ComExecLine()
{
}

Int_t TGo4ComExecLine::ExeCom()
{
   TGo4Task* cli = dynamic_cast<TGo4Task*> (fxReceiverBase);
   if(cli==0) {
      TGo4Log::Debug(" !!! ComExecLine ''%s'': NO RECEIVER ERROR!!!",GetName());
      return 1;
   }

   cli->SendStatusMessage(-1,kTRUE, TString::Format("Executing: %s",GetLine()));
   cli->ExecuteString(GetLine());
   //cli->ExecuteString("Int_t res=fflush(stdout);"); // JAM will done in analysis anyway
   return 0;
}
