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

#include "TGo4ObjConnectorTimer.h"

#include "TGo4Log.h"
#include "TGo4HistogramServer.h"


TGo4ObjConnectorTimer::TGo4ObjConnectorTimer (TGo4HistogramServer* server, Long_t msperiod)
   :TTimer(msperiod,kTRUE), fxServer(server)
{
   GO4TRACE((15,"TGo4ObjConnectorTimer::TGo4ObjConnectorTimer(TGo4ServerTask*,Long_t)",__LINE__, __FILE__));
}


TGo4ObjConnectorTimer::~TGo4ObjConnectorTimer()
{
   GO4TRACE((15,"TGo4ObjConnectorTimer::~TGo4ObjConnectorTimer()",__LINE__, __FILE__));
}

Bool_t TGo4ObjConnectorTimer::Notify ()
{
//   GO4TRACE((12,"TGo4ObjConnectorTimer::Notify()",__LINE__, __FILE__));
//   TurnOff();
   fxServer->TimerConnect();
   Reset();
//   TurnOn();
   return kFALSE;
}
