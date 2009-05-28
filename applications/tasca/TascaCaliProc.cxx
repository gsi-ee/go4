//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaCaliProc.h"
#include "TGo4EventEndException.h"

#include "Riostream.h"
#include <time.h>

#include "TObjArray.h"
#include "TObjString.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TGo4Fitter.h"
#include "snprintf.h"
#include "TPaveStats.h"

#include "TGo4Picture.h"

#include "TascaControl.h"
#include "TascaParameter.h"
#include "TascaCaliFitter.h"
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

  fControl   = (TascaControl *) GetParameter("Controls");
  if(fControl==0){
	  fControl = new TascaControl("Controls");
	  AddParameter(fControl);
  }
  fCaliFit   = (TascaCaliFitter *) GetParameter("CaliFitter");
  if(fCaliFit==0){
	  fCaliFit = new TascaCaliFitter("CaliFitter");
	  AddParameter(fCaliFit);
  }
  fCaliGraph = (TGraph *)GetObject("CaliGraph");
  if(fCaliGraph ==0){
	  fCaliGraph=new TGraph;
	  fCaliGraph->SetName("CaliGraph");
	  fCaliGraph->SetMarkerStyle(24);
	  AddObject(fCaliGraph);
  }
  // setup calibration
  fCaliFit->Setup(fCaliGraph);
  fCalibration   = (TascaCalibration *) GetParameter("Calibration");
  if(fCalibration==0){
	  fCalibration = new TascaCalibration("Calibration");
	  AddParameter(fCalibration);
  }
  // sets coefficients a0,a2 to 0, a1 to 1.
  fCalibration->Preset();
  gROOT->ProcessLine(".x setcali.C()"); // en-disable calibration
  if(fCalibration->UseCalibration){ // was set in setcali.C
	  fCalibration->ReadCoefficients();
	  cout << "Tasca> TascaCaliProc: Use calibration" << endl;
  }
  else   cout << "Tasca> TascaCaliProc: No calibration used" << endl;

  evcount=0;
	fhdStopXL=anl->CreateTH1D("Cali/Sum","StopXL", "StopX all low",144,0,144);
	fhdStopYL=anl->CreateTH1D("Cali/Sum","StopYL", "StopY all low",96,0,96);
	fhdStopXLsum=anl->CreateTH1D("Cali/Sum","StopXLsum", "StopX sum low",4000,0.5,4000.5);
	fhdStopXHsum=anl->CreateTH1D("Cali/Sum","StopXHsum", "StopX sum high",4000,0.5,4000.5);
	fhdBackL= anl->CreateTH1D("Cali/Sum","BackL",  "Back all low",64,0,64);
	fhdVetoL= anl->CreateTH1D("Cali/Sum","VetoL",  "Veto all low",16,0,16);
	fhdStopXH=anl->CreateTH1D("Cali/Sum","StopXH", "StopX all high",144,0,144);
	fhdStopYH=anl->CreateTH1D("Cali/Sum","StopYH", "StopY all high",96,0,96);
	fhdBackH= anl->CreateTH1D("Cali/Sum","BackH",  "Back all high",64,0,64);
	fhdVetoH= anl->CreateTH1D("Cali/Sum","VetoH",  "Veto all high",16,0,16);
	fhStopL=  anl->CreateTH2I("Cali/Sum","StopL",  "Stop detector",144,0,144,96,0,96);
	if(fCalibration->UseCalibration){
		fhdStopXLsum->GetXaxis()->SetLimits(0,300);
		fhdStopXHsum->GetXaxis()->SetLimits(0,30000);
		fhdStopXLsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdStopXHsum->GetXaxis()->SetTitle("Energy [Kev]");
	}
  for(i=0;i<144;i++){
	snprintf(chis,15,"StopXL_%03d",i);
	snprintf(chead,63,"StopX Low %03d",i);
	fhStopXL[i]=anl->CreateTH1I("Cali/StopXL",chis,chead,4000,0.5,4000.5);
	snprintf(chis,15,"StopXH_%03d",i);
	snprintf(chead,63,"StopX High %03d",i);
	fhStopXH[i]=anl->CreateTH1I("Cali/StopXH",chis,chead,4000,0.5,4000.5);
	fhStopXH[i]->GetXaxis()->SetTitle("Channels");
	fhStopXH[i]->GetYaxis()->SetTitle("Counts");
	fhStopXL[i]->GetXaxis()->SetTitle("Channels");
	fhStopXL[i]->GetYaxis()->SetTitle("Counts");
	if(fCalibration->UseCalibration){
		fhStopXL[i]->GetXaxis()->SetLimits(0,300);
		fhStopXH[i]->GetXaxis()->SetLimits(0,30000);
		fhStopXL[i]->GetXaxis()->SetTitle("Energy [Kev]");
		fhStopXH[i]->GetXaxis()->SetTitle("Energy [Kev]");
	}
  }
  for(i=0;i<96;i++){
	snprintf(chis,15,"StopYL_%03d",i);
	snprintf(chead,63,"StopY Low %03d",i);
	fhStopYL[i]=anl->CreateTH1I("Cali/StopYL",chis,chead,4000,0.5,4000.5);
	snprintf(chis,15,"StopYH_%03d",i);
	snprintf(chead,63,"StopY High %03d",i);
	fhStopYH[i]=anl->CreateTH1I("Cali/StopYH",chis,chead,4000,0.5,4000.5);
	fhStopYH[i]->GetXaxis()->SetTitle("Channels");
	fhStopYH[i]->GetYaxis()->SetTitle("Counts");
	fhStopYL[i]->GetXaxis()->SetTitle("Channels");
	fhStopYL[i]->GetYaxis()->SetTitle("Counts");
	if(fCalibration->UseCalibration){
		fhStopYL[i]->GetXaxis()->SetLimits(0,300);
		fhStopYH[i]->GetXaxis()->SetLimits(0,30000);
		fhStopYL[i]->GetXaxis()->SetTitle("Energy [Kev]");
		fhStopYH[i]->GetXaxis()->SetTitle("Energy [Kev]");
	}
  }
  for(i=0;i<64;i++){
	snprintf(chis,15,"BackL_%03d",i);
	snprintf(chead,63,"Back Low %03d",i);
	fhBackL[i]=anl->CreateTH1I("Cali/BackL",chis,chead,4000,0.5,4000.5);
	snprintf(chis,15,"BackH_%03d",i);
	snprintf(chead,63,"Back High %03d",i);
	fhBackH[i]=anl->CreateTH1I("Cali/BackH",chis,chead,4000,0.5,4000.5);
	fhBackH[i]->GetXaxis()->SetTitle("Channels");
	fhBackH[i]->GetYaxis()->SetTitle("Counts");
	fhBackL[i]->GetXaxis()->SetTitle("Channels");
	fhBackL[i]->GetYaxis()->SetTitle("Counts");
	if(fCalibration->UseCalibration){
		fhBackL[i]->GetXaxis()->SetLimits(0,30000);
		fhBackH[i]->GetXaxis()->SetLimits(0,30000);
		fhBackL[i]->GetXaxis()->SetTitle("Energy [Kev]");
		fhBackH[i]->GetXaxis()->SetTitle("Energy [Kev]");
	}
  }
  for(i=0;i<16;i++){
	snprintf(chis,15,"VetoL_%03d",i);
	snprintf(chead,63,"Veto Low %03d",i);
	fhVetoL[i]=anl->CreateTH1I("Cali/VetoL",chis,chead,4000,0.5,4000.5);
	snprintf(chis,15,"VetoH_%03d",i);
	snprintf(chead,63,"Veto High %03d",i);
	fhVetoH[i]=anl->CreateTH1I("Cali/VetoH",chis,chead,4000,0.5,4000.5);
	fhVetoH[i]->GetXaxis()->SetTitle("Channels");
	fhVetoH[i]->GetYaxis()->SetTitle("Counts");
	fhVetoL[i]->GetXaxis()->SetTitle("Channels");
	fhVetoL[i]->GetYaxis()->SetTitle("Counts");
	if(fCalibration->UseCalibration){
		fhVetoL[i]->GetXaxis()->SetLimits(0,30000);
		fhVetoH[i]->GetXaxis()->SetLimits(0,30000);
		fhVetoL[i]->GetXaxis()->SetTitle("Energy [Kev]");
		fhVetoH[i]->GetXaxis()->SetTitle("Energy [Kev]");
	}
  }
  for(i =0;i<8;i++)
  {
	snprintf(chis,15,"GammaKev_%d",i);
	snprintf(chead,63,"Gamma [Kev] %d",i);
	fhGammaKev[i] = anl->CreateTH1I ("Cali/GammaKev",chis,chead,2000,0.5,2000.5);
	snprintf(chis,15,"GammaMysec_%d",i);
	snprintf(chead,63,"Gamma [mysec] %d",i);
	fhGammaMysec[i] = anl->CreateTH1I ("Cali/GammaMysec",chis,chead,5000,0,5000);
  }
  fhGammaSumKev = anl->CreateTH1I ("Cali/Sum","GammaSumKev","Sum of crystals [Kev]",2000,0.5,2000.5);
  fhGammaAddbackKev = anl->CreateTH1I ("Cali/Sum","GammaAddback","Sum of energies [Kev]",2000,0.5,2000.5);

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
  poutevt->fisTof=fInput->fisTof;
  poutevt->fisChopper=fInput->fisChopper;
  poutevt->fisMicro=fInput->fisMicro;
  poutevt->fisMacro=fInput->fisMacro;

  poutevt->fiStopXLhitI=fInput->fiStopXLhitI;
  poutevt->fiStopXHhitI=fInput->fiStopXHhitI;
  poutevt->fiStopYLhitI=fInput->fiStopYLhitI;
  poutevt->fiStopYHhitI=fInput->fiStopYHhitI;
  poutevt->fiBackHhitI=fInput->fiBackHhitI;
  poutevt->fiBackLhitI=fInput->fiBackLhitI;
  poutevt->fiVetoHhitI=fInput->fiVetoHhitI;
  poutevt->fiVetoLhitI=fInput->fiVetoLhitI;
  // value of maximum hit, if we had more than one hit
  poutevt->ffStopXLhitV=fCalibration->CalibrateStopXL(fInput->fiStopXLhitV,fInput->fiStopXLhitI);
  poutevt->ffStopXHhitV=fCalibration->CalibrateStopXH(fInput->fiStopXHhitV,fInput->fiStopXHhitI);
  poutevt->ffStopYLhitV=fCalibration->CalibrateStopYL(fInput->fiStopYLhitV,fInput->fiStopYLhitI);
  poutevt->ffStopYHhitV=fCalibration->CalibrateStopYH(fInput->fiStopYHhitV,fInput->fiStopYHhitI);
  poutevt->ffBackHhitV =fCalibration->CalibrateBackH(fInput->fiBackHhitV,fInput->fiBackHhitI);
  poutevt->ffBackLhitV =fCalibration->CalibrateBackL(fInput->fiBackLhitV,fInput->fiBackLhitI);
  poutevt->ffVetoHhitV =fCalibration->CalibrateVetoH(fInput->fiVetoHhitV,fInput->fiVetoHhitI);
  poutevt->ffVetoLhitV =fCalibration->CalibrateVetoL(fInput->fiVetoLhitV,fInput->fiVetoLhitI);
  poutevt->ffTimeStamp=(Float_t)fInput->fiTimeStamp;
  poutevt->ffSystemSec=(Float_t)fInput->fiSystemSec;
  poutevt->ffSystemMysec=(Float_t)fInput->fiSystemMysec;
  for(i=0;i<144;i++){
	  poutevt->ffStopXL[i]=fCalibration->CalibrateStopXL(fInput->fiStopXL[i],i);
	  poutevt->ffStopXH[i]=fCalibration->CalibrateStopXH(fInput->fiStopXH[i],i);
	  if(fInput->fiStopXL[i]>0)fhdStopXL->Fill(i);
	  if(fInput->fiStopXH[i]>0)fhdStopXH->Fill(i);
	  fhStopXL[i]->Fill(poutevt->ffStopXL[i]);
	  fhStopXH[i]->Fill(poutevt->ffStopXH[i]);
	  fhdStopXLsum->Fill(poutevt->ffStopXL[i]);
	  fhdStopXHsum->Fill(poutevt->ffStopXH[i]);
  }
  for(i=0;i<96;i++){
	  poutevt->ffStopYL[i]=fCalibration->CalibrateStopYL(fInput->fiStopYL[i],i);
	  poutevt->ffStopYH[i]=fCalibration->CalibrateStopYH(fInput->fiStopYH[i],i);
	  if(fInput->fiStopYL[i]>0)fhdStopYL->Fill(i);
	  if(fInput->fiStopYH[i]>0)fhdStopYH->Fill(i);
	  fhStopYL[i]->Fill(poutevt->ffStopYL[i]);
	  fhStopYH[i]->Fill(poutevt->ffStopYH[i]);
  }
  for(i=0;i<64;i++){
	  poutevt->ffBackL[i]=fCalibration->CalibrateBackL(fInput->fiBackL[i],i);
	  poutevt->ffBackH[i]=fCalibration->CalibrateBackH(fInput->fiBackH[i],i);
	  if(fInput->fiBackL[i]>0)fhdBackL->Fill(i);
	  if(fInput->fiBackH[i]>0)fhdBackH->Fill(i);
	  fhBackL[i]->Fill(poutevt->ffBackL[i]);
	  fhBackH[i]->Fill(poutevt->ffBackH[i]);
  }
  for(i=0;i<16;i++){
	  poutevt->ffVetoL[i]=fCalibration->CalibrateVetoL(fInput->fiVetoL[i],i);
	  poutevt->ffVetoH[i]=fCalibration->CalibrateVetoH(fInput->fiVetoH[i],i);
	  if(fInput->fiVetoL[i]>0)fhdVetoL->Fill(i);
	  if(fInput->fiVetoH[i]>0)fhdVetoH->Fill(i);
	  fhVetoL[i]->Fill(poutevt->ffVetoL[i]);
	  fhVetoH[i]->Fill(poutevt->ffVetoH[i]);
  }
Double_t sum=0.;
  for(i=0;i<8;i++){
	  poutevt->ffGammaKev[i]   = fCalibration->CalibrateGammaE(fInput->fiGammaE[i],i);
	  poutevt->ffGammaMysec[i] = fInput->fiGammaT[i];
	  fhGammaKev[i]->Fill(poutevt->ffGammaKev[i]);
	  fhGammaSumKev->Fill(poutevt->ffGammaKev[i]);
	  sum += poutevt->ffGammaKev[i];
	  fhGammaMysec[i]->Fill(poutevt->ffGammaMysec[i]);
  }
  fhGammaAddbackKev->Fill(sum);
  poutevt->SetValid(kTRUE); // store
}
