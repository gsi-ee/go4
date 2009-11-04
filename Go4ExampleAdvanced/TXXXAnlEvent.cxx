// $Id: TXXXAnlEvent.cxx 478 2009-10-29 12:26:09Z linev $
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

#include "TXXXAnlEvent.h"

//***********************************************************
TXXXAnlEvent::TXXXAnlEvent() : TGo4EventElement()
{
}
//***********************************************************
TXXXAnlEvent::TXXXAnlEvent(const char * name) : TGo4EventElement(name)
{
}
//***********************************************************
TXXXAnlEvent::~TXXXAnlEvent()
{
}

//-----------------------------------------------------------
void TXXXAnlEvent::Clear(Option_t *t)
{
  void* destfield;
  destfield = (void*) &frData[0];
  memset(destfield,0, sizeof(frData));
}
