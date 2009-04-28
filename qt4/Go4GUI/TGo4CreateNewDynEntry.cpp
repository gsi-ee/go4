#include "TGo4CreateNewDynEntry.h"

#include "TGo4DynamicEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4HistogramEntry.h"

TGo4CreateNewDynEntry::TGo4CreateNewDynEntry( QWidget* parent )
    : QDialog( parent)
{
	setObjectName("Go4CreateNewDynEntry");
	setupUi(this);
   setAcceptDrops(FALSE);
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
     entry->SetName(EntryName->text().toAscii());
     entry->SetTitle(EntryTitle->text().toAscii());
   }

   return entry;
}
