void testgui()
{
   new TH1I("testhisto1","histo title", 100, 1., 100.);
   new TH1I("testhisto2","histo title", 100, 1., 100.);
   new TH1I("testhisto3","histo title", 100, 1., 100.);

   TGo4ObjectManager objm("ObjManager","Go4 object manager");

   TNamed* name1 = new TNamed("name1","name1 title");
   TNamed* name2 = new TNamed("name2","name2 title");
   TNamed* name3 = new TNamed("name3","name3 title");

   objm.Add("data/folder1/folder2", name1);
   objm.Add("data/folder1/folder5", name2);
   objm.Add("data/folder3", name3);


/*
   objm.AddLink("data/folder1/folder2/name1","data/linksfolder");
   objm.AddLink("data/folder1/folder5/name2","data/linksfolder");
   objm.AddLink("data/folder3/name3","data/linksfolder");

   objm.AddFile("data", "hist.root");
   objm.AddFile("data", "asf.root");

   objm.SyncGuiSlots("gui","data");
   objm.PrintSlots();

   objm.AddROOTFolders("data");

   objm.AddFile("data", "tree.root");

*/

   objm.AddROOTFolders("data");
   objm.AddFile("data", "hist.root");

   objm.SyncGuiSlots("gui","data");
   objm.PrintSlots();

//   objm.DeleteSlot("data/folder1/folder5");

//   objm.SyncGuiSlots("gui","data");
//   objm.PrintSlots();

   std::cout << std::endl << "------------------------------- " << std::endl;
   objm.DeleteSlot("data/hist.root");

   objm.SyncGuiSlots("gui","data");
   objm.PrintSlots();

//   objm.Print();


}
