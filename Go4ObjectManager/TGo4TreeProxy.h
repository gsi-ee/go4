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

#ifndef TGO4TREESLOT_H
#define TGO4TREESLOT_H

#include "TGo4Proxy.h"

class TTree;

class TGo4TreeProxy : public TGo4Proxy {
   public:
      TGo4TreeProxy();
      TGo4TreeProxy(TTree* tree, Bool_t owner = kFALSE);
      virtual ~TGo4TreeProxy();

      virtual Bool_t HasSublevels() const { return fTree!=0; }

      virtual TGo4LevelIter* MakeIter()
        { return (fTree==0) ? 0 : ProduceIter(fTree); }

      virtual TGo4Access* ProvideAccess(const char* name)
        { return CreateAccess(fTree, name); }

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();

      static TGo4Access* CreateAccess(TTree* tree, const char* name);
      static TGo4LevelIter* ProduceIter(TTree* tree);

   protected:
      TTree*   fTree;   //!
      Bool_t   fOwner;  //!

   ClassDef(TGo4TreeProxy, 1);
};

#endif


