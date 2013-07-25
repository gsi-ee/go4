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

#include "TYYYUnpackEvent.h"

//***********************************************************
TYYYUnpackEvent::TYYYUnpackEvent() :
   TGo4EventElement()
{
}
//***********************************************************
TYYYUnpackEvent::TYYYUnpackEvent(const char* name) :
   TGo4EventElement(name)
{
}
//***********************************************************
TYYYUnpackEvent::~TYYYUnpackEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
void  TYYYUnpackEvent::Clear(Option_t *t)
{
   void* destfield;
   //std::cout << "+++ event clear" << std::endl;
   destfield = (void*) &fdR[0];
   memset(destfield, 0, sizeof(fdR));
   destfield = (void*) &fdV[0];
   memset(destfield, 0, sizeof(fdV));
   destfield = (void*) &fdGam[0];
   memset(destfield, 0, sizeof(fdGam));
   fiNumScatt=0;
   fdEmittX=0;
   fdEmittY=0;
   fdEmitt4d=0;
}
