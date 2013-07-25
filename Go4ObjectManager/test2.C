void test2()
{
   TGo4ObjectManager objm;

   TNamed* name1 = new TNamed("name1","name1 title");
   TNamed* name2 = new TNamed("name2","name2 title");
   TNamed* name3 = new TNamed("name3","name3 title");
   TNamed* name4 = new TNamed("name4","name4 title");

   objm.Add("pad/pad_1/pad_1_1", name1);
   objm.Add("pad/pad_1/pad_1_2", name2);
   objm.Add("pad/pad_2/pad_2_1", name3);
   objm.Add("pad/pad_2/pad_2_2", name4);

//   objm.AddLink("folder1/folder2/name1","linksfolder");
//   objm.AddLink("folder1/folder5/name2","linksfolder");
//   objm.AddLink("folder3/name3","linksfolder");

   objm.Print();

//   TGo4Slot* slot = objm.GetSlot("pad");
//   slot->Print("*");

//   std::cout << "Now delete name2  " << std::endl;

//   delete name2;

//   objm.Print();
}
