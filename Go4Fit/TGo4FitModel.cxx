#include "TGo4FitModel.h"

#include "Riostream.h"

#include "TArrayD.h"
#include "TArrayI.h"
#include "TClass.h"

#include "TGo4FitData.h"
#include "TGo4FitParameter.h"

TGo4FitAssignment::TGo4FitAssignment() :
   TNamed(), fxRatio(0), fxData(0), fxModelMask(0), fxModelBins(0) {
}

TGo4FitAssignment::TGo4FitAssignment(const char* DataName) :
   TNamed(DataName,""),
   fxRatio(0), fxData(0), fxModelMask(0), fxModelBins(0) {
}

TGo4FitAssignment::~TGo4FitAssignment() {
   if (fxRatio) delete fxRatio;
   if (fxModelMask) delete[] fxModelMask;
   if (fxModelBins) delete[] fxModelBins;
}

Double_t TGo4FitAssignment::RatioValue() {
   return (fxRatio==0) ? 1. : fxRatio->GetValue();
}

void TGo4FitAssignment::Print(Option_t* option) const {
   cout << "  " << GetName();
}

// **************************************************************************************************

TGo4FitModel::TGo4FitModel() : TGo4FitComponent(),
   fiMinIntegrDepth(0), fiMaxIntegrDepth(0), fdIntegrEps(0.), fbAbsoluteEps(kFALSE), fbIntegrScaling(kFALSE),
   fxAssigments(), fiGroupIndex(-1),
   fxCurrentPars(), fxCurrentParsArray(0),
   fbNeedToRebuild(kFALSE), fxAllPars(0), fxAllParsValues(0)  {
}

TGo4FitModel::TGo4FitModel(const char* iName, const char* iTitle, Bool_t MakeAmplitude) :
  TGo4FitComponent(iName,iTitle),
  fiMinIntegrDepth(0), fiMaxIntegrDepth(0), fdIntegrEps(0.), fbAbsoluteEps(kFALSE), fbIntegrScaling(kFALSE),
  fxAssigments(), fiGroupIndex(-1),
  fxCurrentPars(), fxCurrentParsArray(0),
  fbNeedToRebuild(kFALSE), fxAllPars(0), fxAllParsValues(0)  {
    fxAssigments.SetOwner(kTRUE);
    if (MakeAmplitude) NewAmplitude();
}

TGo4FitModel::~TGo4FitModel() {
  RemoveAllPars();
}

Int_t TGo4FitModel::NumPars() {
   Int_t res = TGo4FitParsList::NumPars();
   if (NumAssigments()>0) res += NumAssigments()-1;
   return res;
}

TGo4FitParameter* TGo4FitModel::Get(Int_t n) {
   Int_t numpars = TGo4FitParsList::NumPars();
   if (n<numpars) return TGo4FitParsList::Get(n);
   n-=numpars-1;
   return (n>=0) && (n<NumAssigments()) ? GetAssigment(n)->fxRatio : 0;
}

void TGo4FitModel::AssignToData(const char* DataName, Double_t RatioValue, Bool_t FixRatio) {
  if (FindAssigment(DataName)==0) {
     TGo4FitAssignment* ass = new TGo4FitAssignment(DataName);
     if (NumAssigments()>0) {
       ass->fxRatio = new TGo4FitParameter(GetRatioName(NumAssigments()),
                                           "Amplitude ratio to first data",
                                           RatioValue);
       ass->fxRatio->SetOwner(this);
       if (FixRatio) ass->fxRatio->SetFixed(kTRUE);
     }
     fxAssigments.Add(ass);
  }
}

void TGo4FitModel::ChangeDataNameInAssignments(const char* oldname, const char* newname) {
   TGo4FitAssignment* ass = FindAssigment(oldname);
   if (ass) ass->SetName(newname);
}

void TGo4FitModel::ClearAssignmentTo(const char* DataName) {
   TGo4FitAssignment* ass = FindAssigment(DataName);
   if (ass==0) return;

   fxAssigments.Remove(ass);
   delete ass;
   fxAssigments.Compress();

   for (Int_t n=0;n<NumAssigments();n++) {
     ass = GetAssigment(n);
     if (ass->fxRatio==0) continue;
     if (n==0) {
       delete ass->fxRatio;
       ass->fxRatio = 0;
     } else
        ass->fxRatio->SetName(GetRatioName(n+1));
   }
}

void TGo4FitModel::ClearAssignments() {
  fxAssigments.Clear();
  fxAssigments.Compress();
}

void TGo4FitModel::ConnectToDataIfAssigned(TGo4FitData* data) {
   if (data==0) return;
   TGo4FitAssignment* ass = FindAssigment(data->GetName());
   if ( ass != 0 ) ass->fxData = data;
}

