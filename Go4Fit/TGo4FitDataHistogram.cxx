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

#include "TGo4FitDataHistogram.h"

#include "Riostream.h"

#include "TH1.h"
#include "TArrayD.h"
#include "TArrayF.h"

TGo4FitDataHistogram::TGo4FitDataHistogram() :
  TGo4FitData(), fxHistogram(this, TH1::Class()) {
}

TGo4FitDataHistogram::TGo4FitDataHistogram(const char* iName, TH1 *iHistogram, Bool_t iHistogramOwned, Bool_t AddAmpl) :
  TGo4FitData(iName,"TH1 and derived classes", dtHistogram, AddAmpl),
  fxHistogram("Histogram","Histogram object of TH1 class ",this, TH1::Class(), kTRUE, iHistogram, iHistogramOwned) {
}

TGo4FitDataHistogram::~TGo4FitDataHistogram() {
}

void TGo4FitDataHistogram::SetHistogram(TH1 *iHistogram, Bool_t iHistogramOwned)
{
   fxHistogram.SetObject(iHistogram,iHistogramOwned);
}

TGo4FitDataIter* TGo4FitDataHistogram::MakeIter() {
   return new TGo4FitDataHistogramIter(this);
}

void TGo4FitDataHistogram::FillSlotList(TSeqCollection* list) {
   TGo4FitData::FillSlotList(list);
   list->Add(&fxHistogram);
}

void TGo4FitDataHistogram::Print(Option_t* option) const {
   TGo4FitData::Print(option);
   cout << "   ";
   fxHistogram.Print(option);
}

// *********************************************************************************


TGo4FitDataHistogramIter::TGo4FitDataHistogramIter() : TGo4FitDataIter(),
    fxData(0), fxLimits(), fxOwnScales(), fxOwnWidths() {
}

TGo4FitDataHistogramIter::TGo4FitDataHistogramIter(TGo4FitDataHistogram* data) :
    TGo4FitDataIter(), fxData(data), fxLimits(), fxOwnScales(), fxOwnWidths() {
}

TGo4FitDataHistogramIter::~TGo4FitDataHistogramIter() {
}

Bool_t TGo4FitDataHistogramIter::StartReset()
{
  if (fxData==0) return kFALSE;
  TH1* histo = fxData->GetHistogram();
  if (histo==0) return kFALSE;

  Int_t NumDimen = histo->GetDimension();
  fxLimits.Set(NumDimen);
  fxLimits[0] = histo->GetNbinsX();
  if (NumDimen>1) fxLimits[1] = histo->GetNbinsY();
  if (NumDimen>2) fxLimits[2] = histo->GetNbinsZ();

  fxOwnScales.Set(NumDimen); fxOwnScales.Reset(0.);
  fxOwnWidths.Set(NumDimen); fxOwnWidths.Reset(0.);

  return ReserveArrays(NumDimen, NumDimen, kTRUE);
}

Bool_t TGo4FitDataHistogramIter::ReadCurrentPoint()
{
  if (fxData==0) return kFALSE;
  TH1* histo = fxData->GetHistogram();
  if (histo==0) return kFALSE;

  switch (histo->GetDimension()) {
     case 1: fdValue = histo->GetBinContent(fxIndexes[0]+1); break;
     case 2: fdValue = histo->GetBinContent(fxIndexes[0]+1, fxIndexes[1]+1); break;
     case 3: fdValue = histo->GetBinContent(fxIndexes[0]+1, fxIndexes[1]+1, fxIndexes[2]+1); break;
     default: fdValue = 0;
  }

  if (!GetDeviation()) fdStandardDeviation = fdValue;

  for (Int_t n=0;n<fxOwnScales.GetSize();n++)
    switch (n) {
      case 0: fxOwnScales[0] = histo->GetXaxis()->GetBinCenter(fxIndexes[0]+1); break;
      case 1: fxOwnScales[1] = histo->GetYaxis()->GetBinCenter(fxIndexes[1]+1); break;
      case 2: fxOwnScales[2] = histo->GetZaxis()->GetBinCenter(fxIndexes[2]+1); break;
    }

  for (Int_t n=0;n<fxOwnWidths.GetSize();n++)
    switch (n) {
      case 0: fxOwnWidths[0] = histo->GetXaxis()->GetBinWidth(fxIndexes[0]+1); break;
      case 1: fxOwnWidths[1] = histo->GetYaxis()->GetBinWidth(fxIndexes[1]+1); break;
      case 2: fxOwnWidths[2] = histo->GetZaxis()->GetBinWidth(fxIndexes[2]+1); break;
    }

  return ProduceScales(fxIndexes.GetArray(), fxOwnScales.GetArray(), fxOwnWidths.GetArray());
}

Bool_t TGo4FitDataHistogramIter::ShiftToNextPoint() {
  return NextIndex(fxIndexes,fxLimits);
}
