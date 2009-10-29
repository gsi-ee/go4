// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXEvent.h"

#include "Riostream.h"

//***********************************************************
TXXXEvent::TXXXEvent() :
   TGo4EventElement()
{
    cout << "**** TXXXEvent: Create instance" << endl;
}
//***********************************************************
TXXXEvent::TXXXEvent(const char* name) :
   TGo4EventElement(name)
{
  cout << "**** TXXXEvent: Create instance " << name << endl;
}
//***********************************************************
TXXXEvent::~TXXXEvent()
{
  cout << "**** TXXXEvent: Delete instance " << endl;
}

//-----------------------------------------------------------
void  TXXXEvent::Clear(Option_t *t)
{
  // all members should be cleared.
   memset((void*) &fCrate1[0],0, sizeof(fCrate1));
   memset((void*) &fCrate2[0],0, sizeof(fCrate2));
}
