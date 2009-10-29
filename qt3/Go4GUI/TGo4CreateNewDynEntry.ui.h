// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------


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
