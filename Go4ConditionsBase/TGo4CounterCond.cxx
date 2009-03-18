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

