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

#include "TGo4FitModelFromData.h"

#include "Riostream.h"

#include "TH1.h"
#include "TArrayI.h"

#include "TGo4FitData.h"
#include "TGo4FitDataHistogram.h"

TGo4FitModelFromData::TGo4FitModelFromData() : TGo4FitModel(), fxData(this, TGo4FitData::Class()), fxIter(0) {
}

TGo4FitModelFromData::TGo4FitModelFromData(const char* iName, TGo4FitData *iDataAsModel, Bool_t Amplitude) :
  TGo4FitModel(iName,"data used as model",Amplitude),
  fxData("forModel", "Data, used to represent model component",
          this, TGo4FitData::Class(), kTRUE, iDataAsModel, kTRUE), fxIter(0) {
}

TGo4FitModelFromData::TGo4FitModelFromData(const char* iName, TH1* histo, Bool_t iOwned, Bool_t Amplitude) :
   TGo4FitModel(iName,"data used as model",Amplitude),
   fxData("forModel", "Data, used to represent model component",
           this, TGo4FitData::Class(), kTRUE), fxIter(0) {
      SetDataAsModel(new TGo4FitDataHistogram("Histogram",histo,iOwned), kTRUE);
}

TGo4FitModelFromData::~TGo4FitModelFromData()
{
   if (fxIter) delete fxIter;
}

TGo4FitData* TGo4FitModelFromData::GetDataAsModel() const
{
   return dynamic_cast<TGo4FitData*> (fxData.GetObject());
}

void TGo4FitModelFromData::SetDataAsModel(TGo4FitData* iData, Bool_t iOwned) {
  fxData.SetObject(iData,iOwned);
  SetUpdateSlotList();
}

Bool_t TGo4FitModelFromData::Initialize(Int_t UseBuffers) {
   if (GetDataAsModel() == 0) return kFALSE;

   for(Int_t n=0;n<NumAssigments();n++)
     if (GetAssignedConnection(n))
        if (!GetAssignedConnection(n)->IsCompatibleData(GetDataAsModel())) {
            cout << "TGo4FitModelFromData: incompatible data used for model" << endl;
            return kFALSE;
         }

   return TGo4FitModel::Initialize(UseBuffers);
}

Bool_t TGo4FitModelFromData::BeforeEval(Int_t) {
  if (fxIter!=0) { delete fxIter; fxIter = 0; }
  if (GetDataAsModel()==0) return kFALSE;
  fxIter = GetDataAsModel()->MakeIter();
  if (fxIter==0) return kFALSE;
  return fxIter->Reset(kFALSE);
}

Double_t TGo4FitModelFromData::EvaluateAtPoint(TGo4FitData* data, Int_t nbin, Bool_t UseRanges) {
  if (data==0) return 0.;
  return FindDataPoint(GetDataIndexesSize(data), GetDataFullIndex(data,nbin));
}

Double_t TGo4FitModelFromData::EvaluateAtPoint(TGo4FitDataIter* iter, Bool_t UseRanges) {
   if (iter==0) return 0.;
   return FindDataPoint(iter->IndexesSize(), iter->Indexes());
}

void TGo4FitModelFromData::AfterEval() {
  if (fxIter!=0) { delete fxIter; fxIter = 0; }
}


void TGo4FitModelFromData::FillSlotList(TSeqCollection* list) {
  TGo4FitModel::FillSlotList(list);
  list->Add(&fxData);
  if (GetDataAsModel()) GetDataAsModel()->FillSlotList(list);
}

void TGo4FitModelFromData::Print(Option_t* option) const {
   TGo4FitModel::Print(option);
   cout << "Model driven from data"  << endl;
   if (GetDataAsModel()) GetDataAsModel()->Print(option);
}

Double_t TGo4FitModelFromData::FindDataPoint(Int_t NumIndexes, const Int_t* Indexes) {
   if ((fxIter==0) || (fxIter->ReachEnd()) || (Indexes==0) || (NumIndexes!=fxIter->IndexesSize())) return 0.;
   Int_t num = NumIndexes-1;
   while (num>=0)
     if (fxIter->Indexes()[num]==Indexes[num]) { num--; continue; } else
       if (fxIter->Indexes()[num]>Indexes[num]) return 0.; else
         if (!fxIter->Next(kFALSE)) return 0.;
   return fxIter->Value();
}
