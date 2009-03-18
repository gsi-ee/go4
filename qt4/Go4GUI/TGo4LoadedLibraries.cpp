#include "TGo4LoadedLibraries.h"

#include <Q3FileDialog>
#include <QDateTime>

#include "TSystem.h"
#include "TInterpreter.h"

TGo4LoadedLibraries::TGo4LoadedLibraries( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{ 
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
    RefreshLibs();
    UnloadLibBtn->hide();
}

void TGo4LoadedLibraries::LoadNewLibrary()
{
  Q3FileDialog fd(this, "Load Library", TRUE );
  fd.setMode( Q3FileDialog::ExistingFiles);
  fd.setName( "Load Shared Library ");
  fd.setFilter( "Library (*.so)" );

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
   Q3ListViewItemIterator it(LoadedLibsD);
   for ( ; it.current(); ++it )
     if ( it.current()->isSelected() )
       gSystem->Unload(it.current()->text(0).latin1());

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
      new Q3ListViewItem(LoadedLibsD,token,QString(buffer),
                        fi.lastModified().toString(), fi.owner(), fi.group() );
      token = strtok_r(NULL, seps, &tokbuf);
   }

   LoadedLibsS->clear();
   QString T1 = gSystem->GetLinkedLibs();

   token =strtok_r((char *)T1.latin1(), seps, &tokbuf);

   while(token!=NULL) {
      new Q3ListViewItem(LoadedLibsS,token);
      token = strtok_r(NULL, seps, &tokbuf);
   }
}
