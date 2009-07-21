//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompControl.h"

#include "Riostream.h"

//***********************************************************
CompControl::CompControl() : TGo4Parameter()
{
    init();
}
//***********************************************************
CompControl::CompControl(const char* name) : TGo4Parameter(name),ChainCounter(0)
{
    cout << "Comp> CompControl: " << name << " created" << endl;
    init();
}
//***********************************************************
CompControl::~CompControl(){
    //cout << "Comp> CompControl: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t CompControl::PrintParameter(){
if(!Printed){
cout << "Parameter " << GetName()<<":" <<endl;
cout<<"      Histograms Unpack:"<<UnpackHisto
	<<" Cali:"<<CaliHisto
	<<" Check:"<<CheckHisto
	<<" Anl:"<<AnlHisto<<endl;
cout<<"      Check Tof:"<<checkTof<<"="<<TofMustbe
	<<" Chop:"<<checkChopper<<"="<<ChopperMustbe
	<<" Macro:"<<checkMacro<<"="<<MacroMustbe
	<<" Micro:"<<checkMicro<<"="<<MicroMustbe<<endl;
cout<<"      Chain:"<<writeChainTree<<" Counter:"<<ChainCounter<<endl;
}
  Printed=kTRUE;
  return 0;
}
//-----------------------------------------------------------
Bool_t CompControl::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("CompControl")) {
    CompControl * from = (CompControl *) pp;
    writeChainTree=from->writeChainTree;
    ChainCounter=from->ChainCounter;
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
    CheckHisto=from->CheckHisto;
    AnlHisto=from->AnlHisto;
   cout << "Comp> CompControl: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

