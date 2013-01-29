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

#include "TGo4FitLinearTrans.h"

#include "Riostream.h"
#include "TGo4FitParameter.h"

TGo4FitLinearTrans::TGo4FitLinearTrans() : TGo4FitAxisTrans() {}

TGo4FitLinearTrans::TGo4FitLinearTrans(const char* iName, const char* iTitle, Int_t iAxis) :
 TGo4FitAxisTrans(iName, iTitle), fiAxis(iAxis)
{
   TGo4FitParameter* par = new TGo4FitParameter("CoefK","linear coefficient of calibration", 1.);
   par->SetFixed(kTRUE);
   AddPar(par);

   par = new TGo4FitParameter("CoefB","constant value of calibration", 0.);
   par->SetFixed(kTRUE);
   AddPar(par);
}

TGo4FitLinearTrans::~TGo4FitLinearTrans() {
}

void TGo4FitLinearTrans::SetCoefByRange(Int_t nbins, Double_t y1, Double_t y2) {
  Double_t k = (nbins>0) ? (y2-y1) / nbins : 0.;
  SetCoef(k,y1);
}

void TGo4FitLinearTrans::SetCoefByPoints(Double_t n1, Double_t y1, Double_t n2, Double_t y2) {
  Double_t k = (n2-n1 != 0.) ? (y2-y1) / (n2 - n1) : 0.;
  Double_t b = y1 - fdCoefK * n1;
  SetCoef(k,b);
}

void TGo4FitLinearTrans::Transformation(Double_t* scales, Int_t naxis) {
  if ((scales!=0) && (naxis>GetAxis()))
    scales[GetAxis()] = GetCoefK() * scales[GetAxis()] + GetCoefB();
}

void TGo4FitLinearTrans::Print(Option_t* option) const {
   std::cout << "Linear transformation of " << fiAxis << " axis" << std::endl;
   TGo4FitAxisTrans::Print(option);
}
