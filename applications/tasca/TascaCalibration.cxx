#include "TascaCalibration.h"

#include "Riostream.h"

//***********************************************************
TascaCalibration::TascaCalibration() : TGo4Parameter() {}
//***********************************************************
TascaCalibration::TascaCalibration(const char* name) : TGo4Parameter(name){
    cout << "Tasca> TascaCalibration: " << name << " created" << endl;
    fbSave=kFALSE;
}
//***********************************************************
TascaCalibration::~TascaCalibration(){
    cout << "Tasca> TascaCalibration: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaCalibration::PrintParameter(){
  return 0;
  cout << "Calibration " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaCalibration::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaCalibration")) {
    TascaCalibration * from = (TascaCalibration *) pp;
    for(UInt_t i=0;i<8;i++){
    	ffGammaEa0[i]=from->ffGammaEa0[i];
    	ffGammaEa1[i]=from->ffGammaEa1[i];
    	ffGammaEa2[i]=from->ffGammaEa2[i];
    	ffGammaTa0[i]=from->ffGammaTa0[i];
    	ffGammaTa1[i]=from->ffGammaTa1[i];
    	ffGammaTa2[i]=from->ffGammaTa2[i];
    }
   cout << "Tasca> TascaCalibration: " << GetName() << " updated" << endl;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

