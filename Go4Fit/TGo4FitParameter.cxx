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

void TGo4FitParameter::Print(Option_t* option) const {
   cout << "   Name: " << ((TGo4FitParameter*) this) -> GetFullName();
   cout << "   value=" << fdValue << "  error = " << fdError;
   if (GetFixed()) cout << "  fixed";
   if (fbRange) cout << "   range=[" << fdRangeMin << "," << fdRangeMax << "]";
   if (fbEpsilon) cout << "  epsilon=" << fdEpsilon;
   cout << endl;
//   cout << "   " << (void*) this << endl;
}

void TGo4FitParameter::Reset() {
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
