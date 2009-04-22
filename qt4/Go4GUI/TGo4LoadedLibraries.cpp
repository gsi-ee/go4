#include "TGo4LoadedLibraries.h"

#include <QFileDialog>
#include <QDateTime>

#include "TSystem.h"
#include "TInterpreter.h"

TGo4LoadedLibraries::TGo4LoadedLibraries( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
   setupUi(this);
   RefreshLibs();
   UnloadLibBtn->hide();
}

void TGo4LoadedLibraries::LoadNewLibrary()
{
  QFileDialog fd(this, "Load Shared Library ", QString(), "Library (*.so)");
  fd.setMode( QFileDialog::ExistingFiles);

  if ( fd.exec() != QDialog::Accepted ) return;

  QStringList list = fd.selectedFiles();
  QStringList::Iterator it = list.begin();
  while( it != list.end() ) {
     gSystem->Load((*it).latin1());
     ++it;
  }

  RefreshLibs();
}

void TGo4LoadedLibraries::UnloadLibrary()
{
   QTreeWidgetItemIterator it(LoadedLibsD);
   while(*it) {
     if ( (*it)->isSelected() )
        gSystem->Unload((*it)->text(0).latin1());
     it++;
   }

   RefreshLibs();
}

void TGo4LoadedLibraries::RefreshLibs()
{
   char seps[]=" ,\t\n";
   char buffer[16];

   LoadedLibsD->clear();
   QString T = gInterpreter->GetSharedLibs();

   char* tokbuf = 0;
   char *token = strtok_r((char *)T.latin1(), seps, &tokbuf);

   while(token!=NULL) {
      QFileInfo fi(token);
      snprintf(buffer,15,"%d",fi.size());

      QStringList columns;
      columns << token << QString(buffer) << fi.lastModified().toString() << fi.owner() << fi.group();

      LoadedLibsD->addTopLevelItem(new QTreeWidgetItem(columns));

      token = strtok_r(NULL, seps, &tokbuf);
   }

   LoadedLibsS->clear();
   QString T1 = gSystem->GetLinkedLibs();

   token =strtok_r((char *)T1.latin1(), seps, &tokbuf);

   while(token!=NULL) {
      QStringList columns;
      columns << token;
      LoadedLibsS->addTopLevelItem(new QTreeWidgetItem(columns));
      token = strtok_r(NULL, seps, &tokbuf);
   }
}
