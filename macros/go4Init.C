// macro for initialisation of Go4 communication classes

void go4Init()
{
gSystem->Load("libThread.so");
gSystem->Load("libMinuit.so");
gSystem->Load("$GO4SYS/lib/libGo4Base.so");
gSystem->Load("$GO4SYS/lib/libGo4Fit.so");
gSystem->Load("$GO4SYS/lib/libGo4ThreadManager.so");
gSystem->Load("$GO4SYS/lib/libGo4TaskHandler.so");
gSystem->Load("$GO4SYS/lib/libGo4Version.so");
gSystem->Load("$GO4SYS/lib/libGo4AnalBase.so");
gSystem->Load("$GO4SYS/lib/libGo4Analysis.so");
go4StartServer();
}

void go4StartServer()
{
    TDirectory* savdir=gDirectory;
    TGo4Log::Instance(); // init logger object
    TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
                               // set this to 2 to get warnings and errors only
                               // set this to 3 to get errors only
    TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages to file

    TGo4Analysis* go4 = 0;

    if (TGo4Analysis::Exists()) {
       go4 = TGo4Analysis::Instance();
       go4->CloseAnalysis(); // make this macro reentrant: cleanup dynamic list, etc
       go4->InitEventClasses(); // dito, initialize compiled analysis if there is one
    } else {
       new TGo4AnalysisClient("Go4CintServer", // name
                              0, // external analysis (use internal if 0)
                              "localhost", // node (dummy)
                              1234, // port (dummy)
                              0,  // histogram server on/off
                              "", // histogram server basename
                              "", // histogram server passwd
                              kTRUE, // start this slave as server
                              kFALSE, // do not autorun analysis
                              kTRUE); // use in CINT enviroment
       go4 = TGo4Analysis::Instance();
   }
   if(savdir) savdir->cd();
}

void go4RegisterAll()
{
TGo4Analysis* go4 = TGo4Analysis::Instance();
TDirectory* gdir = gDirectory;
//   TIter caniter(gROOT->GetListOfCanvases());
//   TCanvas* can = 0;
//   while (can = caniter())
//      go4->AddCanvas(can);

//   go4->DeleteObjects("Histograms");

   if (gdir!=0) {
      gdir->cd();
      TIter iter(gdir->GetList());
      TObject* obj = 0;
      while (obj = iter()) {
         if (obj->InheritsFrom(TH1::Class()))
           go4->AddHistogram((TH1*)obj);
      }
   }
}

