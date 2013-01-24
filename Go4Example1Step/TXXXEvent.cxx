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

#include "TXXXEvent.h"

#include "TGo4Log.h"

//***********************************************************
TXXXEvent::TXXXEvent() :
   TGo4EventElement()
{
   TGo4Log::Info("TXXXEvent: Create instance");
}
//***********************************************************
TXXXEvent::TXXXEvent(const char* name) :
   TGo4EventElement(name)
{
   TGo4Log::Info("TXXXEvent: Create instance %s", name);
}
//***********************************************************
TXXXEvent::~TXXXEvent()
{
   TGo4Log::Info("TXXXEvent: Delete instance");
}

//-----------------------------------------------------------
void TXXXEvent::Clear(Option_t *t)
{
  // all members should be cleared.
   memset((void*) &fCrate1[0],0, sizeof(fCrate1));
   memset((void*) &fCrate2[0],0, sizeof(fCrate2));
}
