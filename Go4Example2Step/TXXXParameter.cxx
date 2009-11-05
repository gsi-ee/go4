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

//***********************************************************
TXXXParameter::TXXXParameter() :
   TGo4Parameter()
{
   frP1=0;
   frP2=0;
   fbHisto=kTRUE;
}
//***********************************************************
TXXXParameter::TXXXParameter(const char* name) :
   TGo4Parameter(name)
{
   frP1=10;
   frP2=20;
   fbHisto=kTRUE;
}
//***********************************************************
TXXXParameter::~TXXXParameter()
{
}
//***********************************************************
//-----------------------------------------------------------
Int_t TXXXParameter::PrintParameter(Text_t * n, Int_t)
{
   cout << "Parameter " << GetName()<<":" <<endl;
   cout << "  P1="<<frP1<<endl;
   cout << "  P2="<<frP2<< endl;
   cout << "  fbHisto="<<fbHisto<<endl;
   return 0;
}
//-----------------------------------------------------------
Bool_t TXXXParameter::UpdateFrom(TGo4Parameter *pp)
{
   TXXXParameter* from = dynamic_cast<TXXXParameter*> (pp);
   if (from==0) {
      cout << "Wrong parameter object: " << pp->ClassName() << endl;
      return kFALSE;
   }
   cout << "Parameter " << GetName() << " updated from auto-save file!" << endl;
   frP1=from->frP1;
   frP2=from->frP2;
   fbHisto=from->fbHisto;
  return kTRUE;
}

