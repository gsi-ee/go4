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

#ifndef TGO4ITER_H
#define TGO4ITER_H

#include "TGo4AccessWrapper.h"

#include "TObjArray.h"
#include "TString.h"

class TGo4Iter : public TGo4AccessWrapper {
   public:
      TGo4Iter(TGo4Slot *slot, Bool_t onlyslots = kFALSE);
      virtual ~TGo4Iter();

      Bool_t next(Bool_t goesinto = kTRUE);
      Bool_t nextobj();

      Bool_t isfolder();
      const char *getname();
      const char *getfullname();

      Int_t getkindofitem();
      const char *getclassname();
      const char *getinfo();
      Int_t getsizeinfo();
      Int_t getflag(const char *flagname);

      Int_t level() const;
      Int_t levelchange() const { return fLevelChange; }
      Bool_t isslot() const { return getslot() != nullptr; }
      TGo4Slot *getslot() const;

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name = nullptr) override;

   protected:
      TGo4Iter();

      TGo4LevelIter *currentiter() const
       { return fStatus != 0 ? nullptr : (TGo4LevelIter*) fLevels.Last(); }

      TGo4Slot *fSlot{nullptr};      //!
      Bool_t    fOnlySlots{kFALSE};  //!
      TObjArray fLevels;             //!
      Int_t     fStatus{0};          //!  -1 start, 0 continue, 1 end
      TString   fFullName;           //!
      Int_t     fLevelChange{0};     //!

   ClassDefOverride(TGo4Iter,1);
};

#endif
