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

#include "TMeshDummyEvent.h"

#include "TMeshProviderProc.h"

//***********************************************************
TMeshDummyEvent::TMeshDummyEvent()
  :TGo4EventElement("DummyEvent"),fxProviderProc(0)
{

}
//***********************************************************
TMeshDummyEvent::TMeshDummyEvent(const char* name)
  :TGo4EventElement(name),fxProviderProc(0)
{
}
//***********************************************************
TMeshDummyEvent::~TMeshDummyEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TMeshDummyEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
   Clear();
  if(CheckEventSource("TMeshProviderProc"))
   {
      fxProviderProc = dynamic_cast<TMeshProviderProc*>(GetEventSource());
      //std::cout << "**** TMeshDummyEvent init"<< std::endl;
   }
  else
    rev=1;
  return rev;

}
//-----------------------------------------------------------
Int_t TMeshDummyEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   if(fxProviderProc)
      fxProviderProc->SetRealInput();
   else
      rev=1;
   return rev;

}
//-----------------------------------------------------------
void TMeshDummyEvent::Clear(Option_t*)
{

}
