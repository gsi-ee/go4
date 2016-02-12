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

#include "TGo4FitData.h"

#include "Riostream.h"
#include "TBuffer.h"

#include "TMath.h"
#include "TObjArray.h"
#include "TArrayC.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"
#include "TClass.h"

#include "TGo4FitSlot.h"
#include "TGo4FitAxisTrans.h"


TGo4FitData::TGo4FitData() : TGo4FitComponent(),
    fiDataType(0), fbUseBinScale(kFALSE), fiTakeSigmasFrom(1), fdSigmaValue(1.), fdExcludeLessThen(0.),
    fxAxisTrans() {
       ResetAllPoinetrs();
}

TGo4FitData::TGo4FitData(const char* iName, const char* iTitle, Int_t iDataType, Bool_t AddAmpl) :
    TGo4FitComponent(iName,iTitle), fiDataType(iDataType),
    fbUseBinScale(kFALSE), fiTakeSigmasFrom(1), fdSigmaValue(1.), fdExcludeLessThen(0.),
    fxAxisTrans()  {

       ResetAllPoinetrs();

       if(AddAmpl) NewAmplitude("Ampl",1.0,kTRUE);

       fxAxisTrans.SetOwner(kTRUE);
}

TGo4FitData::~TGo4FitData() {
  ReleaseAllPointers();
}


Bool_t TGo4FitData::SetNumberOfTransSlots(Int_t nslots) {
  Int_t oldnum = GetNumberOfTransSlots();
  if ( (nslots<0) || (nslots == oldnum) ) return kFALSE;

  if (oldnum<nslots)
    for(Int_t n=oldnum;n<nslots;n++) {
       TString name("Trans");
       name+=n;
       fxAxisTrans.Add(new TGo4FitSlot(name.Data(),"Axis transformation", this, TGo4FitAxisTrans::Class(), kFALSE));
    }
  else
  for (Int_t n=oldnum;n>nslots;n--) {
      TObject* slot = fxAxisTrans.Last();
      fxAxisTrans.Remove(slot);
      fxAxisTrans.Compress();
      delete slot;
  }
  SetUpdateSlotList();
  return kTRUE;
}

TGo4FitSlot* TGo4FitData::GetAxisTransSlot(Int_t nslot)
{
   return (nslot>=0) && (nslot<=fxAxisTrans.GetLast()) ? dynamic_cast<TGo4FitSlot*> (fxAxisTrans[nslot]) : 0;
}

TGo4FitAxisTrans* TGo4FitData::GetAxisTrans(Int_t nslot)
{
   TGo4FitSlot* slot = GetAxisTransSlot(nslot);
   return (slot==0) ? 0 : dynamic_cast<TGo4FitAxisTrans*> (slot->GetObject());
}

void TGo4FitData::SetAxisTrans(Int_t nslot, TGo4FitAxisTrans *Trans, Bool_t TransOwned)
{
  if (nslot<0) return;
  if(nslot>=GetNumberOfTransSlots())
    SetNumberOfTransSlots(nslot+1);
  ((TGo4FitSlot*) (fxAxisTrans[nslot]))->SetObject(Trans,TransOwned);
}

void TGo4FitData::AddAxisTrans(TGo4FitAxisTrans* Trans, Bool_t TransOwned) {
   Int_t nslot = GetNumberOfTransSlots();
   SetNumberOfTransSlots(nslot+1);
   ((TGo4FitSlot*) (fxAxisTrans[nslot]))->SetObject(Trans,TransOwned);
}

void TGo4FitData::SetAxisTransNeeded(Int_t nslot, Bool_t iNeeded) {
  if( iNeeded && (nslot>=GetNumberOfTransSlots()))
    SetNumberOfTransSlots(nslot+1);
  if ((nslot>=0) && (nslot<GetNumberOfTransSlots()))
    ((TGo4FitSlot*) (fxAxisTrans[nslot]))->SetNeeded(iNeeded);
}

Bool_t TGo4FitData::IsAnyDataTransform() {
  if (GetUseBinScale() || (GetExcludeLessThen()>0)) return kTRUE;
  for (Int_t n=0;n<GetNumberOfTransSlots();n++)
    if (GetAxisTrans(n)) return kTRUE;
  return kFALSE;
}

