//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompEventStack.h"

#include "Riostream.h"

//***********************************************************
CompEventStack::CompEventStack() {}
//***********************************************************
CompEventStack::CompEventStack(UInt_t slots)
{
	entries=slots;
	used=0;
	for(UInt_t i=0;i<entries;i++) AddLast(new CompEvent());
    cout << "Comp> EventStack: " << entries << " created" << endl;
}
//***********************************************************
CompEventStack::~CompEventStack(){
    cout << "Comp> EventStack: deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t CompEventStack::PrintEventStack(){
  return 0;
  cout << "Comp> EventStack :" <<endl;
  return 0;
}

