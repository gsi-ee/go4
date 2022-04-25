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

#ifndef TGO4EVENTSTOREPARAMETER_H
#define TGO4EVENTSTOREPARAMETER_H

#include "TGo4Parameter.h"

/**
 * Basic type for all classes containing information
 * to parametrize the event store. For use in the
 * event factory; factory method gets (possibly subclassed)
 * parameter object which is suited to the concrete user
 * factory. Name string can be used as filename, e.g.
 */
class TGo4EventStoreParameter : public TGo4Parameter {
  public:
    TGo4EventStoreParameter();

    TGo4EventStoreParameter(const char* name, Int_t id = 0);

    virtual ~TGo4EventStoreParameter();

    Int_t GetID() const { return fiID; }

    void Print(Option_t* = "") const override;

    /** update contents of paramter class with external object. */
    Bool_t UpdateFrom(TGo4Parameter* rhs) override;

  private:
    /** Identity number to specify the kind. Optional, default is 0. */
     Int_t fiID{0};

  ClassDefOverride(TGo4EventStoreParameter,1)
};

#endif //TGO4EVENTSTOREPARAMETER_H
