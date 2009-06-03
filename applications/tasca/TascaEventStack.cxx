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
TascaEventStack::TascaEventStack(UInt_t slots)
{
	entries=slots;
	for(UInt_t i=0;i<entries;i++) AddLast(new TascaEvent());
    cout << "Tasca> EventStack: " << entries << " created" << endl;
}
//***********************************************************
TascaEventStack::~TascaEventStack(){
    cout << "Tasca> EventStack: deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaEventStack::PrintEventStack(){
  return 0;
  cout << "Tasca> EventStack :" <<endl;
  return 0;
}

