void testr() {
   std::cout << "Read object manager from file " << std::endl << std::endl;

   TFile* f2 = TFile::Open("objm.root");
   TGo4ObjectManager* objm2 = 0;
   f2->GetObject("ObjectManager", objm2);
   if (objm2) objm2->ReadDataFromFile(f2);
   delete f2;

   if (objm2) {
      objm2->Print();
//      objm2->PrintSlots();
   }
   delete objm2;
}
