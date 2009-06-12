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
  fControl->UnpackHisto  =kFALSE;
  fControl->CaliHisto    =kFALSE;
  fControl->CheckHisto   =kFALSE;
  fControl->AnlHisto     =kFALSE;
  fControl->checkTof     =kFALSE;
  fControl->checkChopper =kFALSE;
  fControl->checkMacro   =kFALSE;
  fControl->checkMicro   =kFALSE;
  fControl->TofMustbe    =kTRUE;//true-false
  fControl->ChopperMustbe=kTRUE;//true-false
  fControl->MacroMustbe  =kFALSE;//true-false
  fControl->MicroMustbe  =kFALSE;//true-false
  fControl->PrintParameter();
 }
 else cout<<"Controls not found"<<endl;
}
