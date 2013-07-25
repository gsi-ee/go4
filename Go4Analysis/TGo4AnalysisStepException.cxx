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

#include "TGo4AnalysisStepException.h"

#include "TGo4Log.h"
#include "TGo4AnalysisStep.h"

TGo4AnalysisStepException::TGo4AnalysisStepException (TGo4AnalysisStep* step)
: fxStep(0)
   {
   GO4TRACE((14,"TGo4AnalysisStepException:TGo4AnalysisStepException (TGo4TaskHandler*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Analysis Step Exception --!!!";
   if(step)
      {
         fxStep=step;
         fxStepName=fxStep->GetName();
         fxStatusMessage=fxStep->GetStatusMessage();
         fiProcessStatus=fxStep->GetProcessStatus();
      }
}

TGo4AnalysisStepException::~TGo4AnalysisStepException()
{
 GO4TRACE((14,"TGo4AnalysisStepException:~TGo4AnalysisStepException", __LINE__, __FILE__));

}

TGo4AnalysisStepException & TGo4AnalysisStepException::operator= (const TGo4AnalysisStepException & right)
{
 GO4TRACE((14,"TGo4AnalysisStepException:operator=",__LINE__, __FILE__));
 if (&right!=this)
    {
      TGo4RuntimeException::operator=(right); // copy base class members
      // put additional member copies here...
      fxStep=right.fxStep;
      fxStepName=right.fxStepName;
      fxStatusMessage=right.fxStatusMessage;
      fiProcessStatus=right.fiProcessStatus;
      return *this;
    }
  else
    {
      // copy is already source object
      return *this;
    }



}
TGo4AnalysisStepException::TGo4AnalysisStepException(const TGo4AnalysisStepException &right)
  :TGo4RuntimeException(right)
{
  GO4TRACE((14,"TGo4AnalysisStepException:TGo4AnalysisStepException(right)", __LINE__, __FILE__));   ;
  fxStep=right.fxStep;
  fxStepName=right.fxStepName;
  fxStatusMessage=right.fxStatusMessage;
  fiProcessStatus=right.fiProcessStatus;
}

Int_t TGo4AnalysisStepException::Handle()
{
   GO4TRACE((12,"TGo4AnalyisStepException::Handle()", __LINE__, __FILE__));

   TGo4Log::Error("%s \n Step %s, ProcessStatus:%d, %s",
            What(),GetStepName(),GetProcessStatus(),GetStatusMessage());
   return 0;
}
