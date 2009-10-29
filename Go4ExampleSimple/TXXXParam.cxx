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

#include "TXXXParam.h"

#include "Riostream.h"

TXXXParam::TXXXParam(const char* name) : TGo4Parameter(name)
{
  fillRaw = kTRUE;
}

//***********************************************************
TXXXParam::~TXXXParam()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXParam::PrintParameter(Text_t *, Int_t)
{
   cout << "Parameter " << GetName() << ":" << endl;
   cout <<" fillRaw = " << (fillRaw ? "kTRUE" : "kFALSE") << endl;
   return 0;
}

//-----------------------------------------------------------
Bool_t TXXXParam::UpdateFrom(TGo4Parameter *source)
{
   TXXXParam* from = dynamic_cast<TXXXParam*>(source);
   if (from==0) {
      cout << "Wrong parameter class: " << source->ClassName() << endl;
      return kFALSE;
   }

   fillRaw = from->fillRaw;
   return kTRUE;
}
