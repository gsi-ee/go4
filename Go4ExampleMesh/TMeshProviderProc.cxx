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

#include "TMeshProviderProc.h"

#include "go4iostream.h"

#include "TGo4Analysis.h"
#include "TGo4EventElement.h"
#include "TGo4UserException.h"

//***********************************************************
// this one is used in TXXXAnlFact
TMeshProviderProc::TMeshProviderProc(const char* name)
  :TGo4EventProcessor(name)
{
  cout << "**** TMeshProviderProc: Create" << endl;
}
//***********************************************************
TMeshProviderProc::TMeshProviderProc()
  : TGo4EventProcessor("ProviderProcessor")
{
}
//***********************************************************
TMeshProviderProc::~TMeshProviderProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TMeshProviderProc::SetRealInput()
{
TGo4EventElement* myinput=GetInputEvent();
if(myinput==0 ||
      (myinput!=0 && myinput->CheckEventSource("TGo4EventProcessor")))
   {
      // source of our input event is processor and not file:
      // we have to reset the pointer from framework!
      // the trick is: name of this processor (up to the "_") is name of required event
      // note that TGo4StepFactory forbids to use same name for different objects,
      // since object name is used as pointer name in the processline call
      TString eventname=GetName();
      eventname.Resize(eventname.Index("_",1)); //
      //cout <<"SetRealInput of"<<GetName()<<" sees eventname "<<eventname.Data() << endl;
      myinput=TGo4Analysis::Instance()->GetEventStructure(eventname.Data());
      if(myinput==0)
         throw TGo4UserException(3,"Error setting real input event %s", GetName());
      else
         SetInputEvent(myinput);
   }
//else if(myinput!=0 && myinput->CheckEventSource("TGo4FileSource"))
//   {
//      // this provider reads from file. ok
//
//   }
//else
//   {
//      // neither processor or file input: something is very wrong
//      throw TGo4UserException(3,"No source found for provider %s", GetName());
//   }
// we skip the last checks for performance reasons...

}
