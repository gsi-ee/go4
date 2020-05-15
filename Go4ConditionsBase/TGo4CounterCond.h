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

#ifndef TGO4COUNTERCOND_H
#define TGO4COUNTERCOND_H

#include "TGo4Condition.h"

class TGo4CounterCond : public TGo4Condition {
  public:
    TGo4CounterCond();
    TGo4CounterCond(const char* name, const char* title = "TGo4CounterCond");
    virtual ~TGo4CounterCond();

    /** Noop */
    using TGo4Condition::SetValues;
    virtual void SetValues() {}

    using TGo4Condition::Test;
    virtual Bool_t Test(Bool_t result);

    void PrintCondition(Bool_t full);

    Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts);

  ClassDef(TGo4CounterCond,1);
};

#endif //TGO4COUNTERCOND_H
