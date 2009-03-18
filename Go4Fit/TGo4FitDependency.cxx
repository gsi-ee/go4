#include "TGo4FitDependency.h"

#include "Riostream.h"

#include "TFormula.h"

TGo4FitDependency::TGo4FitDependency() :
    TObject(), fxParameter(), fxExpression(), fdInitValue(0.), fiNumPar(0), fxFormula(0)  {
}


TGo4FitDependency::TGo4FitDependency(const char* iParameter, const char* iExpression) :
    TObject(), fxParameter(iParameter), fxExpression(iExpression), fdInitValue(0.), fiNumPar(0), fxFormula(0) {
}

TGo4FitDependency::TGo4FitDependency(const char* iParameter, Double_t InitValue) :
    TObject(), fxParameter(iParameter), fxExpression(), fdInitValue(InitValue), fiNumPar(0), fxFormula(0) {
}

TGo4FitDependency::~TGo4FitDependency() {
    if (fxFormula) delete fxFormula;
}

void TGo4FitDependency::SetParameter(const char* iParameter) {
  fxParameter = iParameter;
}

void TGo4FitDependency::SetInitValue(Double_t InitValue) 
{
  fxExpression.Remove(0);
  fdInitValue = InitValue;

}
void TGo4FitDependency::SetExpression(const char* iExpression) 
{
  fxExpression = iExpression;
}

void TGo4FitDependency::Initialize(Int_t iNumPar, const char* iFormula) 
{
    fiNumPar = iNumPar;
    if (iFormula) {
       if (fxFormula) delete fxFormula;
       fxFormula = new TFormula("", iFormula);
    } else fxFormula=0;
}

Double_t TGo4FitDependency::ExecuteDependency(Double_t* Params) 
{
    Double_t res = (fxFormula!=0) ? fxFormula->EvalPar(0,Params) : fdInitValue;
    if (fiNumPar>=0) Params[fiNumPar] = res;
    return res;
}

void TGo4FitDependency::Finalize() {
    if (fxFormula) { delete fxFormula; fxFormula=0; }
}

void TGo4FitDependency::Print(Option_t* option) const
{
   cout << "Pars dependency:   " << fxParameter << "  =  ";
   if (fxExpression.Length()>0) cout << fxExpression << endl;
                           else cout << fdInitValue << endl;
}
