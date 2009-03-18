
void TGo4CreateNewDynEntry::init()
{
   setAcceptDrops(FALSE);
   EntryName->setText("entry");
   EntryTitle->setText("Dynamic entry title");
   EntryType->setCurrentItem(0);
}

TGo4DynamicEntry* TGo4CreateNewDynEntry::MakeEntry()
{
   TGo4DynamicEntry* entry = 0;

   if (EntryType->currentItem()==0)
      entry = new TGo4HistogramEntry();
   else
   if (EntryType->currentItem()==1)
      entry = new TGo4TreeHistogramEntry();

   if (entry!=0) {
     entry->EnableProcessing(kFALSE);
     entry->SetName(EntryName->text().latin1());
     entry->SetTitle(EntryTitle->text().latin1());
   }

   return entry;
}
