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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void TGo4LoadedLibraries::init()
{
    RefreshLibs();
    UnloadLibBtn->hide();
}

void TGo4LoadedLibraries::LoadNewLibrary()
{
  QFileDialog fd(this, "Load Library", TRUE );
  fd.setMode( QFileDialog::ExistingFiles);
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
   QListViewItemIterator it(LoadedLibsD);
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
      new QListViewItem(LoadedLibsD,token,QString(buffer),
                        fi.lastModified().toString(), fi.owner(), fi.group() );
      token = strtok_r(NULL, seps, &tokbuf);
   }

   LoadedLibsS->clear();
   QString T1 = gSystem->GetLinkedLibs();

   token =strtok_r((char *)T1.latin1(), seps, &tokbuf);

   while(token!=NULL) {
      new QListViewItem(LoadedLibsS,token);
      token = strtok_r(NULL, seps, &tokbuf);
   }
}
