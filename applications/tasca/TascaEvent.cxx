//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaEvent.h"

#include "Riostream.h"

//***********************************************************
TascaEvent::TascaEvent() {}
//***********************************************************
TascaEvent::TascaEvent(const char* name)
{
    cout << "Tasca> TascaEvent: " << name << " created" << endl;
}
//***********************************************************
TascaEvent::~TascaEvent(){
    cout << "Tasca> TascaEvent: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaEvent::PrintEvent(){
  return 0;
  cout << "Event " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaEvent::UpdateFrom(TascaCheckEvent *pp){
   cout << "Tasca> TascaEvent: " << GetName() << " updated" << endl;
  return kTRUE;
}

