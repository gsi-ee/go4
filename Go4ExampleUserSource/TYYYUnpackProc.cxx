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

   fParam1   = (TYYYParameter *) GetParameter("YYYPar1");
   fParam1->PrintParameter(0,0);

   fX = MakeTH1('D', "Position/Xfinal", "Scatt sim x (nm)",1000,-1e7,1e+7);
   fY = MakeTH1('D', "Position/Yfinal", "Scatt sim y (nm)",1000,-1e7,1e+7);
   fZ = MakeTH1('D', "Position/Zfinal", "Scatt sim z (nm)",1000, 1., 1e+8);

   fVX = MakeTH1('D', "Velocity/Vxfinal", "Scatt sim vx (nm/ps)",1000, -5e+3,5e+3);
   fVY = MakeTH1('D', "Velocity/Vyfinal", "Scatt sim vy (nm/ps)",1000, -5e+3,5e+3);
   fVZ = MakeTH1('D', "Velocity/Vzfinal", "Scatt sim vz (nm/ps)",1000, 1.,3e+4);

   fNumScatt = MakeTH1('D', "Nscatt", "Multiple scattering collisions",50000, 15000., 25000.);

   fXY = MakeTH2('D', "Position/X-Y","x versus y final",100,-1e7,1e+7,100,-1e+7,1e+7);
   fVXVY = MakeTH2('D', "Velocity/Vx-Vy","vx versus vy final",100,-5e+3,5e+3,100,-5e+3,5e+3);
   fXYCond = MakeTH2('D', "Position/X-Y-cond","x versus y final",100,-1e+7,1e+7,100,-1e+7,1e+7);
   fVXVYCond = MakeTH2('D', "Velocity/Vx-Vy-cond","vx versus vy final",100,-1e+3,1e+3,100,-1e+3,1e+3);

   fEmitX = MakeTH2('D', "X-X'","transverse emittance x",100,-1e+7,1e+7,100,-0.1,0.1);
   fEmitY = MakeTH2('D', "Y-Y'","transverse emittance y",100,-1e+7,1e+7,100,-0.1,0.1);
   fEmitDist = MakeTH1('D', "Emit4d","transverse emittance distribution",4000,0,2e+5);

   fWinConR = MakeWinCond("RCondition", 50., 70.);
   fWinConV = MakeWinCond("VCondition", 50, 70, 90, 120);
   if (IsObjMade()) {
      fWinConV->Disable(true);
      fWinConV->Invert(kTRUE);
   }

   Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
   fPolyConEmit = MakePolyCond("EmittCondition", 3, cutpnts);

   fWinConR->PrintCondition(true);
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
