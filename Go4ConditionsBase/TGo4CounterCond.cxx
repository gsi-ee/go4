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

#include "TGo4CounterCond.h"

TGo4CounterCond::TGo4CounterCond() :
   TGo4Condition()
{
}

TGo4CounterCond::TGo4CounterCond(const char* name, const char* title) :
   TGo4Condition(name,title)
{
}

TGo4CounterCond::~TGo4CounterCond()
{
}

Bool_t TGo4CounterCond::Test(Bool_t result)
{
  IncCounts();
  if(result) IncTrueCounts();
  return result;
}

void TGo4CounterCond::PrintCondition(Bool_t full)
{
   TGo4Condition::PrintCondition();
}

Bool_t TGo4CounterCond::UpdateFrom(TGo4Condition * cond, Bool_t counts)
{
   return (TGo4Condition::UpdateFrom(cond,counts));
}

