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

#include "TGo4ListCond.h"

#include <algorithm>

TGo4ListCond::TGo4ListCond() :
	TGo4Condition()
{
   SetDimension(1);
}

TGo4ListCond::TGo4ListCond(const char *name, const char* title) :
    TGo4Condition(name, title)
{
  fxValues.clear();
  SetDimension(1);
}

TGo4ListCond::TGo4ListCond(const char *name, const char* title, const Int_t num, const Int_t * input) :
	TGo4Condition(name, title)//, fiNum(num)
{
   SetDimension(1);
   SetValues(num,input);
}

TGo4ListCond::TGo4ListCond(const char *name, const char * title, const Int_t start, const Int_t stop, const Int_t step) :
	TGo4Condition(name, title)
{
    SetDimension(1);
    SetValues(start,stop,step);

}

void TGo4ListCond::SetValues(const Int_t num, const Int_t *values)
{
   fxValues.clear();
   for (int i = 0; i < num; ++i) {
      fxValues.push_back(values[i]);
   }
}
void TGo4ListCond::SetValues(const Int_t start, const Int_t stop, const Int_t step)
{
   fxValues.clear();
   Int_t num = (stop - start) / step;
   if (num <= 0) {
      num = 0;
   }
   num += 1;

   for (int i = 0; i < num; ++i) {
      fxValues.push_back(start + i * step);
   }
}

TGo4ListCond::~TGo4ListCond() {}

Bool_t TGo4ListCond::Test(Double_t dvalue)
{
   Int_t value = dvalue; // need this to avoid framework ambiguity
   IncCounts();
   if (!IsEnabled()) {
      if (FixedResult())
         IncTrueCounts();
      return FixedResult();
   }

   for (size_t i = 0; i < fxValues.size(); ++i) {
      if (value == fxValues[i]) {
         //			std::cout << i << " " << fxValues[i] << " " << value << std::endl;
         IncTrueCounts();
         return kTRUE;
      }
   }

   return kFALSE;
}

void TGo4ListCond::PrintCondition(Bool_t full)
{
   TGo4Condition::PrintCondition();
   PrintValues();
}

void TGo4ListCond::PrintValues()
{
   size_t num = fxValues.size();
   std::cout << "fxValues[" << num << "]: {";
   if (num > 0) {
      std::cout << fxValues[0];
      for (size_t i = 1; i < num; ++i) {
         std::cout << ", " << fxValues[i];
      }
   }
   std::cout << "}" << std::endl;
}

Bool_t TGo4ListCond::UpdateFrom(TGo4Condition *cond, Bool_t counts)
{
   TGo4ListCond *from = dynamic_cast<TGo4ListCond *>(cond);
   if (from == 0)
      return kFALSE;
   fxValues.clear();
   for (size_t i = 0; i < from->GetNumValues(); ++i) {
      fxValues.push_back(from->GetValue(i));
   }
   return (TGo4Condition::UpdateFrom(cond, counts));
}

Double_t TGo4ListCond::GetXUp()
{
   return *std::max_element(fxValues.begin(), fxValues.end());
}

Double_t TGo4ListCond::GetXLow()
{
   return *std::min_element(fxValues.begin(), fxValues.end());
}

Double_t TGo4ListCond::GetYUp()
{
   return 0;
}

Double_t TGo4ListCond::GetYLow()
{
   return 0;
}
