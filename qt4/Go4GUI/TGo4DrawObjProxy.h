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

#ifndef TGO4DRAWOBJPROXY_H
#define TGO4DRAWOBJPROXY_H

#include "TGo4ObjectProxy.h"

class TGo4DrawObjProxy : public TGo4ObjectProxy {
   public:
      TGo4DrawObjProxy(TObject* obj, Bool_t owner, Bool_t doclear = kFALSE);
      virtual ~TGo4DrawObjProxy();
      Bool_t RemoveRegisteredObject(TObject* obj) override;

   protected:
      void TryToClearObject();

      Bool_t  fDoClear{kFALSE};   //!
};

#endif
