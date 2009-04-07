#include "TascaAnlProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "TGo4WinCond.h"

#include "TascaAnlEvent.h"
#include "TascaUnpackEvent.h"
#include "TascaParameter.h"
#include "TascaCalibPar.h"

//***********************************************************
TascaAnlProc::TascaAnlProc()
  : TGo4EventProcessor(),fInput(0),fParam1(0),fParam2(0),fWinCon(0)
{
}
//***********************************************************
// this one is used in TascaAnlFact
TascaAnlProc::TascaAnlProc(const char* name) :
  TGo4EventProcessor(name)
{
  cout << "**** TascaAnlProc: Create" << endl;
  //// init user analysis objects:
  fParam1 = (TascaParameter*)  GetParameter("TascaPar1");
  fParam2 = (TascaParameter*)  GetParameter("TascaPar2");
  fWinCon = (TGo4WinCond *)   GetAnalysisCondition("wincon1");

  // we must check, if the histograms have been restored from auto-save file
  if(GetHistogram("Sum1")==0)
  {

    }
  else
    { // restored from auto-save file, get pointers
      cout << "AnlProc: Restored histograms from autosave" << endl;
    }
  if(GetParameter("CaliPar")==0)
    {
    }
  else
    {
    }
}
//***********************************************************
TascaAnlProc::~TascaAnlProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TascaAnlProc::TascaEventAnalysis(TascaAnlEvent* poutevt)
{
  Int_t ii,i;
  fInput  = (TascaUnpackEvent*) GetInputEvent();

  poutevt->SetValid(kTRUE);       // events are not stored until kTRUE is set


} // BuildCalEvent
