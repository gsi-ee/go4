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
  fControl->UnpackHisto  =kTRUE;
  fControl->CaliHisto    =kTRUE;
  fControl->CheckHisto   =kTRUE;
  fControl->AnlHisto     =kTRUE;
  fControl->checkTof     =kFALSE;
  fControl->checkChopper =kFALSE;
  fControl->checkMacro   =kFALSE;
  fControl->checkMicro   =kFALSE;
  fControl->TofMustbe    =kTRUE;//true-false
  fControl->ChopperMustbe=kTRUE;//true-false
  fControl->MacroMustbe  =kTRUE;//true-false
  fControl->MicroMustbe  =kTRUE;//true-false
  fControl->PrintParameter();
 }
 else cout<<"Controls not found"<<endl;
}
