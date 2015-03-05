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


#include "TGo4Dabc.h"

#include "TGo4Sniffer.h"
#include "THttpServer.h"
#include "TGo4AnalysisImp.h"
#include "TGo4Log.h"

THttpServer* TGo4Dabc::gHttpServer = 0;

bool TGo4Dabc::CreateEngine(const char* args)
{
   if (gHttpServer == 0) {
      gHttpServer = new THttpServer("");

      gHttpServer->SetSniffer(new TGo4Sniffer("go4_dabc"));

      gHttpServer->SetTimer(0);  // we disable timer - go4 will call Process method itself
   }

   const char* separ = (strchr(args,'?')!=0) ? "&" : "?";

   TString allargs;

   const char* go4sys = TGo4Log::GO4SYS();

   allargs.Form("%s%splayer=TGo4DabcPlayer&loc0d=%s&loc0a=go4sys/&top=Go4", args, separ, go4sys);

   if (TGo4Analysis::Instance()!=0) {
      allargs.Append("/");
      allargs.Append(TGo4Analysis::Instance()->GetName());
   }

   return gHttpServer->CreateEngine(allargs.Data());
}
