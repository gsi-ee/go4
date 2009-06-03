//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void setcontrol()
{
TascaControl *fControl = (TascaControl *) go4->GetObject("Controls","Go4");
 if(fControl!=0){
  fControl->UnpackHisto=kTRUE;
  fControl->CaliHisto=kTRUE;
  fControl->CheckHisto=kTRUE;
  fControl->AnlHisto=kTRUE;
 }
}