TObject* TGo4FitData::CreateDrawObject(const char* ObjName) {
   TGo4FitDataIter* iter = MakeIter();
   if (iter==0) return 0;
   TObject* obj = iter->CreateDrawObject(ObjName);
   delete iter;
   return obj;
}

Bool_t TGo4FitData::Initialize(Int_t UseBuffers) {

    TGo4FitDataIter* iter = MakeIter();
    if (iter==0) return kFALSE;

    fiBinsSize = iter->CountPoints(kTRUE);

    fiIndexesSize = iter->IndexesSize();
    fiScalesSize = iter->ScalesSize();

    Bool_t use = ((UseBuffers<0) && GetUseBuffers()) || (UseBuffers>0);

    if (use)
      for(Int_t n=0; n<GetNumberOfTransSlots();n++) {
         TGo4FitAxisTrans* trans = GetAxisTrans(n);
         if (trans && !trans->IsAllParsFixed()) {
           use = kFALSE;
           break;
         }
      }

    if (use) {

        fxValues = new Double_t[fiBinsSize];
        fxStandDev = new Double_t[fiBinsSize];
        fxBinsResult = new Double_t[fiBinsSize];

        if (iter->HasIndexes()) fxFullIndex = new Int_t[fiBinsSize*fiIndexesSize];
        fxFullScale = new Double_t[fiBinsSize*fiScalesSize];
        if (iter->HasWidths()) fxFullWidth = new Double_t [fiBinsSize*fiScalesSize];

        Int_t nbin = 0;
        if (iter->Reset()) do {

          fxValues[nbin] = iter->Value();
          fxStandDev[nbin] = iter->StandardDeviation();

          if(fxFullIndex)
             for(Int_t n=0;n<fiIndexesSize;n++)
               fxFullIndex[nbin*fiIndexesSize+n] = iter->Indexes()[n];

          if(fxFullScale)
             for(Int_t naxis = 0;naxis<fiScalesSize;naxis++)
               fxFullScale[nbin*fiScalesSize+naxis] = iter->Scales()[naxis];

          if(fxFullWidth && iter->HasWidths())
             for(Int_t naxis = 0;naxis<fiScalesSize;naxis++)
               fxFullWidth[nbin*fiScalesSize+naxis] = iter->Widths()[naxis];

           nbin++;
        } while (iter->Next());
    }

    delete iter;

    return kTRUE;
}

void TGo4FitData::Finalize() {
  ReleaseAllPointers();
}

void TGo4FitData::ResetAllPoinetrs() {
   fiBinsSize = 0;
   fiIndexesSize = 0;
   fiScalesSize = 0;

   fxValues = 0;
   fxStandDev = 0;
   fxBinsResult = 0;

   fxFullScale = 0;
   fxFullWidth = 0;

   fxFullIndex = 0;
}

void TGo4FitData::ReleaseAllPointers() {
  if (fxValues) delete[] fxValues;

  if (fxStandDev) delete[] fxStandDev;

  if (fxBinsResult) delete[] fxBinsResult;

  if (fxFullIndex) delete[] fxFullIndex;

  if (fxFullScale) delete[] fxFullScale;

  if (fxFullWidth) delete[] fxFullWidth;

  ResetAllPoinetrs();
}

Bool_t TGo4FitData::DefineScaleMinMax(Int_t naxis, Double_t& min, Double_t& max) {
   TGo4FitDataIter* iter = MakeIter();
   if (iter==0) return kFALSE;
   Bool_t res = kFALSE;
   if (iter->Reset(kFALSE) && (iter->ScalesSize()<=naxis)) {
      min = iter->Scales()[naxis]; max = min;
      do {
        Double_t value = iter->Scales()[naxis];
        if (value<min) min = value; else
          if (value>max) max = value;
      } while (iter->Next(kFALSE));
      res = kTRUE;
   }

   delete iter;
   return res;
}

Int_t TGo4FitData::DefineDimensions() {
  TGo4FitDataIter* iter = MakeIter();
  if (iter==0) return 0;
  Int_t res = 0;
  if (iter->Reset(kFALSE)) res = iter->IndexesSize();
  delete iter;
  return res;
}

Int_t TGo4FitData::DefineBinsSize()
{
   TGo4FitDataIter* iter = MakeIter();
   if (iter==0) return 0;

   Int_t res = iter->CountPoints(kTRUE);
   delete iter;

   return res;
}

