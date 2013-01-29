// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4FitParameter.h"

#include "Riostream.h"
#include "TGo4FitParsList.h"

TGo4FitParameter::TGo4FitParameter() : TGo4FitNamed() {
    Reset();
}

TGo4FitParameter::TGo4FitParameter(const char* name, const char* title, Double_t iValue) : TGo4FitNamed(name,title) {
    Reset();
    fdValue = iValue;
}

TGo4FitParameter::TGo4FitParameter(const char* name, const char* title, Double_t iValue, Double_t iRangeMin, Double_t iRangeMax) : TGo4FitNamed(name,title) {
    Reset();
    fdValue = iValue;
    SetRange(iRangeMin,iRangeMax);
}

TGo4FitParameter::TGo4FitParameter(const char* name, const char* title, Double_t iValue, Double_t iEpsilon) : TGo4FitNamed(name,title) {
    Reset();
    fdValue = iValue;
    SetEpsilon(iEpsilon);
}

TGo4FitParameter::TGo4FitParameter(const char* name, const char* title, Double_t iValue, Double_t iRangeMin, Double_t iRangeMax, Double_t iEpsilon) : TGo4FitNamed(name,title) {
    Reset();
    fdValue = iValue;
    SetRange(iRangeMin,iRangeMax);
    SetEpsilon(iEpsilon);
}

void TGo4FitParameter::SetRange(Double_t iRangeMin, Double_t iRangeMax) {
   fbRange=kTRUE;
   fdRangeMin = iRangeMin;
   fdRangeMax = iRangeMax;
   if (fdRangeMin==fdRangeMax) { fdValue = fdRangeMin; fbFixed = kTRUE; } else
   if (fdValue<fdRangeMin) fdValue = fdRangeMin; else
   if (fdValue>fdRangeMax) fdValue = fdRangeMax;
}

TGo4FitParameter::~TGo4FitParameter() {
}

void TGo4FitParameter::Print(Option_t* option) const
{
   std::cout << "   Name: " << ((TGo4FitParameter*) this) -> GetFullName();
   std::cout << "   value=" << fdValue << "  error = " << fdError;
   if (GetFixed()) std::cout << "  fixed";
   if (fbRange) std::cout << "   range=[" << fdRangeMin << "," << fdRangeMax << "]";
   if (fbEpsilon) std::cout << "  epsilon=" << fdEpsilon;
   std::cout << std::endl;
}

void TGo4FitParameter::Reset()
{
  fdValue = 0.;
  fdError = 0.;
  fbFixed = kFALSE;
  fbEpsilon = kFALSE;
  fdEpsilon = .000001;
  fbRange = kFALSE;
  fdRangeMin = 0.;
  fdRangeMax = 0.;
  fbBlocked = kFALSE;
  fdRememberedValue = 0.;
}
