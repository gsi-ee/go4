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

#ifndef TGO4DIRPROXY_H
#define TGO4DIRPROXY_H

#include "TGo4Proxy.h"

class TFile;
class TDirectory;

class TGo4DirProxy : public TGo4Proxy {
   protected:
      TDirectory*  fDir;          //!
      Bool_t       fOwner;        //!
      Bool_t       fReadRight;    //!
      TGo4Slot    *fxParentSlot;  //!

      void SetDir(TDirectory* dir, Bool_t readright, Bool_t owner);
      void ClearDir();

   public:
      TGo4DirProxy();
      TGo4DirProxy(TDirectory* dir, Bool_t readright, Bool_t owner);
      virtual ~TGo4DirProxy();

      virtual void Initialize(TGo4Slot* slot) { fxParentSlot = slot; }
      virtual void Finalize(TGo4Slot* slot) {}

      virtual Bool_t HasSublevels() const { return fDir!=0; }

      virtual TGo4LevelIter* MakeIter()
        { return (fDir==0) ? 0 : ProduceIter(fDir, fReadRight); }

      virtual TGo4Access* ProvideAccess(const char* name)
        { return CreateAccess(fDir, fReadRight, name, fxParentSlot); }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();
      virtual const char* GetContainedObjectInfo();
      virtual Int_t GetObjectSizeInfo();

      static TGo4Access* CreateAccess(TDirectory* dir, Bool_t readright, const char* name, TGo4Slot* browser_slot = 0);
      static TGo4LevelIter* ProduceIter(TDirectory* dir, Bool_t readright);

      Bool_t UpdateObjectInFile(const char* filepath, TObject* obj);

      Bool_t IsFile() const;
      const char* GetFileName() const;

   ClassDef(TGo4DirProxy, 1);
};

#endif
