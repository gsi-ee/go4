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

#ifndef TGO4OBJECTMANAGER_H
#define TGO4OBJECTMANAGER_H

#include "TGo4Slot.h"
#include "TGo4AccessWrapper.h"

#include "TObjArray.h"

class TFile;
class TDirectory;
class TTree;
class TFolder;

class TGo4ObjectManager : public TGo4Slot, public TGo4AccessWrapper, public TGo4ObjectReceiver {
   public:
      TGo4ObjectManager();

      TGo4ObjectManager(const char* name, const char* title);

      virtual ~TGo4ObjectManager();

      virtual void ProduceFullName(TString& name, TGo4Slot* toparent = 0);
      virtual TGo4ObjectManager* GetOM() const;

      void MakeFolder(const char* pathname);

      TGo4Slot* Add(const char* pathname, TObject* obj, Bool_t owner = kFALSE, Bool_t canrename = kFALSE);

      TGo4Slot* MakeObjSlot(const char* foldername, const char* name = 0, const char* title = 0);

      TGo4Slot* AddLink(TGo4Slot* source, const char* pathname, const char* linkname, const char* linktitle);

      TGo4Slot* AddLink(TGo4Slot* source, const char* pathname);

      TGo4Slot* AddLink(const char* sourcename, const char* pathname);

      TGo4Slot* GetLinked(TGo4Slot* link);

      void AddFile(const char* pathname, const char* filename);

      void CloseFiles(const char* pathname);

      void AddDir(const char* pathname, TDirectory* dir, Bool_t owner = kFALSE, Bool_t readright = kFALSE);

      void AddTree(const char* pathname, TTree* tree, Bool_t owner = kFALSE);

      void AddFolder(const char* pathname, TFolder* folder, Bool_t owner = kFALSE);

      void AddROOTFolder(const char* pathname, const char* foldername);

      void AddROOTFolders(const char* pathname, Bool_t selected = kTRUE);

      void AddProxy(const char* pathname, TGo4Proxy* cont, const char* name, const char* title = "title");
      TGo4Proxy* GetProxy(const char* name);

      void DeleteSlot(const char* pathname);

      void SaveDataToFile(TFile* f, Bool_t onlyobjs = kFALSE, TGo4Slot* startslot = 0);

      void ReadDataFromFile(TFile* f);

      void RegisterLink(TGo4Slot* source, TGo4Slot* target, Bool_t exapndchilds = kFALSE);
      void UnregisterLink(TGo4Slot* target);

      virtual TGo4Access* ProvideAccess(const char* name = 0)
          { return ProvideSlotAccess(name); }

      virtual void Event(TGo4Slot* source, Int_t id, void* param = 0);
      void RetranslateEvent(TGo4Slot* source, Int_t id, void* param = 0);
      void RegisterObjectWith(TObject* obj, TGo4Slot* slot);
      void UnregisterObject(TObject* obj, TGo4Slot* slot);

      void PrintSlots();

      Int_t IterateSlots();

      Int_t RequestObject(const char* source, const char* targetslot, Int_t waittime_millisec = 0);

      virtual void RecursiveRemove(TObject* obj);

   protected:

      virtual Bool_t AssignObject(const char* path, TObject* obj, Bool_t ownership);

      void RemoveFromLinks(const TGo4Slot* slot);

      TObjArray  fLinks;     //! list of links between slots

      TObjArray  fCleanups;  //! list of registered cleanups

   ClassDef(TGo4ObjectManager,1);  // Object manager of all Go4 GUI objects
};

#endif