TGo4FitData* TGo4FitModel::GetAssignedConnection(Int_t n) {
   TGo4FitAssignment* ass = GetAssigment(n);
   if(ass) return ass->fxData;
      else return 0;
}

Bool_t TGo4FitModel::GetPosition(Int_t naxis, Double_t& pos) {
   if(GetPosPar(naxis)) {
      pos = GetPosPar(naxis)->GetValue();
      return kTRUE;
   } else return kFALSE;
}

Bool_t TGo4FitModel::SetPosition(Int_t naxis, Double_t pos) {
   if(GetPosPar(naxis)) {
      GetPosPar(naxis)->SetValue(pos);
      return kTRUE;
   } else return kFALSE;
}

Bool_t TGo4FitModel::GetWidth(Int_t naxis, Double_t& width) {
   if(GetWidthPar(naxis)) {
      width = GetWidthPar(naxis)->GetValue();
      return kTRUE;
   } else return kFALSE;
}

Bool_t TGo4FitModel::SetWidth(Int_t naxis, Double_t width) {
   if(GetWidthPar(naxis)) {
      GetWidthPar(naxis)->SetValue(width);
      return kTRUE;
   } else return kFALSE;
}

void TGo4FitModel::SetIntegrationsProperty(Int_t iMinIntegrDepth, Int_t iMaxIntegrDepth, Double_t iIntegrEps, Bool_t iAbsoluteEps, Bool_t iIntegrScaling) {
  fiMinIntegrDepth = iMinIntegrDepth;
  fiMaxIntegrDepth = iMaxIntegrDepth;
  fdIntegrEps = iIntegrEps;
  fbAbsoluteEps = iAbsoluteEps;
  fbIntegrScaling = iIntegrScaling;
  if (fdIntegrEps<=0.)
    fiMaxIntegrDepth = fiMinIntegrDepth;
}

void TGo4FitModel::RemoveAllPars() {
  if (fxAllPars) { delete fxAllPars; fxAllPars=0; }
  if (fxAllParsValues) { delete fxAllParsValues; fxAllParsValues=0; }
}

Bool_t TGo4FitModel::Initialize(Int_t UseBuffers) {
   Bool_t use = ((UseBuffers<0) && GetUseBuffers()) || (UseBuffers>0);
   for(Int_t n=0;n<NumAssigments();n++) {
      TGo4FitAssignment* ass = GetAssigment(n);
      if (ass->fxData==0) {
         cout << "Data " << ass->GetName() << "  not assigned to model " << GetName() << endl;
         continue;
      }

      if (use && ass->fxData->BuffersAllocated()) {
         if (IsAnyRangeLimits()) {
             ass->fxModelMask = new Char_t[ass->fxData->GetBinsSize()];
             ass->fxData->ApplyRangesForModelMask(this,ass->fxModelMask);
         }

         ass->fxModelBins = new Double_t[ass->fxData->GetBinsSize()];
      }
   }

   SetNeedToRebuild();

   if (use) {
      RemoveAllPars();

      fxAllPars = new TGo4FitParsList(kFALSE);
      CollectParsTo(*fxAllPars);

      fxAllParsValues = new TArrayD(fxAllPars->NumPars());
      fxAllPars->GetParsValues(fxAllParsValues->GetArray());
   }

   return kTRUE;
}

void TGo4FitModel::Finalize() {
   RemoveAllPars();
   fxCurrentPars.Set(0);
   for(Int_t n=0;n<NumAssigments();n++) {
      TGo4FitAssignment* ass = GetAssigment(n);
      if (ass->fxModelMask) { delete[] ass->fxModelMask; ass->fxModelMask = 0; }
      if (ass->fxModelBins) { delete[] ass->fxModelBins; ass->fxModelBins = 0; }
   }
}

Bool_t TGo4FitModel::BuffersAllocated() const {
   Bool_t res = (NumAssigments()>0);
   for(Int_t n=0;n<NumAssigments();n++)
      res = res && (GetAssigment(n)->fxModelBins!=0);
   return res;
}

Double_t* TGo4FitModel::GetModelBins(const char* DataName) const {
   TGo4FitAssignment* ass = FindAssigment(DataName);
   return ass != 0 ? ass->fxModelBins : 0;
}

