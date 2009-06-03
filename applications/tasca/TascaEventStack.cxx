//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaEventStack.h"

#include "Riostream.h"

//***********************************************************
TascaEventStack::TascaEventStack() {}
//***********************************************************
TascaEventStack::TascaEventStack(const char* name)
{
    cout << "Tasca> TascaEventStack: " << name << " created" << endl;
}
//***********************************************************
TascaEventStack::~TascaEventStack(){
    cout << "Tasca> TascaEventStack: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaEventStack::PrintEventStack(){
  return 0;
  cout << "EventStack " << GetName()<<":" <<endl;
  return 0;
}

