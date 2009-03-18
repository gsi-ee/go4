#include "TGo4FitComponent.h"

#include "Riostream.h"

#include "TCutG.h"
#include "TGo4FitParameter.h"

const UInt_t kExcludeCut = 0x00100000;

TGo4FitComponent::TGo4FitComponent() : TGo4FitParsList(), TGo4FitSlotList(),
    fxRangeAxis(), fxRangeValue(), fxCuts(), fiAmplIndex(-1), fbUseBuffers(kFALSE) {
}

TGo4FitComponent::TGo4FitComponent(const char* iName, const char* iTitle) :
    TGo4FitParsList(iName,iTitle,kTRUE), TGo4FitSlotList(),
    fxRangeAxis(), fxRangeValue(), fxCuts(), fiAmplIndex(-1), fbUseBuffers(kFALSE) {
  fxCuts.SetOwner(kTRUE);
}

TGo4FitComponent::~TGo4FitComponent() {
}

TGo4FitParameter* TGo4FitComponent::NewParameter(const char* Name, const char* Title, Double_t iValue, Bool_t Fixed, Int_t AtIndx) {
   TGo4FitParameter* par = new TGo4FitParameter(Name, Title, iValue);
   if (AtIndx<0) AddPar(par);
            else InsertPar(par,AtIndx);
   if(Fixed) par->SetFixed(kTRUE);
   return par;
}

TGo4FitParameter* TGo4FitComponent::NewAmplitude(const char* Name, Double_t iValue, Bool_t IsFixed, Int_t AtIndx) {
   TGo4FitParameter* par = 0;
   if (Name==0) par = new TGo4FitParameter("Ampl", "Amplitude", iValue);
           else par = new TGo4FitParameter(Name, "Amplitude", iValue);
   if (AtIndx<0) { AddPar(par); AtIndx = NumPars()-1; }
            else InsertPar(par,AtIndx);
   SetAmplIndex(AtIndx);
   if(IsFixed) par->SetFixed(kTRUE);
   return par;
}

TGo4FitParameter* TGo4FitComponent::GetAmplPar()
{
   return fiAmplIndex<0 ? 0 : GetPar(fiAmplIndex);
}

const char* TGo4FitComponent::GetAmplName()
{
   return GetAmplPar() ? GetAmplPar()->GetName() : 0;
}

const char* TGo4FitComponent::GetAmplFullName()
{
   return GetAmplPar() ? GetAmplPar()->GetFullName() : 0;
}
Double_t TGo4FitComponent::GetAmplValue()
{
   return GetAmplPar() ? GetAmplPar()->GetValue() : 1.0;
}

void TGo4FitComponent::SetAmplValue(Double_t iAmpl)
{
   if(GetAmplPar()) Get(fiAmplIndex)->SetValue(iAmpl);
}

Double_t TGo4FitComponent::GetAmplError()
{
   return GetAmplPar() ? GetAmplPar()->GetError() : 0.0;
}

void TGo4FitComponent::SetAmplError(Double_t iError)
{
   if(GetAmplPar()) Get(fiAmplIndex)->SetError(iError);
}

Bool_t TGo4FitComponent::MakeAmpl(Bool_t isFixed) {
  if (GetAmplPar()!=0) return kFALSE;
  NewAmplitude();
  if (isFixed) GetAmplPar()->SetFixed(kTRUE);
  return kTRUE;
}

Bool_t TGo4FitComponent::RemoveAmpl() {
   if (GetAmplPar()==0) return kFALSE;
   RemoveParByIndex(GetAmplIndex());
   SetAmplIndex(-1);
   return kTRUE;
}

