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

#include "TMeshB1InputEvent.h"

#include "TGo4Log.h"
#include "TMeshUnpackProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB1InputEvent::TMeshB1InputEvent()
  :TGo4EventElement("InputEvent1"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB1InputEvent::TMeshB1InputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB1InputEvent::~TMeshB1InputEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshB1InputEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("TMeshUnpackProc")){
    fxProcessor = dynamic_cast<TMeshUnpackProc*>(GetEventSource());
    TGo4Log::Info("TMeshB1InputEvent init for Unpack step");
  }
  // or is it used from provider step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxFile = dynamic_cast<TGo4FileSource*> (GetEventSource());
    TGo4Log::Info("TMeshB1InputEvent init for file input");
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TMeshB1InputEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxProcessor)
      fxProcessor->UnpackBranch1(this);
   else if(fxFile)
      fxFile->BuildEvent(this);
   else
      rev=1;
   return rev;
}

//-----------------------------------------------------------
void  TMeshB1InputEvent::Clear(Option_t *t)
{
void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fiCrate1[0];
   memset(destfield,0, sizeof(fiCrate1));
   //cout << "+++ event clear" << endl;
}
