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

#ifndef TGO4DABCPROXY_H
#define TGO4DABCPROXY_H

#include "TGo4Proxy.h"
#include "TString.h"

class TH1;

class TGo4DabcProxy : public TGo4Proxy {
   protected:
      TString    fNodeName;
      void*      fxHierarchy;    //!  pointer on dabc::Hierarchy class
      TGo4Slot*  fxParentSlot;

   public:
      TGo4DabcProxy();
      virtual ~TGo4DabcProxy();

      Bool_t Connect(const char* nodename);
      Bool_t UpdateHierarchy(Bool_t sync = kTRUE);
      Bool_t ReplyCommand(void* cmd);

      const char* GetServerName() const { return fNodeName.Data(); }

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);

      virtual Bool_t HasSublevels() const;
      virtual TGo4Access* MakeProxy(const char* name);
      virtual TGo4LevelIter* MakeIter();

      virtual Int_t GetObjectKind() {  return TGo4Access::kndFolder; }
      virtual const char* GetContainedClassName() { return "TGo4DabcProxy"; }
      virtual const char* GetContainedObjectInfo() { return 0; }
      virtual Int_t GetObjectSizeInfo() { return -1; }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual void Update(TGo4Slot* slot, Bool_t strong);

      void RefreshNamesList();

   ClassDef(TGo4DabcProxy, 1);
};

#endif