void TGo4FitComponent::GetRangeCondition(Int_t n, Int_t& typ, Int_t& naxis, Double_t& left, Double_t& right) const {
   if ((n>=0) && (n<GetNumRangeCondition())) {
     Int_t naxis1 = fxRangeAxis[n*2];
     Int_t naxis2 = fxRangeAxis[n*2+1];
     Double_t value1 = fxRangeValue[n*2];
     Double_t value2 = fxRangeValue[n*2+1];

     if ((naxis1<0) && (naxis2>0)) {
         typ = 0;
         naxis = -naxis1 - 1;
         left = value1;
         right = value2;
       } else
       if ((naxis1>0) && (naxis2<0)) {
         typ = 1;
         naxis = naxis1 - 1;
         left = value1;
         right = value2;
       } else
       if (naxis1<0) {
         typ = 2;
         naxis = -naxis1 - 1;
         left = value1;
         right = 0.;
       }  else
       if (naxis1>0) {
         typ = 3;
         naxis = naxis1 - 1;
         left = 0.;
         right = value1;
       }
   }
}

void TGo4FitComponent::SetRangeCondition(Int_t n, Int_t typ, Int_t naxis, Double_t left, Double_t right) {
  if ((n>=0) && (n<GetNumRangeCondition())) {
     Int_t naxis1, naxis2;
     Double_t value1, value2;

     switch (typ) {
       case 0: naxis1 = -(naxis+1); value1 = left;
               naxis2 = +(naxis+1); value2 = right;
               break;
       case 1: naxis1 = +(naxis+1); value1 = left;
               naxis2 = -(naxis+1); value2 = right;
               break;
       case 2: naxis1 =  -(naxis+1); value1 = left;
               naxis2 = 0; value2 = 0.;
               break;
       case 3: naxis1 =  +(naxis+1); value1 = right;
               naxis2 = 0; value2 = 0.;
               break;
       default: return;
     }

     fxRangeAxis[n*2] = naxis1;
     fxRangeAxis[n*2+1] = naxis2;
     fxRangeValue[n*2] = value1;
     fxRangeValue[n*2+1] = value2;
  }
}

void TGo4FitComponent::AddRangeCondition(Int_t typ, Int_t naxis, Double_t left, Double_t right) {
  Int_t sz = fxRangeValue.GetSize()+2;
  fxRangeAxis.Set(sz);  fxRangeValue.Set(sz);
  SetRangeCondition(sz/2-1,typ,naxis,left,right);
}

void TGo4FitComponent::RemoveRangeCondition(Int_t n) {
   if ((n<0) || (n>=GetNumRangeCondition())) return;

   Int_t indx = n*2;
   while (indx<fxRangeAxis.GetSize()-2) {
     fxRangeAxis[indx]=fxRangeAxis[indx+2];
     fxRangeValue[indx]=fxRangeValue[indx+2];
     indx+=1;
   }
   fxRangeAxis.Set(indx);
   fxRangeValue.Set(indx);
}

void TGo4FitComponent::SetRange(Int_t naxis, Double_t min, Double_t max) {
   AddRangeCondition(0,naxis,min,max);
}

void TGo4FitComponent::ExcludeRange(Int_t naxis, Double_t min, Double_t max) {
   AddRangeCondition(1,naxis,min,max);
}

void TGo4FitComponent::SetRangeMin(Int_t naxis, Double_t value) {
   AddRangeCondition(2,naxis,value,0.);
}

void TGo4FitComponent::SetRangeMax(Int_t naxis, Double_t value) {
   AddRangeCondition(3,naxis,0.,value);
}

void TGo4FitComponent::ClearRanges(Int_t naxis) {
   if (naxis<0) { fxRangeAxis.Set(0); fxRangeValue.Set(0); fxCuts.Delete(); } else {
      Int_t indx = 0, dindx = 0;
      while (indx<fxRangeAxis.GetSize()) {
         if ( TMath::Abs(fxRangeAxis[indx]) != (naxis+1) ) {
            fxRangeAxis[dindx]=fxRangeAxis[indx];
            fxRangeAxis[dindx+1]=fxRangeAxis[indx+1];
            fxRangeValue[dindx]=fxRangeValue[indx];
            fxRangeValue[dindx+1]=fxRangeValue[indx+1];
            dindx+=2;
         }
         indx+=2;
      }
      fxRangeAxis.Set(dindx);
      fxRangeValue.Set(dindx);
      if ((naxis==0) || (naxis==1)) fxCuts.Delete();
   }
}

