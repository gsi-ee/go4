//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------
#include <Riostream.h>

void setcontrol()
{
TascaControl *fControl = (TascaControl *) go4->GetObject("Controls","Go4");
 if(fControl!=0){
  fControl->writeChainTree =kTRUE; // used by Analyzer
  //fControl->ChainCounter =0; // used by Analyzer. Without Autosave: will be 0
  fControl->UnpackHisto    =kFALSE; // used by Unpacker
  fControl->CaliHisto      =kFALSE; // used by Calibrator
  fControl->CheckHisto     =kFALSE; // used by Checker
  fControl->AnlHisto       =kFALSE; // used by Analysis
  fControl->checkTof       =kFALSE; // used by unpacker
  fControl->checkChopper   =kFALSE; // used by unpacker
  fControl->checkMacro     =kFALSE; // used by unpacker
  fControl->checkMicro     =kFALSE; // used by unpacker
  fControl->TofMustbe      =kTRUE;  // used by unpacker
  fControl->ChopperMustbe  =kTRUE;  // used by unpacker
  fControl->MacroMustbe    =kFALSE; // used by unpacker
  fControl->MicroMustbe    =kFALSE; // used by unpacker
  fControl->PrintParameter();
 }
 else cout<<"Controls not found"<<endl;
}
