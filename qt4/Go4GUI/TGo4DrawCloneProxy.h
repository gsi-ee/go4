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

#ifndef TGO4DRAWCLONEPROXY_H
#define TGO4DRAWCLONEPROXY_H

#include "TGo4LinkProxy.h"

class TGo4ViewPanel;

class TGo4DrawCloneProxy : public TGo4LinkProxy {
   public:
      TGo4DrawCloneProxy(TGo4Slot *slot, TGo4ViewPanel* panel);
      virtual ~TGo4DrawCloneProxy();
      Bool_t AssignClone(TObject *obj, TGo4Slot *slot);
      void CleanupClone(TGo4Slot *slot);
      void ChangeTitle(TObject *obj);
      void UpdateTitle();
      void PerformRebin();

      Bool_t RemoveRegisteredObject(TObject *obj) override;
      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;
      TObject *GetAssignedObject() override;
      Bool_t ProcessEvent(TGo4Slot *slot, TGo4Slot *source, Int_t id, void* param) override;

   protected:
      TObject * fClone{nullptr}; //!
      TGo4ViewPanel* fPanel{nullptr}; //!
      TGo4Slot *fParentSlot{nullptr}; //!
};

#endif
