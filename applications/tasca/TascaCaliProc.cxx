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

  Text_t chis[16];
  Text_t chead[64];

  fCalibration   = (TascaCalibration *) GetParameter("Calibration");
  if(fCalibration==0){
	  fCalibration = new TascaCalibration("Calibration");
	  AddParameter(fCalibration);
  }
  evcount=0;
  anl=(TascaAnalysis *)TGo4Analysis::Instance();
  fhStopXL[0]=anl->CreateTH1I("Cali","Name","Title",1000,0.5,1000.5);
  fhStopXL[1]=anl->CreateTH1I(0,"Name2","Title2",1000,0.5,1000.5);
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
}
