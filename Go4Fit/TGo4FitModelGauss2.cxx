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

#include "TGo4FitModelGauss2.h"

#include "Riostream.h"
#include "TMath.h"

#include "TGo4FitParameter.h"

TGo4FitModelGauss2::TGo4FitModelGauss2() : TGo4FitModel(), fiNaxis1(0), fiNaxis2(1) {
}

TGo4FitModelGauss2::TGo4FitModelGauss2(const char* iName, Double_t iPos1, Double_t iPos2,
                             Double_t iWidth1, Double_t iWidth2, Double_t iCov12,
                             Int_t iNaxis1, Int_t iNaxis2) :
   TGo4FitModel(iName,"2-dimensional Gaussian", kTRUE), fiNaxis1(iNaxis1), fiNaxis2(iNaxis2) {
     NewParameter("Pos0","2D gaussian position 1",iPos1);
     NewParameter("Pos1","2D gaussian position 2",iPos2);
     NewParameter("Width0","2D gaussian width 1",iWidth1);
     NewParameter("Width1","2D gaussian width 2",iWidth2);
     NewParameter("Cov0_1","2D gaussian covariation",iCov12);
}

TGo4FitModelGauss2::~TGo4FitModelGauss2() {
}


Bool_t TGo4FitModelGauss2::BeforeEval(Int_t ndim) {
   Par_mu1 = GetPar(1)->GetValue();
   Par_mu2 = GetPar(2)->GetValue();
   Par_sig1 = GetPar(3)->GetValue();
   if (Par_sig1==0.)
     { std::cout << " TGo4FitModelGauss2:: invalid Sig0 value" << std::endl; return kFALSE; }
   Par_sig2 = GetPar(4)->GetValue();
   if (Par_sig2==0.)
     { std::cout << " TGo4FitModelGauss2:: invalid Sig1 value" << std::endl; return kFALSE; }
   Par_ro = GetPar(5)->GetValue()/Par_sig1/Par_sig2;
   Par_mult = -0.5/(1.-Par_ro*Par_ro);

   if ((fiNaxis1>=ndim) || (fiNaxis2>=ndim))
     { std::cout << " TGo4FitModelGauss2:: invalid index value" << std::endl; return kFALSE; }
   return kTRUE;
}

Double_t TGo4FitModelGauss2::EvalN(const Double_t* v) {
   Double_t x1 = (v[fiNaxis1]-Par_mu1)/Par_sig1;
   Double_t x2 = (v[fiNaxis2]-Par_mu2)/Par_sig2;
   Double_t z = Par_mult*(x1*x1-2*Par_ro*x1*x2+x2*x2);
   return TMath::Exp(z);
}

void TGo4FitModelGauss2::Print(Option_t* option) const
{
    TGo4FitModel::Print(option);
    std::cout << "   2-dimensional Gauss for axis " << fiNaxis1 << " & " << fiNaxis2 << std::endl;
}
