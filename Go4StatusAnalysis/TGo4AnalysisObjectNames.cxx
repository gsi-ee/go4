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

#include "TGo4AnalysisObjectNames.h"

#include "TROOT.h"
#include "TList.h"
#include "TFolder.h"

TGo4AnalysisObjectNames::TGo4AnalysisObjectNames() :
   TGo4Status()
{
}

TGo4AnalysisObjectNames::TGo4AnalysisObjectNames(const char* name) :
   TGo4Status(name)
{
}

TGo4AnalysisObjectNames::~TGo4AnalysisObjectNames()
{
   if(fxTopFolder) {
      delete fxTopFolder;
      fxTopFolder = nullptr;
   }
}

TList* TGo4AnalysisObjectNames::GetFolderList()
{
   if(fxTopFolder)
      return dynamic_cast<TList*> (fxTopFolder->GetListOfFolders());

   return nullptr;
}

TFolder* TGo4AnalysisObjectNames::GetNamesFolder(Bool_t chown)
{
   TFolder* reval = fxTopFolder;
   if(chown) fxTopFolder = nullptr;
   return reval;
}


void TGo4AnalysisObjectNames::Print(Option_t *) const
{
   gROOT->SetDirLevel(0);
   PrintLine("G-OOOO-> Analysis Object Names Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   PrintFolder(fxTopFolder);
}

void TGo4AnalysisObjectNames::PrintFolder(TFolder* fold) const
{
   if (!fold) return;

   gROOT->IncreaseDirLevel();

   PrintLine("G-OOOO-> Status Folder %s Printout <-OOOO-G", fold->GetName());
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");

   TIter iter(fold->GetListOfFolders());
   TObject* entry = nullptr;

   while((entry = iter()) != nullptr) {
      if(entry->InheritsFrom(TFolder::Class())) {
         PrintFolder(dynamic_cast<TFolder*> (entry));
      } else if (entry->InheritsFrom(TGo4Status::Class())) {
         entry->Print();
      } else {
         entry->ls();
      }
   }

   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");

   gROOT->DecreaseDirLevel();
}
