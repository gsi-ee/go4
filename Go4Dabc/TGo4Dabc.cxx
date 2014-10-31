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

THttpServer* gGo4HttpServer = 0;

bool TGo4Dabc::CreateEngine(const char* args)
{
   if (gGo4HttpServer == 0) {
      gGo4HttpServer = new THttpServer("");

      gGo4HttpServer->SetSniffer(new TGo4Sniffer("go4_dabc"));
   }

   const char* separ = (strchr(args,'?')!=0) ? "&" : "?";

   TString allargs;

   const char* go4sys = TGo4Log::GO4SYS();

   allargs.Form("%s%splayer=TGo4DabcPlayer&loc0d=%s&loc0a=go4sys/&loc0n=go4_&loc0s=/html/&top=Go4", args, separ, go4sys);

   if (TGo4Analysis::Instance()!=0) {
      allargs.Append("/");
      allargs.Append(TGo4Analysis::Instance()->GetName());
   }

   return gGo4HttpServer->CreateEngine(allargs.Data());
}
