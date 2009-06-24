//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void filtercheckedOff(const char* dirfile, const char* rootfile, unsigned int events)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

  char filename[128];
  TString fname(filename);
  UInt_t firstevent=0,lastevent=0,totalevents=0,takenevents=0;
  UInt_t maxevents=0;
  if(events == 0)maxevents=0xffffffff;
  else maxevents=events;

// open dir file with one root file name per line
  FILE* flist=fopen(dirfile,"r");
// TYasca event
   TascaCheckEvent* eve=new TascaCheckEvent();
// opne root file for output
   TFile *outfile = new TFile(rootfile,"RECREATE");
// create tree
   TTree * newTree = new TTree("CheckerTree","new");
// create one branch. The dot adds the event name to each member name
   newTree->Branch("Checked.","TascaCheckEvent",&eve,100000,1);
   cout<<"Write "<<rootfile<<endl;

// loop over file names from dirfile
// -------------------------------------------------
   while(fgets(&filename,120,flist)){
     filename[strlen(filename)-1]=0; // strip CR
     if(filename[0]=='#') continue;
     if(filename[0]=='!') continue;
     printf("%s ",filename);
// open root file for read
     TFile infile(filename);
     TTree* oldTree=0;
     oldTree=(TTree *)infile.Get("CheckerxTree");
     TBranch* br=oldTree->GetBranch("Checked.");
     if(br != 0){
     TBranch* brfilter=oldTree->GetBranch("Checked.fisMacro");
     if(brfilter != 0){
     br->SetAddress(&eve);
     Int_t nentries = oldTree->GetEntries();
     oldTree->GetEntry(0);
     firstevent= eve->fiEventNumber;
     printf("events: %9d first %9d ",nentries,firstevent);
     for (Int_t i=0;i<nentries;i++)
     {
       brfilter->GetEntry(i);
       if(!eve->fisMacro){
         oldTree->GetEntry(i);
         lastevent= eve->fiEventNumber;
         newTree->Fill();
	 takenevents++;
       }
       totalevents ++;
       if(totalevents >= maxevents) break;
     }
     printf("last %9d taken %9d %d%\n",lastevent, takenevents,100*nentries/(lastevent-firstevent));
     }// filter found
     else printf("Filter not found\n");
     }// branch found
     else
       printf(" no data\n");
     infile.Close(); // close input root file
     if(totalevents >= maxevents) break;
   } // one input root file
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
