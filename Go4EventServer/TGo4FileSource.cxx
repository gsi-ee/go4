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

#include "TGo4FileSource.h"

#include "TKey.h"
#include "TFile.h"
#include "TTree.h"
#include "TList.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TRegexp.h"

#include "TGo4Log.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4EventElement.h"
// get static name constants from here
#include "TGo4FileStore.h"

TGo4FileSource::TGo4FileSource(const char* name) :
   TGo4EventSource(name),
   fxFile(nullptr),
   fxTree(nullptr),
   fiMaxEvents(0),
   fiCurrentEvent(0),
   fiGlobalEvent(0),
   fbActivated(kFALSE),
   fxTopEvent(nullptr),
   fxFilesNames(nullptr)
{
   fxFilesNames = ProducesFilesList(GetName());

   if (!OpenNextFile())
      ThrowError(66,0, TString::Format("!!! ERROR: Cannot open source %s!!!", GetName()).Data());
}

TGo4FileSource::TGo4FileSource(TGo4FileSourceParameter* par) :
   TGo4EventSource(par->GetName()),
   fxFile(nullptr),
   fxTree(nullptr),
   fiMaxEvents(0),
   fiCurrentEvent(0),
   fiGlobalEvent(0),
   fbActivated(kFALSE),
   fxTopEvent(nullptr),
   fxFilesNames(nullptr)
{
   fxFilesNames = ProducesFilesList(GetName());

   if (!OpenNextFile())
      ThrowError(66,0, TString::Format("!!! ERROR: Cannot open source %s!!!", GetName()).Data());
}


TGo4FileSource::TGo4FileSource() :
   TGo4EventSource("Go4FileSource"),
   fxFile(nullptr),
   fxTree(nullptr),
   fiMaxEvents(0),
   fiCurrentEvent(0),
   fiGlobalEvent(0),
   fbActivated(kFALSE),
   fxTopEvent(nullptr),
   fxFilesNames(nullptr)
{
   // for streamer, do not open here!
}

TGo4FileSource::~TGo4FileSource()
{
   CloseCurrentFile();

   if (fxFilesNames) {
      delete fxFilesNames;
      fxFilesNames = nullptr;
   }
}

TList* TGo4FileSource::ProducesFilesList(const char* mask)
{
   if (!mask || (strlen(mask)==0)) return nullptr;

   TString dirname, basename(mask);

   if (!basename.MaybeWildcard()) {

      // add default suffix
      if(strstr(basename.Data(),TGo4FileStore::fgcFILESUF)==0)
         basename += TGo4FileStore::fgcFILESUF;

      TList* lst = new TList();
      lst->SetOwner(kTRUE);
      lst->Add(new TObjString(basename));
      return lst;
   }

   Bool_t withdir = kFALSE;
   Int_t slash = basename.Last('/');

#ifdef _MSC_VER
   if (slash < 0) slash = basename.Last('\\');
#endif

   if (slash >= 0) {
      dirname = basename(0, slash);
      basename.Remove(0, slash+1);
      withdir = kTRUE;
   } else {
      dirname = gSystem->WorkingDirectory();
   }

   void *dir = gSystem->OpenDirectory(gSystem->ExpandPathName(dirname.Data()));

   if (!dir) return nullptr;

   TList* lst = nullptr;

   TRegexp re(basename, kTRUE);
   const char* file = nullptr;
   while ((file = gSystem->GetDirEntry(dir)) != nullptr) {
      if (!strcmp(file,".") || !strcmp(file,"..")) continue;
      TString s = file;
      if ((basename != s) && (s.Index(re) == kNPOS)) continue;
      if (!lst) {
         lst = new TList;
         lst->SetOwner(kTRUE);
      }
      if (withdir)
         lst->Add(new TObjString(dirname + "/" + file));
      else
         lst->Add(new TObjString(file));
   }
   gSystem->FreeDirectory(dir);

   if (lst) lst->Sort();

   return lst;
}

Bool_t TGo4FileSource::OpenNextFile()
{
   CloseCurrentFile();

   if (!fxFilesNames || (fxFilesNames->GetSize()==0)) return kFALSE;

   TObject* obj = fxFilesNames->First();
   fxCurrentFileName = obj->GetName();
   fxFilesNames->Remove(fxFilesNames->FirstLink());
   delete obj;

   fxFile = TFile::Open(fxCurrentFileName.Data(), "READ"); // in such way rfio etc is also supported!
   if(!fxFile) ThrowError(66,0,TString::Format("!!! ERROR: FILE %s not found !!!", fxCurrentFileName.Data()).Data());
   if(!fxFile->IsOpen()) ThrowError(66,0, TString::Format("!!! ERROR: FILE %s cannot open !!!", fxCurrentFileName.Data()).Data());

   TKey* kee = nullptr;
   TIter iter(fxFile->GetListOfKeys());
   while ( (kee = dynamic_cast<TKey*>(iter())) != nullptr ) {
      fxTree = dynamic_cast<TTree*>(kee->ReadObj());
      if (fxTree) break; // we take first Tree in file, disregarding its name...
   }
   if (!fxTree) {
      ThrowError(77,0,"!!! ERROR: Tree not found !!!");
   } else {
      SetCreateStatus(0);
      fiMaxEvents = fxTree->GetEntries();
   }

   return kTRUE;
}


Bool_t TGo4FileSource::CloseCurrentFile()
{
   if (fxFile) {
      delete fxFile;
      TGo4Log::Info("TGo4FileSource: Close file %s", fxCurrentFileName.Data());
   }

   fxFile = nullptr;
   fxTree = nullptr;
   fiMaxEvents = 0;
   fiCurrentEvent = 0;
   fxTopEvent = nullptr;
   fbActivated = kFALSE;
   fxCurrentFileName = "";

   return kTRUE;
}

Bool_t TGo4FileSource::BuildEvent(TGo4EventElement* dest)
{
   if(!dest) ThrowError(0,22,"!!! ERROR BuildEvent: no destination event!!!");
   if (!fxTree) ThrowError(0,33,"!!! ERROR BuildEvent: no Tree !!!");

   if(fiCurrentEvent >= fiMaxEvents) {
      if (!OpenNextFile())
         ThrowEOF(0,44,"End at event %ld !!!", fiGlobalEvent);
   }

   if(!fbActivated) {
      // Event dest should be full event as filled into the tree
      // the name of the event element may indicate the subpart
      //(tree branchname) that should be read partially only
      fxTopEvent = dest;
      dest->synchronizeWithTree(fxTree, &fxTopEvent);
      fbActivated = kTRUE;
      TGo4Log::Info("TGo4FileSource: Open file %s", fxCurrentFileName.Data());
   }  //  if(!fbActivated)
   // end init section ////////////

   fiGlobalEvent++;

   return fxTree->GetEntry(fiCurrentEvent++) > 0;
}
