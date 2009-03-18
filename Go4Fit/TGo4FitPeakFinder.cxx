#include "TGo4FitPeakFinder.h"

#include "TMath.h"
#include "TH1D.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "TSpectrum.h"

#include "TGo4Fitter.h"
#include "TGo4FitData.h"
#include "TGo4FitModelPolynom.h"

TGo4FitPeakFinder::TGo4FitPeakFinder() : TGo4FitterAction() {

}

TGo4FitPeakFinder::TGo4FitPeakFinder(const char* Name,  const char* DataName, Bool_t ClearModels, Int_t PolOrder) :
  TGo4FitterAction(Name, "Peak finder action"),
  fiPeakFinderType(0), fxDataName(DataName), fbClearModels(ClearModels),
  fbUsePolynom(PolOrder>=0), fiPolynomOrder(PolOrder>=0 ? PolOrder : 0),
  fd0MinWidth(5.), fd0MaxWidth(30.), fd0MaxAmplFactor(0.2),
  fd1LineWidth(5.),
  fd2NoiseFactor(2.), fd2NoiseMinimum(5.), fi2ChannelSum(2) {
}

TGo4FitPeakFinder::~TGo4FitPeakFinder() {
}

void TGo4FitPeakFinder::SetupPolynomialBackground(Int_t PolynomOrder) {
   if (PolynomOrder<0) SetUsePolynom(kFALSE);
   else {
     SetUsePolynom(kTRUE);
     SetPolynomOrder(PolynomOrder);
   }
}

void TGo4FitPeakFinder::SetupForFirst(Double_t MaxAmplFactor, Double_t MinWidth, Double_t MaxWidth) {
  SetPeakFinderType(0);
  Set0MaxAmplFactor(MaxAmplFactor);
  Set0MinWidth(MinWidth);
  Set0MaxWidth(MaxWidth);
}

void TGo4FitPeakFinder::SetupForSecond(Double_t LineWidth) {
  SetPeakFinderType(1);
  Set1LineWidth(LineWidth);
}

void TGo4FitPeakFinder::SetupForThird(Double_t NoiseFactor, Double_t NoiseMinimum, Int_t ChannelSum) {
  SetPeakFinderType(2);
  Set2NoiseFactor(NoiseFactor);
  Set2NoiseMinimum(NoiseMinimum);
  Set2ChannelSum(ChannelSum);
}

void TGo4FitPeakFinder::DoAction(TGo4FitterAbstract* Fitter) {
  TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (Fitter);
  if (fitter==0) return;

  TGo4FitData* data = fitter->FindData(GetDataName());
  if (data==0) return;

  if (GetClearModels())
    fitter->DeleteModelsAssosiatedTo(data->GetName());

  if (GetPeakFinderType()==0)
     SergeyLinevPeakFinder(fitter,
                           data,
                           GetUsePolynom() ? GetPolynomOrder() : -1,
                           Get0MaxAmplFactor(),
                           Get0MinWidth(),
                           Get0MaxWidth());

   if (GetPeakFinderType()==1)
     ROOTPeakFinder(fitter,
                    data,
                    GetUsePolynom() ? GetPolynomOrder() : -1,
                    Get1LineWidth());

   if (GetPeakFinderType()==2)
     HansEsselPeakFinder(fitter,
                         data,
                         500,
                         Get2ChannelSum(),
                         Get2NoiseFactor(),
                         Get2NoiseMinimum(),
                         GetUsePolynom() ? GetPolynomOrder() : -1);
}

void TGo4FitPeakFinder::Print(Option_t* option) const {
  TGo4FitterAction::Print(option);
}
/*
void TGo4FitPeakFinder::ROOTPeakFinder(TGo4Fitter* fitter, TGo4FitData* data, Int_t PolynomOrder, Double_t Sigma) {
   if ((fitter==0) || (data==0)) return;

   TGo4FitDataIter* iter = data->MakeIter();
   if (iter==0) return;

   Int_t size = iter->CountPoints();

   if ((size<10) || (iter->ScalesSize()!=1))  { delete iter; return; }

   TArrayF Bins(size), Scales(size);

   Int_t nbin = 0;
   if (iter->Reset()) do {
      Bins[nbin] = data->GetAmplValue() * iter->Value();
      Scales[nbin] = iter->x();
      nbin++;
   } while (iter->Next());

   delete iter;

   if (PolynomOrder>=0)
     fitter->AddPolynomX(data->GetName(), "Pol", PolynomOrder);

   TSpectrum sp(100);
   sp.Search1(Bins.GetArray(), size, Sigma);

   for(Int_t n=0;n<sp.GetNPeaks();n++) {
      Double_t dindx = sp.GetPositionX()[n];
      Int_t left = TMath::Nint(dindx-Sigma/2.);
      Int_t right = TMath::Nint(dindx+Sigma/2.);

      Double_t pos = Scales[TMath::Nint(dindx)];
      Double_t width = TMath::Abs(Scales[right]-Scales[left]);

      fitter->AddGauss1(data->GetName(), fitter->FindNextName("Gauss",n), pos, width);
    }
}
*/

