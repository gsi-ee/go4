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
  fCalibration->EnableCalibration(kTRUE); // use calibration or not
  fCalibration->SetPrefix("cali2"); // prefix for coefficient files
  fCalibration->Preset();
  if(fCalibration->UseCalibration){
	  fCalibration->ReadCoefficients();
	  cout << "Tasca> TascaCaliProc: Use calibration " << fCalibration->prefix<<endl;
  }
  else   cout << "Tasca> TascaCaliProc: No calibration used" << endl;
 }
}
