#include "TascaAnalysis.h"

#include "Riostream.h"
#include <time.h>

#include "TH1.h"
#include "TFile.h"

#include "TascaParameter.h"


//***********************************************************
TascaAnalysis::TascaAnalysis()
{
  cout << "Tasca> TascaAnalysis: Create" << endl;


  //////////////// Parameter //////////////////////////
  // At this point, autosave file has not yet been read!
  // Therefore parameter values set here will be overwritten
  // if an autosave file is there.
  fPar = new TascaParameter("Parameters");
  AddParameter(fPar);

}

//***********************************************************
TascaAnalysis::~TascaAnalysis()
{
  cout << "Tasca> TascaAnalysis: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaAnalysis::UserPreLoop()
{
  cout << "Tasca> TascaAnalysis: PreLoop" << endl;

   return 0;
}
//-----------------------------------------------------------
Int_t TascaAnalysis::UserPostLoop()
{
  cout << "Tasca> TascaAnalysis: PostLoop" << endl;
   return 0;
}

//-----------------------------------------------------------
Int_t TascaAnalysis::UserEventFunc()
{
//// This function is called once for each event.
   return 0;
}
//-----------------------------------------------------------
void TascaAnalysis::SetPicture(TGo4Picture *p, TH1 *h, UInt_t ix, UInt_t iy, UInt_t mode)
{
p->Pic(ix,iy)->AddH1(h);
if(mode == 1){
p->Pic(ix,iy)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
p->Pic(ix,iy)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
p->Pic(ix,iy)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
p->Pic(ix,iy)->SetHisTitle(false);
p->Pic(ix,iy)->SetTitleAttr(0.1,0.75,0.7,0.9);
}}
TH1I* TascaAnalysis::CreateTH1I(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t channels, Double_t low, Double_t high)
{
TH1I *h=0;
Text_t full[128];
if(folder==0)strncpy(full,name,127);
else 		 snprintf(full,127,"%s/%s",folder,name);
h=(TH1I *)GetHistogram(full);
if(h == 0){
	h=new TH1I(name,title,channels,low,high);
	AddHistogram(h,folder);
	}
return h;
}
