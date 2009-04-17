void tree(const char* filename, const char *detector, int channel)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

  // histograms like in analysis
  TH2I * fc1x2 = new TH2I("c1x2","Crate 1 channel 1x2",200,1,5000,200,1,5000);

  TString fname(filename);
  TString dname(detector);
  TString title=TString::Format("Channel %d",channel);
  
  TH1I * fHis1 = new TH1I (dname.Data(),title.Data(),1000,1,5000);
  
  if(!fname.EndsWith(".root")) fname.Append(".root");
  TFile myfile(fname.Data());
  TTree* theTree=0;
  TKey* kee=0;
  TIter iter(myfile.GetListOfKeys());
   while ( ( kee=dynamic_cast<TKey*>(iter()) ) !=0 ) {
      theTree = dynamic_cast<TTree*>(kee->ReadObj());
      if (theTree)
         break; // we take first Tree in file, disregarding its name...
   }
   if(theTree==0)
     {
       cout <<"Error: no Tree in file "<<fname.Data() << endl;
       return;
     }
  
   // Create an event of our class
   TascaUnpackEvent* eve=new TascaUnpackEvent;
   // We must give the name of the Event object (was set in setup.C)
   theTree->SetBranchAddress("Unpacked.",&eve);
   Int_t nentries = theTree->GetEntries();
   cout <<"Starting tree read for "<<nentries<< " events..." << endl;
if(dname.BeginsWith("StopXL")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiStopXL[channel]);
     }
else if(dname.BeginsWith("StopXH")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiStopXH[channel]);
     }
else if(dname.BeginsWith("StopYL")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiStopYL[channel]);
     }
else if(dname.BeginsWith("StopYH")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiStopYH[channel]);
     }
else if(dname.BeginsWith("BackL")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiBackL[channel]);
     }
else if(dname.BeginsWith("BackH")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiBackH[channel]);
     }
else if(dname.BeginsWith("VetoL")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiVetoL[channel]);
     }
else if(dname.BeginsWith("VetoH")) 
   for (Int_t i=0;i<nentries;i++) 
     {
       theTree->GetEntry(i);
       fHis1->Fill(eve->fiVetoH[channel]);
     }
else cout << "Detector " << detector << " not found in " << filename  << endl;
   //cout <<"Printout last event:" << endl;
   //theTree->Show(); // printout  contents of last event!
   fHis1->Draw();
}
