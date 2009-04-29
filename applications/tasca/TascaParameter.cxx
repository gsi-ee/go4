#include "TascaParameter.h"

#include "Riostream.h"

//***********************************************************
TascaParameter::TascaParameter() : TGo4Parameter() {}
//***********************************************************
TascaParameter::TascaParameter(const char* name) : TGo4Parameter(name),
fill(1),shift(7)
{
    cout << "Tasca> TascaParameter: " << name << " created" << endl;
    fbSave=kFALSE;
}
//***********************************************************
TascaParameter::~TascaParameter(){
    cout << "Tasca> TascaParameter: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaParameter::PrintParameter(){
  return 0;
  cout << "Parameter " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaParameter::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaParameter")) {
    TascaParameter * from = (TascaParameter *) pp;
    if(from->fbSave){
    }
    fill=from->fill;
    shift=from->shift;
   cout << "Tasca> TascaParameter: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

