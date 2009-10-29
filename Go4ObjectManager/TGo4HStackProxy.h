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

#ifndef TGO4HSTACKPROXY_H
#define TGO4HSTACKPROXY_H

#include "TGo4Proxy.h"

#include "THStack.h"

class TGo4HStackProxy : public TGo4Proxy {
   public:
      TGo4HStackProxy();
      TGo4HStackProxy(THStack* hs, Bool_t owner = kFALSE);
      virtual ~TGo4HStackProxy();

      virtual Bool_t HasSublevels() const { return fHS!=0; }
      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);
      virtual Bool_t RemoveRegisteredObject(TObject* obj);

      virtual TGo4LevelIter* MakeIter()
        { return (fHS==0) ? 0 : ProduceIter(fHS); }

      virtual TGo4Access* MakeProxy(const char* name)
        { return ProduceProxy(fHS, name); }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Bool_t IsAcceptObject(TClass* cl);
      virtual Bool_t AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner);
      virtual TObject* GetAssignedObject();

      static TGo4Access* ProduceProxy(THStack* canv, const char* name);
      static TGo4LevelIter* ProduceIter(THStack* canv);

   protected:
      THStack* fHS;              //!
      Bool_t   fOwner;           //!

   ClassDef(TGo4HStackProxy, 1);
};


#endif
