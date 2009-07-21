//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void printcalievent(const char* rootinfile, unsigned int event)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

  char filename[128];
  TString fname(filename);

// TYasca event
   TascaCaliEvent* eve=new TascaCaliEvent();
   cout<<rootinfile<<endl;

// open root file for read
     TFile* infile=new TFile(rootinfile);
     TTree* oldTree=0;
     oldTree=(TTree *)infile->Get("CalibratorxTree");
     if(oldTree==0)oldTree=(TTree *)infile->Get("CalibratorTree");
     if(oldTree==0){
       cout<<"Tree not found"<<endl;
       exit(0);
     }
     TBranch* br=oldTree->GetBranch("Calibrated.");
     if(br != 0){
     TBranch* brfilter=oldTree->GetBranch("Calibrated.fiEventNumber");
     if(brfilter != 0){
     br->SetAddress(&eve);
     Int_t nentries = oldTree->GetEntries();
     for (Int_t i=0;i<nentries;i++)
     {
       brfilter->GetEntry(i);
       if(eve->fiEventNumber==event){
         oldTree->GetEntry(i);
         eve->PrintEvent();
         break;
       }
     }}}
     infile->Close(); // close input root file
   exit(0);
}
