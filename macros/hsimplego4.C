{
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: This script requires call of .x go4Init.C before running!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//*********************************************************
// hsimplego4.C - modified standard root example to run
// with go4 analysis server environment
// Shows how to use some basic calls to go4 framework
// to let this macro be controllable from go4 gui
// (J.Adamczewski, 25-Apr-2005)
//*********************************************************
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*
//*-*  This program creates :
//*-*    - a one dimensional histogram
//*-*    - a two dimensional histogram
//*-*    - a profile histogram
//*-*    - a memory-resident ntuple
//*-*
//*-*  These objects are filled with some random numbers and saved on a file.
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//gROOT->Reset();
//gROOT->SetBatch(kTRUE);

TGo4Analysis* go4=TGo4Analysis::Instance();
go4->CloseAnalysis(); // make this macro reentrant: cleanup dynamic list, etc
go4->InitEventClasses(); // dito, initialize compiled analysis if there is one
// Create a new canvas.
  c1 = new TCanvas("c1","Dynamic Filling Example",200,10,700,500);
  c1->SetFillColor(42);
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

// Create a new ROOT binary machine independent file.
// Note that this file may contain any kind of ROOT objects, histograms,
// pictures, graphics objects, detector geometries, tracks, events, etc..
// This file is now becoming the current directory.

   TFile *hfile = (TFile*)gROOT->FindObject("hsimple.root"); if (hfile) hfile->Close();
   hfile = TFile::Open("hsimple.root","RECREATE","Demo ROOT file with histograms");

// Create some histograms, a profile histogram and an ntuple
  hpx    = new TH1F("hpx","This is the px distribution",100,-4,4);
  hpxpy  = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
  hprof  = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);



  ntuple = new TNtuple("ntuple","Demo ntuple","px:py:pz:random:i");

// Go4 part: register objects

go4->AddHistogram(hpx);
go4->AddHistogram(hpxpy);
go4->AddHistogram(hprof);
go4->AddTree(ntuple);

//  Set canvas/frame attributes (save old attributes)
  hpx->SetFillColor(48);

// Fill histograms randomly
  gRandom->SetSeed();
  Float_t px, py, pz;
  const Int_t kUPDATE = 1000;
  Int_t i=0;
while(1){
   std::cout <<"Waiting for the Go4 start button.";
   std::cout <<" \n\tUse Canvas menu 'Options/Interrupt' to leave macro." << std::endl;
   Int_t seconds=go4->WaitForStart();
   if(seconds<0) break; // react on cint canvas interrupt
   std::cout <<"Starting execution loop after "<<seconds<<" s of waiting" << std::endl;
   gBenchmark->Reset();
   gBenchmark->Start("hsimple");
   ///// event loop here:
   while(go4->Process()==0){ // inner event loopcycle, returns -1 if go4 is stopped
     // user event loop follows here:
     gRandom->Rannor(px,py);
     pz = px*px + py*py;
     Float_t random = gRandom->Rndm(1);
     hpx->Fill(px);
     hpxpy->Fill(px,py);
     hprof->Fill(px,pz);
     ntuple->Fill(px,py,pz,random,i);
     if (i && (i%kUPDATE) == 0) {
        if (i == kUPDATE) hpx->Draw();
        c1->Modified();
        c1->Update();
        if (gSystem->ProcessEvents()) // enable root system events, returns nonzero in case of user interrupt
           {
               go4->SetRunning(kFALSE); // stop go4 loop on canvas interrupt
               gROOT->SetInterrupt(kFALSE); // clear root interrupt flag
               break;
           }

     }
     i++;
   } // while inner
   ///// end event loop
  gBenchmark->Show("hsimple");
// Save all objects in this file
   hpx->SetFillColor(0);
   hfile->Write();
   hpx->SetFillColor(48);
   c1->Modified();
}// while outer
  gROOT->SetInterrupt(kFALSE); // reset interrupt flag after leaving loop
  go4->RemoveTree(ntuple); // to be reentrant
// Note that the file is automatically close when application terminates
// or when the file destructor is called.

}
