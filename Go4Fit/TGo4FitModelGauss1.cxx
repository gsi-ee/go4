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

#include "TGo4FitModelGauss1.h"

#include "Riostream.h"

#include "TMath.h"

#include "TGo4FitParameter.h"

TGo4FitModelGauss1::TGo4FitModelGauss1() : TGo4FitModel(), fiNaxis(0) {
}

TGo4FitModelGauss1::TGo4FitModelGauss1(const char* iName, Double_t iPosition, Double_t iWidth, Int_t iNaxis) :
   TGo4FitModel(iName,"1-dimensional Gaussian", kTRUE), fiNaxis(iNaxis) {
   NewParameter("Pos","Gaussian position",iPosition);
   NewParameter("Width","Gaussian width",iWidth);
}

TGo4FitModelGauss1::~TGo4FitModelGauss1() {
}

Bool_t TGo4FitModelGauss1::BeforeEval(Int_t ndim) {
   Par_x0 = GetPar(1)->GetValue();
   Double_t w = GetPar(2)->GetValue();
   if (w==0.)
     { cout << "TGo4FitModelGauss1::   Invalid sigma value " << endl; return kFALSE; }
   Par_k = -0.5/w/w;

   if (fiNaxis>=ndim)
     { cout << "TGo4FitModelGauss1:   invalid index "; return kFALSE; }
   return kTRUE;
}

Double_t TGo4FitModelGauss1::EvalN(const Double_t* v) {
   return TMath::Exp(Par_k*(v[fiNaxis]-Par_x0)*(v[fiNaxis]-Par_x0));
}

void TGo4FitModelGauss1::Print(Option_t* option) const {
    TGo4FitModel::Print(option);
    cout << "   1-dimensional Gauss for axis " << fiNaxis << endl;
}

Double_t TGo4FitModelGauss1::Integral() {
   return GetAmplValue() * TMath::Sqrt(2.*TMath::Pi()) * GetPar(2)->GetValue();
}
