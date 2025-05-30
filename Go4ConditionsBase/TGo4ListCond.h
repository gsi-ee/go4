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

#ifndef TGO4LISTCOND_H
#define TGO4LISTCOND_H

#include "TGo4Condition.h"
#include <vector>

/**
 * Condition that checks a value against a list of "good" (or bad) values.
 * Developed from TRemiWhitelistCond class by Sven Augustin
 * */

class TGo4ListCond : public TGo4Condition {
public:
  TGo4ListCond();
  TGo4ListCond(const char *name, const char *title = "Go4 list condition");
  TGo4ListCond(const char *name, const char *title, const Int_t num, const Int_t *values);
  TGo4ListCond(const char *name, const char *title, const Int_t start, const Int_t stop, const Int_t step = 1);
  virtual ~TGo4ListCond();

  using TGo4Condition::SetValues;
  virtual void SetValues(const Int_t num, const Int_t * values);
  virtual void SetValues(const Int_t start, const Int_t stop, const Int_t step);

  using TGo4Condition::Test;
  Bool_t Test(Double_t value) override;

  void PrintCondition(Bool_t full = kFALSE) override;
  void PrintValues();

  Bool_t UpdateFrom(TGo4Condition *cond, Bool_t counts) override;

  UInt_t GetNumValues() const { return fxValues.size(); }

  Int_t GetValue(UInt_t ix) const { return ix < fxValues.size() ? fxValues[ix] : 0; }

  void SetValue(UInt_t ix, Int_t val)
  {
     if (ix >= fxValues.size())
        fxValues.resize(ix);
     fxValues[ix] = val;
  }

  void InsertValue(UInt_t ix, Int_t val) { fxValues.insert(fxValues.begin() + ix, val); }

  void RemoveValue(UInt_t ix) { fxValues.erase(fxValues.begin() + ix); }

  void Resize(size_t newsize) { fxValues.resize(newsize); }

  /** For base class displays: minimum value of test list.  */
  Double_t GetXLow() const override;

  /** For base class displays: maximum value of test list */
  Double_t GetXUp() const override;

  /**  For base class displays: not used for 1d list */
  Double_t GetYLow() const override;

  /**  For base class displays: not used for 1d list */
  Double_t GetYUp() const override;

  /** Factory method to generate the subclass implementation for painter
   * TODO: do we need graphical representation of whitelist, e.g. by set of point markers in working histogram?
   * For the moment, no graphics for list conditions 1-2019*/
  /*      virtual TGo4ConditionPainter *CreatePainter();*/

private:

  std::vector<Int_t> fxValues;

//		/** number of values in list: todo: use vector here?*/
//		Int_t fiNum;
//		/** array with values to test: todo: use vector here?*/
//		Int_t * fiValues;

   ClassDefOverride(TGo4ListCond, 1);
};

#endif // TGO4LISTCOND_H
