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

#ifndef TGO4CANVASPROXY_H
#define TGO4CANVASPROXY_H

#include "TGo4Proxy.h"

class TCanvas;

class TGo4CanvasProxy : public TGo4Proxy {
   public:
      TGo4CanvasProxy();
      TGo4CanvasProxy(TCanvas *c, Bool_t owner = kFALSE);
      virtual ~TGo4CanvasProxy();

      Bool_t HasSublevels() const override { return fCanvas != nullptr; }
      Int_t GetObjectKind() const override;
      const char *GetContainedClassName() const override;

      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;
      Bool_t RemoveRegisteredObject(TObject *obj) override;

      TGo4LevelIter *MakeIter() override
        { return fCanvas ? ProduceIter(fCanvas) : nullptr; }

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override
        { return CreateAccess(fCanvas, name); }

      void WriteData(TGo4Slot *slot, TDirectory *dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory *dir) override;

      Bool_t IsAcceptObject(TClass *cl) const override;
      Bool_t AssignObject(TGo4Slot *slot, TObject *obj, Bool_t owner) override;
      TObject *GetAssignedObject() override;

      static std::unique_ptr<TGo4Access> CreateAccess(TCanvas *canv, const char *name);
      static TGo4LevelIter *ProduceIter(TCanvas *canv);

   protected:
      TCanvas *fCanvas{nullptr}; //!
      Bool_t   fOwner{kFALSE};   //!

   ClassDefOverride(TGo4CanvasProxy, 0);
};


#endif
