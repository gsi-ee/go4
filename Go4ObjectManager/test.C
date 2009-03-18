void test() {

   gSystem->Load("libGo4ObjMng.so");

   new TH1I("testhisto1","histo title", 100, 1., 100.);
   new TH1I("testhisto2","histo title", 100, 1., 100.);
   new TH1I("testhisto3","histo title", 100, 1., 100.);

   TGo4ObjectManager objm("ObjManager","Go4 object manager");

   TNamed* name1 = new TNamed("name1","name1 title");
   TNamed* name2 = new TNamed("name2","name2 title");
   TNamed* name3 = new TNamed("name3","name3 title");

   objm.Add("folder1/folder2", name1);
   objm.Add("folder1/folder5", name2);
   objm.Add("folder3", name3);

   objm.AddLink("folder1/folder2/name1","linksfolder");
   objm.AddLink("folder1/folder5/name2","linksfolder");
   objm.AddLink("folder3/name3","linksfolder");

   objm.AddFile("", "hist.root");
   objm.AddFile("", "../asf2.root");
   objm.AddROOTFolders("");
   objm.AddFile("", "../tree.root");

   objm.Print("");

//   cout << "Saving OM to file" << endl;
//   TFile* f = TFile::Open("objm.root","recreate");
//   objm.SaveDataToFile(f);
//   objm.Write("ObjectManager");
//   delete f;

   //return;

/*
   TFile* fa = TFile::Open("asf.root");
   fa->Get("Histograms");
   fa->Get("Histograms/Crate1");
   fa->Get("Histograms/Crate2");
   fa->Get("Conditions");
   fa->Get("Parameters");
   fa->Get("DynamicLists");
   fa->Get("Picture");
   fa->Get("Canvases");
   fa->Get("UserObjects");

   objm.AddDir("", fa, kFALSE, kFALSE);
*/

/*


   cout << endl << " Print OM" << endl;
   objm.Print();

   cout << endl << " Print slots" << endl;
   objm.PrintSlots();
*/

/*
   objm.Print();
   objm.Print();
   objm.Print();
   return;


   objm.IterateSlots();

   cout << "----------------" << endl;

   objm.IterateSlots();

   return;
*/

//   objm.Print();

   cout << "Press any key" << endl;
   char c = getchar();

   gBenchmark->Reset();
   gBenchmark->Start("TestOM");

   Int_t numiter = 0;

   for(int n=0;n<100000;n++)
      numiter += objm.IterateSlots();

   gBenchmark->Stop("TestOM");

   gBenchmark->Show("TestOM");

   double RealTime = gBenchmark->GetRealTime("TestOM");
//   double CpuTime = gBenchmark->GetCpuTime("TestOM");

   cout << "Num iterations = " << numiter << endl;
   cout << "Rate/sec = " << numiter/RealTime << endl;
}
