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

#ifndef TGO4BACKSTOREPARAMETER_H
#define TGO4BACKSTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"

/**
 * @brief Backstore parameter
 * @ingroup go4_event
 */

class TGo4BackStoreParameter : public TGo4EventStoreParameter {

   public:
      TGo4BackStoreParameter();

      TGo4BackStoreParameter(const char *name,
                              Int_t splitlevel = 99,
                              Int_t bufsize = 32000);

      virtual ~TGo4BackStoreParameter();

      void Print(Option_t *opt = "") const override;

      /** update contents of parameter class with external object. */
      Bool_t UpdateFrom(TGo4Parameter *rhs) override;

      Int_t GetSplitlevel() const { return fiSplit; }
      void SetSplitlevel(Int_t split) { fiSplit = split; }

      Int_t GetBufsize() const { return fiBufsize; }
      void SetBufsize(Int_t bufsize) { fiBufsize = bufsize; }

   private:

      /** Splitlevel for Tree. Default is 1. */
      Int_t fiSplit{99};

      /** Size of the root io buffer. */
      Int_t fiBufsize{64000};

   ClassDefOverride(TGo4BackStoreParameter,1)
};

#endif //TGO4BACKSTOREPARAMETER_H
