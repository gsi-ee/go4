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

#include "TMeshB3OutputEvent.h"

#include "TGo4Log.h"
#include "TMeshB3AnlProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TMeshB3OutputEvent::TMeshB3OutputEvent()
  :TGo4EventElement("OutputEvent3"),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB3OutputEvent::TMeshB3OutputEvent(const char* name)
  :TGo4EventElement(name),fxProcessor(0),fxFile(0)
{
}
//***********************************************************
TMeshB3OutputEvent::~TMeshB3OutputEvent()
{
}
//***********************************************************


//-----------------------------------------------------------
Int_t TMeshB3OutputEvent::Init()
{
   // check for different source types
   Int_t rev = 0;
   if(CheckEventSource("TMeshB3AnlProc"))
      {
         fxProcessor = dynamic_cast<TMeshB3AnlProc*>(GetEventSource());
         TGo4Log::Info("TMeshB3OutputEvent Init for analysis step");
      }
   else if(CheckEventSource("TGo4FileSource"))
      {
         fxFile = dynamic_cast<TGo4FileSource*>(GetEventSource());
         TGo4Log::Info("TMeshB3OutputEvent Init for file input");
      }
   else
      rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TMeshB3OutputEvent::Fill()
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
void TMeshB3OutputEvent::Clear(Option_t *t)
{
   void* destfield = (void*) &frData[0];
   memset(destfield,0, sizeof(frData));
}

