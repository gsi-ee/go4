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

#include "TGo4ClientStatus.h"


TGo4ClientStatus::TGo4ClientStatus() :
   TGo4TaskStatus()
{
}

TGo4ClientStatus::TGo4ClientStatus(const char *name) :
   TGo4TaskStatus(name)
{
}

TGo4ClientStatus::~TGo4ClientStatus()
{
}

void TGo4ClientStatus::Print(Option_t *) const
{
   PrintLine("G-OOOO->    Client Status Class %s Printout:  <-OOOO-G", GetName());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   PrintLine("G-OOOO-> Remote Server Hostname: %s", fxServerHostName.Data());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   TGo4TaskStatus::Print();
   PrintLine("G-OOOO-> END Client Status Class Printout END <-OOOO-G");
}

void TGo4ClientStatus::SetNames(const char *serverhost)
{
   fxServerHostName = serverhost;
}
