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

#include "Riostream.h"

TXXXParameter::TXXXParameter(const char* name) :
   TGo4Parameter(name)
{
   frP1=10;
   frP2=20;
   fbHisto=kTRUE;
}

TXXXParameter::~TXXXParameter()
{
}

Bool_t TXXXParameter::UpdateFrom(TGo4Parameter *pp)
{
   TXXXParameter* from = dynamic_cast<TXXXParameter*> (pp);
   if (from==0) {
      cout << "Wrong parameter object: " << pp->ClassName() << endl;
      return kFALSE;
   }
   cout << "Updating parameter " << GetName() << " values!" << endl;
   frP1 = from->frP1;
   frP2 = from->frP2;
   fbHisto = from->fbHisto;
   return kTRUE;
}
