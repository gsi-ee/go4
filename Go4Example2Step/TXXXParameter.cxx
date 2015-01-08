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

#include "TXXXParameter.h"

TXXXParameter::TXXXParameter(const char* name) :
   TGo4Parameter(name),
   fArr()
{
   frP1 = 10;
   frP2 = 20;
   fbHisto = kTRUE;

   for (Int_t n1=0;n1<3;n1++)
      for (Int_t n2=0;n2<4;n2++)
        fArr2[n1][n2] = 0;
}
