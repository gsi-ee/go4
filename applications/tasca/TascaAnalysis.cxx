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
#include "TFile.h"
#include "TSystem.h"

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
#include "Go4EventServerTypes.h"
#include "TGo4AnalysisStep.h"
#include "TGo4StepFactory.h"

//***********************************************************
TascaAnalysis::TascaAnalysis()
{
// this constructor was called by MainUserAnalysis executable
  cout << "Wrong constructor TascaAnalysis()!" << endl;
}

//***********************************************************
// this constructor is called by go4analysis executable
TascaAnalysis::TascaAnalysis(int argc, char** argv){

	cout << "Tasca> TascaAnalysis: Created by go4analysis" << endl;

	Text_t *pc,*tmpname,*outname;
	switch(argc){
	case 1:
		cout << "Tasca> **** Default arguments: -file test ****" << endl;
		strcpy(kind,"-file");
		strcpy(input,"test");
		break;
	case 2:
		cout << "Tasca> **** Arguments: -file "<<argv[1]<<" ****" << endl;
		strcpy(kind,"-file");
		strcpy(input,argv[1]);
		break;
	case 3:
	default:
		cout << "Tasca> **** Arguments: "<<argv[1]<<" "<<argv[2]<<" ****" << endl;
		strcpy(kind,argv[1]);
		strcpy(input,argv[2]);
		break;
	}
	// depending on the running mode, we prefix the file names
	if(IsBatchMode())strcpy(prefix,"b_");
	if(IsClientMode())strcpy(prefix,"i_");
	if(IsServerMode())strcpy(prefix,"s_");

	intype=GO4EV_MBS_FILE;  // some defaults:
	strcpy(idir,"");
	strcpy(odir,"");
	strcpy(serv,"TascaAnalysisServer"); // name (servermode only)
	strcpy(Unpout,"Unpacked");
	strcpy(Calout,"Calibrated");
	strcpy(Chkout,"Checked");
	strcpy(Anlout,"Analyzed");
	strcpy(hostname,"localhost");
	// optional output path
	strcpy(odir,"");
	strcpy(idir,"");
	if(getenv("TASCASTORE")!=0){
		strcpy(odir,getenv("TASCASTORE"));
		if(strlen(odir)>0)strcat(odir,"/");
		cout << "Tasca> TASCASTORE is "<<odir<<endl;
	}
	else cout << "Tasca> TASCASTORE not set, use current"<<endl;
	// optional input path
	if(getenv("TASCASOURCE")!=0){
		strcpy(idir,getenv("TASCASOURCE"));
		if(strlen(idir)>0)strcat(idir,"/");
		cout << "Tasca> TASCASOURCE is "<<idir<<endl;
	}
	else cout << "Tasca> TASCASOURCE not set, use current"<<endl;
	strcpy(Unpout,prefix);
	strcpy(Calout,prefix);
	strcpy(Chkout,prefix);
	strcpy(Anlout,prefix);
	strcpy(ASfile,prefix);

	// Now parse arguments
	// strip any path information from input:
	// outname is name without path and postfix
	// postfix .lmd is default in all cases
	// postfix .lml is a metafile, @ is prefixed
	strcpy(filetype,".lmd");
	tmpname=input+strlen(input)-4;
	if((pc=strstr(tmpname,".lmd")) != 0)*pc=0;
	if((pc=strstr(tmpname,".lml")) != 0){*pc=0;strcpy(filetype,".lml");}
	tmpname=input;
	if((pc=strstr(input,"@")) != 0){
		tmpname++;
		strcpy(idir,""); // do not use source path for lml files
	}
	outname=tmpname; // file name or node name
	if((tmpname=strrchr(outname,'/')) != 0) outname=tmpname+1;
	strncat(ASfile,outname,120);     // auto save file
	strcat(ASfile,"_AS");
	strncat(Unpout,outname,110);     // output root file
	strcat(Unpout,"_Unpacked"); // append name of output event object
	strncat(Calout,outname,110);     // output root file
	strcat(Calout,"_Calibrated"); // append name of output event object
	strncat(Chkout,outname,110);     // output root file
	strcat(Chkout,"_Checked"); // append name of output event object
	strncat(Anlout,outname,110);     // output root file
	strcat(Anlout,"_Analysis");   // append name of output event object
	strncpy(serv,input,120);     // input (file with full path)
	if(strstr(kind,"-f")){
		intype=GO4EV_MBS_FILE;
		strncpy(serv,idir,250);
		strncat(serv,input,120);     // input (file with full path)
		strcat(serv,filetype);}
	else if(strstr(kind,"-t")) intype=GO4EV_MBS_TRANSPORT;
	else if(strstr(kind,"-s")) intype=GO4EV_MBS_STREAM;
	else if(strstr(kind,"-e")) intype=GO4EV_MBS_EVENTSERVER;
	else if(strstr(kind,"-ra"))intype=GO4EV_MBS_RANDOM;
	else if(strstr(kind,"-r")) intype=GO4EV_MBS_REVSERV;
	else {
		cout << "invalid input type: " << kind << endl;
	}
	TGo4Log::Instance();        // init logger object
	TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
	// set this to 2 to get warnings and errors only
	// set this to 3 to get errors only
	TGo4Log::OpenLogfile("TascaLogfile.txt","--Logfile for go4 Tasca--",kFALSE);
	TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages

	TGo4StepFactory*  unpackfactory  = new TGo4StepFactory("UnpackFact");
	unpackfactory->DefEventProcessor("Unpacker","TascaUnpackProc");// object name, class name
	unpackfactory->DefOutputEvent("Unpacked","TascaUnpackEvent"); // object name, class name
	TGo4AnalysisStep* unpackstep     = new TGo4AnalysisStep("Unpacker",unpackfactory,0,0,0);
	AddAnalysisStep(unpackstep);
	if(intype==GO4EV_MBS_FILE)
		unpackstep->SetEventSource(new TGo4MbsFileParameter(serv));
	if(intype==GO4EV_MBS_TRANSPORT)
		unpackstep->SetEventSource(new TGo4MbsTransportParameter(serv));

	TGo4StepFactory*  califactory  = new TGo4StepFactory("CaliFact");
	califactory->DefEventProcessor("Calibrator","TascaCaliProc");// object name, class name
	califactory->DefInputEvent("Unpacked","TascaUnpackEvent"); // object name, class name
	califactory->DefOutputEvent("Calibrated","TascaCaliEvent"); // object name, class name
	TGo4AnalysisStep* calistep     = new TGo4AnalysisStep("Calibrator",califactory,0,0,0);
	AddAnalysisStep(calistep);

	TGo4StepFactory*  checkfactory  = new TGo4StepFactory("CheckFact");
	checkfactory->DefEventProcessor("Checker","TascaCheckProc");// object name, class name
	checkfactory->DefInputEvent("Calibrated","TascaCaliEvent"); // object name, class name
	checkfactory->DefOutputEvent("Checked","TascaCheckEvent"); // object name, class name
	TGo4AnalysisStep* checkstep     = new TGo4AnalysisStep("Checker",checkfactory,0,0,0);
	AddAnalysisStep(checkstep);

	TGo4StepFactory*  analysisfactory  = new TGo4StepFactory("AnalysisFact");
	analysisfactory->DefEventProcessor("Analyzer","TascaAnlProc");// object name, class name
	analysisfactory->DefInputEvent("Checked","TascaCheckEvent"); // object name, class name
	analysisfactory->DefOutputEvent("Analyzed","TascaAnlEvent"); // object name, class name
	TGo4AnalysisStep* analysisstep     = new TGo4AnalysisStep("Analyzer",analysisfactory,0,0,0);
	AddAnalysisStep(analysisstep);

	DefineServerPasswords("tascadmin", "tascactrl", "tascaview");

	// use macros to set up
	snprintf(macro,1023,".x setup.C(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",
			idir,odir,ASfile,Unpout,Calout,Chkout,Anlout);
	gROOT->ProcessLine(macro);


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
  cout << "**************************************" << endl;
  cout << "Tasca> TascaAnalysis: PreLoop" << endl;
  Print(); // printout the step settings
  cout << "**************************************" << endl;
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