Bool_t TGo4FitComponent::IsAnyRangeLimits() {
   return (GetNumRangeCondition()>0) || (GetNumRangeCut()>0);
}

void TGo4FitComponent::AddRangeCut(TCutG* cut, Bool_t exclude) {
  cut->SetBit(kExcludeCut, exclude);
  fxCuts.Add(cut);
}

Int_t TGo4FitComponent::GetNumRangeCut() const {
  return fxCuts.GetLast()+1;
}

TCutG* TGo4FitComponent::GetRangeCut(Int_t n) const {
  return (n>=0) && (n<=fxCuts.GetLast()) ? dynamic_cast<TCutG*> (fxCuts.At(n)) : 0;
}

Bool_t TGo4FitComponent::IsRangeCutExcluding(Int_t n) {
  TCutG* cut = GetRangeCut(n);
  return cut==0 ? kFALSE : cut->TestBit(kExcludeCut);
}

void TGo4FitComponent::SetRangeCutExcluding(Int_t n, Bool_t exclude) {
  TCutG* cut = GetRangeCut(n);
  if (cut) cut->SetBit(kExcludeCut, exclude);
}

void TGo4FitComponent::RemoveRangeCut(Int_t n) {
  TCutG* cut = GetRangeCut(n);
  if (cut) {
    fxCuts.Remove(cut);
    delete cut;
    fxCuts.Compress();
  }
}

Bool_t TGo4FitComponent::GetRangeMin(Int_t naxis, Double_t& value) {
   Bool_t isany = kFALSE;
   for(Int_t indx=0;indx<fxRangeAxis.GetSize();indx+=2)
      if ( fxRangeAxis[indx] == -(naxis+1) ) {
         Double_t zn = fxRangeValue[indx];
         if( (!isany) || (zn<value) ) value = zn;
         isany = kTRUE;
   }
   if(naxis<2)
     for(Int_t ncut=0;ncut<GetNumRangeCut();ncut++) {
        TCutG* cut = GetRangeCut(ncut);
        if(cut->TestBit(kExcludeCut)) continue;
        Double_t* arr = naxis == 0 ? cut->GetX() : cut->GetY();
        Double_t zn = arr[0];
        for(Int_t i=1;i<cut->GetN();i++)
          if(arr[i]<zn) zn = arr[i];
        if( (!isany) || (zn<value) ) value = zn;
        isany = kTRUE;
     }

   return isany;
}

Bool_t TGo4FitComponent::GetRangeMax(Int_t naxis, Double_t& value) {
   Bool_t isany = kFALSE;
   Double_t zn;
   for(Int_t indx=0;indx<fxRangeAxis.GetSize();indx+=2) {
      if ( (fxRangeAxis[indx] == -(naxis+1))  && (fxRangeAxis[indx+1] == +(naxis+1)))  zn = fxRangeValue[indx+1]; else
      if ( (fxRangeAxis[indx] == +(naxis+1))  && (fxRangeAxis[indx+1] == 0))  zn = fxRangeValue[indx]; else continue;
      if( (!isany) || (zn>value) ) value = zn;
      isany = kTRUE;
   }
   if(naxis<2)
     for(Int_t ncut=0;ncut<GetNumRangeCut();ncut++) {
        TCutG* cut = GetRangeCut(ncut);
        if(cut->TestBit(kExcludeCut)) continue;
        Double_t* arr = naxis == 0 ? cut->GetX() : cut->GetY();
        zn = arr[0];
        for(Int_t i=1;i<cut->GetN();i++)
          if(arr[i]>zn) zn = arr[i];
        if( (!isany) || (zn>value) ) value = zn;
        isany = kTRUE;
     }

   return isany;
}