void TGo4FitPeakFinder::ROOTPeakFinder(TGo4Fitter* fitter, TGo4FitData* data, Int_t PolynomOrder, Double_t Sigma) {
   if ((fitter==0) || (data==0)) return;

   TGo4FitDataIter* iter = data->MakeIter();
   if (iter==0) return;

   Int_t size = iter->CountPoints();

   if ((size<10) || (iter->ScalesSize()!=1))  { delete iter; return; }

   TArrayD Bins(size), Scales(size), HScales(size+1);

   Int_t nbin = 0;
   if (iter->Reset()) do {
      Bins[nbin] = data->GetAmplValue() * iter->Value();
      Double_t pos = iter->x();
      Double_t width = iter->HasWidths() ? iter->Widths()[0] : 1.;
      Scales[nbin] = pos;
      HScales[nbin] = pos - width/2.;
      HScales[nbin+1] = pos + width/2;
      nbin++;
   } while (iter->Next());

   delete iter;

   if (PolynomOrder>=0)
     fitter->AddPolynomX(data->GetName(), "Pol", PolynomOrder);

   TH1D histo("ROOTPeakFinder", "ROOTPeakFinder spectrum", size, HScales.GetArray());
   histo.SetDirectory(0);
   for (Int_t i=0;i<size;i++)
      histo.SetBinContent(i+1, Bins[i]);

   TSpectrum sp(100);
   sp.Search(&histo, Sigma);
//   sp.Search1(Bins.GetArray(), size, Sigma);

   for(Int_t n=0;n<sp.GetNPeaks();n++) {
      Double_t pos = sp.GetPositionX()[n];
      Double_t width = Sigma;


//   this is old code for ROOT 3.*, may be usefull for somebody
/*
       Double_t dindx = sp.GetPositionX()[n];
      Int_t left = TMath::Nint(dindx-Sigma/2.);
      Int_t right = TMath::Nint(dindx+Sigma/2.);
      if (left<0) left = 0;
      if (right>=size) right = size-1;

      Double_t pos = Scales[TMath::Nint(dindx)];
      Double_t width = TMath::Abs(Scales[right]-Scales[left]);
*/
      fitter->AddGauss1(data->GetName(), fitter->FindNextName("Gauss",n), pos, width);
    }
}


Double_t TGo4FitPeakFinder::CalcPolynom(const TArrayD& Coef, Double_t x) {
   Double_t power = 1., sum = 0.;
   for(Int_t n=0;n<Coef.GetSize();n++)
     { sum+=Coef[n]*power; power*=x; }
   return sum;
}

void TGo4FitPeakFinder::DefinePolynom(Int_t size, Double_t* bins, Double_t* scales, TArrayD& Coef,
                   Double_t* weight, Double_t* backgr, Char_t* use) {
   Int_t maxorder = Coef.GetSize()-1;

   TMatrixD matr(maxorder+1,maxorder+1);
   TVectorD vector(maxorder+1);

   for(Int_t order1=0;order1<=maxorder;order1++)
     for(Int_t order2=order1;order2<=maxorder;order2++) {
        Double_t sum = 0;
        for(Int_t i=0;i<size;i++)
           if((use==0) || use[i]) {
              Double_t zn = TMath::Power(scales[i],order1+order2);
              if (weight) zn*=weight[i];
              sum+=zn;
           }
        matr(order1,order2) = sum;
        matr(order2,order1) = sum;
     }

   for(Int_t order1=0;order1<=maxorder;order1++)  {
      Double_t sum = 0;
      for(Int_t i=0;i<size;i++)
        if((use==0) || use[i]) {
          Double_t zn = bins[i];
          if (backgr) zn-=backgr[i];
          zn*=TMath::Power(scales[i],order1);
          if (weight) zn*=weight[i];
          sum+=zn;
        }
      vector(order1) = sum;
   }

   matr.Invert();
   vector*=matr;

   for(Int_t order=0;order<=maxorder;order++)
     Coef[order]=vector(order);
}

