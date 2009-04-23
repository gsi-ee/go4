#include "TascaCaliProc.h"
#include "TGo4EventEndException.h"

#include "Riostream.h"
#include <time.h>

#include "TObjArray.h"
#include "TObjString.h"
#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "snprintf.h"
#include "TPaveStats.h"

#include "TGo4Picture.h"

#include "TascaParameter.h"
#include "TascaCalibration.h"
#include "TascaCaliEvent.h"
#include "TascaUnpackEvent.h"
#include "TascaAnalysis.h"

//***********************************************************
TascaCaliProc::TascaCaliProc() :
  TGo4EventProcessor()
{}
//***********************************************************
// this one is used
TascaCaliProc::TascaCaliProc(const char* name) :
   TGo4EventProcessor(name)
{
  cout << "Tasca> TascaCaliProc: Create" << endl;

  Text_t chis[32];
  Text_t chead[64];

  anl=(TascaAnalysis *)TGo4Analysis::Instance();

  fCalibration   = (TascaCalibration *) GetParameter("Calibration");
  if(fCalibration==0){
	  fCalibration = new TascaCalibration("Calibration");
	  AddParameter(fCalibration);
  }
  evcount=0;
  for(i=0;i<144;i++){
	snprintf(chis,15,"StopXL_%03d",i);
	snprintf(chead,63,"StopX Low %03d",i);
	fhStopXL[i]=anl->CreateTH1I("Cali/Stop",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"StopXH_%03d",i);
	snprintf(chead,63,"StopX High %03d",i);
	fhStopXH[i]=anl->CreateTH1I("Cali/Stop",chis,chead,5000,0.5,5000.5);
  }
  for(i=0;i<96;i++){
	snprintf(chis,15,"StopYL_%03d",i);
	snprintf(chead,63,"StopY Low %03d",i);
	fhStopYL[i]=anl->CreateTH1I("Cali/Stop",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"StopYH_%03d",i);
	snprintf(chead,63,"StopY High %03d",i);
	fhStopYH[i]=anl->CreateTH1I("Cali/Stop",chis,chead,5000,0.5,5000.5);
  }
  for(i=0;i<64;i++){
	snprintf(chis,15,"BackL_%03d",i);
	snprintf(chead,63,"Back Low %03d",i);
	fhBackL[i]=anl->CreateTH1I("Cali/Back",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"BackH_%03d",i);
	snprintf(chead,63,"Back High %03d",i);
	fhBackH[i]=anl->CreateTH1I("Cali/Back",chis,chead,5000,0.5,5000.5);
  }
  for(i=0;i<16;i++){
	snprintf(chis,15,"VetoL_%03d",i);
	snprintf(chead,63,"Veto Low %03d",i);
	fhVetoL[i]=anl->CreateTH1I("Cali/Veto",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"VetoH_%03d",i);
	snprintf(chead,63,"Veto High %03d",i);
	fhVetoH[i]=anl->CreateTH1I("Cali/Veto",chis,chead,5000,0.5,5000.5);
  }
  for(i =0;i<8;i++)
  {
	snprintf(chis,15,"GammaKev_%d",i);
	snprintf(chead,63,"Gamma [Kev] %d",i);
	fhGammaKev[i] = anl->CreateTH1I ("Cali/Gamma",chis,chead,9000,0,2000);
	snprintf(chis,15,"GammaMysec_%d",i);
	snprintf(chead,63,"Gamma [mysec] %d",i);
	fhGammaMysec[i] = anl->CreateTH1I ("Cali/Gamma",chis,chead,5000,0,5000);
  }
  // pictures rows, columns
  StopX[0] = anl->CreatePicture("Cali","StopXL0","Stop X low",8,6);
  StopX[1] = anl->CreatePicture("Cali","StopXL1","Stop X low",8,6);
  StopX[2] = anl->CreatePicture("Cali","StopXL2","Stop X low",8,6);
  StopX[3] = anl->CreatePicture("Cali","StopXH0","Stop X high",8,6);
  StopX[4] = anl->CreatePicture("Cali","StopXH1","Stop X high",8,6);
  StopX[5] = anl->CreatePicture("Cali","StopXH2","Stop X high",8,6);

  StopY[0] = anl->CreatePicture("Cali","StopYL0","Stop Y low",8,6);
  StopY[1] = anl->CreatePicture("Cali","StopYL1","Stop Y low",8,6);
  StopY[2] = anl->CreatePicture("Cali","StopYH0","Stop Y high",8,6);
  StopY[3] = anl->CreatePicture("Cali","StopYH1","Stop Y high",8,6);

  Back[0] = anl->CreatePicture("Cali","BackL","Back low",8,8);
  Back[1] = anl->CreatePicture("Cali","BackH","Back high",8,8);
  Veto    = anl->CreatePicture("Cali","Veto","Veto",8,4);
  Gamma   = anl->CreatePicture("Cali","Gamma","Gamma Cal",8,2);

  Int_t m=0;
    // enlarge stats box and position in [0:1] coordinates
    // show only Mean value (ROOT manual "Statistics Display")
    for(i=0;i<8;i++){ // 8 rows
    	  anl->SetPicture(Gamma,fhGammaKev[i],i,0,1);
      	  anl->SetPicture(Gamma,fhGammaMysec[i],i,1,1);
      	  anl->SetPicture(Veto,fhVetoL[i],i,0,1);
      	  anl->SetPicture(Veto,fhVetoL[i+8],i,1,1);
      	  anl->SetPicture(Veto,fhVetoH[i],i,2,1);
      	  anl->SetPicture(Veto,fhVetoH[i+8],i,3,1);
  	  for(k=0;k<6;k++){ // 6 columns
  		  anl->SetPicture(StopX[0],fhStopXL[m],i,k,1);
  		  anl->SetPicture(StopX[1],fhStopXL[m+48],i,k,1);
  		  anl->SetPicture(StopX[2],fhStopXL[m+96],i,k,1);
  		  anl->SetPicture(StopX[3],fhStopXH[m],i,k,1);
  		  anl->SetPicture(StopX[4],fhStopXH[m+48],i,k,1);
  		  anl->SetPicture(StopX[5],fhStopXH[m+96],i,k,1);

  		  anl->SetPicture(StopY[0],fhStopYL[m],i,k,1);
  		  anl->SetPicture(StopY[1],fhStopYL[m+48],i,k,1);
  		  anl->SetPicture(StopY[2],fhStopYH[m],i,k,1);
  		  anl->SetPicture(StopY[3],fhStopYH[m+48],i,k,1);
  	  m++;
    }}
    m=0;
    for(i=0;i<8;i++){ // 8 rows
    	for(k=0;k<8;k++){ // 8 columns
    		anl->SetPicture(Back[0],fhBackL[m],i,k,1);
    		anl->SetPicture(Back[1],fhBackH[m],i,k,1);
    		m++;
    	}}

}
//***********************************************************
TascaCaliProc::~TascaCaliProc()
{
  cout << "Tasca> TascaCaliProc: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
void TascaCaliProc::TascaCalibrate(TascaCaliEvent* poutevt)
{

  poutevt->SetValid(kFALSE); // not to store
  fInput    = (TascaUnpackEvent* ) GetInputEvent(); // from this
  poutevt->ffTimeStamp=fInput->fiTimeStamp;
  for(i=0;i<144;i++){
	  poutevt->ffStopXL[i]=fInput->fiStopXL[i];
	  poutevt->ffStopXH[i]=fInput->fiStopXH[i];
	  fhStopXL[i]->Fill(fInput->fiStopXL[i]);
	  fhStopXH[i]->Fill(fInput->fiStopXH[i]);
  }
  for(i=0;i<96;i++){
	  poutevt->ffStopYL[i]=fInput->fiStopYL[i];
	  poutevt->ffStopYH[i]=fInput->fiStopYH[i];
	  fhStopYL[i]->Fill(fInput->fiStopYL[i]);
	  fhStopYH[i]->Fill(fInput->fiStopYH[i]);
  }
  for(i=0;i<64;i++){
	  poutevt->ffBackL[i]=fInput->fiBackL[i];
	  poutevt->ffBackH[i]=fInput->fiBackH[i];
	  fhBackL[i]->Fill(fInput->fiBackL[i]);
	  fhBackH[i]->Fill(fInput->fiBackH[i]);
  }
  for(i=0;i<16;i++){
	  poutevt->ffVetoL[i]=fInput->fiVetoL[i];
	  poutevt->ffVetoH[i]=fInput->fiVetoH[i];
	  fhVetoL[i]->Fill(fInput->fiVetoL[i]);
	  fhVetoH[i]->Fill(fInput->fiVetoH[i]);
  }
  for(i=0;i<8;i++){
	  poutevt->ffGammaKev[i]   = fInput->fiGammaE[i];
	  poutevt->ffGammaMysec[i] = fInput->fiGammaT[i];
	  fhGammaKev[i]->Fill(fInput->fiGammaE[i]);
	  fhGammaMysec[i]->Fill(fInput->fiGammaT[i]);
  }
  poutevt->SetValid(kTRUE); // store
}
