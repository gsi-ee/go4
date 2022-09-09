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

#include "TGo4FitDataRidge.h"

#include <iostream>

TGo4FitDataRidge::TGo4FitDataRidge() : TGo4FitData(), fxData(this, TGo4FitData::Class()), fiSelectedAxis(0) {}

TGo4FitDataRidge::TGo4FitDataRidge(const char *iName, TGo4FitData *Data, Int_t SelectedAxis)
   : TGo4FitData(iName, "Histogram transform", dtGraph, kFALSE),
     fxData("DataForRidge", "Data, used to represent bins for ridge", this, TGo4FitData::Class(), kTRUE, Data, kTRUE),
     fiSelectedAxis(SelectedAxis)
{
   SetExcludeLessThen(-1e50);
}

TGo4FitDataRidge::~TGo4FitDataRidge() {}

TGo4FitDataIter *TGo4FitDataRidge::MakeIter()
{
   return new TGo4FitDataRidgeIter(this);
}

void TGo4FitDataRidge::FillSlotList(TSeqCollection *list)
{
   TGo4FitData::FillSlotList(list);
   list->Add(&fxData);
   if (GetData())
      GetData()->FillSlotList(list);
}

void TGo4FitDataRidge::Print(Option_t *option) const
{
   TGo4FitData::Print(option);
   if (GetData())
      GetData()->Print(option);
   std::cout << "  selected axis for transform " << fiSelectedAxis << std::endl;
}

// *********************************************************************************

TGo4FitDataRidgeIter::TGo4FitDataRidgeIter() : TGo4FitDataIter(), fxData(nullptr), iter(nullptr), fxOwnScales() {}

TGo4FitDataRidgeIter::TGo4FitDataRidgeIter(TGo4FitDataRidge *data)
   : TGo4FitDataIter(), fxData(data), iter(nullptr), fxOwnScales()
{
}

TGo4FitDataRidgeIter::~TGo4FitDataRidgeIter()
{
   if (iter)
      delete iter;
}

Bool_t TGo4FitDataRidgeIter::StartReset()
{
   if (iter) {
      delete iter;
      iter = nullptr;
   }

   if (!fxData)
      return kFALSE;
   TGo4FitData *data = fxData->GetData();
   if (!data)
      return kFALSE;

   if (data->GetDataType() != TGo4FitData::dtHistogram) {
      std::cout << " Only histogramic data can be used for transform " << std::endl;
      return kFALSE;
   }

   iter = data->MakeIter();
   if (!iter || !iter->Reset(kTRUE))
      return kFALSE;

   fxOwnScales.Set(iter->ScalesSize() - 1);
   fxOwnScales.Reset(0.);

   return ReserveArrays(iter->IndexesSize(), iter->ScalesSize() - 1, kFALSE);
}

Bool_t TGo4FitDataRidgeIter::ReadCurrentPoint()
{
   if (!iter || !fxData)
      return kFALSE;

   if (!GetDeviation()) {
      if (iter->Value() > 0.)
         fdStandardDeviation = 1. / iter->Value();
      else
         fdStandardDeviation = 0.;
   }
   Int_t n1 = 0;
   for (Int_t n = 0; n < iter->ScalesSize(); n++)
      if (n == fxData->GetSelectedAxis())
         fdValue = iter->Scales()[n];
      else
         fxOwnScales[n1++] = iter->Scales()[n];

   return ProduceScales(iter->Indexes(), fxOwnScales.GetArray(), nullptr);
}

Bool_t TGo4FitDataRidgeIter::ShiftToNextPoint()
{
   if (!iter)
      return kFALSE;
   Bool_t res = iter->Next(kTRUE);
   while (res && (iter->Value() <= 0.))
      res = iter->Next(kTRUE);
   return res;
}
