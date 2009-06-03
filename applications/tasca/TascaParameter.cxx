//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaParameter.h"

#include "Riostream.h"

//***********************************************************
TascaParameter::TascaParameter() : TGo4Parameter() {}
//***********************************************************
TascaParameter::TascaParameter(const char* name) : TGo4Parameter(name),
Save(0),Fill(1),shift(5),EventStackSize(10)
{
    cout << "Tasca> TascaParameter: " << name << " created" << endl;
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
    if(from->Save){
    }
    Fill=from->Fill;
    shift=from->shift;
    EventStackSize=from->EventStackSize;
   cout << "Tasca> TascaParameter: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

