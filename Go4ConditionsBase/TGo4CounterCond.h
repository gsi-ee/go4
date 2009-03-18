#ifndef TGO4COUNTERCOND_H
#define TGO4COUNTERCOND_H

#include "TGo4Condition.h"

class TGo4CounterCond : public TGo4Condition {
  public:
    TGo4CounterCond();
    TGo4CounterCond(const char* name, const char* title = "TGo4CounterCond");
    virtual ~TGo4CounterCond();

    /** Noop */
    virtual void SetValues() {}
//    virtual void SetValues(Double_t low1, Double_t up1) {}
//    virtual void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2) {}
//    virtual void SetValues(TCutG * newcut) {}
//    virtual void SetValues(Double_t * x, Double_t * y, Int_t len) {}
    
//    virtual Bool_t Test() { return true; }
//    virtual Bool_t Test(Double_t x) { return true; }
//    virtual Bool_t Test(Double_t x, Double_t y) { return true; }
    virtual Bool_t Test(Bool_t result);

    void PrintCondition(Bool_t full);

    Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts);

  ClassDef(TGo4CounterCond,1);
};

#endif //TGO4COUNTERCOND_H
