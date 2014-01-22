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

#include "TGo4CreateNewDynEntry.h"

#include "TGo4DynamicEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4HistogramEntry.h"

TGo4CreateNewDynEntry::TGo4CreateNewDynEntry( QWidget* parent )
    : QDialog( parent)
{
   setObjectName("Go4CreateNewDynEntry");
   setupUi(this);
   setAcceptDrops(false);
   EntryName->setText("entry");
   EntryTitle->setText("Dynamic entry title");
   EntryType->setCurrentIndex(0);
}

TGo4DynamicEntry* TGo4CreateNewDynEntry::MakeEntry()
{
   TGo4DynamicEntry* entry = 0;

   if (EntryType->currentIndex()==0)
      entry = new TGo4HistogramEntry();
   else
   if (EntryType->currentIndex()==1)
      entry = new TGo4TreeHistogramEntry();

   if (entry!=0) {
     entry->EnableProcessing(kFALSE);
     entry->SetName(EntryName->text().toLatin1().constData());
     entry->SetTitle(EntryTitle->text().toLatin1().constData());
   }

   return entry;
}
