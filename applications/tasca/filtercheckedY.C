//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void filtercheckedY(const char* rootinfile, const char* rootfile, unsigned int events)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

  char filename[128];
  TString fname(filename);
  UInt_t totalevents=0,takenevents=0;
  UInt_t maxevents=0;
  if(events == 0)maxevents=0xffffffff;
  else maxevents=events;

// TYasca event
   TascaCheckEvent* eve=new TascaCheckEvent();
// opne root file for output
   TFile *outfile = new TFile(rootfile,"RECREATE");
// create tree
   TTree * newTree = new TTree("CheckerTree","new");
// create one branch. The dot adds the event name to each member name
   newTree->Branch("Checked.","TascaCheckEvent",&eve,100000,1);
   cout<<"Select StepYHhitI >= 0"<<endl;
   cout<<"Copy "<<rootinfile<<" to "<<rootfile<<endl;

// open root file for read
     TFile infile(rootinfile);
     TTree* oldTree=0;
     oldTree=(TTree *)infile.Get("CheckerTree");
     TBranch* br=oldTree->GetBranch("Checked.");
     if(br != 0){
     TBranch* brfilter=oldTree->GetBranch("Checked.fiStopYHhitI");
     if(brfilter != 0){
     br->SetAddress(&eve);
     Int_t nentries = oldTree->GetEntries();
     printf("events: %9d ",nentries);
     for (Int_t i=0;i<nentries;i++)
     {
       brfilter->GetEntry(i);
       if(eve->fiStopYHhitI >= 0){
         oldTree->GetEntry(i);
         newTree->Fill();
	     takenevents++;
       }
       totalevents ++;
       if(totalevents >= maxevents) break;
     }
     printf("Total %9d, taken %9d %d%\n",totalevents,takenevents);
     }// filter found
     else printf("Filter not found\n");
     }// branch found
     else
       printf(" no data\n");
     infile.Close(); // close input root file
   // newTree->Print("toponly");
   cout<<"Total events processed "<<totalevents<<endl;
   if(!gROOT->IsBatch()){
     new TBrowser();
     newTree->StartViewer();
   }
   outfile->Write();
   outfile->Close();
   exit(0);
}
