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

#include "TGo4FitModelPolynom.h"

#include "Riostream.h"

#include "TMath.h"
#include "TArrayD.h"

#include "TGo4FitParameter.h"

TGo4FitModelPolynom::TGo4FitModelPolynom() : TGo4FitModel(), fxAllOrders(0) {
}

TGo4FitModelPolynom::TGo4FitModelPolynom(const char* iName, Double_t iOrderX) :
   TGo4FitModel(iName,"polynomial function",kTRUE), fxAllOrders(0) {
     CreateOrdersPars(&iOrderX, 0, 0);
     SetBackgroundGroupIndex();
}

TGo4FitModelPolynom::TGo4FitModelPolynom(const char* iName, Double_t iOrderX, Double_t iOrderY) :
   TGo4FitModel(iName,"polynomial function",kTRUE), fxAllOrders(0) {
       Double_t arr[2] = { iOrderX, iOrderY };
       CreateOrdersPars(arr, 0, 1);
       SetBackgroundGroupIndex();
}

TGo4FitModelPolynom::TGo4FitModelPolynom(const char* iName, Double_t iOrderX, Double_t iOrderY, Double_t iOrderZ) :
   TGo4FitModel(iName,"polynomial function",kTRUE), fxAllOrders(0) {
       Double_t arr[3] = { iOrderX, iOrderY, iOrderZ };
       CreateOrdersPars(arr, 0, 2);
       SetBackgroundGroupIndex();
}

TGo4FitModelPolynom::TGo4FitModelPolynom(const char* iName, const TArrayD& iPolynomOrders) :
  TGo4FitModel(iName,"polynomial function",kTRUE), fxAllOrders(0) {
      CreateOrdersPars(iPolynomOrders.GetArray(),0,iPolynomOrders.GetSize()-1);
      SetBackgroundGroupIndex();
}

TGo4FitModelPolynom::~TGo4FitModelPolynom() {
  if (fxAllOrders) delete fxAllOrders;
}

void TGo4FitModelPolynom::CreateOrdersPars(const Double_t* orders, Int_t startaxis, Int_t maxaxis, Int_t AtIndx) {
   for(Int_t n=startaxis;n<=maxaxis;n++) {
      TString infostr;
      infostr.Form("Polynom order for axis %d",n);
      if (orders) NewParameter(GetOrderParName(n),infostr.Data(),orders[n],kTRUE, AtIndx);
            else  NewParameter(GetOrderParName(n),infostr.Data(),0.,kTRUE, AtIndx);
   }
}

TString TGo4FitModelPolynom::GetOrderParName(Int_t naxis)
{
   TString res;
   res.Form("Order%d",naxis);
   return res;
}

Double_t TGo4FitModelPolynom::GetPolynomOrder(Int_t naxis) {
   TGo4FitParameter *par = FindPar(GetOrderParName(naxis));
   if (par==0) return 0.;
          else return par->GetValue();
}

Int_t TGo4FitModelPolynom::GetMaxNumAxis() {
   Int_t naxis = 0;
   while (FindPar(GetOrderParName(naxis))!=0) naxis++;
   return naxis-1;
}

Bool_t TGo4FitModelPolynom::SetMaxNumAxis(Int_t numaxis) {
   Int_t numaxisold = GetMaxNumAxis();
   if ((numaxis<0) || (numaxis==numaxisold)) return kFALSE;
   if (numaxis<numaxisold)
     for(Int_t n=numaxis+1;n<=numaxisold;n++)
        RemovePar(GetOrderParName(n));
   else {
      Int_t indx = GetParIndex(FindPar(GetOrderParName(numaxisold)));
      CreateOrdersPars(0,numaxisold+1,numaxis, indx+1);
   }

   return kTRUE;
}

Bool_t TGo4FitModelPolynom::BeforeEval(Int_t NDimension) {
   if (fxAllOrders) delete fxAllOrders;
   Par_ndim = NDimension;
   fxAllOrders = new TArrayD(Par_ndim);
   fxAllOrders->Reset(0.);
   Par_orders = fxAllOrders->GetArray();
   for(Int_t i=0;i<Par_ndim;i++)
      Par_orders[i]=GetPolynomOrder(i);
   return kTRUE;
}

Double_t TGo4FitModelPolynom::EvalN(const Double_t* v) {
    Double_t zn = 1.;
    for(Int_t n=0;n<Par_ndim;n++)
       zn *= TMath::Power(v[n],Par_orders[n]);
    return zn;
}

void TGo4FitModelPolynom::AfterEval() {
   if (fxAllOrders) { delete fxAllOrders; fxAllOrders=0; }
}

void TGo4FitModelPolynom::Print(Option_t* option) const {
   TGo4FitModel::Print(option);
}
