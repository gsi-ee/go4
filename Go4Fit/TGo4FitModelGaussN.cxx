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

#include "TGo4FitModelGaussN.h"

#include "Riostream.h"
#include "TMath.h"
#include "TGo4FitParameter.h"

TGo4FitModelGaussN::TGo4FitModelGaussN() : TGo4FitModel(), fxIndexes(),
    Vect_mu(0),Matr_sig(0), Vect_x(0), Vect_dx(0) {
}

TGo4FitModelGaussN::TGo4FitModelGaussN(const char* iName, Int_t iNDimension) :
   TGo4FitModel(iName,"N-dimensional Gaussian", kTRUE), fxIndexes(iNDimension),
      Vect_mu(0),Matr_sig(0), Vect_x(0), Vect_dx(0) {
       for(Int_t n=0;n<iNDimension;n++) fxIndexes[n] = n;

       for(Int_t n=0;n<iNDimension;n++)
          NewParameter(GetPosParName(n),"position",1.);

       for(Int_t n=0;n<iNDimension;n++)
          NewParameter(GetWidthParName(n),"width",.5);

       for(Int_t n1=0;n1<iNDimension;n1++)
         for(Int_t n2=n1+1;n2<iNDimension;n2++)
            NewParameter(GetCovarParName(n1,n2),"covariation",0.);
}

TGo4FitModelGaussN::~TGo4FitModelGaussN() {
   if(Vect_mu) { delete Vect_mu; Vect_mu = 0; }
   if(Matr_sig) { delete Matr_sig; Matr_sig = 0; }
   if(Vect_x) { delete Vect_x; Vect_x = 0; }
   if (Vect_dx) { delete Vect_dx; Vect_dx = 0; }
}

TString TGo4FitModelGaussN::GetPosParName(Int_t naxis)
{
   TString res;
   res.Form("Pos%d",naxis);
   return res;
}

TString TGo4FitModelGaussN::GetWidthParName(Int_t naxis)
{
   TString res;
   res.Form("Width%d",naxis);
   return res;
}

TString TGo4FitModelGaussN::GetCovarParName(Int_t naxis1, Int_t naxis2)
{
   TString res;
   res.Form("Cov%d_%d",naxis1,naxis2);
   return res;
}



Bool_t TGo4FitModelGaussN::SetAxisNumbers(Int_t naxis) {
  Int_t oldnaxis = GetAxisNumbers();
  if ((naxis<0) || (oldnaxis==naxis)) return kFALSE;
  if(naxis<oldnaxis) {
     for(Int_t n=naxis;n<oldnaxis;n++) {
        RemovePar(GetPosParName(n));
        RemovePar(GetWidthParName(n));
     }
     for(Int_t n1=0;n1<oldnaxis;n1++)
       for(Int_t n2=n1+1;n2<oldnaxis;n2++)
         if((n1>=naxis) || (n2>=naxis))
           RemovePar(GetCovarParName(n1,n2));
      fxIndexes.Set(naxis);
  } else {
     Int_t lastindx = 0;
     for(Int_t n=oldnaxis;n<naxis;n++) {
        lastindx = GetParIndex(FindPar(GetPosParName(n-1)))+1;
        NewParameter(GetPosParName(n),"position",1.,kFALSE,lastindx);
        lastindx = GetParIndex(FindPar(GetWidthParName(n-1)))+1;
        NewParameter(GetWidthParName(n),"width",5.,kFALSE,lastindx);
     }
     lastindx++;
     for(Int_t n1=0;n1<naxis;n1++)
       for(Int_t n2=n1+1;n2<naxis;n2++) {
          TGo4FitParameter* par =  FindPar(GetCovarParName(n1,n2));
          if (par) lastindx = GetParIndex(par)+1;
              else NewParameter(GetCovarParName(n1,n2),"covariation",0.,kFALSE,lastindx++);
       }

     fxIndexes.Set(naxis);
     for (Int_t n=oldnaxis;n<naxis;n++) {
        Int_t test = 0;
        Bool_t find;
        do {
          find = kFALSE;
          for (Int_t n1=0;n1<n;n1++)
            if(test==fxIndexes[n1]) { find=kTRUE; test++; break; }
        } while(find);
        fxIndexes[n] = test;
     }
  }

  return kTRUE;
}

