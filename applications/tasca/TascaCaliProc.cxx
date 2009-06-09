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
#include "TGo4WinCond.h"

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

  fParam       = (TascaParameter *)   anl->CreateParameter("Parameter","Parameters");
  fControl     = (TascaControl *)     anl->CreateParameter("Control","Controls");
  fCaliFit     = (TascaCaliFitter *)  anl->CreateParameter("CaliFitter","CaliFitter");
  fCalibration = (TascaCalibration *) anl->CreateParameter("Calibration","Calibration");
  fCaliGraph   = (TGraph *)           anl->CreateObject("Graph","CaliGraph",24);
  // setup calibration
  fCaliFit->Setup(fCaliGraph);
  gROOT->ProcessLine(".x setcontrol.C()");
  gROOT->ProcessLine(".x setparam.C()");
  gROOT->ProcessLine(".x setcali.C()"); // en-disable calibration

if(fControl->CaliHisto){
	fhStopXLH=anl->CreateTH2I("Cali/Sum","StopXLH","Stop X low/high","Low Energy [Kev]","High Energy [Kev]","Counts",4000,0,30000,4000,0,30000);
	fhStopYLH=anl->CreateTH2I("Cali/Sum","StopYLH","Stop Y low/high","Low Energy [Kev]","High Energy [Kev]","Counts",4000,0,30000,4000,0,30000);
    fhdStopXL=anl->CreateTH1D("Cali/Sum","StopXL", "StopX all low",144,0,144);
	fhdStopXH=anl->CreateTH1D("Cali/Sum","StopXH", "StopX all high",144,0,144);
	fhdStopYL=anl->CreateTH1D("Cali/Sum","StopYL", "StopY all low",96,0,96);
	fhdStopYH=anl->CreateTH1D("Cali/Sum","StopYH", "StopY all high",96,0,96);
	fhdBackL= anl->CreateTH1D("Cali/Sum","BackL",  "Back all low",64,0,64);
	fhdBackH= anl->CreateTH1D("Cali/Sum","BackH",  "Back all high",64,0,64);
	fhdVetoL= anl->CreateTH1D("Cali/Sum","VetoL",  "Veto all low",16,0,16);
	fhdVetoH= anl->CreateTH1D("Cali/Sum","VetoH",  "Veto all high",16,0,16);
	fhdStopXLsum=anl->CreateTH1D("Cali/Sum","StopXLsum", "StopX sum low",4000,0.5,4000.5);
	fhdStopXHsum=anl->CreateTH1D("Cali/Sum","StopXHsum", "StopX sum high",4000,0.5,4000.5);
	fhdStopYLsum=anl->CreateTH1D("Cali/Sum","StopYLsum", "StopY sum low",4000,0.5,4000.5);
	fhdStopYHsum=anl->CreateTH1D("Cali/Sum","StopYHsum", "StopY sum high",4000,0.5,4000.5);
	fhdBackLsum= anl->CreateTH1D("Cali/Sum","BackLsum",  "Back sum low",4000,0.5,4000.5);
	fhdBackHsum= anl->CreateTH1D("Cali/Sum","BackHsum",  "Back sum high",4000,0.5,4000.5);
	fhdVetoLsum= anl->CreateTH1D("Cali/Sum","VetoLsum",  "Veto sum low",4000,0.5,4000.5);
	fhdVetoHsum= anl->CreateTH1D("Cali/Sum","VetoHsum",  "Veto sum high",4000,0.5,4000.5);
	if(fCalibration->UseCalibration){
		fhdStopXLsum->GetXaxis()->SetLimits(1,30001);
		fhdStopYLsum->GetXaxis()->SetLimits(1,30001);
		fhdBackLsum->GetXaxis()->SetLimits(1,30001);
		fhdVetoLsum->GetXaxis()->SetLimits(1,30001);
		fhdStopXHsum->GetXaxis()->SetLimits(200,300200);
		fhdStopYHsum->GetXaxis()->SetLimits(200,300200);
		fhdBackHsum->GetXaxis()->SetLimits(200,300200);
		fhdVetoHsum->GetXaxis()->SetLimits(200,300200);
		fhdStopXLsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdStopXHsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdStopYLsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdStopYHsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdBackLsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdBackHsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdVetoLsum->GetXaxis()->SetTitle("Energy [Kev]");
		fhdVetoHsum->GetXaxis()->SetTitle("Energy [Kev]");
	}
  for(i=0;i<144;i++){
	snprintf(chis,15,"StopXL_%03d",i);
	snprintf(chead,63,"StopX Low %03d",i);
	fhStopXL[i]=anl->CreateTH1I("Cali/StopXL",chis,chead,4000,0.5,4000.5);
	snprintf(chis,15,"StopXH_%03d",i);
	snprintf(chead,63,"StopX High %03d",i);
	fhStopXH[i]=anl->CreateTH1I("Cali/StopXH",chis,chead,4000,0.5,4000.5);
	if(fCalibration->UseCalibration){
		fhStopXL[i]->GetXaxis()->SetLimits(1,30001);
		fhStopXH[i]->GetXaxis()->SetLimits(200,300200);
		fhStopXL[i]->GetXaxis()->SetTitle("Energy [Kev]");
		fhStopXH[i]->GetXaxis()->SetTitle("Energy [kev]");
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
		fhStopYL[i]->GetXaxis()->SetLimits(1,30001);
		fhStopYH[i]->GetXaxis()->SetLimits(200,300200);
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
		fhBackL[i]->GetXaxis()->SetLimits(1,30001);
		fhBackH[i]->GetXaxis()->SetLimits(200,300200);
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
		fhVetoL[i]->GetXaxis()->SetLimits(1,30001);
		fhVetoH[i]->GetXaxis()->SetLimits(200,300200);
		fhVetoL[i]->GetXaxis()->SetTitle("Energy [Kev]");
		fhVetoH[i]->GetXaxis()->SetTitle("Energy [Kev]");
	}
  }
  for(i =0;i<7;i++)
  {
		snprintf(chis,15,"GammaKev_%d",i);
		snprintf(chead,63,"Gamma [Kev] %d",i);
		fhGammaKev[i] = anl->CreateTH1I ("Cali/GammaKev",chis,chead,"Energy [Kev]","Channels",2000,0.5,2000.5);
		snprintf(chis,15,"Gamma10ns_%d",i);
		snprintf(chead,63,"Gamma [10ns] %d",i);
		fhGamma10ns[i] = anl->CreateTH1I ("Cali/Gamma10ns",chis,chead,"Time [10ns]","Channels",2000,0.5,20000.5);
  }
  snprintf(chis,15,"GammaMysec");
  snprintf(chead,63,"Gamma [mysec]");
  fhGammaMysec = anl->CreateTH1I ("Cali",chis,chead,"Gamma [mysec]","Counts",5000,0,2000);
  fhGammaSumKev = anl->CreateTH1I ("Cali/Sum","GammaSumKev","Sum of crystals [Kev]","Energy [Kev]","Counts",2000,0.5,2000.5);
  fhGammaAddbackKev = anl->CreateTH1I ("Cali/Sum","GammaAddback","Sum of energies [Kev]","Energy [Kev]","Counts",2000,0.5,2000.5);

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
  Gamma   = anl->CreatePicture("Cali","pGamma","Gamma Cal",8,1);
  Sum     = anl->CreatePicture("Cali","pDetectorSum","Summary detectors",4,2);

  anl->SetPicture(Sum,fhdStopXLsum,0,0,1);
  anl->SetPicture(Sum,fhdStopYLsum,1,0,1);
  anl->SetPicture(Sum,fhdBackLsum, 2,0,1);
  anl->SetPicture(Sum,fhdVetoLsum, 3,0,1);
  anl->SetPicture(Sum,fhdStopXHsum,0,1,1);
  anl->SetPicture(Sum,fhdStopYHsum,1,1,1);
  anl->SetPicture(Sum,fhdBackHsum, 2,1,1);
  anl->SetPicture(Sum,fhdVetoHsum, 3,1,1);

  Hits     = anl->CreatePicture("Cali","pDetectorHits","Summary detector hits",4,2);

  anl->SetPicture(Hits,fhdStopXL,0,0,1);
  anl->SetPicture(Hits,fhdStopYL,1,0,1);
  anl->SetPicture(Hits,fhdBackL, 2,0,1);
  anl->SetPicture(Hits,fhdVetoL, 3,0,1);
  anl->SetPicture(Hits,fhdStopXH,0,1,1);
  anl->SetPicture(Hits,fhdStopYH,1,1,1);
  anl->SetPicture(Hits,fhdBackH, 2,1,1);
  anl->SetPicture(Hits,fhdVetoH, 3,1,1);

  for(i=0;i<7;i++) // 8 rows
	  anl->SetPicture(Gamma,fhGammaKev[i],i,0,1);
  	  anl->SetPicture(Gamma,fhGammaMysec,7,0,1);
  Int_t m=0;
    // enlarge stats box and position in [0:1] coordinates
    // show only Mean value (ROOT manual "Statistics Display")
	  for(i=0;i<8;i++){ // 8 rows
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
    fadcKevH = (TGo4WinCond *)anl->CreateCondition("Check","adcKevH",0,kTRUE,0,300000);
    fadcKevL = (TGo4WinCond *)anl->CreateCondition("Check","adcKevL",0,kTRUE,0,30000);
} // fControl->CaliHisto
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

  if(fInput->fisVeto) return;

// Fill output event
  poutevt->fisTof=fInput->fisTof;
  poutevt->fisChopper=fInput->fisChopper;
  poutevt->fisMicro=fInput->fisMicro;
  poutevt->fisMacro=fInput->fisMacro;
  poutevt->fisVeto=fInput->fisVeto;
  poutevt->fiEventNumber=fInput->fiEventNumber;

  poutevt->fiStopXLhitI=fInput->fiStopXLhitI;
  poutevt->fiStopXHhitI=fInput->fiStopXHhitI;
  poutevt->fiStopYLhitI=fInput->fiStopYLhitI;
  poutevt->fiStopYHhitI=fInput->fiStopYHhitI;
  poutevt->fiBackHhitI=fInput->fiBackHhitI;
  poutevt->fiBackLhitI=fInput->fiBackLhitI;
  poutevt->fiVetoHhitI=fInput->fiVetoHhitI;
  poutevt->fiVetoLhitI=fInput->fiVetoLhitI;
  poutevt->fiMultiStopXL=fInput->fiMultiStopXL;
  poutevt->fiMultiStopXH=fInput->fiMultiStopXH;
  poutevt->fiMultiStopYL=fInput->fiMultiStopYL;
  poutevt->fiMultiStopYH=fInput->fiMultiStopYH;
  poutevt->fiMultiBackH=fInput->fiMultiBackH;
  poutevt->fiMultiBackL=fInput->fiMultiBackL;
  poutevt->fiMultiVetoH=fInput->fiMultiVetoH;
  poutevt->fiMultiVetoL=fInput->fiMultiVetoL;
  // value of maximum hit, if we had more than one hit
  poutevt->ffStopXLhitV=fCalibration->CalibrateStopXL(fInput->fiStopXLhitV,fInput->fiStopXLhitI);
  poutevt->ffStopXHhitV=fCalibration->CalibrateStopXH(fInput->fiStopXHhitV,fInput->fiStopXHhitI);
  poutevt->ffStopYLhitV=fCalibration->CalibrateStopYL(fInput->fiStopYLhitV,fInput->fiStopYLhitI);
  poutevt->ffStopYHhitV=fCalibration->CalibrateStopYH(fInput->fiStopYHhitV,fInput->fiStopYHhitI);
  poutevt->ffBackHhitV =fCalibration->CalibrateBackH(fInput->fiBackHhitV,fInput->fiBackHhitI);
  poutevt->ffBackLhitV =fCalibration->CalibrateBackL(fInput->fiBackLhitV,fInput->fiBackLhitI);
  poutevt->ffVetoHhitV =fCalibration->CalibrateVetoH(fInput->fiVetoHhitV,fInput->fiVetoHhitI);
  poutevt->ffVetoLhitV =fCalibration->CalibrateVetoL(fInput->fiVetoLhitV,fInput->fiVetoLhitI);

  if(poutevt->ffStopXLhitV>fParam->AlphaMaxL)poutevt->ffStopXLhitV=poutevt->ffStopXHhitV;
  if(poutevt->ffStopYLhitV>fParam->AlphaMaxL)poutevt->ffStopYLhitV=poutevt->ffStopYHhitV;

  poutevt->fiTimeStamp=fInput->fiTimeStamp;
  poutevt->fiSystemMysec=fInput->fiSystemMysec;
  poutevt->fiGammaMysec = fCalibration->CalibrateGammaT(fInput->fiGammaTime);
  for(i=0;i<144;i++){
	  poutevt->ffStopXL[i]=fCalibration->CalibrateStopXL(fInput->fiStopXL[i],i);
	  poutevt->ffStopXH[i]=fCalibration->CalibrateStopXH(fInput->fiStopXH[i],i);
  }
  for(i=0;i<96;i++){
	  poutevt->ffStopYL[i]=fCalibration->CalibrateStopYL(fInput->fiStopYL[i],i);
	  poutevt->ffStopYH[i]=fCalibration->CalibrateStopYH(fInput->fiStopYH[i],i);
  }
  for(i=0;i<64;i++){
	  poutevt->ffBackL[i]=fCalibration->CalibrateBackL(fInput->fiBackL[i],i);
	  poutevt->ffBackH[i]=fCalibration->CalibrateBackH(fInput->fiBackH[i],i);
  }
  for(i=0;i<16;i++){
	  poutevt->ffVetoL[i]=fCalibration->CalibrateVetoL(fInput->fiVetoL[i],i);
	  poutevt->ffVetoH[i]=fCalibration->CalibrateVetoH(fInput->fiVetoH[i],i);
  }
  poutevt->ffGammaMax=0.;
  poutevt->ffGammaSum=0.;
  for(i=0;i<7;i++){
	  poutevt->ffGammaKev[i]   = fCalibration->CalibrateGammaE(fInput->fiGammaE[i],i);
  	  poutevt->ffGammaSum += poutevt->ffGammaKev[i];
  	  if(poutevt->ffGammaKev[i]>poutevt->ffGammaMax)
  		  poutevt->ffGammaMax=poutevt->ffGammaKev[i];
  }

// now filling histograms
  if(fControl->CaliHisto){
	  fhStopXLH->Fill(poutevt->ffStopXLhitV,poutevt->ffStopXHhitV);
	  fhStopYLH->Fill(poutevt->ffStopYLhitV,poutevt->ffStopYHhitV);
  for(i=0;i<144;i++){
          if(fadcKevL->Test(poutevt->ffStopXL[i]))fhdStopXL->Fill(i);
	  if(fadcKevH->Test(poutevt->ffStopXH[i]))fhdStopXH->Fill(i);
	  fhStopXL[i]->Fill(poutevt->ffStopXL[i]);
	  fhStopXH[i]->Fill(poutevt->ffStopXH[i]);
	  fhdStopXLsum->Fill(poutevt->ffStopXL[i]);
	  fhdStopXHsum->Fill(poutevt->ffStopXH[i]);
  }
  for(i=0;i<96;i++){
          if(fadcKevL->Test(poutevt->ffStopYL[i]))fhdStopYL->Fill(i);
	  if(fadcKevH->Test(poutevt->ffStopYH[i]))fhdStopYH->Fill(i);
	  fhStopYL[i]->Fill(poutevt->ffStopYL[i]);
	  fhStopYH[i]->Fill(poutevt->ffStopYH[i]);
	  fhdStopYLsum->Fill(poutevt->ffStopYL[i]);
	  fhdStopYHsum->Fill(poutevt->ffStopYH[i]);
  }
  for(i=0;i<64;i++){
	  if(fInput->fiBackL[i]>0)fhdBackL->Fill(i);
	  if(fInput->fiBackH[i]>0)fhdBackH->Fill(i);
	  fhBackL[i]->Fill(poutevt->ffBackL[i]);
	  fhBackH[i]->Fill(poutevt->ffBackH[i]);
	  fhdBackLsum->Fill(poutevt->ffBackL[i]);
	  fhdBackHsum->Fill(poutevt->ffBackH[i]);
 }
  for(i=0;i<16;i++){
	  if(fInput->fiVetoL[i]>0)fhdVetoL->Fill(i);
	  if(fInput->fiVetoH[i]>0)fhdVetoH->Fill(i);
	  fhVetoL[i]->Fill(poutevt->ffVetoL[i]);
	  fhVetoH[i]->Fill(poutevt->ffVetoH[i]);
	  fhdVetoLsum->Fill(poutevt->ffVetoL[i]);
	  fhdVetoHsum->Fill(poutevt->ffVetoH[i]);
  }
  for(i=0;i<7;i++){
	  fhGamma10ns[i]->Fill(fInput->fiGammaChannelTime[i]);
	  fhGammaKev[i]->Fill(poutevt->ffGammaKev[i]);
	  fhGammaSumKev->Fill(poutevt->ffGammaKev[i]);
  }
  fhGammaMysec->Fill(poutevt->fiGammaMysec);
  fhGammaAddbackKev->Fill(poutevt->ffGammaSum);
} // fControl->CaliHisto

  poutevt->SetValid(kTRUE); // store
}
