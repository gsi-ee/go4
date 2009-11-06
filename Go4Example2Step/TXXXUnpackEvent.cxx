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

#include "TXXXUnpackEvent.h"

#include <string.h>

//***********************************************************
TXXXUnpackEvent::TXXXUnpackEvent() :
   TGo4EventElement()
{
}
//***********************************************************
TXXXUnpackEvent::TXXXUnpackEvent(const char* name) :
   TGo4EventElement(name)
{
}
//***********************************************************
TXXXUnpackEvent::~TXXXUnpackEvent()
{
}
//***********************************************************
void  TXXXUnpackEvent::Clear(Option_t *t)
{
   memset(fiCrate1, 0, sizeof(fiCrate1));
   memset(fiCrate2, 0, sizeof(fiCrate2));
   memset(fiCrate3, 0, sizeof(fiCrate3));
   memset(fiCrate4, 0, sizeof(fiCrate4));
}
