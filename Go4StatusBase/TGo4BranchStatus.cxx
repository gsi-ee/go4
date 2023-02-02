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

#include "TGo4BranchStatus.h"

#include "TGo4Log.h"

#include "TBranchElement.h"

TGo4BranchStatus::TGo4BranchStatus() :
   TGo4ObjectStatus()
{
}

TGo4BranchStatus::TGo4BranchStatus(TBranch *branch) :
   TGo4ObjectStatus(branch, kFALSE)
{
   if (branch) {
      if (branch->InheritsFrom(TBranchElement::Class()))
         fxObjectClass = (dynamic_cast<TBranchElement*>(branch))->GetTypeName();
      else
         fxObjectClass = branch->GetClassName();
   }
   if (fxObjectClass.IsNull())
      fxObjectClass = branch ? branch->ClassName() : "BranchElement";
}

TGo4BranchStatus::~TGo4BranchStatus()
{
}

void TGo4BranchStatus::Print(Option_t *) const
{
   TGo4ObjectStatus::Print();
   PrintLine("G-OOOO-> TreeBranch Status Class Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
}
