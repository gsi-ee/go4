// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4LoadedLibraries.h"

#include <QFileDialog>
#include <QDateTime>

#include "TSystem.h"
#include "TString.h"
#include "TInterpreter.h"
#include "TObjArray.h"

TGo4LoadedLibraries::TGo4LoadedLibraries(QWidget *parent) : QDialog(parent)
{
   setObjectName("Go4LoadedLibraries");
   setupUi(this);
   QObject::connect(LoadLibBtn, &QPushButton::clicked, this, &TGo4LoadedLibraries::LoadNewLibrary);
   QObject::connect(UnloadLibBtn, &QPushButton::clicked, this, &TGo4LoadedLibraries::UnloadLibrary);
   QObject::connect(RefreshBtn, &QPushButton::clicked, this, &TGo4LoadedLibraries::RefreshLibs);

   RefreshLibs();
   UnloadLibBtn->hide();
}

void TGo4LoadedLibraries::LoadNewLibrary()
{
  QFileDialog fd(this, "Load Shared Library ", QString(), "Library (*.so)");
  fd.setFileMode( QFileDialog::ExistingFiles);

  if (fd.exec() != QDialog::Accepted) return;

  QStringList list = fd.selectedFiles();
  for(auto &lib : list)
     gSystem->Load(lib.toLatin1().constData());

  RefreshLibs();
}

void TGo4LoadedLibraries::UnloadLibrary()
{
   QTreeWidgetItemIterator it(LoadedLibsD);
   while(*it) {
     if ( (*it)->isSelected() )
        gSystem->Unload((*it)->text(0).toLatin1().constData());
     it++;
   }

   RefreshLibs();
}

void TGo4LoadedLibraries::RefreshLibs()
{
   LoadedLibsD->clear();

   TObjArray *libs = TString(gInterpreter->GetSharedLibs()).Tokenize(" ,\t\n");

   if (libs)
      for (int n = 0; n <= libs->GetLast(); n++) {
         QFileInfo fi(libs->At(n)->GetName());

         QStringList columns;

         columns << fi.fileName() << QString::number(fi.size()) << fi.lastModified().toString() << fi.owner() << fi.group();

         LoadedLibsD->addTopLevelItem(new QTreeWidgetItem(columns));
      }

   delete libs;

   LoadedLibsS->clear();

   libs = TString(gSystem->GetLinkedLibs()).Tokenize(" ,\t\n");

   if (libs)
      for (int n = 0; n <= libs->GetLast(); n++) {
         QStringList columns;
         columns << libs->At(n)->GetName();
         LoadedLibsS->addTopLevelItem(new QTreeWidgetItem(columns));
      }

   delete libs;
}