void TGo4FitPeakFinder::DefinePolynomEx(Int_t size, Double_t* bins, Double_t* scales, Double_t* weight, Double_t* backgr,
                                        Int_t lbound, Int_t rbound, TArrayD& Coef) {
    TArrayC use(size); use.Reset(0);
    if (lbound<0) lbound = 0;
    if (rbound>=size) rbound=size-1;
    for(Int_t i=lbound;i<=rbound;i++) use[i]=1;
    DefinePolynom(size, bins, scales, Coef, weight, backgr, use.GetArray());
}

Bool_t FindValue(Double_t* bins, Double_t* backgr, Char_t* use, Int_t size, Int_t& p, Int_t dir, Double_t value) {
   p += dir;
   while ((p>=0) && (p<size) && use[p]) {
      Double_t zn = bins[p]-backgr[p];
      if (zn<value) return kTRUE;
      p+=dir;
   }
   return kFALSE;
}

void TGo4FitPeakFinder::SergeyLinevPeakFinder(TGo4Fitter* fitter,
                                              TGo4FitData* data,
                                              Int_t PolOrder,
                                              Double_t AmplThreshold,
                                              Double_t MinWidth,
                                              Double_t MaxWidth) {
   if ((fitter==0) || (data==0)) return;

   TGo4FitDataIter* iter = data->MakeIter();
   if (iter==0) return;

   Int_t size = iter->CountPoints();

   if ((size<10) || (iter->ScalesSize()!=1))  { delete iter; return; }

   TArrayD Bins(size), Scales(size), Weights(size), Background(size);

   Weights.Reset(1.); Background.Reset(0.);

   Double_t* bins = Bins.GetArray();
   Double_t* scales = Scales.GetArray();
   Double_t* weights = Weights.GetArray();
   Double_t* backgr = Background.GetArray();

   Int_t nbin = 0;
   if (iter->Reset()) do {
      bins[nbin] = data->GetAmplValue() * iter->Value();
      if (iter->StandardDeviation()>0) weights[nbin] = 1./iter->StandardDeviation();
      scales[nbin] = iter->x();
      nbin++;
   } while (iter->Next());

   delete iter;

   if ((PolOrder>=0) && (PolOrder<(size+1)*5)) {
      Int_t nsegments = (PolOrder+1)*2;
      TArrayC usage(size); usage.Reset(0);
      for (Int_t nseg=0; nseg<nsegments; nseg++) {
         Int_t lbound = Int_t((nseg+0.)/nsegments*size);
         Int_t rbound = Int_t((nseg+1.)/nsegments*size-1.);
         Int_t nselect = (rbound-lbound)/10;
         if (nselect<1) nselect = 1;
         Int_t nsel = 0;
         while (nsel++<nselect) {
            Int_t pmin=-1;
            for(Int_t i=lbound;i<=rbound;i++)
              if(!usage[i])
                if ((pmin<0) || (bins[i]<bins[pmin])) pmin = i;
            usage[pmin] = 1;
         }
      }

      TArrayD Coef(PolOrder+1);
      DefinePolynom(size, bins, scales, Coef, weights, 0, usage.GetArray());

      for(Int_t i=0;i<size;i++)
         backgr[i] = CalcPolynom(Coef, scales[i]);

      fitter->AddPolynomX(data->GetName(), "Pol", Coef);
   }

   Double_t max=0.;
   for(Int_t i=0;i<size;i++) {
      Double_t zn = bins[i]-backgr[i];
      if(zn>max) max = zn;
   }
   AmplThreshold = AmplThreshold*max;

   TArrayC usage(size); usage.Reset(1);
   Char_t* use = usage.GetArray();

   Bool_t validline = kFALSE;

   do {

   validline = kFALSE;

   Int_t pmax=-1; Double_t max=0.;
   for(Int_t i=0;i<size;i++)
     if (use[i]) {
       Double_t zn = bins[i]-backgr[i];
       if((pmax<0) || (zn>max)) { pmax = i; max = zn; }
     }

   Double_t sigma = TMath::Sqrt(1./weights[pmax]);

   if ((max<2.*sigma) || (max<AmplThreshold)) break;

   Int_t lbound = pmax, rbound = pmax;

   Bool_t lok = FindValue(bins, backgr, use, size, lbound, -1, max*0.5);
   Bool_t rok = FindValue(bins, backgr, use, size, rbound, +1, max*0.5);

   if(lok || rok) {

     validline = kTRUE;

     Int_t w = (lok && rok) ? (rbound-lbound)/4 : ( lok ? (pmax-lbound)/2 : (rbound-pmax)/2 ) ;
     if (w<2) w=2;

     TArrayD Coef1(3), Coef2(2);
     DefinePolynomEx(size, bins, scales, weights, backgr, pmax-w, pmax+w, Coef1);
     Double_t middle = -0.5*Coef1[1]/Coef1[2];
     Double_t amplitude = CalcPolynom(Coef1, middle);

     Double_t left = 0, right = 0;
     if (lok) {
       DefinePolynomEx(size, bins, scales, weights, backgr, lbound-w, lbound+w, Coef2);
       left = (amplitude*.5-Coef2[0])/Coef2[1];
     }

     if (rok) {
       DefinePolynomEx(size, bins, scales, weights, backgr, rbound-w, rbound+w, Coef2);
       right = (amplitude*.5-Coef2[0])/Coef2[1];
     }

     if (!lok) left = 2*middle-right; else
     if (!rok) right = 2*middle-left;

     Double_t width = (right-left)*0.5;

     lbound = pmax-4*w; if (lbound<0) lbound=0;
     rbound = pmax+4*w; if (rbound>=size) rbound=size-1;
     for(Int_t i=lbound;i<=rbound;i++) use[i]=0;

     if(lok && rok && (width>=MinWidth) && (width<=MaxWidth) && (amplitude>AmplThreshold) && (amplitude>2*sigma))
        fitter->AddGauss1(data->GetName(), fitter->FindNextName("Gauss",0), middle, width, amplitude);
   }

   } while(validline);
}

