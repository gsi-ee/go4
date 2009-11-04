// $Id: TXXXUnpackEvent.cxx 478 2009-10-29 12:26:09Z linev $
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

#include "TXXXUnpackEvent.h"


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
   void* destfield;
   destfield = (void*) &fiCrate1[0];
   memset(destfield, 0, sizeof(fiCrate1));
   destfield = (void*) &fiCrate2[0];
   memset(destfield, 0, sizeof(fiCrate2));
   destfield = (void*) &fiCrate3[0];
   memset(destfield, 0, sizeof(fiCrate3));
   destfield = (void*) &fiCrate4[0];
   memset(destfield, 0, sizeof(fiCrate4));
}
