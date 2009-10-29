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

#include "TGo4FitterAction.h"


TGo4FitterAction::TGo4FitterAction() : TGo4FitNamed() {
}

TGo4FitterAction::TGo4FitterAction(const char* Name, const char* Title, TNamed* Owner) :
   TGo4FitNamed(Name,Title,Owner) {
}

TGo4FitterAction::~TGo4FitterAction() {
}