Bool_t TGo4FitComponent::CheckRangeConditions(const Double_t* values, Int_t numaxis) {
   Int_t condsize = GetNumRangeCondition();

   if ((condsize==0) && (GetNumRangeCut()==0)) return kTRUE;

   Bool_t res1 = kTRUE, res2 = kTRUE;

   Bool_t isanycond = kFALSE;

   for(Int_t naxis=0;naxis<numaxis;naxis++) {
      Char_t resaxis1 = kTRUE, resaxis2 = kFALSE;
      Bool_t isany = kFALSE;
      for(Int_t ncond=0;ncond<condsize;ncond++) {

        Int_t typ, axisnumber;
        Double_t left, right;
        GetRangeCondition(ncond, typ, axisnumber, left, right);

        if (axisnumber != naxis) continue;

        switch(typ) {
          case 0:
            isany = kTRUE;                                               // include range
            if((values[naxis]>=left) && (values[naxis]<=right)) resaxis2 = kTRUE;
            break;
          case 1:                                                       // exclude range
            if((values[naxis]>=left) && (values[naxis]<=right)) resaxis1 = kFALSE;
            break;
          case 2: if(values[naxis]<left) resaxis1 = kFALSE; break;      // set left bound
          case 3: if(values[naxis]>right) resaxis1 = kFALSE; break;     // set right bound
        }
      }

      res1 = res1 && resaxis1;
      if (!isany) resaxis2 = kTRUE;
      res2 = res2 && resaxis2;
      isanycond = isanycond || isany;
      if (!res1) break;
   }

   if ( res1 && !(isanycond && res2) && (numaxis>1) && (GetNumRangeCut()>0)) {
     res2 = kFALSE;
     for (Int_t n=0;n<GetNumRangeCut();n++) {
        TCutG* cut = GetRangeCut(n);
        if (cut->IsInside(values[0],values[1]))
          {
            if (cut->TestBit(kExcludeCut)) { res1 = kFALSE; break; }
                                    else { res2 = kTRUE; break; }
          }
     }
   }

   return res1 && res2;
}

void TGo4FitComponent::CollectParsTo(TGo4FitParsList& list) {
  TGo4FitParsList::CollectParsTo(list);
  for(Int_t n=0;n<NumSlots();n++) {
     TGo4FitSlot* slot = GetSlot(n);
     if (slot && slot->GetOwned()) {
        TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (slot->GetObject());
        if (pars) pars->CollectParsTo(list);
     }
  }
}

void TGo4FitComponent::Print(Option_t* option) const {
    cout << "***************************************************************************" << endl;
    TGo4FitNamed::Print(option);
    TGo4FitParsList::Print(option);
    cout << "  Amplitude index: " << fiAmplIndex << endl;

    for (Int_t ncond=0; ncond<GetNumRangeCondition(); ncond++) {
       if (ncond==0) cout << " Range selection: " << endl;
       Int_t typ, naxis;
       Double_t left, right;
       GetRangeCondition(ncond, typ, naxis, left, right);

       cout << "   axis " << naxis << " ";

       switch(typ) {
          case 0:cout << "  select range from " << left << " to " << right; break;
          case 1: cout << "  exclude range from " << left << " to " << right; break;
          case 2: cout << "  set left bound to " << left; break;
          case 3: cout << "  set right bound to " << right; break;
       }

       cout << endl;
     }

     for (Int_t n=0;n<GetNumRangeCut();n++) {
        TCutG* cut = GetRangeCut(n);
        if (cut->TestBit(kExcludeCut)) cout << "  Exclude"; else cout << "  Include";
        cout << " axises ranges, using TCutG object " << endl;
        cut->Print(option);
     }
}
