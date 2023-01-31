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

#ifndef TGO4EVENTSOURCEPARAMETER_H
#define TGO4EVENTSOURCEPARAMETER_H

#include "TGo4Parameter.h"

/**
 * Basic type for all classes containing information
 * to parameterize the event source. For use in the
 * event factory; factory method gets (possibly subclassed)
 * parameter object which is suited to the concrete user
 * factory. Name string can be used as filename, e.g.
 */
class TGo4EventSourceParameter : public TGo4Parameter {
   public:
      TGo4EventSourceParameter();

      TGo4EventSourceParameter(const char *name, Int_t id = 0);

      virtual ~TGo4EventSourceParameter();

      Int_t GetID() const { return fiID; }

      Int_t GetTimeout() const { return fiTimeout; }

      void SetTimeout(Int_t out) { fiTimeout=out; }

      void Print(Option_t* = "") const override;

      /** update contents of parameter class with external object. */
      Bool_t UpdateFrom(TGo4Parameter *rhs) override;

   private:

      /** Identity number to specify the kind. Optional, default is 0. */
      Int_t fiID{0};

      /** Timeout in seconds. If -1, no timeout (default)  */
      Int_t fiTimeout{1};

   ClassDefOverride(TGo4EventSourceParameter,1)
};

#endif //TGO4EVENTSOURCEPARAMETER_H
