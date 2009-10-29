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

#ifndef TGO4SLOT_H
#define TGO4SLOT_H

#include "TNamed.h"
#include "TObjArray.h"
#include "TGo4Proxy.h"

class TClass;
class TDirectory;
class TGo4ObjectManager;

class TGo4Slot : public TNamed  {
   protected:

      enum ESlotBits {
         kStartDelete = BIT(23)
      };

      void CleanProxy();

      void AddChild(TGo4Slot* child);
      void RemoveChild(TGo4Slot* child);

      TGo4Slot*          fParent;
      TObjArray*         fChilds;
      TObjArray          fPars;
      TGo4Proxy*         fProxy;     //!
      Int_t              fAssignFlag; //!

   public:
      enum { evDelete = 0,
             evCreate = 1,
             evObjAssigned = 2,
             evContAssigned = 3,
             evObjUpdated = 4,
             evSubslotUpdated = 5,
             evObjDeleted = 6 };

      TGo4Slot();
      TGo4Slot(TGo4Slot* parent);
      TGo4Slot(TGo4Slot* parent, const char* name, const char* title);
      virtual ~TGo4Slot();

      TGo4Slot* GetParent() const { return fParent; }
      void SetParent(TGo4Slot* parent) { fParent = parent; }
      Bool_t IsParent(const TGo4Slot* slot) const;

      virtual void ProduceFullName(TString& name, TGo4Slot* toparent = 0);
      TString GetFullName(TGo4Slot* toparent = 0);
      virtual TGo4ObjectManager* GetOM() const;

      const char* GetInfo();
      Int_t GetSizeInfo();

      void SetPar(const char* name, const char* value);
      const char* GetPar(const char* name) const;
      void RemovePar(const char* name);
      void SetIntPar(const char* name, Int_t value);
      Bool_t GetIntPar(const char* name, Int_t& value);
      void PrintPars(Int_t level = 3);

      Int_t NumChilds() const { return (fChilds!=0) ? fChilds->GetLast()+1 : 0; }
      TGo4Slot* GetChild(Int_t n) const { return (fChilds!=0) ? (TGo4Slot*) fChilds->At(n) : 0; }
      TGo4Slot* FindChild(const char* name);
      TGo4Slot* GetNextChild(TGo4Slot* child);
      TGo4Slot* GetNext();
      void DeleteChild(const char* name);
      void DeleteChilds(const char* startedwith = 0);
      virtual void Delete(Option_t *opt = "");

      TGo4Slot* DefineSubSlot(const char* name, const char* &subname) const;
      TGo4Slot* GetSlot(const char* name, Bool_t force = kFALSE);
      TGo4Slot* FindSlot(const char* fullpath, const char** subname = 0);
      Bool_t ShiftSlotBefore(TGo4Slot* slot, TGo4Slot* before);
      Bool_t ShiftSlotAfter(TGo4Slot* slot, TGo4Slot* after);

      void SetProxy(TGo4Proxy* cont);
      TGo4Proxy* GetProxy() const { return fProxy; }

      Int_t GetSlotKind() const;
      const char* GetSlotClassName() const;

      Bool_t IsAcceptObject(TClass* cl);
      Bool_t AssignObject(TObject* obj, Bool_t owner);
      TObject* GetAssignedObject();
      virtual void Update(Bool_t strong = kFALSE);
      Int_t GetAssignFlag() const { return fAssignFlag; }
      void ResetAssignFlag() { fAssignFlag = -1; }

      Bool_t HasSubLevels() const;
      Bool_t HasSlotsSubLevels() const;
      TGo4LevelIter* MakeLevelIter() const;
      TGo4Access* ProvideSlotAccess(const char* name);

      void SaveData(TDirectory* dir, Bool_t onlyobjs = kFALSE);
      void ReadData(TDirectory* dir);

      // mnethod required to be virtual for object manager
      virtual void Event(TGo4Slot* source, Int_t id, void* param = 0);
      void ForwardEvent(TGo4Slot* source, Int_t id, void* param = 0);

      virtual void RecursiveRemove(TObject* obj);

      virtual void Print(Option_t* option) const;

      static const char* FindFolderSeparator(const char* name);
      static void ProduceFolderAndName(const char* fullname, TString& foldername, TString& objectname);

      Bool_t DoingDelete() const { return TestBit(kStartDelete); }

   ClassDef(TGo4Slot, 1);
};


#endif

