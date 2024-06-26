// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ObjConnectorRunnable.h"

#include <iostream>

#include "TGo4Thread.h"
#include "TGo4Task.h"
#include "TGo4HistogramServer.h"
#include "TGo4AnalysisClientImp.h"

TGo4ObjConnectorRunnable::TGo4ObjConnectorRunnable(const char *name, TGo4HistogramServer *hserv)
   : TGo4Runnable(name, hserv->GetAnalysisClient()->GetTask())
{
   fxHistogramServer = hserv;
}

TGo4ObjConnectorRunnable::TGo4ObjConnectorRunnable() : TGo4Runnable(nullptr, nullptr) {}

TGo4ObjConnectorRunnable::~TGo4ObjConnectorRunnable() {}

Int_t TGo4ObjConnectorRunnable::Run(void *)
{
   if (!fxHistogramServer) {
      std::cerr << "error, no histogram server!!" << std::endl;
      fxGo4Thread->Stop();
      return -1;
   }
   Int_t rev = fxHistogramServer->ServeObjectClient();
   if (rev < -1) {
      fxGo4Thread->Stop(); // stop runnable for termination
   }
   return 0;
}
