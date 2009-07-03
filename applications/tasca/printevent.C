//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void printevent(const char* rootinfile, unsigned int chain)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

char filename[128];
TString fname(filename);
Bool_t select;
UInt_t run,curchain,lastms;
Int_t i=0;

// TYasca event
TascaEvent* eve=new TascaEvent();
cout<<rootinfile<<endl;

// open root file for read
TFile infile(rootinfile);
TTree* oldTree=0;
oldTree=(TTree *)infile.Get("SearcherTree");
TBranch* br=oldTree->GetBranch("Searched.");
if(br == 0){cout<<"Branch SearcherTree not found"<<endl;exit(0);}
TBranch* brfilter=oldTree->GetBranch("Searched.fiChainNumber");
if(brfilter == 0){cout<<"Branch not found"<<endl;exit(0);}
br->SetAddress(&eve);
Int_t nentries = oldTree->GetEntries();
if(chain>0){
	for (i=0;i<nentries;i++)
	{
		brfilter->GetEntry(i);
		if(eve->fiChainNumber==chain){
			oldTree->GetEntry(i);
			eve->PrintEvent();
		}
	}}
else {
	i=0;
	while(i<nentries)
	{
		oldTree->GetEntry(i);
		select=kFALSE;
		curchain=0;
		run=eve->fiFileNumber>>16;
		     if((run == 67)&(eve->fiStopXLhitI ==  91))curchain=eve->fiChainNumber;
				else if((run == 72)&(eve->fiStopXLhitI == 103))curchain=eve->fiChainNumber;
				else if((run == 73)&(eve->fiStopXLhitI == 83))curchain=eve->fiChainNumber;
				else if((run == 86)&(eve->fiStopXLhitI == 102))curchain=eve->fiChainNumber;
				else if((run == 89)&(eve->fiStopXLhitI == 110))curchain=eve->fiChainNumber;
				else if((run == 89)&(eve->fiStopXLhitI == 119))curchain=eve->fiChainNumber;
				else if((run == 124)&(eve->fiStopXLhitI == 131))curchain=eve->fiChainNumber;
				else if((run == 42)&(eve->fiStopXLhitI == 22))curchain=eve->fiChainNumber;
				else if((run == 59)&(eve->fiStopXLhitI == 59))curchain=eve->fiChainNumber;
				else if((run == 114)&(eve->fiStopXLhitI == 86))curchain=eve->fiChainNumber;
				else if((run == 156)&(eve->fiStopXLhitI == 89))curchain=eve->fiChainNumber;
				else if((run == 163)&(eve->fiStopXLhitI == 100))curchain=eve->fiChainNumber;
				else if((run == 179)&(eve->fiStopXLhitI == 122))curchain=eve->fiChainNumber;
				else if((run == 198)&(eve->fiStopXLhitI == 59))curchain=eve->fiChainNumber;
				else if((run == 219)&(eve->fiStopXLhitI == 89))curchain=eve->fiChainNumber;
		else i++;
		lastms=eve->fiSystemmsec;
		while(curchain==eve->fiChainNumber){
			if((eve->fisAlpha)&(eve->ffBackLhitV>4000))eve->ffStopXLhitV += eve->ffBackLhitV;
			if((eve->fisFission)&(eve->ffBackHhitV>4000))eve->ffStopXHhitV += eve->ffBackHhitV;
			eve->fiDeltaSystemTime=eve->fiSystemmsec-lastms;
			eve->PrintEvent(kTRUE);
			lastms=eve->fiSystemmsec;
		   i++;
		   if(i==nentries)break;
		   oldTree->GetEntry(i);
		}
		if(curchain>0)cout<<"---------------------------------------"<<endl;
	}
}
infile.Close(); // close input root file
exit(0);
}
