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

#include "TMeshFinalProc.h"

#include "go4iostream.h"

#include "TMeshFinalEvent.h"

//***********************************************************
// this one is used in TXXXAnlFact
TMeshFinalProc::TMeshFinalProc(const char* name)
  :TGo4EventProcessor(name),fxInput1(0),fxInput2(0),fxInput3(0),fxInput12(0)
{
  cout << "**** TMeshFinalProc: Create" << endl;
}
//***********************************************************
TMeshFinalProc::TMeshFinalProc()
  : TGo4EventProcessor("FinalProcessor"),fxInput1(0),fxInput2(0),fxInput3(0),fxInput12(0)
{
}
//***********************************************************
TMeshFinalProc::~TMeshFinalProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TMeshFinalProc::Analysis(TMeshFinalEvent* poutevt)
{
if(poutevt==0) return;
/////// initialization section: ///////////////////////7
if(fxInput1==0)
      {
         fxInput1=dynamic_cast<TMeshB1OutputEvent*>(GetInputEvent("Output1Provider"));
      }
if(fxInput2==0)
      {
         fxInput2=dynamic_cast<TMeshB2OutputEvent*>(GetInputEvent("Output2Provider"));
      }
if(fxInput3==0)
      {
         fxInput3=dynamic_cast<TMeshB3OutputEvent*>(GetInputEvent("Output3Provider"));
      }
if(fxInput12==0)
      {
         fxInput12=dynamic_cast<TMeshB12OutputEvent*>(GetInputEvent("Output12Provider"));
      }

/////// final processing section: //////////////
//if(fxInput12 && fxInput3)
//   {
//      // normal case, combine all final results
//
//   }
//else if (fxInput1 && fxInput2 && fxInput3 )
//   {
//      // work on these. any combination of inputs may be processed
//      // depending on the previous setup
//      // we might steer the final processing via parameter
//   }
//else
//      {
//         throw TGo4UserException(3,"Error: not all input events available for processor %s",GetName(););
//      }

// for the moment, we just copy the component results into final event;
if(fxInput1)
      poutevt->fxB1=*fxInput1; // be sure assignment operator is correct!
if(fxInput2)
      poutevt->fxB2=*fxInput2; // be sure assignment operator is correct!
if(fxInput3)
      poutevt->fxB3=*fxInput3; // be sure assignment operator is correct!
if(fxInput12)
      poutevt->fxB12=*fxInput12; // be sure assignment operator is correct!


}
