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

#include "TGo4FitAxisTrans.h"

TGo4FitAxisTrans::TGo4FitAxisTrans() : TGo4FitParsList() {
}

TGo4FitAxisTrans::TGo4FitAxisTrans(const char* iName, const char* iTitle) :
  TGo4FitParsList(iName, iTitle, kTRUE) {
}

TGo4FitAxisTrans::~TGo4FitAxisTrans() {
}
