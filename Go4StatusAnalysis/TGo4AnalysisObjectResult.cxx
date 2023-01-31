// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4AnalysisObjectResult.h"

#include "TROOT.h"

#include "TGo4AnalysisObjectNames.h"


TGo4AnalysisObjectResult::TGo4AnalysisObjectResult() :
   TGo4Status()
{
}

TGo4AnalysisObjectResult::TGo4AnalysisObjectResult(const char *name) :
   TGo4Status(name)
{
}

TGo4AnalysisObjectResult::~TGo4AnalysisObjectResult()
{
   // names list is not owned by the result object
   //  delete fxNamesList;
   // fxNamesList = nullptr;
}

TGo4AnalysisObjectNames *TGo4AnalysisObjectResult::GetNamesList(Bool_t chown)
{
   TGo4AnalysisObjectNames *reval = fxNamesList;
   if(chown) fxNamesList = nullptr;
   return reval;
}

void TGo4AnalysisObjectResult::Print(Option_t*) const
{
   gROOT->SetDirLevel(0);
   PrintLine("G-OOOO-> Analysis Object Result Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");

   if(fxNamesList)
      fxNamesList->Print();

   PrintLine(" Full object name: \t%s", GetObjectFullName());
   PrintLine(" Message: \t%s", GetMessage());
   PrintLine(" Action: \t%d", Action());
}