Double_t TGo4FitModel::EvaluateAndIntegrate(Int_t NumScales, const Double_t* Scales, const Double_t* Widths) {
  if ((NumScales<1) || (Scales==0)) return 0.;

  if ( (fiMinIntegrDepth>0) && (fiMaxIntegrDepth>0) && (Widths!=0)) {
      TArrayI IntegrIndexes(NumScales);
      TArrayD ScaleValues(NumScales, Scales);
      TArrayD WidthValues(NumScales, Widths);
      TArrayD dScaleValues(NumScales);

      Int_t* dindx = IntegrIndexes.GetArray();
      Double_t* vector = ScaleValues.GetArray();
      Double_t* width = WidthValues.GetArray();
      Double_t* dvector = dScaleValues.GetArray();

      Int_t n=0;

      // set value and  range for current point of model
      for(n=0;n<NumScales;n++)
         vector[n] -= 0.5*width[n];

      // include left bound of interval
      Double_t TotalSum = EvalN(vector);
      Int_t TotalNumPnt = 1;

      Int_t IntegrDepth = 0;
      Int_t NumStep = 1;
      Double_t Step = 1.0;
      Bool_t stopcondition = kFALSE;

      // cycle over integration depths
      do {

         IntegrIndexes.Reset(0);
         Double_t Sum = 0.;
         Int_t NumPnt = 0;

         // run over all integration points
         do {
           for(n=0;n<NumScales;n++)
              dvector[n] = vector[n]+Step*width[n]*(dindx[n]+0.5);

           Sum+=EvalN(dvector);
           NumPnt++;

           n=0;
           do {
             dindx[n]++;
             if (dindx[n]<NumStep) break;
             dindx[n]=0; n++;
           } while (n<NumScales);
         } while (n<NumScales);

         // check stop condition for integrations depth
         stopcondition = kFALSE;
         if (IntegrDepth>=fiMinIntegrDepth) { 
              if (IntegrDepth>=fiMaxIntegrDepth) stopcondition = kTRUE; else {
                 Double_t v1 = TotalSum/TotalNumPnt;
                 Double_t v2 = Sum/NumPnt;
                 Double_t v = TMath::Abs(v1-v2);
                 if (!fbAbsoluteEps) {
                    if ((v1!=0) || (v2!=0)) v=v/(TMath::Abs(v1)+TMath::Abs(v2));
                                       else v=0.;
                 }
                 if (v<=fdIntegrEps) stopcondition = kTRUE;
              }
           }
         TotalSum+=Sum;
         TotalNumPnt+=NumPnt;
         IntegrDepth++;
         NumStep*=2;
         Step = Step/2.;
     } while (!stopcondition);

     Double_t value = 0;
     if (TotalNumPnt>0) value = TotalSum / TotalNumPnt;
     if (fbIntegrScaling)
        for(n=0;n<NumScales;n++) value*=width[n];
     return value;
   } else return EvalN(Scales);
}

Double_t TGo4FitModel::EvaluateAtPoint(TGo4FitData* data, Int_t nbin, Bool_t UseRanges) {
   if (data==0) return 0.;
   const Double_t* scales = data->GetScaleValues(nbin);
   Int_t scalessize = data->GetScalesSize();
   if (UseRanges && !CheckRangeConditions(scales, scalessize)) return 0.;
   return EvaluateAndIntegrate(scalessize, scales, data->GetWidthValues(nbin));
}

Double_t TGo4FitModel::EvaluateAtPoint(TGo4FitDataIter* iter, Bool_t UseRanges) {
   if (iter==0) return 0;
   const Double_t* scales = iter->Scales();
   Int_t scalessize = iter->ScalesSize();
   if (UseRanges && !CheckRangeConditions(scales, scalessize)) return 0.;
   return EvaluateAndIntegrate(scalessize, scales, iter->Widths());
}

void TGo4FitModel::RebuildShape(Bool_t ForceBuild) {
   if ((fxAllPars==0) || (fxAllParsValues==0)) return;

   Bool_t fill = ForceBuild || fbNeedToRebuild;
   if (!fill)
     for(Int_t n=0;n<fxAllPars->NumPars();n++) {
       if (n==GetAmplIndex()) continue;
       if ((*fxAllParsValues)[n] != fxAllPars->GetPar(n)->GetValue()) {
         fill = kTRUE;
         break;
       }
     }

   if(fill)
      for(Int_t n=0;n<NumAssigments();n++) {
          TGo4FitAssignment* ass = GetAssigment(n);

          if ((ass->fxData==0) || (ass->fxModelBins==0)) continue;

          TGo4FitData* data = ass->fxData;

          if (!BeforeEval(data->GetScalesSize())) continue;

          Int_t size = data->GetBinsSize();
          Double_t* bins = ass->fxModelBins;
          Char_t* mask = ass->fxModelMask;
          Double_t ratio = ass->RatioValue();

          for(Int_t nbin=0;nbin<size;nbin++) {
             if ((mask!=0) && (mask[nbin]==0)) continue;
             bins[nbin] = ratio * EvaluateAtPoint(data, nbin, kFALSE);
          }

          AfterEval();
      }


   for(Int_t n=0;n<fxAllPars->NumPars();n++)
     (*fxAllParsValues)[n] = fxAllPars->GetPar(n)->GetValue();

   fbNeedToRebuild = kFALSE;
}

