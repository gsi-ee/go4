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

#include "TMeshB12OutputEvent.h"

#include "Riostream.h"

#include "TMeshB12AnlProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB12OutputEvent::TMeshB12OutputEvent()
  :TGo4EventElement("OutputEvent12"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB12OutputEvent::TMeshB12OutputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB12OutputEvent::~TMeshB12OutputEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TMeshB12OutputEvent::Init()
{
   // check for different source types
   Int_t rev = 0;
   if(CheckEventSource("TMeshB12AnlProc"))
      {
         fxProcessor = dynamic_cast<TMeshB12AnlProc*>(GetEventSource());
         cout << "**** TMeshB12OutputEvent Init for analysis step"<< endl;
      }
   else if(CheckEventSource("TGo4FileSource"))
      {
         fxFile = dynamic_cast<TGo4FileSource*>(GetEventSource());
         cout << "**** TMeshB12OutputEvent Init for file input"<< endl;
      }
   else
      rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TMeshB12OutputEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxProcessor)
      fxProcessor->Analysis(this);
   else if(fxFile)
      fxFile->BuildEvent(this);
   else
      rev = 1;
   return rev;

}
//-----------------------------------------------------------
void TMeshB12OutputEvent::Clear(Option_t *t)
{
//-----------------------------------------------------------
void* destfield;
   destfield = (void*) &frData[0];
   memset(destfield,0, sizeof(frData));
}