const Double_t* TGo4FitData::GetScaleValues(const Int_t nbin)
{
   if(fxFullScale) return &(fxFullScale[nbin*GetScalesSize()]);
              else return 0;
}

const Double_t* TGo4FitData::GetWidthValues(const Int_t nbin)
{
  if(fxFullWidth) return &(fxFullWidth[nbin*GetScalesSize()]);
             else return 0;
}

const Int_t* TGo4FitData::GetFullIndex(Int_t nbin)
{
  if (fxFullIndex) return &(fxFullIndex[nbin*GetIndexesSize()]);
              else return 0;
}

Bool_t TGo4FitData::IsCompatibleData(TGo4FitData* data)
{
   if (data==0) return kFALSE;
   TGo4FitDataIter* iter = data->MakeIter();
   if (iter==0) return kFALSE;

   Bool_t res = kFALSE;
   if (iter->Reset(kFALSE)) res = (iter->IndexesSize()==GetIndexesSize()) && (GetIndexesSize()>0);
   delete iter;

   return res;
}

void TGo4FitData::ApplyRangesForModelMask(TGo4FitComponent* model, Char_t* ModelMask)
{

  if (ModelMask==0) return;

  if (BuffersAllocated())
    for(Int_t nbin=0;nbin<GetBinsSize();nbin++) {
       const Double_t* values = GetScaleValues(nbin);

       Bool_t res = model->CheckRangeConditions(values, GetScalesSize());

       ModelMask[nbin] = res ? 1 : 0;
    }
  else {
    TGo4FitDataIter* iter = MakeIter();
    Int_t nbin = 0;
    if (iter->Reset()) do {
       Bool_t res = model->CheckRangeConditions(iter->Scales(), iter->ScalesSize());
       ModelMask[nbin] = res ? 1 : 0;
       nbin++;
    } while (iter->Next());
  }
}

void TGo4FitData::FillSlotList(TSeqCollection* list)
{
   TGo4FitComponent::FillSlotList(list);
   for(Int_t n=0;n<=fxAxisTrans.GetLast();n++)
     list->Add(fxAxisTrans[n]);
}

void TGo4FitData::Print(Option_t* option) const
{
   TGo4FitComponent::Print(option);
   std::cout << "   Data type: ";
   switch(fiDataType) {
      case 1: std::cout << "histogram" << std::endl; break;
      case 2: std::cout << "graph" << std::endl; break;
      default: std::cout << fiDataType << std::endl;
   }
   std::cout << "   Use bin scale:  " << fbUseBinScale << std::endl;
   std::cout << "   Take sigmas from:  " ;
   switch(GetSigmaSource()) {
      case 0: std::cout << "none" << std::endl; break;
      case 1: std::cout << "data" << std::endl; break;
      case 2: std::cout << "const value " << GetSigmaValue() << std::endl; break;
   }
   std::cout << "   Exclude bins less then: " << GetExcludeLessThen() << std::endl;
   std::cout << "   Axis transformation data: " << std::endl;
   fxAxisTrans.Print(option);
}

void TGo4FitData::Streamer(TBuffer& b)
{
   if (b.IsReading()) {

     TGo4FitData::Class()->ReadBuffer(b, this);

     for(Int_t n=0;n<=fxAxisTrans.GetLast();n++) {
         TGo4FitSlot* dc = (TGo4FitSlot*) fxAxisTrans[n];
         dc->SetDefaults(this, TGo4FitAxisTrans::Class());
     }

   } else {
     TGo4FitData::Class()->WriteBuffer(b, this);
   }
}

// *******************************************************************************************


TGo4FitDataIter::TGo4FitDataIter() : TObject(),
  fxIndexes(), fxScales(), fxWidths(), fdValue(0.), fdStandardDeviation(1.), fiNumPoint(0), fbReachEnd(kTRUE) {
}

TGo4FitDataIter::~TGo4FitDataIter() {
}

Bool_t TGo4FitDataIter::ReserveArrays(Int_t NumDimen, Int_t NumOwnAxis, Bool_t HasWidth) {
   TGo4FitData* data = GetData();
   if (data==0) return kFALSE;

   fxIndexes.Set(NumDimen); fxIndexes.Reset(0);

   Int_t size = 0;
   if (data->GetUseBinScale()) size = NumDimen;
                          else size = NumOwnAxis;

   if (size<=0) return kFALSE;

   fxScales.Set(size); fxScales.Reset(0.);
   if (HasWidth) { fxWidths.Set(size); fxWidths.Reset(1.); }
             else fxWidths.Set(0);

   return kTRUE;
}

