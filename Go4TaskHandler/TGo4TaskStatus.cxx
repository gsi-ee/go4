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

#include "TGo4TaskStatus.h"

#include "TGo4TaskHandlerStatus.h"

TGo4TaskStatus::TGo4TaskStatus() :
   TGo4Status()
{
}

TGo4TaskStatus::TGo4TaskStatus(const char *name) :
   TGo4Status(name)
{
}

TGo4TaskStatus::~TGo4TaskStatus()
{
   delete fxTaskHandlerStatus;
   fxTaskHandlerStatus = nullptr;
}

void TGo4TaskStatus::SetFlags(Bool_t blocking,
                                 Bool_t autocreate,
                                 Bool_t autostart,
                                 Bool_t terminating,
                                 Bool_t initdone)
{
   fbAppBlocking = blocking;
   fbAutoCreate = autocreate;
   fbAutoStart = autostart;
   fbTerminating = terminating;
   fbInitDone = initdone;
}

void TGo4TaskStatus::SetTaskHandlerStatus(TGo4TaskHandlerStatus* thstate)
{
   fxTaskHandlerStatus = thstate;
}

void TGo4TaskStatus::Print(Option_t*) const
{
   PrintLine("G-OOOO->    Client Status Class %s Printout:  <-OOOO-G",GetName());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   PrintLine("G-OOOO-> Blockingmode: %d", fbAppBlocking);
   PrintLine("G-OOOO-> Autocreate: %d", fbAutoCreate);
   PrintLine("G-OOOO-> Autostart: %d", fbAutoStart);
   PrintLine("G-OOOO-> InitDone: %d", fbInitDone);
   PrintLine("G-OOOO-> IsTerminating: %d", fbTerminating);
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   fxTaskHandlerStatus->Print();
   PrintLine("G-OOOO-> END Client Status Class Printout END <-OOOO-G");
}
