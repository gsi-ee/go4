//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaAnlPROCESSOR_H
#define TascaAnlPROCESSOR_H

#include "TGo4EventProcessor.h"

class TascaCheckEvent;
class TascaControl;
class TascaParameter;
class TascaAnlEvent;
class TascaAnalysis;
class TascaEvent;
class TascaEventStack;
#include "TList.h"
#include "TFile.h"
#include "TTree.h"

class TascaAnlProc : public TGo4EventProcessor {
public:
	TascaAnlProc() ;
	TascaAnlProc(const char * name);
	virtual ~TascaAnlProc();

	void TascaEventAnalysis(TascaAnlEvent* target);
	void PrintFission(Bool_t store);
	void PrintAlpha(Bool_t store);
	void PrintEvr(Bool_t store);
	inline UInt_t TimeDiff(UInt_t later, UInt_t earlier){
		if(later<earlier)
			return 0xFFFFFFFF-earlier+later+1;
		else return later-earlier;
	}

	TascaAnalysis  * anl;
	TascaCheckEvent *fInput;
	TascaControl  *fControl;
	TascaParameter *fParam;
	TH2D* fStopXYalp;
	TH2D* fStopXYEvr;
	TH2D* fStopXYSF ;
	TH2D* fStopXYSFoff ;
	TH2D* fStopXYall;
	TH2D* fStopXYcalp;
	TH2D* fStopXYcEvr;
	TH2D* fStopXYcSF ;
	TH2D* fStopXYcall ;
	TH2D* fStopXYtalp;
	TH2D* fStopXYtEvr;
	TH2D* fStopXYtSF ;
	TH2D* fStopXYtSFoff ;
	TH2D* fStopXYtall;
	TGo4WinCond   *fwinEVR;
	TGo4WinCond   *fwinAlpha;
	TGo4WinCond   *fwinFission1;
	TGo4WinCond   *fwinAlpha1;
	TGo4WinCond   *fwinAlpha2;
	TGo4WinCond   *fwinFission2;
	TGo4WinCond   *fwinBack;
	TascaEventStack *fEventStack;
	TListIter *fStackIter;
	TListIter *fEventIter;
	TascaEvent *fEventCurrent;
	TascaEvent *fFissionEvent;
	TascaEvent *fAlphaEvent;
	TascaEvent *fStackEvent;
	TascaEvent *fEvent;
	TObjLink *fStackLinkCurrent;
	TObjLink *fStackLinkEntry;
	TFile* fChainFile;
	TTree* fChainTree;
	TBranch* fChainBranch;
	UInt_t fChainNumber;
	Bool_t fChainStore;
	UInt_t i,ii;
	UInt_t fFissions;
	UInt_t fAlphas;
	UInt_t fEvrs;
	UInt_t fFirstEvent;
	Bool_t fStackfilled;
	Bool_t fAlphaFound;
	Bool_t fEvrFound;
	UInt_t fTimeDiff;
	UInt_t fiEventsProcessed;
	UInt_t fiEventsWritten;
	UInt_t fiFileNumber;
	UInt_t fiSFprocessed;
	UInt_t fiSFtaken;
	UInt_t fiEvprocessed;
	UInt_t fiEvprocessedTotal;
	UInt_t fiLastFissionEvent;
	Text_t cfilename[256];

	ClassDef(TascaAnlProc,1)
};
#endif //TascaAnlPROCESSOR_H
