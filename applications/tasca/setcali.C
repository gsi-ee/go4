//---------------------------------------------
// Go4 Tasca analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void setcali()
{
TascaCalibration *fCalibration = (TascaCalibration *) go4->GetObject("Calibration","Go4");
 if(fCalibration!=0){
  fCalibration->EnableCalibration(kFALSE); // use calibration or not
  fCalibration->SetPrefix("new"); // prefix for coefficient files
 }
}