Bool_t TGo4FitModel::AddModelToDataResult(TGo4FitData* data) {
   if ((data==0) || (!data->BuffersAllocated())) return kFALSE;

   Double_t* result = data->GetBinsResult();
   if (result==0) return kFALSE;
   Int_t size = data->GetBinsSize();

   Double_t* modelbins = GetModelBins(data->GetName());
   if (modelbins) {
      Double_t ampl = GetAmplValue();
      for (Int_t nbin=0;nbin<size;nbin++)
        result[nbin] += ampl * modelbins[nbin];
      return kTRUE;
   }

   if (!BeforeEval(data->GetScalesSize())) return kFALSE;

   Double_t ampl = GetAmplValue() * GetRatioValueFor(data->GetName());

   for(Int_t nbin=0;nbin<size;nbin++)
      result[nbin] += ampl * EvaluateAtPoint(data, nbin);

   AfterEval();

   return kTRUE;
}

Bool_t TGo4FitModel::BeforeEval(Int_t ndim) {
   fxCurrentPars.Set(NumPars());
   GetParsValues(fxCurrentPars.GetArray());
   fxCurrentParsArray = fxCurrentPars.GetArray();
   if (GetAmplPar()!=0) fxCurrentParsArray++;
   return kTRUE;
}

Double_t TGo4FitModel::EvalN(const Double_t* v) {
   return UserFunction((Double_t*)v, fxCurrentParsArray);
}

Double_t TGo4FitModel::Evaluate(Double_t x) {
   Double_t zn = 0.;
   if (BeforeEval(1)) {
     zn = GetAmplValue() * EvalN(&x);
     AfterEval();
   }
   return zn;
}

Double_t TGo4FitModel::Evaluate(Double_t x, Double_t y) {
   Double_t zn = 0.;
   if (BeforeEval(2)) {
     Double_t vector[2] = {x,y};
     zn = GetAmplValue() * EvalN(vector);
     AfterEval();
   }
   return zn;
}

Double_t TGo4FitModel::Evaluate(Double_t x, Double_t y, Double_t z) {
   Double_t zn = 0.;
   if (BeforeEval(3)) {
     Double_t vector[3] = {x,y,z};
     zn = GetAmplValue() * EvalN(vector);
     AfterEval();
   }
   return zn;}

Double_t TGo4FitModel::Evaluate(Double_t* v, Int_t ndim) {
   Double_t zn = 0.;
   if (BeforeEval(ndim)) {
     zn = GetAmplValue() * EvalN(v);
     AfterEval();
   }
   return zn;
}

Double_t TGo4FitModel::Integral() {
   return 0;
}


const Int_t* TGo4FitModel::GetDataFullIndex(TGo4FitData* data, Int_t nbin) {
  return data==0 ? 0 : data->GetFullIndex(nbin);
}

Int_t TGo4FitModel::GetDataIndexesSize(TGo4FitData* data) {
  return data==0 ? 0 : data->GetIndexesSize();
}

TGo4FitAssignment* TGo4FitModel::FindAssigment(const char* DataName) const {
  for (Int_t n=0;n<NumAssigments();n++)
    if (strcmp(DataName, GetAssigment(n)->GetName()) == 0)
      return GetAssigment(n);
  return 0;
}

TString TGo4FitModel::GetRatioName(Int_t n)
{
   TString res;
   res.Form("Ratio%d",n);
   return res;
}

Double_t TGo4FitModel::GetRatioValueFor(const char* DataName) {
   TGo4FitAssignment* ass = FindAssigment(DataName);
   return (ass==0) ? 1. : ass->RatioValue();
}

void TGo4FitModel::Print(Option_t* option) const {
    TGo4FitComponent::Print(option);
    cout << "  Assigned to: ";
    fxAssigments.Print(option);
    cout << endl;
    if ( (fiMinIntegrDepth>0) && (fiMaxIntegrDepth>0) ) {
       cout << "   Integration property: depths from " << fiMinIntegrDepth << " to " <<  fiMaxIntegrDepth;
       if (fbAbsoluteEps) cout << " absolute"; else cout << "  relative";
       cout << " error " << fdIntegrEps << endl;
    }
}

void TGo4FitModel::Streamer(TBuffer& b) {
   if (b.IsReading()) {

     TGo4FitModel::Class()->ReadBuffer(b, this);

     for (Int_t n=0;n<NumAssigments();n++)
        if (GetAssigment(n)->fxRatio)
          GetAssigment(n)->fxRatio->SetOwner(this);

   } else {
     TGo4FitModel::Class()->WriteBuffer(b, this);
   }
}
