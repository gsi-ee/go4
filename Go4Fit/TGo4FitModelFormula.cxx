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

#include "TGo4FitModelFormula.h"

#include "Riostream.h"
#include "TFormula.h"
#include "TGo4FitParameter.h"

TGo4FitModelFormula::TGo4FitModelFormula() : TGo4FitModel(),
   fxExpression(), fxPosIndex(), fxWidthIndex(), fxFormula(0) {
}

TGo4FitModelFormula::TGo4FitModelFormula(const char* iName, const char* iExpressionStr, Int_t iNPars, Bool_t AddAmplitude) :
  TGo4FitModel(iName,"based on TFormula line shape",AddAmplitude),
  fxExpression(iExpressionStr), fxPosIndex(), fxWidthIndex(), fxFormula(0) {

   for (Int_t n=0;n<iNPars;n++)
     NewParameter(GetExprParName(n),"formula parameter",0.);
}


TGo4FitModelFormula::~TGo4FitModelFormula() {
  CloseFormula();
}

Int_t TGo4FitModelFormula::GetNumberOfExprPar() {
   Int_t num = TGo4FitParsList::NumPars();
   if (GetAmplPar()) num--;
   return num;
}

TGo4FitParameter* TGo4FitModelFormula::GetExprPar(Int_t n) {
   if ((n<0) || (n>=GetNumberOfExprPar())) return 0;
   if ((GetAmplIndex()>=0) && (n>=GetAmplIndex())) n++;
   return GetPar(n);
}

Bool_t TGo4FitModelFormula::SetNumberOfExprPar(Int_t num) {
   Int_t oldnum = GetNumberOfExprPar();
   if ((num<0) || (oldnum==num)) return kFALSE;
   if(num<oldnum) {
      for(Int_t n=oldnum-1;n>=num;n--)
        RemovePar(GetExprPar(n)->GetName());
   } else {
      for(Int_t n=oldnum;n<num;n++)
        NewParameter(GetExprParName(n),"formula parameter",0.);
   }
   return kTRUE;
}

void TGo4FitModelFormula::SetPosParIndex(Int_t naxis, Int_t indx) {
  if (naxis<0) return;
  Int_t oldsize = fxPosIndex.GetSize();
  if (naxis>=oldsize) {
    fxPosIndex.Set(naxis+1);
    for (Int_t n=oldsize;n<fxPosIndex.GetSize();n++) fxPosIndex[n] = -1;
  }
  fxPosIndex[naxis] = indx;
}

void TGo4FitModelFormula::SetWidthParIndex(Int_t naxis, Int_t indx) {
  if (naxis<0) return;
  Int_t oldsize = fxWidthIndex.GetSize();
  if (naxis>=oldsize) {
    fxWidthIndex.Set(naxis+1);
    for (Int_t n=oldsize;n<fxWidthIndex.GetSize();n++) fxWidthIndex[n] = -1;
  }
  fxWidthIndex[naxis] = indx;
}

Int_t TGo4FitModelFormula::GetPosParIndex(Int_t naxis) {
   if ((naxis<0) || (naxis>=fxPosIndex.GetSize())) return -1;
   return GetParIndex(GetExprPar(fxPosIndex[naxis]));
}

Int_t TGo4FitModelFormula::GetWidthParIndex(Int_t naxis) {
   if ((naxis<0) || (naxis>=fxWidthIndex.GetSize())) return -1;
   return GetParIndex(GetExprPar(fxWidthIndex[naxis]));
}

TString TGo4FitModelFormula::GetExprParName(Int_t n)
{
   TString res;
   res.Form("Par%d",n);
   return res;
}

Bool_t TGo4FitModelFormula::Initialize(Int_t UseBuffers) {
   if (!CompileFormula()) return kFALSE;
   CloseFormula();
   return TGo4FitModel::Initialize(UseBuffers);
}

Bool_t TGo4FitModelFormula::BeforeEval(Int_t ndim) {
  if (!CompileFormula()) return kFALSE;
  Par_ndim = ndim;
  for(Int_t n=0;n<NumPars();n++)
    fxFormula->SetParameter(n, GetPar(n)->GetValue());
  return kTRUE;
}

Double_t TGo4FitModelFormula::EvalN(const Double_t* v) {
   switch (Par_ndim) {
       case 0: return 0.;
       case 1: return fxFormula->Eval(v[0]);
       case 2: return fxFormula->Eval(v[0],v[1]);
       case 3: return fxFormula->Eval(v[0],v[1],v[2]);
       default: return fxFormula->EvalPar(v, 0);
   }
}

void TGo4FitModelFormula::AfterEval() {
  CloseFormula();
}

void TGo4FitModelFormula::Finalize() {
  CloseFormula();
  TGo4FitModel::Finalize();
}

void TGo4FitModelFormula::Print(Option_t* option) const {
   TGo4FitModel::Print(option);
   std::cout << "  Expression = " << *fxExpression << std::endl;
   for (Int_t naxis=0;naxis<fxPosIndex.GetSize();naxis++) {
     TGo4FitParameter* par = ((TGo4FitModelFormula*) this)->GetExprPar(fxPosIndex[naxis]);
     if (par)
        std::cout << "  Position on " << naxis << " axis is " << par->GetName() << std::endl;
   }
   for (Int_t naxis=0;naxis<fxWidthIndex.GetSize();naxis++) {
     TGo4FitParameter* par = ((TGo4FitModelFormula*) this)->GetExprPar(fxWidthIndex[naxis]);
     if (par)
        std::cout << "  Width on " << naxis << " axis is " << par->GetName() << std::endl;
   }
}

Bool_t TGo4FitModelFormula::CompileFormula()
{
   CloseFormula();

   TString Expr(fxExpression);
   for (Int_t n=0;n<NumPars();n++) {
      TString str;
      str.Form("[%d]",n);
      Expr.ReplaceAll(GetParName(n), str);
   }

   fxFormula = new TFormula("Expression", Expr);

   Int_t err = fxFormula->Compile(Expr);
   if (err!=0) {
      std::cerr << "Error in formula: " << fxExpression.Data() << "     code " << err  << std::endl;
      CloseFormula();
      return kFALSE;
   }
   return kTRUE;
}

void TGo4FitModelFormula::CloseFormula() {
   if (fxFormula) { delete fxFormula; fxFormula = 0; }
}
