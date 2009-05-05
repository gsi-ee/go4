#include "TYYYUnpackProc.h"

#include "Riostream.h"

#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"

#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"

#include "TYYYUnpackEvent.h"
#include "TYYYRawEvent.h"
#include "TYYYParameter.h"

//***********************************************************
TYYYUnpackProc::TYYYUnpackProc() :
   TGo4EventProcessor("YYYProc"),
   fX(0),fY(0),fZ(0),fVX(0),fVY(0),fVZ(0),fNumScatt(0),
   fXY(0),fVXVY(0), fXYCond(0), fVXVYCond(0), fEmitX(0), fEmitY(0), fEmitDist(0),
   fWinConR(0), fWinConV(0), fPolyConEmit(0),
   fParam1(0)
{
}
//***********************************************************
// this one is used in TYYYUnpackFact.cxx
TYYYUnpackProc::TYYYUnpackProc(const char* name) :
  TGo4EventProcessor(name)

{
  cout << "**** TYYYUnpackProc: Create" << endl;

  //// init user analysis objects:

  fParam1   = (TYYYParameter *)   GetParameter("YYYPar1");
  fParam1->PrintParameter(0,0);

  if(GetHistogram("Position/Xfinal")==0) {
     // Creation of histograms:
      fX = new TH1D ("Xfinal", "Scatt sim x (nm)",1000,-1e7,1e+7);
      fY = new TH1D ("Yfinal", "Scatt sim y (nm)",1000,-1e7,1e+7);
      fZ = new TH1D ("Zfinal", "Scatt sim z (nm)",1000,1,1e+8);
      fVX = new TH1D ("Vxfinal", "Scatt sim vx (nm/ps)",1000,-5e+3,5e+3);
      fVY = new TH1D ("Vyfinal", "Scatt sim vy (nm/ps)",1000,-5e+3,5e+3);
      fVZ = new TH1D ("Vzfinal", "Scatt sim vz (nm/ps)",1000,1,3e+4);
      fNumScatt = new TH1D ("Nscatt", "Multiple scattering collisions",50000,15000,25000);
      fXY = new TH2D("X-Y","x versus y final",100,-1e7,1e+7,100,-1e+7,1e+7);
      fVXVY = new TH2D("Vx-Vy","vx versus vy final",100,-5e+3,5e+3,100,-5e+3,5e+3);
      fXYCond = new TH2D("X-Y-cond","x versus y final",100,-1e+7,1e+7,100,-1e+7,1e+7);
      fVXVYCond = new TH2D("Vx-Vy-cond","vx versus vy final",100,-1e+3,1e+3,100,-1e+3,1e+3);
      fEmitX = new TH2D("X-X'","transverse emittance x",100,-1e+7,1e+7,100,-0.1,0.1);
      fEmitY = new TH2D("Y-Y'","transverse emittance y",100,-1e+7,1e+7,100,-0.1,0.1);
      fEmitDist = new TH1D("Emit4d","transverse emittance distribution",4000,0,2e+5);


      AddHistogram(fX,"Position");
      AddHistogram(fY,"Position");
      AddHistogram(fZ,"Position");
      AddHistogram(fXY,"Position");
      AddHistogram(fXYCond,"Position");
      AddHistogram(fVX,"Velocity");
      AddHistogram(fVY,"Velocity");
      AddHistogram(fVZ,"Velocity");
      AddHistogram(fVXVY,"Velocity");
      AddHistogram(fVXVYCond,"Velocity");
      AddHistogram(fNumScatt);
      AddHistogram(fEmitX);
      AddHistogram(fEmitY);
      AddHistogram(fEmitDist);
  } else {

     // got them from autosave file

      fX = (TH1D*)GetHistogram("Position/Xfinal");
      fY = (TH1D*)GetHistogram("Position/Yfinal");
      fZ = (TH1D*)GetHistogram("Position/Zfinal");
      fVX = (TH1D*)GetHistogram("Velocity/Vxfinal");
      fVY = (TH1D*)GetHistogram("Velocity/Vyfinal");
      fVZ = (TH1D*)GetHistogram("Velocity/Vzfinal");
      fNumScatt = (TH1D*)GetHistogram("Nscatt");;
      fXY = (TH2D*)GetHistogram("Position/X-Y");
      fVXVY = (TH2D*)GetHistogram("Velocity/Vx-Vy");
      fXYCond = (TH2D*)GetHistogram("Position/X-Y-cond");
      fVXVYCond = (TH2D*)GetHistogram("Velocity/Vx-Vy-cond");
      fEmitX = (TH2D*)GetHistogram("X-X'");
      fEmitY = (TH2D*)GetHistogram("Y-Y'");
      fEmitDist = (TH1D*)GetHistogram("Emit4d");

      cout << "Unpack: Restored histograms from autosave" << endl;
    }

  if(GetAnalysisCondition("RCondition")==0) {
     // Creation of conditions:

      fWinConR= new TGo4WinCond("RCondition");
      fWinConR->SetValues(50,70);
      fWinConR->Disable(true); // return always true

      fWinConV= new TGo4WinCond("VCondition");
      fWinConV->SetValues(50,70,90,120);
      fWinConV->Disable(true);
      fWinConV->Invert(kTRUE);
      Double_t xvalues[4]={400,700,600,400};
      Double_t yvalues[4]={800,900,1100,800};
      TCutG* mycut= new TCutG("initialcut",4,xvalues,yvalues);
      fPolyConEmit= new TGo4PolyCond("EmittCondition");
      fPolyConEmit->SetValues(mycut); // copies mycat into fPolyConEmit
      fPolyConEmit->Disable(true);
      delete mycut; // mycat has been copied into the conditions

      AddAnalysisCondition(fWinConR);
      AddAnalysisCondition(fWinConV);
      AddAnalysisCondition(fPolyConEmit);

  } else {
     // got them from autosave file

      fWinConR  = (TGo4WinCond*)  GetAnalysisCondition("RCondition");
      fWinConV  = (TGo4WinCond*)  GetAnalysisCondition("VCondition");
      fPolyConEmit = (TGo4PolyCond*) GetAnalysisCondition("EmittCondition");
      fWinConR->ResetCounts();
      fWinConV->ResetCounts();
      fPolyConEmit->ResetCounts();
      cout << "Unpack: Restored conditions from autosave" << endl;
    }
   fWinConR->Enable();
   fWinConR->PrintCondition(true);
   fPolyConEmit->Enable();
   fPolyConEmit->PrintCondition(true);
}
//***********************************************************
TYYYUnpackProc::~TYYYUnpackProc()
{
   fWinConR->PrintCondition(true);
   fPolyConEmit->PrintCondition(true);
}
//***********************************************************

