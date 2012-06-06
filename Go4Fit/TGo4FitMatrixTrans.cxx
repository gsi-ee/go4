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

#include "TGo4FitMatrixTrans.h"

#include "go4iostream.h"

#include "TArrayD.h"
#include "TString.h"
#include "TMath.h"

#include "TGo4FitParameter.h"

TGo4FitMatrixTrans::TGo4FitMatrixTrans() : TGo4FitAxisTrans() {

}

TGo4FitMatrixTrans::TGo4FitMatrixTrans(const char* iName, const char* iTitle, Int_t NumAxis) :
  TGo4FitAxisTrans(iName, iTitle)
{
   SetNumAxis(NumAxis);
}

TGo4FitMatrixTrans::~TGo4FitMatrixTrans()
{
}

Int_t TGo4FitMatrixTrans::GetNumAxis()
{
   return Int_t(TMath::Sqrt(1.*NumPars()));
}


void TGo4FitMatrixTrans::SetNumAxis(Int_t NumAxis) {
  Int_t oldnum = GetNumAxis();
  TArrayD oldzn(NumPars());
  GetParsValues(oldzn.GetArray());

  ClearPars();

  for(Int_t n1=0;n1<NumAxis;n1++)
    for(Int_t n2=0;n2<NumAxis;n2++) {
       TString name("Matr");
       name+="_"; name+=n1;
       name+="_"; name+=n2;
       Double_t zn = 0.;
       if((n1<oldnum) && (n2<oldnum)) zn = oldzn[n1*oldnum+n2];
       CreatePar(name.Data(),"matrix element",zn);
    }
}

void TGo4FitMatrixTrans::Transformation(Double_t* scales, Int_t naxis) {
  Int_t num = GetNumAxis();
  if ((scales==0) || (num==0) || (naxis==0)) return;
  Int_t limit = num<naxis ? num : naxis;
  TArrayD oldscales(naxis, scales);
  for (Int_t n1=0;n1<limit;n1++) {
    Double_t sum = 0.;
    for (Int_t n2=0;n2<limit;n2++)
      sum += oldscales[n2] * GetPar(n1*num + n2)->GetValue();
    scales[n1] = sum;
  }
}

void TGo4FitMatrixTrans::Print(Option_t* option) const {
   cout << "Matrix transformation of scale values " << endl;
   TGo4FitAxisTrans::Print(option);
}
