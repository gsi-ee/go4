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

// TYasca event
   TascaEvent* eve=new TascaEvent();
   cout<<rootinfile<<endl;

// open root file for read
     TFile infile(rootinfile);
     TTree* oldTree=0;
     oldTree=(TTree *)infile.Get("SearcherTree");
     TBranch* br=oldTree->GetBranch("Searched.");
     if(br != 0){
     TBranch* brfilter=oldTree->GetBranch("Searched.fiChainNumber");
     if(brfilter != 0){
     br->SetAddress(&eve);
     Int_t nentries = oldTree->GetEntries();
     for (Int_t i=0;i<nentries;i++)
     {
       brfilter->GetEntry(i);
       if(eve->fiChainNumber==chain){
         oldTree->GetEntry(i);
         eve->PrintEvent();
       }
     }}}
     infile.Close(); // close input root file
   exit(0);
}
