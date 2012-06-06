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

#include "TMeshB2AnlProc.h"

#include "go4iostream.h"

#include "TGo4UserException.h"

#include "TMeshRawEvent.h"
#include "TMeshB2OutputEvent.h"
#include "TMeshB2InputEvent.h"

//***********************************************************
// this one is used in TXXXAnlFact
TMeshB2AnlProc::TMeshB2AnlProc(const char* name)
  :TGo4EventProcessor(name),fxInput(0)
{
  cout << "**** TMeshB2AnlProc: Create" << endl;
}
//***********************************************************
TMeshB2AnlProc::TMeshB2AnlProc()
  : TGo4EventProcessor("Processor2"),fxInput(0)
{
}
//***********************************************************
TMeshB2AnlProc::~TMeshB2AnlProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TMeshB2AnlProc::Analysis(TMeshB2OutputEvent* poutevt)
{
   if(fxInput==0)
      {
         // lazy init for input event from framework
         TGo4EventElement* providerinput=GetInputEvent("Input2Provider");
         TMeshRawEvent* raw=dynamic_cast<TMeshRawEvent*>(providerinput);
         if(raw)
            fxInput=&(raw->fxSub2);
               // provider delivers full raw event, we use only our component
         else
            fxInput=dynamic_cast<TMeshB2InputEvent*>(providerinput);
               // provider with partial io delivers just our component
      }
   if(fxInput)
      {
         // do the processing here:
         for(Int_t i=0;i<4;i++)
            {
               poutevt->frData[i]=(Float_t)fxInput->fiCrate2[i];
            }


      }
   else
      {
         throw TGo4UserException(3,"Error: no input event for processor %s",GetName());
      }

} // BuildCalEvent
