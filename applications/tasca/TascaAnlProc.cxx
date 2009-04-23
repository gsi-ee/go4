#include "TascaAnlProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "TGo4WinCond.h"

#include "TascaAnlEvent.h"
#include "TascaCaliEvent.h"
#include "TascaParameter.h"
#include "TascaAnalysis.h"

//***********************************************************
TascaAnlProc::TascaAnlProc()
  : TGo4EventProcessor(),fInput(0)
{
}
//***********************************************************
// this one is used in TascaAnlFact
TascaAnlProc::TascaAnlProc(const char* name) :
  TGo4EventProcessor(name)
{
  cout << "Tasca> TascaAnlProc: Create" << endl;
  //// init user analysis objects:

  anl=(TascaAnalysis *)TGo4Analysis::Instance();

  fParPed = (TascaParameter*)  GetParameter("TascaParPed");

  // we must check, if the histograms have been restored from auto-save file
  if(GetHistogram("Analysis/Stop2d")==0)
  {
	fStop=new TH2D("Stop2d","Stripes",144,0,144,96,0,96);
	AddHistogram(fStop,"Analysis");
  }
  else
    { // restored from auto-save file, get pointers
	  fStop=(TH2D*)GetHistogram("Analysis/Stop2d");
      cout << "Tasca> TascaAnlProc: Restored histograms from autosave" << endl;
    }
}
//***********************************************************
TascaAnlProc::~TascaAnlProc()
{
	  cout << "Tasca> TascaAnlProc: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
void TascaAnlProc::TascaEventAnalysis(TascaAnlEvent* poutevt)
{
  Int_t ii,i;
  fInput  = (TascaCaliEvent*) GetInputEvent();
  for(ii=0;ii<96;ii++)
	  for(i=0;i<144;i++){
		  fStop->Fill(i,ii,fInput->ffStopXL[i]+fInput->ffStopYL[ii]);
	  }

  poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set


} // BuildCalEvent
