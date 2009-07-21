//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------
// reads *_Calibrated.root file with TascaCaliEvent
// Writes ROOT file with histograms
void anacalievent(const char* dir, const char* rootinfile, int events)
{
  //gSystem->Load("libGo4UserAnalysis.so"); // if there is no .rootmap file!

  char filename[128];
  char rootname[128];
  UInt_t fileindex=0;
  UInt_t totaltime=0,starttime,endtime,lasttime=0;
  UInt_t totalevents=0;
  TString fname(filename);
  TString rfile(rootname);
  TFile* infile;
  rfile.Form("%s_histo.root",rootinfile);
  TH1I* h1=new TH1I("Timediff","Event time diff [ms]",40000,0,40000);
  TH1I* h2=new TH1I("MultiH","Multiplicity StopXH",100,0,100);
  TH1I* h3=new TH1I("MultiL","Multiplicity StopXL",100,0,100);
  TH1I* h2g=new TH1I("MultiHg","Multiplicity StopXH gate E",100,0,100);
  TH1I* h3g=new TH1I("MultiLg","Multiplicity StopXL gate E",100,0,100);
// TYasca event
   TascaCaliEvent* eve=new TascaCaliEvent();
for(fileindex=0;fileindex<1000;fileindex++){
	   fname.Form("%sb_%s%1d_Calibrated.root",dir,rootinfile,fileindex);

// open root file for read
     infile=new TFile(fname.Data());
     TTree* oldTree=0;
     oldTree=(TTree *)infile->Get("CalibratorxTree");
     if(oldTree==0)oldTree=(TTree *)infile->Get("CalibratorTree");
     if(oldTree==0){ cout<<"Tree CalibratorTree not found"<<endl; break;}

     TBranch* br=oldTree->GetBranch("Calibrated.");
     if(br == 0){ cout<<"Branch Calibrated not found"<<endl; break;}
     br->SetAddress(&eve);
     Int_t nentries = oldTree->GetEntries();
     if(events>0)nentries=events;
     cout<<fname.Data()<<endl;;
     printf(" Events: %9d ",nentries);
     oldTree->GetEntry(0);
     starttime=eve->fiSystemmsec;
     oldTree->GetEntry(nentries-1);
     endtime=eve->fiSystemmsec;
     totaltime+=(endtime-starttime);
     totalevents += nentries;
     printf(" total %10d time [s] %7d total %7d\n",totalevents,(endtime-starttime)/1000,totaltime/1000);
     TBranch* brmsec=oldTree->GetBranch("Calibrated.fiSystemmsec");
     if(brmsec == 0){ cout<<"Branch fiSystemmsec not found"<<endl; break;}
     TBranch* brxhm=oldTree->GetBranch("Calibrated.fiMultiStopXH");
     if(brxhm == 0){ cout<<"Branch fiMultiStopXH not found"<<endl; break;}
     TBranch* brxlm=oldTree->GetBranch("Calibrated.fiMultiStopXL");
     if(brxlm == 0){ cout<<"Branch fiMultiStopXL not found"<<endl; break;}
     TBranch* brxhv=oldTree->GetBranch("Calibrated.ffStopXHhitV");
     if(brxhv == 0){ cout<<"Branch ffStopXHhitv not found"<<endl; break;}
     TBranch* brxlv=oldTree->GetBranch("Calibrated.ffStopXLhitV");
     if(brxlv == 0){ cout<<"Branch fiMultiStopXL not found"<<endl; break;}
     for (Int_t i=0;i<nentries;i++)
     {
         brmsec->GetEntry(i);
         if(lasttime==0)lasttime=eve->fiSystemmsec;
         h1->Fill(eve->fiSystemmsec-lasttime);
         lasttime=eve->fiSystemmsec;
         brxhm->GetEntry(i);
         brxhv->GetEntry(i);
         brxlm->GetEntry(i);
         brxlv->GetEntry(i);
         h2->Fill(eve->fiMultiStopXH);
         h3->Fill(eve->fiMultiStopXL);
         if(eve->ffStopXHhitV>4000)h2g->Fill(eve->fiMultiStopXH);
         if(eve->ffStopXLhitV>4000)h3g->Fill(eve->fiMultiStopXL);
     }
     infile->Close(); // close input root file
   }
   TFile rootfile(rfile,"RECREATE");
   cout<<rfile.Data()<<endl;
   h1->Write();
   h2->Write();
   h3->Write();
   h2g->Write();
   h3g->Write();
   rootfile.Close();
   exit(0);
}
