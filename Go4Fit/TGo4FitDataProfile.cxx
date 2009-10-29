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

#include "TGo4FitDataProfile.h"

#include "Riostream.h"

#include "TProfile.h"
#include "TArrayD.h"

TGo4FitDataProfile::TGo4FitDataProfile() :
  TGo4FitData(), fxProfile(this, TProfile::Class()) {
}

TGo4FitDataProfile::TGo4FitDataProfile(const char* iName, TProfile *iProfile, Bool_t iProfileOwned, Bool_t AddAmpl) :
  TGo4FitData(iName,"TProfile classes", dtGraph, AddAmpl),
  fxProfile("Profile","Data object of TProfile class",this, TProfile::Class(), kTRUE, iProfile, iProfileOwned) {
    SetExcludeLessThen(-1e50);
}

TGo4FitDataProfile::~TGo4FitDataProfile() {
}

void TGo4FitDataProfile::SetProfile(TProfile *iProfile, Bool_t iProfileOwned)
{
  fxProfile.SetObject(iProfile,iProfileOwned);
}

TGo4FitDataIter* TGo4FitDataProfile::MakeIter() {
  return new TGo4FitDataProfileIter(this);
}

void TGo4FitDataProfile::FillSlotList(TSeqCollection* list) {
   TGo4FitData::FillSlotList(list);
   list->Add(&fxProfile);
}

void TGo4FitDataProfile::Print(Option_t* option) const {
   TGo4FitData::Print(option);
   cout << "   ";
   fxProfile.Print(option);
}


// ********************************************************************************************


TGo4FitDataProfileIter::TGo4FitDataProfileIter() : TGo4FitDataIter(), fxData(0), fiNumPoints(0) {
}

TGo4FitDataProfileIter::TGo4FitDataProfileIter(TGo4FitDataProfile* Data) :
  TGo4FitDataIter(), fxData(Data), fiNumPoints(0) {
}

TGo4FitDataProfileIter::~TGo4FitDataProfileIter() {
}

Bool_t TGo4FitDataProfileIter::StartReset() {
  if ((fxData==0) || (fxData->GetProfile()==0)) return kFALSE;

  fiNumPoints = fxData->GetProfile()->GetNbinsX();

  return ReserveArrays(1, 1, kFALSE);
}

Bool_t TGo4FitDataProfileIter::ReadCurrentPoint() {
   if (fxData==0) return kFALSE;
   TProfile* pr = fxData->GetProfile();
   if (pr==0) return kFALSE;

   Double_t xvalue = pr->GetXaxis()->GetBinCenter(fxIndexes[0]+1);
   fdValue = pr->GetBinContent(fxIndexes[0]+1);

   if (!GetDeviation()) {
      Double_t zn = pr->GetBinError(fxIndexes[0]+1);
      if (zn>0) fdStandardDeviation = zn*zn;
           else fdStandardDeviation = 1.;
   }

   return ProduceScales(fxIndexes.GetArray(), &xvalue, 0);
}


Bool_t TGo4FitDataProfileIter::ShiftToNextPoint() {
   fxIndexes[0]+=1;
   return (fxIndexes[0]<fiNumPoints);
}
