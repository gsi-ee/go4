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
	fhdStopXL=anl->CreateTH1D("Cali/Sum","StopXL", "StopX all low",144,0,144);
	fhdStopYL=anl->CreateTH1D("Cali/Sum","StopYL", "StopY all low",96,0,96);
	fhdBackL= anl->CreateTH1D("Cali/Sum","BackL",  "Back all low",64,0,64);
	fhdVetoL= anl->CreateTH1D("Cali/Sum","VetoL",  "Veto all low",16,0,16);
	fhdStopXH=anl->CreateTH1D("Cali/Sum","StopXH", "StopX all high",144,0,144);
	fhdStopYH=anl->CreateTH1D("Cali/Sum","StopYH", "StopY all high",96,0,96);
	fhdBackH= anl->CreateTH1D("Cali/Sum","BackH",  "Back all high",64,0,64);
	fhdVetoH= anl->CreateTH1D("Cali/Sum","VetoH",  "Veto all high",16,0,16);
	fhStopL=  anl->CreateTH2I("Cali/Sum","StopL",  "Stop detector",144,0,144,96,0,96);
  for(i=0;i<144;i++){
	snprintf(chis,15,"StopXL_%03d",i);
	snprintf(chead,63,"StopX Low %03d",i);
	fhStopXL[i]=anl->CreateTH1I("Cali/StopL",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"StopXH_%03d",i);
	snprintf(chead,63,"StopX High %03d",i);
	fhStopXH[i]=anl->CreateTH1I("Cali/StopH",chis,chead,5000,0.5,5000.5);
  }
  for(i=0;i<96;i++){
	snprintf(chis,15,"StopYL_%03d",i);
	snprintf(chead,63,"StopY Low %03d",i);
	fhStopYL[i]=anl->CreateTH1I("Cali/StopL",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"StopYH_%03d",i);
	snprintf(chead,63,"StopY High %03d",i);
	fhStopYH[i]=anl->CreateTH1I("Cali/StopH",chis,chead,5000,0.5,5000.5);
  }
  for(i=0;i<64;i++){
	snprintf(chis,15,"BackL_%03d",i);
	snprintf(chead,63,"Back Low %03d",i);
	fhBackL[i]=anl->CreateTH1I("Cali/BackL",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"BackH_%03d",i);
	snprintf(chead,63,"Back High %03d",i);
	fhBackH[i]=anl->CreateTH1I("Cali/BackH",chis,chead,5000,0.5,5000.5);
  }
  for(i=0;i<16;i++){
	snprintf(chis,15,"VetoL_%03d",i);
	snprintf(chead,63,"Veto Low %03d",i);
	fhVetoL[i]=anl->CreateTH1I("Cali/VetoL",chis,chead,5000,0.5,5000.5);
	snprintf(chis,15,"VetoH_%03d",i);
	snprintf(chead,63,"Veto High %03d",i);
	fhVetoH[i]=anl->CreateTH1I("Cali/VetoH",chis,chead,5000,0.5,5000.5);
  }
  for(i =0;i<8;i++)
  {
	snprintf(chis,15,"GammaKev_%d",i);
	snprintf(chead,63,"Gamma [Kev] %d",i);
	fhGammaKev[i] = anl->CreateTH1I ("Cali/GammaKev",chis,chead,9000,0,2000);
	snprintf(chis,15,"GammaMysec_%d",i);
	snprintf(chead,63,"Gamma [mysec] %d",i);
	fhGammaMysec[i] = anl->CreateTH1I ("Cali/GammaMysec",chis,chead,5000,0,5000);
  }
  // pictures rows, columns
  StopX[0] = anl->CreatePicture("Cali","pStopXL0","Stop X low",8,6);
  StopX[1] = anl->CreatePicture("Cali","pStopXL1","Stop X low",8,6);
  StopX[2] = anl->CreatePicture("Cali","pStopXL2","Stop X low",8,6);
  StopX[3] = anl->CreatePicture("Cali","pStopXH0","Stop X high",8,6);
  StopX[4] = anl->CreatePicture("Cali","pStopXH1","Stop X high",8,6);
  StopX[5] = anl->CreatePicture("Cali","pStopXH2","Stop X high",8,6);

  StopY[0] = anl->CreatePicture("Cali","pStopYL0","Stop Y low",8,6);
  StopY[1] = anl->CreatePicture("Cali","pStopYL1","Stop Y low",8,6);
  StopY[2] = anl->CreatePicture("Cali","pStopYH0","Stop Y high",8,6);
  StopY[3] = anl->CreatePicture("Cali","pStopYH1","Stop Y high",8,6);

  Back[0] = anl->CreatePicture("Cali","pBackL","Back low",8,8);
  Back[1] = anl->CreatePicture("Cali","pBackH","Back high",8,8);
  Veto    = anl->CreatePicture("Cali","pVeto","Veto",8,4);
  Gamma   = anl->CreatePicture("Cali","pGamma","Gamma Cal",8,2);
  Sum     = anl->CreatePicture("Cali","pDetectors","Summary detectors",4,2);

  anl->SetPicture(Sum,fhdStopXL,0,0,1);
  anl->SetPicture(Sum,fhdStopYL,1,0,1);
  anl->SetPicture(Sum,fhdBackL, 2,0,1);
  anl->SetPicture(Sum,fhdVetoL, 3,0,1);
  anl->SetPicture(Sum,fhdStopXH,0,1,1);
  anl->SetPicture(Sum,fhdStopYH,1,1,1);
  anl->SetPicture(Sum,fhdBackH, 2,1,1);
  anl->SetPicture(Sum,fhdVetoH, 3,1,1);

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
  for(i=0;i<144;i++)
	  for(k=0;k<96;k++)
		  if((fInput->fiStopXL[i]>0)&(fInput->fiStopYL[k]>0))
			  fhStopL->Fill(i,k);
  for(i=0;i<144;i++){
	  poutevt->ffStopXL[i]=fInput->fiStopXL[i];
	  poutevt->ffStopXH[i]=fInput->fiStopXH[i];
	  if(fInput->fiStopXL[i]>0)fhdStopXL->Fill(i);
	  if(fInput->fiStopXH[i]>0)fhdStopXH->Fill(i);
	  fhStopXL[i]->Fill(fInput->fiStopXL[i]);
	  fhStopXH[i]->Fill(fInput->fiStopXH[i]);
  }
  for(i=0;i<96;i++){
	  poutevt->ffStopYL[i]=fInput->fiStopYL[i];
	  poutevt->ffStopYH[i]=fInput->fiStopYH[i];
	  if(fInput->fiStopYL[i]>0)fhdStopYL->Fill(i);
	  if(fInput->fiStopYH[i]>0)fhdStopYH->Fill(i);
	  fhStopYL[i]->Fill(fInput->fiStopYL[i]);
	  fhStopYH[i]->Fill(fInput->fiStopYH[i]);
  }
  for(i=0;i<64;i++){
	  poutevt->ffBackL[i]=fInput->fiBackL[i];
	  poutevt->ffBackH[i]=fInput->fiBackH[i];
	  if(fInput->fiBackL[i]>0)fhdBackL->Fill(i);
	  if(fInput->fiBackH[i]>0)fhdBackH->Fill(i);
	  fhBackL[i]->Fill(fInput->fiBackL[i]);
	  fhBackH[i]->Fill(fInput->fiBackH[i]);
  }
  for(i=0;i<16;i++){
	  poutevt->ffVetoL[i]=fInput->fiVetoL[i];
	  poutevt->ffVetoH[i]=fInput->fiVetoH[i];
	  if(fInput->fiVetoL[i]>0)fhdVetoL->Fill(i);
	  if(fInput->fiVetoH[i]>0)fhdVetoH->Fill(i);
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
