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

#include "TGo4FitterConfig.h"

#include "go4iostream.h"

#include "TString.h"

#include "TGo4FitterAbstract.h"
#include "TGo4FitParameter.h"
#include "TGo4FitDependency.h"

TGo4FitterConfig::TGo4FitterConfig() :
   TGo4FitterAction(),
   fxParsCfg(), fxParsNew(), fxParsInit(), fxParsDepend(), fxResults(), fbFixedByDefault(kFALSE) {
}

TGo4FitterConfig::TGo4FitterConfig(const char* iName, const char* iTitle) :
   TGo4FitterAction(iName,iTitle),
   fxParsCfg(kTRUE), fxParsNew(kTRUE), fxParsInit(), fxParsDepend(), fxResults(), fbFixedByDefault(kFALSE) {
      fxParsInit.SetOwner(kTRUE);
      fxParsDepend.SetOwner(kTRUE);
      fxResults.SetOwner(kTRUE);
}

TGo4FitterConfig::~TGo4FitterConfig() {
}

TGo4FitParameter* TGo4FitterConfig::MakeParForProperties(const char* ParName) {
   TGo4FitParameter* par = fxParsCfg.FindPar(ParName);
   if (par==0) par = fxParsCfg.CreatePar(ParName,"config",0.);
   return par;
}

Bool_t TGo4FitterConfig::SetParFixed(const char* ParName, Bool_t iFixed) {
   TGo4FitParameter* par = MakeParForProperties(ParName);
   if(par) par->SetFixed(iFixed);
   return (par!=0);
}

Bool_t TGo4FitterConfig::SetParRange(const char* ParName, Double_t RangeMin, Double_t RangeMax) {
   TGo4FitParameter* par = MakeParForProperties(ParName);
   if(par) par->SetRange(RangeMin,RangeMax);
   return (par!=0);
}

Bool_t TGo4FitterConfig::SetParEpsilon(const char* ParName, Double_t Epsilon) {
   TGo4FitParameter* par = MakeParForProperties(ParName);
   if(par) par->SetEpsilon(Epsilon);
   return (par!=0);
}

Bool_t TGo4FitterConfig::GetParFixed(const char* ParName) {
   return fxParsCfg.GetParFixed(ParName);
}

Bool_t TGo4FitterConfig::GetParRange(const char * ParName, Double_t & RangeMin, Double_t & RangeMax) {
   return  fxParsCfg.GetParRange(ParName, RangeMin, RangeMax);
}

Bool_t TGo4FitterConfig::GetParEpsilon(const char* ParName, Double_t& Epsilon) {
   return  fxParsCfg.GetParEpsilon(ParName, Epsilon);
}

TGo4FitDependency* TGo4FitterConfig::FindDepen(const char* FullName, TObjArray* list) {
   for(Int_t n=0;n<=list->GetLast();n++) {
      TGo4FitDependency* par = (TGo4FitDependency*) list->At(n);
      if (strcmp(par->GetParameter().Data(),FullName)==0) return par;
   }
   return 0;
}

void TGo4FitterConfig::SetParInit(const char* FullName, Double_t iValue) {
   TGo4FitDependency* par  = FindDepen(FullName,&fxParsInit);
   if(par) par->SetInitValue(iValue);
      else fxParsInit.Add( new TGo4FitDependency(FullName,iValue));
}

void TGo4FitterConfig::SetParInit(const char* FullName, const char* iExpression) {
   TGo4FitDependency* par  = FindDepen(FullName,&fxParsInit);
   if(par) par->SetExpression(iExpression);
      else fxParsInit.Add( new TGo4FitDependency(FullName,iExpression));
}

void TGo4FitterConfig::SetParDepend(const char* FullName, const char* iExpression) {
   TGo4FitDependency* par  = FindDepen(FullName,&fxParsDepend);
   if(par) par->SetExpression(iExpression);
      else fxParsDepend.Add( new TGo4FitDependency(FullName,iExpression));
}

void TGo4FitterConfig::AddResult(const char* Expression) {
  fxResults.Add( new TGo4FitDependency("",Expression));
}

void TGo4FitterConfig::AddResult(Double_t Value) {
  fxResults.Add( new TGo4FitDependency("",Value));
}

void TGo4FitterConfig::DoAction(TGo4FitterAbstract* Fitter) {
   if (Fitter) Fitter->ApplyConfig(this);
}

void TGo4FitterConfig::Print(Option_t* option) const {
   TGo4FitterAction::Print(option);
   cout << "List of minimization config for parameters: " << endl;
   fxParsCfg.Print(option);
   cout << "List of new parameters declarations: " << endl;
   fxParsNew.Print(option);
   cout << "Order of parameters initialization: " << endl;
   fxParsInit.Print(option);
   cout << "Dependency for parameters: " << endl;
   fxParsDepend.Print(option);
   cout << "Results values: " << endl;
   fxResults.Print(option);
}