Bool_t TGo4FitModelGaussN::ResortIndexes(Int_t leaveaxis) {
   Bool_t res = kFALSE;
   for (Int_t n=fxIndexes.GetSize()-1;n>=0;n--) {
      if (n==leaveaxis) continue;
      Int_t test = fxIndexes[n];
      Bool_t find, first = kTRUE;
      do {
        find = kFALSE;
        for (Int_t n1=0;n1<fxIndexes.GetSize();n1++)
          if((n!=n1) && (test==fxIndexes[n1])) {
            if (first) test=0; else test++;
            find = kTRUE;
            first = kFALSE;
            break;
          }
      } while(find);
      fxIndexes[n] = test;
      res = res || (!first);
   }
   return res;
}


Int_t TGo4FitModelGaussN::GetPosParIndex(Int_t naxis) {
   for(Int_t n=0;n<fxIndexes.GetSize();n++)
     if (naxis==fxIndexes[n]) return 1+n;
   return -1;
}

Int_t TGo4FitModelGaussN::GetWidthParIndex(Int_t naxis) {
   for(Int_t n=0;n<fxIndexes.GetSize();n++)
     if (naxis==fxIndexes[n]) return 1+fxIndexes.GetSize()+n;
   return -1;
}

void TGo4FitModelGaussN::FillMuVector(TVectorD& Mu) {
   Int_t ndim = fxIndexes.GetSize();
   Mu.ResizeTo(ndim);
   for(Int_t n=0;n<ndim;n++)
     Mu(n) = GetPar(1+n)->GetValue();
}

void TGo4FitModelGaussN::FillSigmaMatrix(TMatrixD& Sigma)
{
   Int_t ndim = fxIndexes.GetSize();
   Sigma.ResizeTo(ndim,ndim);
   for(Int_t n=0;n<ndim;n++)
     Sigma(n,n) = GetPar(1+ndim+n)->GetValue() * GetPar(1+ndim+n)->GetValue();
   Int_t indx = 1+2*ndim;
   for(Int_t n1=0;n1<ndim;n1++)
     for(Int_t n2=n1+1;n2<ndim;n2++) {
        Double_t zn = GetPar(indx++)->GetValue();
        Sigma(n1,n2) = zn;
        Sigma(n2,n1) = zn;
     }
}

Bool_t TGo4FitModelGaussN::BeforeEval(Int_t NDimension)
{
   Par_ndim = fxIndexes.GetSize();
   Par_indx = fxIndexes.GetArray();
   for (Int_t i=0;i<Par_ndim;i++)
     if (Par_indx[i]>=NDimension)
       { std::cout << "TGo4FitModelGaussN::   invalid index " << std::endl; return kFALSE; }

   Vect_mu = new TVectorD(Par_ndim);
   Matr_sig = new TMatrixD(Par_ndim,Par_ndim);
   Vect_x  = new TVectorD(Par_ndim);
   Vect_dx  = new TVectorD(Par_ndim);

   FillMuVector(*Vect_mu);
   FillSigmaMatrix(*Matr_sig);
   Double_t determ;
   Matr_sig->Invert(&determ);
   if (determ==0.)
     { std::cout << "TGo4FitModelGaussN::   Invalid sigma matrice " << std::endl; AfterEval(); return kFALSE; }

   return kTRUE;
}

Double_t TGo4FitModelGaussN::EvalN(const Double_t* v) {
   for (Int_t n=0;n<Par_ndim;n++)
     (*Vect_x)(n) = v[Par_indx[n]];
   *Vect_x -= *Vect_mu;
   *Vect_dx = *Vect_x;
   *Vect_dx *= *Matr_sig;
   Double_t z = *Vect_dx * *Vect_x; // see 11.1
   return TMath::Exp(-0.5*z);
}

void TGo4FitModelGaussN::AfterEval() {
   if(Vect_mu) { delete Vect_mu; Vect_mu = 0; }
   if(Matr_sig) { delete Matr_sig; Matr_sig = 0; }
   if(Vect_x) { delete Vect_x; Vect_x = 0; }
   if (Vect_dx) { delete Vect_dx; Vect_dx = 0; }
}

void TGo4FitModelGaussN::Print(Option_t* option) const
{
    TGo4FitModel::Print(option);
    std::cout << "   N-dimensional Gauss" << std::endl;
    std::cout << "   axis indexes:";
    for (Int_t i=0;i<fxIndexes.GetSize();i++)
       std::cout << " " << fxIndexes[i];
    std::cout << std::endl;
}