//-----------------------------------------------------------
void TYYYUnpackProc::YYYUnpack(TYYYUnpackEvent* poutevt)
{
   TYYYRawEvent *inp = dynamic_cast<TYYYRawEvent*> (GetInputEvent());
   if (inp==0) {
      cout << "YYYUnpackProc: no input event !"<< endl;
      return;
   }

   // fill poutevt here:
   for(Int_t i=0; i<3;++i)
   {
      poutevt->fdR[i] = inp->fdData[i]; // units: nm
      poutevt->fdV[i] = inp->fdData[i+3]; // units: nm/ps
   }
   poutevt->fdR[0]-=5e+6; // correction for beam axis offset
   poutevt->fdR[1]-=5e+6; // beam was shifted by 5 mm
   poutevt->fiNumScatt= (Int_t) inp->fdData[6];
   // calculate derived values;
   if(poutevt->fdV[2]) poutevt->fdGam[0]=poutevt->fdV[0]/poutevt->fdV[2]; // gammax=vx/vz (rad)
   if(poutevt->fdV[2]) poutevt->fdGam[1]=poutevt->fdV[1]/poutevt->fdV[2]; // gammay=vy/vz (rad)
   Double_t scalefact=1e-3; // transform nm*rad to mm*mrad
   poutevt->fdEmittX=poutevt->fdR[0]*poutevt->fdGam[0]*TMath::Pi()*scalefact;
   poutevt->fdEmittY=poutevt->fdR[1]*poutevt->fdGam[1]*TMath::Pi()*scalefact;
   poutevt->fdEmitt4d=poutevt->fdEmittX*poutevt->fdEmittY;
   // fill histograms:
   fX->Fill(poutevt->fdR[0]);
   fY->Fill(poutevt->fdR[1]);
   fZ->Fill(poutevt->fdR[2]);;
   fVX->Fill(poutevt->fdV[0]);
   fVY->Fill(poutevt->fdV[1]);
   fVZ->Fill(poutevt->fdV[2]);
   fNumScatt->Fill(poutevt->fiNumScatt);
   fXY->Fill(poutevt->fdR[0],poutevt->fdR[1]);
   fVXVY->Fill(poutevt->fdV[0],poutevt->fdV[1]);
   if(fWinConR->Test(poutevt->fdR[0],poutevt->fdR[1]))
      fXYCond->Fill(poutevt->fdR[0],poutevt->fdR[1]);
   if(fWinConV->Test(poutevt->fdV[0],poutevt->fdV[1]))
      fVXVYCond->Fill(poutevt->fdV[0],poutevt->fdV[1]);
   fEmitX->Fill(poutevt->fdR[0],poutevt->fdGam[0]);
   fEmitY->Fill(poutevt->fdR[1],poutevt->fdGam[1]);
   fEmitDist->Fill(poutevt->fdEmitt4d);
}
