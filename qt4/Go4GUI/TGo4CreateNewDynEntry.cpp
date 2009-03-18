#include "TGo4CreateNewDynEntry.h"

#include "TGo4DynamicEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4HistogramEntry.h"

TGo4CreateNewDynEntry::TGo4CreateNewDynEntry( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{ 
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
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
