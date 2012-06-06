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

#include "TMeshB2InputEvent.h"

#include "go4iostream.h"

#include "TMeshUnpackProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB2InputEvent::TMeshB2InputEvent()
  :TGo4EventElement("InputEvent2"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB2InputEvent::TMeshB2InputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB2InputEvent::~TMeshB2InputEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TMeshB2InputEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("TMeshUnpackProc")){
    fxProcessor = dynamic_cast<TMeshUnpackProc*>(GetEventSource());
    cout << "**** TMeshB2InputEvent init for Unpack step"<< endl;
  }
  // or is it used from provider step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxFile = dynamic_cast<TGo4FileSource*> (GetEventSource());
    cout << "**** TMeshB2InputEvent init for file input"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TMeshB2InputEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxProcessor)
      fxProcessor->UnpackBranch2(this);
   else if(fxFile)
      fxFile->BuildEvent(this);
   else
      rev=1;
   return rev;
}

//-----------------------------------------------------------
void  TMeshB2InputEvent::Clear(Option_t *t)
{
void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fiCrate2[0];
   memset(destfield,0, sizeof(fiCrate2));
   //cout << "+++ event clear" << endl;
}
