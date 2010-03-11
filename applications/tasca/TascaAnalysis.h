//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaANALYSIS_H
#define TascaANALYSIS_H

#include "TGo4Analysis.h"
#include "TGo4Picture.h"

class TFile;
class TH1;
class TCutG;
class TascaParameter;
class TGo4Picture;
class TGo4Condition;
class TGo4WinCond;
class TGo4PolyCond;
class TGo4CondArray;

class TascaAnalysis : public TGo4Analysis  {
public:
	TascaAnalysis();
	TascaAnalysis(int argc, char** argv);
	~TascaAnalysis();

	Int_t UserPreLoop();
	Int_t UserEventFunc();
	Int_t UserPostLoop();

	TNamed* CreateObject(const Text_t* type, const Text_t* name, Int_t par1=0);
	TGo4Parameter* CreateParameter(const Text_t* type, const Text_t* name);
	void SetPicture(TGo4Picture *p, TH1 *h, UInt_t ir, UInt_t ic, UInt_t mode);
	TGo4Picture* CreatePicture(const Text_t* folder, const Text_t* name,
			const Text_t* title, UInt_t ir, UInt_t ic);
	TGo4Condition* CreateCondition(const Text_t* folder, const Text_t* name,
			Int_t dim, Bool_t reset, Double_t low, Double_t high);
	TGo4Condition* CreateCondition(const Text_t* folder, const Text_t* name,
			Int_t dim, Bool_t reset, TCutG* poly);
	TH1I* CreateTH1I(const Text_t* folder, const Text_t* name, const Text_t* title,
			UInt_t channels, Double_t low, Double_t high);
	TH1D* CreateTH1D(const Text_t* folder, const Text_t* name, const Text_t* title,
			UInt_t channels, Double_t low, Double_t high);
	TH2I* CreateTH2I(const Text_t* folder, const Text_t* name, const Text_t* title,
			UInt_t channels1, Double_t low1, Double_t high1,
			UInt_t channels2, Double_t low2, Double_t high2);
	TH2D* CreateTH2D(const Text_t* folder, const Text_t* name, const Text_t* title,
			UInt_t channels1, Double_t low1, Double_t high1,
			UInt_t channels2, Double_t low2, Double_t high2);
	TH1I* CreateTH1I(const Text_t* folder, const Text_t* name, const Text_t* title,
			const Text_t* xlett, const Text_t* ylett,
			UInt_t channels, Double_t low, Double_t high);
	TH1D* CreateTH1D(const Text_t* folder, const Text_t* name, const Text_t* title,
			const Text_t* xlett, const Text_t* ylett,
			UInt_t channels, Double_t low, Double_t high);
	TH2I* CreateTH2I(const Text_t* folder, const Text_t* name, const Text_t* title,
			const Text_t* xlett, const Text_t* ylett, const Text_t* zlett,
			UInt_t channels1, Double_t low1, Double_t high1,
			UInt_t channels2, Double_t low2, Double_t high2);
	TH2D* CreateTH2D(const Text_t* folder, const Text_t* name, const Text_t* title,
			const Text_t* xlett, const Text_t* ylett, const Text_t* zlett,
			UInt_t channels1, Double_t low1, Double_t high1,
			UInt_t channels2, Double_t low2, Double_t high2);
private:
	TascaParameter      *fParam;
	UInt_t events;
	Text_t hostname[128]; // hostname used by GUI
	Text_t macro[1024];   // input name
	Text_t serv[512];     // input name
	Text_t Unpout[512];   // output root events
	Text_t Calout[512];   // output root events
	Text_t Chkout[512];   // output root events
	Text_t Anlout[512];   // output root events
	Text_t ASfile[512];   // auto save file (batch)
	Text_t filetype[8];   // file type .lmd or .lml
	Text_t kind[32];      // kind of source
	Text_t input[128];    // input spec
	Text_t odir[128];     // output directory
	Text_t idir[128];     // input directory
	Text_t prefix[128];   // name prefix: b for batch, i for interactive
	Int_t  intype;        // type of source

	ClassDef(TascaAnalysis,1)
};

#endif //TascaANALYSIS_H