#include "f_find_peaks.c"

void TGo4FitPeakFinder::HansEsselPeakFinder(TGo4Fitter* fitter,
                                            TGo4FitData* data,
                                            Int_t MaxNumPeaks,
                                            Int_t ChannelSum,
                                            Double_t NoiseFactor,
                                            Double_t NoiseMinimum,
                                            Int_t MinimasOrder) {
   if ((fitter==0) || (data==0) || (MaxNumPeaks<1)) return;

   TGo4FitDataIter* iter = data->MakeIter();
   if (iter==0) return;

   Int_t size = iter->CountPoints();

   if ((size<10) || (iter->ScalesSize()!=1))  { delete iter; return; }

   TArrayD Bins(size), Scales(size);
   Double_t* bins = Bins.GetArray();
   Double_t* scales = Scales.GetArray();

   Int_t nbin = 0;
   if (iter->Reset()) do {
      bins[nbin] = data->GetAmplValue() * iter->Value();
      scales[nbin] = iter->x();
      nbin++;
   } while (iter->Next());

   delete iter;


   int NumPeaks = 0;
   TArrayI Minimas(MaxNumPeaks), Maximas(MaxNumPeaks), MaximaWidths(MaxNumPeaks);
   TArrayI PeaksLeft(MaxNumPeaks), PeaksRight(MaxNumPeaks), PeaksMaximum(MaxNumPeaks);
   TArrayD MinimaValues(MaxNumPeaks), MinimaScales(MaxNumPeaks), MaximaIntegrals(MaxNumPeaks);

   go4fit_find_peaks(bins,
                TYPE__DOUBLE,
                0,
                size,
                ChannelSum,
                NoiseFactor,
                NoiseMinimum,
                0,
                MaxNumPeaks,
                &NumPeaks,
                Minimas.GetArray(),
                MinimaValues.GetArray(),
                Maximas.GetArray(),
                MaximaWidths.GetArray(),
                MaximaIntegrals.GetArray(),
                PeaksLeft.GetArray(),
                PeaksMaximum.GetArray(),
                PeaksRight.GetArray(),
                0,
                0);

   if (NumPeaks<=0) return;

   Int_t NumMinimas = (NumPeaks<MaxNumPeaks) ? NumPeaks+1 : MaxNumPeaks;
   for(int n=0; n<NumMinimas; n++) {
      MinimaScales[n] = scales[Minimas[n]];
   }

   if ((MinimasOrder>=0) && (NumMinimas>1)) {
     Int_t order = MinimasOrder;
     if (order>=NumMinimas) order = NumMinimas-1;
     TArrayD Coef(order+1);
     DefinePolynom(NumMinimas, MinimaValues.GetArray(), MinimaScales.GetArray(), Coef);
     fitter->AddPolynomX(data->GetName(), "Pol", Coef);
   }

   for(int n=0;n<NumPeaks;n++) {
      Double_t pos = scales[PeaksMaximum[n]];
      Double_t width = 0.;
      if (PeaksRight[n]-PeaksLeft[n]<1)
        width = scales[PeaksMaximum[n]] - scales[PeaksMaximum[n]-1];
      else
        width = TMath::Abs(scales[PeaksRight[n]] - scales[PeaksLeft[n]])/2.;
      Double_t ampl = bins[PeaksMaximum[n]];

      fitter->AddGauss1(data->GetName(), fitter->FindNextName("Gauss",0), pos, width, ampl);
   }
}



