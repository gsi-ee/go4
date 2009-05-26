//---------------------------------------------
// Go4 Tasca analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaControl.h"

#include "Riostream.h"

//***********************************************************
TascaControl::TascaControl() : TGo4Parameter()
{
    init();
}
//***********************************************************
TascaControl::TascaControl(const char* name) : TGo4Parameter(name)
{
    cout << "Tasca> TascaControl: " << name << " created" << endl;
    init();
}
//***********************************************************
TascaControl::~TascaControl(){
    cout << "Tasca> TascaControl: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaControl::PrintParameter(){
  return 0;
  cout << "Parameter " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaControl::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaControl")) {
    TascaControl * from = (TascaControl *) pp;
    checkTof=from->checkTof;
    TofMustbe=from->TofMustbe;//true equals isTof
    checkChopper=from->checkChopper;
    ChopperMustbe=from->ChopperMustbe;//true equals isChopper
    checkMacro=from->checkMacro;
    MacroMustbe=from->MacroMustbe;//true equals isMacro
    checkMicro=from->checkMicro;
    MicroMustbe=from->MicroMustbe;//true equals isMicro
    UnpackHisto=from->UnpackHisto;
    CaliHisto=from->CaliHisto;
    AnlHisto=from->AnlHisto;
   cout << "Tasca> TascaControl: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

