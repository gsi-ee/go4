//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaAnalysis.h"

#include "Riostream.h"
#include <time.h>

#include "TH1.h"
#include "TH2I.h"
#include "TH2D.h"
#include "TFile.h"
#include "TCutG.h"
#include "TNamed.h"
#include "TGraph.h"

#include "TascaControl.h"
#include "TascaParameter.h"
#include "TascaPedestals.h"
#include "TascaCaliFitter.h"
#include "TascaCalibration.h"
#include "TascaCodec.h"
#include "TascaControl.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"

//***********************************************************
TascaAnalysis::TascaAnalysis()
{
  cout << "Tasca> TascaAnalysis: Create" << endl;


  //////////////// Parameter //////////////////////////
  // At this point, autosave file has not yet been read!
  // Therefore parameter values set here will be overwritten
  // if an autosave file is there.
  fParam = new TascaParameter("Parameters");
  AddParameter(fParam);
  events=0;

}

//***********************************************************
TascaAnalysis::~TascaAnalysis()
{
  //cout << "Tasca> TascaAnalysis: Delete" << endl;
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
  cout << "Tasca> TascaAnalysis: Events processed="<<events<<endl;
  events=0;
   return 0;
}

//-----------------------------------------------------------
Int_t TascaAnalysis::UserEventFunc()
{
//// This function is called once for each event.
	events++;
   return 0;
}
//-----------------------------------------------------------
TNamed* TascaAnalysis::CreateObject(const Text_t* type, const Text_t* name, Int_t par1)
{
	TNamed* par=GetObject(name);
	if(par) return par;
	else if(strcmp(type,"Graph") == 0){
		TGraph* gr=new TGraph();
		gr->SetName("CaliGraph");
		gr->SetMarkerStyle(par1);
		AddObject(gr);
		return (TNamed *)gr;
	}
	cout << "Tasca> Object class "<<type<<" not known!" << endl;
	return 0;
}
//-----------------------------------------------------------
TGo4Parameter* TascaAnalysis::CreateParameter(const Text_t* type, const Text_t* name)
{
	TGo4Parameter * par=GetParameter(name);
	if(par) return par;
	else if(strcmp(type,"Codec")      == 0) par = new TascaCodec(name);
	else if(strcmp(type,"Control")    == 0) par = new TascaControl(name);
	else if(strcmp(type,"Parameter")  == 0) par = new TascaParameter(name);
	else if(strcmp(type,"Calibration")== 0) par = new TascaCalibration(name);
	else if(strcmp(type,"Pedestals")  == 0) par = new TascaPedestals(name);
	else if(strcmp(type,"CaliFitter") == 0) par = new TascaCaliFitter(name);
	else {
		cout << "Tasca> Parameter class "<<type<<" not known!" << endl;
		return 0;
	}
	AddParameter(par);
	return par;
}
//-----------------------------------------------------------
TGo4Condition* TascaAnalysis::CreateCondition(const Text_t* folder, const Text_t* name, Int_t dim, Bool_t reset, Double_t low, Double_t high)
{
	TGo4Condition* p;
	Text_t full[128];
	if(folder==0)strncpy(full,name,127);
	else 		 snprintf(full,127,"%s/%s",folder,name);
	p = GetAnalysisCondition(full);
	if(p==0){
		if(dim == 0) p = (TGo4Condition *) new TGo4WinCond(name);
		else p = (TGo4Condition *)new TGo4CondArray(name,dim,"TGo4WinCond");
		AddAnalysisCondition(p,folder);
		p->SetValues(low,high);
		if(reset)p->ResetCounts();	}
	return p;
}
//-----------------------------------------------------------
TGo4Condition* TascaAnalysis::CreateCondition(const Text_t* folder, const Text_t* name, Int_t dim, Bool_t reset, TCutG* poly)
{
	TGo4Condition* p;
	Text_t full[128];
	if(folder==0)strncpy(full,name,127);
	else 		 snprintf(full,127,"%s/%s",folder,name);
	p = GetAnalysisCondition(full);
	if(p==0){
		if(dim == 0) p = (TGo4Condition *) new TGo4PolyCond(name);
		else p = (TGo4Condition *)new TGo4CondArray(name,dim,"TGo4PolyCond");
		AddAnalysisCondition(p,folder);
		p->SetValues(poly);
		if(reset)p->ResetCounts();	}
	return p;
}
//-----------------------------------------------------------
TGo4Picture* TascaAnalysis::CreatePicture(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t ir, UInt_t ic)
{
TGo4Picture* p;
Text_t full[128];
if(folder==0)strncpy(full,name,127);
else 		 snprintf(full,127,"%s/%s",folder,name);
p = GetPicture(full);
if(p==0){
	p = new TGo4Picture(name,title,ir,ic);
	AddPicture(p,folder);
}
return p;
}
//-----------------------------------------------------------
void TascaAnalysis::SetPicture(TGo4Picture *p, TH1 *h, UInt_t ir, UInt_t ic, UInt_t mode)
{
if(p->FindPic(ir,ic)==0){
p->Pic(ir,ic)->AddH1(h);
if(mode == 1){
p->Pic(ir,ic)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
p->Pic(ir,ic)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
p->Pic(ir,ic)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
p->Pic(ir,ic)->SetHisTitle(false);
p->Pic(ir,ic)->SetTitleAttr(0.1,0.75,0.7,0.9);
}}}
TH2I* TascaAnalysis::CreateTH2I(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t channels1, Double_t low1, Double_t high1, UInt_t channels2, Double_t low2, Double_t high2)
{
TH2I *h=0;
Text_t full[128];
if(folder==0)strncpy(full,name,127);
else 		 snprintf(full,127,"%s/%s",folder,name);
h=(TH2I *)GetHistogram(full);
if(h == 0){
	h=new TH2I(name,title,channels1,low1,high1,channels2,low2,high2);
	AddHistogram(h,folder);
	}
return h;
}
TH2D* TascaAnalysis::CreateTH2D(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t channels1, Double_t low1, Double_t high1, UInt_t channels2, Double_t low2, Double_t high2)
{
TH2D *h=0;
Text_t full[128];
if(folder==0)strncpy(full,name,127);
else 		 snprintf(full,127,"%s/%s",folder,name);
h=(TH2D *)GetHistogram(full);
if(h == 0){
	h=new TH2D(name,title,channels1,low1,high1,channels2,low2,high2);
	AddHistogram(h,folder);
	}
return h;
}
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
TH1D* TascaAnalysis::CreateTH1D(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t channels, Double_t low, Double_t high)
{
TH1D *h=0;
Text_t full[128];
if(folder==0)strncpy(full,name,127);
else 		 snprintf(full,127,"%s/%s",folder,name);
h=(TH1D *)GetHistogram(full);
if(h == 0){
	h=new TH1D(name,title,channels,low,high);
	AddHistogram(h,folder);
	}
return h;
}
TH1I* TascaAnalysis::CreateTH1I(const Text_t* folder, const Text_t* name,
		  const Text_t* title, const Text_t* xlett, const Text_t* ylett,
		  UInt_t channels, Double_t low, Double_t high)
{
	TH1I* h=CreateTH1I(folder, name, title, channels, low, high);
	h->GetXaxis()->SetTitle(xlett);
	h->GetYaxis()->SetTitle(ylett);
	return h;
}
TH1D* TascaAnalysis::CreateTH1D(const Text_t* folder, const Text_t* name,
		  const Text_t* title,  const Text_t* xlett, const Text_t* ylett,
		  UInt_t channels, Double_t low, Double_t high)
{
	TH1D* h=CreateTH1D(folder, name, title, channels, low, high);
	h->GetXaxis()->SetTitle(xlett);
	h->GetYaxis()->SetTitle(ylett);
	return h;
}
TH2I* TascaAnalysis::CreateTH2I(const Text_t* folder, const Text_t* name,
		  const Text_t* title,  const Text_t* xlett, const Text_t* ylett, const Text_t* zlett,
		  UInt_t channels1, Double_t low1, Double_t high1,
		  UInt_t channels2, Double_t low2, Double_t high2)
{
	TH2I* h=CreateTH2I(folder, name, title, channels1, low1, high1, channels2, low2, high2);
	h->GetXaxis()->SetTitle(xlett);
	h->GetYaxis()->SetTitle(ylett);
	h->GetZaxis()->SetTitle(zlett);
	return h;
}
TH2D* TascaAnalysis::CreateTH2D(const Text_t* folder, const Text_t* name,
		  const Text_t* title,  const Text_t* xlett, const Text_t* ylett,  const Text_t* zlett,
		  UInt_t channels1, Double_t low1, Double_t high1,
		  UInt_t channels2, Double_t low2, Double_t high2)
{
	TH2D* h=CreateTH2D(folder, name, title, channels1, low1, high1, channels2, low2, high2);
	h->GetXaxis()->SetTitle(xlett);
	h->GetYaxis()->SetTitle(ylett);
	h->GetZaxis()->SetTitle(zlett);
	return h;
}