void TGo4FitDataIter::TransformScales(Double_t* scales)
{
  TGo4FitData* data = GetData();
  for(Int_t nslot=0;nslot<data->GetNumberOfTransSlots();nslot++) {
     TGo4FitAxisTrans* trans = data->GetAxisTrans(nslot);
     if (trans) trans->Transformation(scales, ScalesSize());
  }
}

Bool_t TGo4FitDataIter::ProduceScales(const Int_t* index, const Double_t* ownscales, const Double_t* ownwidths)
{
   TGo4FitData* data = GetData();
   if (data==0) return kFALSE;

   if ( (data->GetUseBinScale()) || (ownscales==0) ) {
     if (index==0) return kFALSE;
     Double_t add = (data->GetDataType() == TGo4FitData::dtHistogram) ? .5 : 0.;
     for(Int_t n=0;n<fxScales.GetSize();n++)
       fxScales[n] = index[n] + add;
     fxWidths.Reset(1.);
   } else {
     for(Int_t n=0; n<fxScales.GetSize();n++)
       fxScales[n] = ownscales[n];
     if (ownwidths!=0)
       for(Int_t n=0; n<fxWidths.GetSize();n++)
         fxWidths[n] = ownwidths[n];
   }

   if (data->GetNumberOfTransSlots()>0) {
      if (fxWidths.GetSize()==ScalesSize()) {
         TArrayD arr1(ScalesSize()), arr2(ScalesSize());
         for(Int_t n=0;n<ScalesSize();n++) {
            arr1[n] = fxScales[n]-fxWidths[n]/2.;
            arr2[n] = fxScales[n]+fxWidths[n]/2.;
         }
         TransformScales(arr1.GetArray());
         TransformScales(arr2.GetArray());
         for(Int_t n=0;n<ScalesSize();n++)
           fxWidths[n] = TMath::Abs(arr2[n]-arr1[n]);
      }

      TransformScales(fxScales.GetArray());
   }

   return kTRUE;
}

Bool_t TGo4FitDataIter::NextIndex(TArrayI& Index, TArrayI& Limits) {
   Int_t n=0;
   while (n<Index.GetSize()) {
      Index[n]++;
      if (Index[n]<Limits[n]) return kTRUE;
      Index[n] = 0; n++;
   }
   return kFALSE;
}

Bool_t TGo4FitDataIter::GetDeviation() {
  TGo4FitData* data = GetData();
  if (data==0) return kFALSE;
  if (data->GetSigmaSource()==2) {
     fdStandardDeviation = data->GetSigmaValue()*data->GetSigmaValue();
     return kTRUE;
  } else return kFALSE;
}

Bool_t TGo4FitDataIter::CheckPointForRange() {
  TGo4FitData* data = GetData();
  if (data==0) return kFALSE;
  if (Value()<data->GetExcludeLessThen()) return kFALSE;
  return data->CheckRangeConditions(Scales(),ScalesSize());
}

Bool_t TGo4FitDataIter::Reset(Bool_t UseRanges) {
  fbReachEnd = kTRUE;

  if (!StartReset()) return kFALSE;

  fiNumPoint = 0;

  if (!ReadCurrentPoint()) return kFALSE;
  if (!UseRanges) { fbReachEnd = kFALSE; return kTRUE; }

  while (!CheckPointForRange()) {
     if (!ShiftToNextPoint()) return kFALSE;
     if (!ReadCurrentPoint()) return kFALSE;
  }

  fbReachEnd = kFALSE;
  return kTRUE;
}

Bool_t TGo4FitDataIter::Next(Bool_t UseRanges) {
  fiNumPoint++;

  if (fbReachEnd || (GetData()==0)) { fbReachEnd = kTRUE; return kFALSE; }

  do {
    if (!ShiftToNextPoint()) { fbReachEnd = kTRUE; return kFALSE; }

    if (!ReadCurrentPoint()) { fbReachEnd = kTRUE; return kFALSE; }

    if (!UseRanges) return kTRUE;

  } while (!CheckPointForRange());

  return kTRUE;
}

