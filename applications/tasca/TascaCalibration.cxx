//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaCalibration.h"

#include "TMath.h"
#include "Riostream.h"
#include "snprintf.h"

#include "TGo4Log.h"

//***********************************************************
TascaCalibration::TascaCalibration() : TGo4Parameter()
{}
//***********************************************************
TascaCalibration::TascaCalibration(const char* name) : TGo4Parameter(name)
{
    cout << "Tasca> TascaCalibration: " << name << " created" << endl;
    UseCalibration=kFALSE;
}
//***********************************************************
TascaCalibration::~TascaCalibration(){
    cout << "Tasca> TascaCalibration: " << GetName() << " deleted" << endl;
}
//-----------------------------------------------------------
Int_t TascaCalibration::PrintParameter(){
  return 0;
  cout << "Calibration " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaCalibration::Preset(){
	Int_t i;
	memset(fdGammaE_a0,0,sizeof(fdGammaE_a0));
	memset(fdStopXL_a0,0,sizeof(fdStopXL_a0));
	memset(fdStopXH_a0,0,sizeof(fdStopXH_a0));
	memset(fdStopYL_a0,0,sizeof(fdStopYL_a0));
	memset(fdStopYH_a0,0,sizeof(fdStopYH_a0));
	memset(fdBackL_a0,0,sizeof(fdBackL_a0));
	memset(fdBackH_a0,0,sizeof(fdBackH_a0));
	memset(fdVetoL_a0,0,sizeof(fdVetoL_a0));
	memset(fdVetoH_a0,0,sizeof(fdVetoH_a0));

	memset(fdGammaE_a2,0,sizeof(fdGammaE_a2));
	memset(fdStopXL_a2,0,sizeof(fdStopXL_a2));
	memset(fdStopXH_a2,0,sizeof(fdStopXH_a2));
	memset(fdStopYL_a2,0,sizeof(fdStopYL_a2));
	memset(fdStopYH_a2,0,sizeof(fdStopYH_a2));
	memset(fdBackL_a2,0,sizeof(fdBackL_a2));
	memset(fdBackH_a2,0,sizeof(fdBackH_a2));
	memset(fdVetoL_a2,0,sizeof(fdVetoL_a2));
	memset(fdVetoH_a2,0,sizeof(fdVetoH_a2));
	for(i=0;i<144;i++){
		fdStopXL_a1[i]=1.;fdStopXH_a1[i]=1.;
	}
	for(i=0;i<96;i++){
		fdStopYL_a1[i]=1.;fdStopYH_a1[i]=1.;
	}
	for(i=0;i<64;i++){
		fdBackL_a1[i]=1.;fdBackH_a1[i]=1.;
	}
	for(i=0;i<16;i++){
		fdVetoL_a1[i]=1.;fdVetoH_a1[i]=1.;
	}
	for(i=0;i<8;i++){
		fdGammaE_a1[i]=1.;
	}
	fdGammaT_a0=0.;
	fdGammaT_a1=0.01;
	fdGammaT_a1=0.;
	return kTRUE;
  }
//-----------------------------------------------------------
Bool_t TascaCalibration::UpdateFrom(TGo4Parameter *pp){
if(pp->InheritsFrom("TascaCalibration")) {
  TascaCalibration * from = (TascaCalibration *) pp;
  UseCalibration=from->UseCalibration;
}
return kTRUE;
}
//-----------------------------------------------------------
void TascaCalibration::ReadSingleCoefficients(const char * file
		, UInt_t size, Double_t *a0, Double_t *a1, Double_t *a2){
	Text_t line[128];
	Double_t v0,v1,v2;
	UInt_t ii;
	//File=file;
	char name[32];
	std::ifstream database(file);
	if(database==0){
	    cout << "Tasca> TascaCalibration: Error open " << file << endl;
	    return;
	}
	cout << "Tasca> TascaCalibration: Coefficients from " << file<< endl;
	while(1){
		database.getline(line,127,'\n' ); // read whole line
		if(database.eof() || !database.good()) break;
		if(strstr(line,"#") || strstr(line,"!")){
			cout << line << endl;
			continue;// skip any comments
		}
		sscanf(line,"%s %d %lf %le %le",name,&ii,&v0,&v1,&v2);
//		cout << name<<" "<<ii<<" "
//			<< v0<<" "<<v1<<" "<<v2<<endl;
		if(ii>=size) cout << "Index error: " << ii << " should less equal "<<size<< endl;
		a0[ii]=v0;
		a1[ii]=v1;
		a2[ii]=v2;
	}
	database.close();
	return;

}
void TascaCalibration::ReadCoefficients(){
	TString full;
	TString pref=prefix;

	UInt_t size=144;
	full.Form("%s_StopXL.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdStopXL_a0,fdStopXL_a1,fdStopXL_a2);
	full.Form("%s_StopXH.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdStopXH_a0,fdStopXH_a1,fdStopXH_a2);
	size=96;
	full.Form("%s_StopYL.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdStopYL_a0,fdStopYL_a1,fdStopYL_a2);
	full.Form("%s_StopYH.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdStopYH_a0,fdStopYH_a1,fdStopYH_a2);
	size=64;
	full.Form("%s_BackL.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdBackL_a0,fdBackL_a1,fdBackL_a2);
	full.Form("%s_BackH.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdBackH_a0,fdBackH_a1,fdBackH_a2);
	size=16;
	full.Form("%s_VetoL.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdVetoL_a0,fdVetoL_a1,fdVetoL_a2);
	full.Form("%s_VetoH.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdVetoH_a0,fdVetoH_a1,fdVetoH_a2);
	size=8;
	full.Form("%s_GammaE.txt",prefix.Data());
	ReadSingleCoefficients(full.Data(),size,fdGammaE_a0,fdGammaE_a1,fdGammaE_a2);
}


