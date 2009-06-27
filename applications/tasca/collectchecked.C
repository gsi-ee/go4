//---------------------------------------------
// Go4 Tasca analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void collectchecked(const char* dirfile, const char* rootfile, unsigned int events)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

  char filename[256];
  UInt_t alphas=0,fissions=0,evrs=0,totalevents=0,skippednoY=0;
  UInt_t offalphas=0,offfissions=0,offtotalevents=0;
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
// open root file for read
     TFile* infile=new TFile(rootinfile);
     TTree* oldTree=0;
     oldTree=(TTree *)infile->Get("CheckerxTree");
     TBranch* br=oldTree->GetBranch("Checked.");
     if(br != 0){
     br->SetAddress(&eve);
     Int_t nentries = oldTree->GetEntries();
     printf("%s events: %8d ",filename,nentries);
     for (Int_t i=0;i<nentries;i++) 
     {
       oldTree->GetEntry(i);
       if(eve->fisAlpha)alphas++;
       if(eve->fisFission)fissions++;
       if(eve->fisEvr)evrs++;
       if(eve->fisAlpha)alphas++;
       totalevents++;
       if(!eve->fisMacro){
         if(eve->fisAlpha)offalphas++;
         if(eve->fisFission)offfissions++;
         if(eve->fisAlpha)offalphas++;
         offtotalevents++;
       }
       // skip alpha without Y and Fission without Y
       if((eve->fisAlpha&&(eve->fiStopYLhitI<0))||
	  (eve->fisFission&&(eve->fiStopYHhitI<0))||
	  (eve->fisEvr&&(eve->fiStopYHhitI<0)))skippednoY++;
       else newTree->Fill();
       if(totalevents >= maxevents) break;
     }
     printf("(%6d) AL %8d (%6d) SF %8d (%6d) EVR %8d -%8d\n",
	    offtotalevents,alphas,offalphas,fissions,offfissions,evrs,skippednoY);
     }// branch found
     else
       printf(" no data\n");
     infile->Close(); // close input root file
     if(totalevents >= maxevents) break;
     skippednoY=0;
   } // one input root file
   // newTree->Print("toponly");
   cout<<"Total events processed "<<totalevents<<" skipped "<<skippednoY<<endl;  
   if(!gROOT->IsBatch()){
     new TBrowser();
     newTree->StartViewer();
   }
   outfile->Write();
   outfile->Close();
   exit(0);
}
