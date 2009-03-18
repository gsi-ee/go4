
void treedrawgo4(const char* filename)
{
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: This script requires call of .x go4Init.C before running!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//*********************************************************
// treedrawgo4.C -
// examples of treedraw with go4 analysis server environment
// Shows how to use some basic calls to go4 framework

// to let this macro be controllable from go4 gui
// example1: direct call of TTree::Draw on registered histograms
// example2: register Tree to go4 and perform dynamic treedraw
//           from go4 gui
// (J.Adamczewski, 25-Apr-2005)


// load additional user libaries here:
gSystem->Load("$GO4SYS/Go4Example2Step/libGo4UserAnalysis.so");

TString fname(filename);
if(!fname.EndsWith(".root")) fname.Append(".root");
TGo4Analysis* go4=TGo4Analysis::Instance();
go4->CloseAnalysis(); // make this macro reentrant: cleanup dynamic list, etc
go4->InitEventClasses(); // dito, initialize compiled analysis if there is one

//gROOT->SetBatch(kFALSE); // when not running in cint server
//TFile* myfile= new TFile("Example2StepAnl.root");
TFile myfile(fname.Data());
//myfile.ls();
TTree* theTree=0;
TKey* kee=0;
TIter iter(myfile.GetListOfKeys());
   while ( ( kee=dynamic_cast<TKey*>(iter()) ) !=0 ) {
      theTree = dynamic_cast<TTree*>(kee->ReadObj());
      if (theTree)
         break; // we take first Tree in file, disregarding its name...
   }
if(theTree)
  {
    go4->AddTree(theTree);
    go4->Message(1,"Loaded tree %s from file %s",theTree->GetName(),myfile.GetName());
////////////////////////////////////////////
    /// example 1: perform treedraw action
    // note: resulthistogram will be filled only after Draw is finished
    // (buffering of ROOT TSelectorDraw object)
    // so go4 monitoring will not show the filling of the histogram
    TH2D* histo2d=go4->GetHistogram("map1");
    if(histo2d==0)
      {
        histo2d=new TH2D("map1","result of treedraw example",100,0,4000,100,0,4000);
        go4->AddHistogram(histo2d);
      }
    TH1D* histo1d=go4->GetHistogram("his1");
    if(histo1d==0)
      {
        histo1d=new TH1D("his1","result of treedraw example",4000,0,4000);
        go4->AddHistogram(histo1d);
      }
     gROOT->cd(); // set current directory to the top, otherwise TTree::Draw
                  // will not find the target histograms as registered in Go4!!
     theTree->Draw("XXXAnlEvent.frData[0]:XXXAnlEvent.frData[1] >>+map1","XXXAnlEvent.frData[0]>500");
     theTree->Draw("XXXAnlEvent.frData[0]>>+his1");
     go4->Message(1,"Tree Draw has finished.");
///////////////////////////////////////////////////////////
    /// example 2: run go4 main loop after the start button
    // and perform dynamic tree draw on registered tree
    go4->Message(1,"Please create dynamic entry for tree and press go4 start.");
    while(1)
      {
        if(go4->WaitForStart()<0) break; // negative value means root canvas interrupt, leave loop
        while(go4->Process()==0){;} // inner event loop, returns -1 if go4 is stopped
      } // outer go4 loop
    gROOT->SetInterrupt(kFALSE); // reset interrupt flag after leaving loop
    go4->RemoveTree(theTree); // to be reentrant: pointer to tree is no longer valid after file has been closed
   }
else
  {
    go4->Message(3,"No Tree in File %s", myfile->GetName());
  }




}
