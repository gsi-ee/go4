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

THttpServer* gGo4HttpServer = 0;

bool TGo4Dabc::CreateEngine(const char* args)
{
   if (gGo4HttpServer == 0) {
      gGo4HttpServer = new THttpServer("");

      gGo4HttpServer->SetSniffer(new TGo4Sniffer("go4_dabc",5));
   }

   TString allargs;
   if (strchr(args,'?')!=0)
      allargs.Form("%s&player=TGo4DabcPlayer", args);
   else
      allargs.Form("%s?player=TGo4DabcPlayer", args);

   return gGo4HttpServer->CreateEngine(allargs.Data());
}
