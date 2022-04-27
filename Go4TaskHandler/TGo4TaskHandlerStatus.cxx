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

#include "TGo4TaskHandlerStatus.h"

TGo4TaskHandlerStatus::TGo4TaskHandlerStatus() :
   TGo4Status()
{
}

TGo4TaskHandlerStatus::TGo4TaskHandlerStatus(const char* name) :
   TGo4Status(name)
{
}

TGo4TaskHandlerStatus::~TGo4TaskHandlerStatus()
{
}

void TGo4TaskHandlerStatus::Print(Option_t*) const
{
   PrintLine("G-OOOO-> TaskHandler Status Class %s Printout: <-OOOO-G",GetName());
   PrintLine("G-OOOO-> --------------------------------------------------------------------- <-OOOO-G");
   PrintLine("G-OOOO-> Remote Server: %s", GetHostName());
   PrintLine("G-OOOO-> Negotiation Port: %d", fuNegPort);
   PrintLine("G-OOOO-> Command Thread: %s", GetComName());
   PrintLine("G-OOOO-> Command Port: %d", fiComPort);
   PrintLine("G-OOOO-> Data Thread: %s", GetDatName());
   PrintLine("G-OOOO-> Data Port: %d", fiDatPort);
   PrintLine("G-OOOO-> Status Thread: %s", GetStatName());
   PrintLine("G-OOOO-> Status Port: %d", fiStatPort);
   PrintLine("G-OOOO-> Aborting State: %s", fbIsAborting ? "TRUE" : "FALSE");
   PrintLine("G-OOOO-> END TaskHandler Status Class Printout END <-OOOO-G");
}

void TGo4TaskHandlerStatus::SetFlags(Bool_t isaborting)
{
   fbIsAborting = isaborting;
}

void TGo4TaskHandlerStatus::SetPorts(UInt_t neg, Int_t com, Int_t stat, Int_t dat)
{
   fuNegPort = neg;
   fiComPort = com;
   fiStatPort = stat;
   fiDatPort = dat;
}

void TGo4TaskHandlerStatus::SetNames(const char* com, const char* stat, const char* dat, const char* host)
{
   fxComName = com;
   fxStatName = stat;
   fxDatName = dat;
   fxHostName = host;
}