Double_t TGo4FitDataIter::xWidths() const
{
   double res = 1.;
   if(HasWidths())
      for(int n=0;n<fxWidths.GetSize();n++)
        res=res*fxWidths[n];
   return res;
}

Int_t TGo4FitDataIter::CountPoints(Bool_t UseRanges) {
  if (!Reset(UseRanges)) return 0;
  Int_t cnt=0;
  do {
    cnt+=1;
  } while (Next(UseRanges)) ;
  return cnt;
}

Bool_t TGo4FitDataIter::DefineIndexesLimits(TArrayI& Limits) {
   if (!Reset(kFALSE)) return kFALSE;
   if (IndexesSize()<=0) return kFALSE;
   Limits.Set(IndexesSize()); Limits.Reset(0);
   do {
     for(Int_t n=0;n<IndexesSize();n++)
       if (Indexes()[n]>Limits[n]) Limits[n] = Indexes()[n];
   } while(Next(kFALSE));
   return kTRUE;
}

TH1* TGo4FitDataIter::CreateHistogram(const char* HistoName, Bool_t UseRanges, Bool_t SetBins) {
   TArrayI Limits;
   if (!DefineIndexesLimits(Limits)) return 0;
   if (!HasIndexes() || (IndexesSize()!=ScalesSize()) || !HasWidths()) return 0;

   Int_t NumDim = IndexesSize();
   if (NumDim>3) NumDim=3;

   Double_t* dummy = 0;
   TH1* histo = 0;
   switch(NumDim) {
      case 1: histo = new TH1D(HistoName, "result", Limits[0]+1, dummy); break;
      case 2: histo = new TH2D(HistoName, "result", Limits[0]+1, dummy, Limits[1]+1, dummy); break;
      case 3: histo = new TH3D(HistoName, "result", Limits[0]+1, dummy, Limits[1]+1, dummy, Limits[2]+1, dummy); break;
      default: return 0;
   }

   histo->SetDirectory(0);

   Double_t* Axises[3];
   for (Int_t n=0;n<NumDim;n++)
     Axises[n] = new Double_t[Limits[n]+2];

   Double_t ampl = GetData()->GetAmplValue();

   if (Reset(UseRanges)) do {
      if (SetBins)
        switch (NumDim) {
           case 1: histo->SetBinContent(Indexes()[0]+1, ampl*Value()); break;
           case 2: histo->SetBinContent(Indexes()[0]+1, Indexes()[1]+1, ampl*Value()); break;
           case 3: histo->SetBinContent(Indexes()[0]+1, Indexes()[1]+1, Indexes()[2]+1, ampl*Value()); break;
        }
      for(Int_t n=0;n<NumDim;n++) {
         Int_t indx = Indexes()[n];
         Axises[n][indx] = Scales()[n]-Widths()[n]/2.;
         Axises[n][indx+1] = Scales()[n]+Widths()[n]/2.;
      }
   } while(Next(UseRanges));

   histo->GetXaxis()->Set(Limits[0]+1,Axises[0]);
   if (NumDim>1) histo->GetYaxis()->Set(Limits[1]+1,Axises[1]);
   if (NumDim>2) histo->GetZaxis()->Set(Limits[2]+1,Axises[2]);

   for (Int_t n=0;n<NumDim;n++)
     delete[] Axises[n];

   return histo;
}

TGraph* TGo4FitDataIter::CreateGraph(const char* GraphName, Bool_t UseRanges, Bool_t SetBins) {
   Int_t NumPoints = CountPoints(UseRanges);
   if ((NumPoints<=0) || (ScalesSize()<1)) return 0;

   TGraph* gr = new TGraph(NumPoints);
   gr->SetName(GraphName);
   if (Reset(UseRanges)) do {
      (gr->GetX())[Point()] = x();
      if (SetBins)
        (gr->GetY())[Point()] = GetData()->GetAmplValue() * Value();
   } while(Next(UseRanges));

   return gr;
}

TObject* TGo4FitDataIter::CreateDrawObject(const char* ObjName) {
  if (!Reset(kFALSE)) return 0;
  if (HasIndexes() && (IndexesSize()==ScalesSize()) && HasWidths()) return CreateHistogram(ObjName, kFALSE, kTRUE);
                                                               else return CreateGraph(ObjName, kFALSE, kTRUE);
}
