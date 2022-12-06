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

#include "TGo4FitModelFromData.h"

#include <iostream>

#include "TGo4FitDataHistogram.h"

TGo4FitModelFromData::TGo4FitModelFromData() : TGo4FitModel(), fxData(this, TGo4FitData::Class()) {}

TGo4FitModelFromData::TGo4FitModelFromData(const char *iName, TGo4FitData *iDataAsModel, Bool_t Amplitude)
   : TGo4FitModel(iName, "data used as model", Amplitude),
     fxData("forModel", "Data, used to represent model component", this, TGo4FitData::Class(), kTRUE, iDataAsModel,
            kTRUE)
{
}

TGo4FitModelFromData::TGo4FitModelFromData(const char *iName, TH1 *histo, Bool_t iOwned, Bool_t Amplitude)
   : TGo4FitModel(iName, "data used as model", Amplitude),
     fxData("forModel", "Data, used to represent model component", this, TGo4FitData::Class(), kTRUE)
{
   SetDataAsModel(new TGo4FitDataHistogram("Histogram", histo, iOwned), kTRUE);
}

TGo4FitModelFromData::~TGo4FitModelFromData()
{
}

TGo4FitData *TGo4FitModelFromData::GetDataAsModel() const
{
   return dynamic_cast<TGo4FitData *>(fxData.GetObject());
}

void TGo4FitModelFromData::SetDataAsModel(TGo4FitData *iData, Bool_t iOwned)
{
   fxData.SetObject(iData, iOwned);
   SetUpdateSlotList();
}

Bool_t TGo4FitModelFromData::Initialize(Int_t UseBuffers)
{
   if (!GetDataAsModel())
      return kFALSE;

   for (Int_t n = 0; n < NumAssigments(); n++)
      if (GetAssignedConnection(n))
         if (!GetAssignedConnection(n)->IsCompatibleData(GetDataAsModel())) {
            std::cout << "TGo4FitModelFromData: incompatible data used for model" << std::endl;
            return kFALSE;
         }

   return TGo4FitModel::Initialize(UseBuffers);
}

Bool_t TGo4FitModelFromData::BeforeEval(Int_t)
{
   fxIter.reset();

   if (!GetDataAsModel())
      return kFALSE;
   fxIter = GetDataAsModel()->MakeIter();
   if (!fxIter)
      return kFALSE;
   return fxIter->Reset(kFALSE);
}

Double_t TGo4FitModelFromData::EvaluateAtPoint(TGo4FitData *data, Int_t nbin, Bool_t UseRanges)
{
   return data ? FindDataPoint(GetDataIndexesSize(data), GetDataFullIndex(data, nbin)) : 0.;
}

Double_t TGo4FitModelFromData::EvaluateAtPoint(std::unique_ptr<TGo4FitDataIter> &iter, Bool_t UseRanges)
{
   return iter ? FindDataPoint(iter->IndexesSize(), iter->Indexes()) : 0.;
}

void TGo4FitModelFromData::AfterEval()
{
   fxIter.reset();
}

void TGo4FitModelFromData::FillSlotList(TSeqCollection *list)
{
   TGo4FitModel::FillSlotList(list);
   list->Add(&fxData);
   if (GetDataAsModel())
      GetDataAsModel()->FillSlotList(list);
}

void TGo4FitModelFromData::Print(Option_t *option) const
{
   TGo4FitModel::Print(option);
   std::cout << "Model driven from data" << std::endl;
   if (GetDataAsModel())
      GetDataAsModel()->Print(option);
}

Double_t TGo4FitModelFromData::FindDataPoint(Int_t NumIndexes, const Int_t *Indexes)
{
   if (!fxIter || fxIter->ReachEnd() || !Indexes || (NumIndexes != fxIter->IndexesSize()))
      return 0.;
   Int_t num = NumIndexes - 1;
   while (num >= 0) {
      if (fxIter->Indexes()[num] == Indexes[num]) {
         num--;
         continue;
      } else if (fxIter->Indexes()[num] > Indexes[num])
         return 0.;
      else if (!fxIter->Next(kFALSE))
         return 0.;
   }
   return fxIter->Value();
}
