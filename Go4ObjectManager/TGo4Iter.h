#ifndef TGO4ITER_H
#define TGO4ITER_H

#include "TGo4AccessWrapper.h"

#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"

class TGo4Iter : public TGo4AccessWrapper {
   public:
      TGo4Iter(TGo4Slot* slot, Bool_t onlyslots = kFALSE);
      virtual ~TGo4Iter();
      
      Bool_t next(Bool_t goesinto = kTRUE);
      Bool_t nextobj();
   
      Bool_t isfolder();
      const char* getname();
      const char* getfullname();
      
      Int_t getkindofitem();
      const char* getclassname();
      const char* getinfo();
      Int_t getsizeinfo();
      Int_t getflag(const char* flagname);
      
      Int_t level() const;
      Int_t levelchange() const { return fLevelChange; }
      Bool_t isslot() const { return getslot()!=0; }
      TGo4Slot* getslot() const;
      
      virtual TGo4Access* ProvideAccess(const char* name = 0);
      
   protected:
      TGo4Iter();
      
      TGo4LevelIter* currentiter() const
       { return (fStatus!=0) ? 0 : (TGo4LevelIter*) fLevels.Last(); }
      
      TGo4Slot* fSlot;       //!
      Bool_t    fOnlySlots;  //!
      TObjArray fLevels;     //!
      Int_t     fStatus;     //!  -1 start, 0 continue, 1 end
      TString   fFullName;   //!
      Int_t     fLevelChange; //!
   
   ClassDef(TGo4Iter,1);
};

#endif
