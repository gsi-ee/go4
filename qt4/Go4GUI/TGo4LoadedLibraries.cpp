#include "TGo4LoadedLibraries.h"

#include <QFileDialog>
#include <QDateTime>

#include "TSystem.h"
#include "TString.h"
#include "TInterpreter.h"
#include "TObjArray.h"

TGo4LoadedLibraries::TGo4LoadedLibraries( QWidget* parent )
    : QDialog( parent )
{
   setObjectName("Go4LoadedLibraries");
   setupUi(this);
   RefreshLibs();
   UnloadLibBtn->hide();
}

void TGo4LoadedLibraries::LoadNewLibrary()
{
  QFileDialog fd(this, "Load Shared Library ", QString(), "Library (*.so)");
  fd.setFileMode( QFileDialog::ExistingFiles);

  if ( fd.exec() != QDialog::Accepted ) return;

  QStringList list = fd.selectedFiles();
  QStringList::Iterator it = list.begin();
  while( it != list.end() ) {
     gSystem->Load((*it).toAscii());
     ++it;
  }

  RefreshLibs();
}

void TGo4LoadedLibraries::UnloadLibrary()
{
   QTreeWidgetItemIterator it(LoadedLibsD);
   while(*it) {
     if ( (*it)->isSelected() )
        gSystem->Unload((*it)->text(0).toAscii());
     it++;
   }

   RefreshLibs();
}

void TGo4LoadedLibraries::RefreshLibs()
{
   LoadedLibsD->clear();

   TObjArray* libs = TString(gInterpreter->GetSharedLibs()).Tokenize(" ,\t\n");

   if (libs!=0)
      for (int n=0; n<=libs->GetLast(); n++) {
         QFileInfo fi(libs->At(n)->GetName());

         QStringList columns;

         columns << fi.fileName() << QString::number(fi.size()) << fi.lastModified().toString() << fi.owner() << fi.group();

         LoadedLibsD->addTopLevelItem(new QTreeWidgetItem(columns));
      }

   delete libs;

   LoadedLibsS->clear();

   libs = TString(gSystem->GetLinkedLibs()).Tokenize(" ,\t\n");

   if (libs!=0)
      for (int n=0; n<=libs->GetLast(); n++) {
         QStringList columns;
         columns << libs->At(n)->GetName();
         LoadedLibsS->addTopLevelItem(new QTreeWidgetItem(columns));
      }
   delete libs;
}
