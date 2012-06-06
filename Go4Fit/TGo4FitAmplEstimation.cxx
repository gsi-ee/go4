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

#include "TGo4FitAmplEstimation.h"

#include "go4iostream.h"

#include "TMath.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TArrayC.h"
#include "TArrayD.h"

#include "TGo4Fitter.h"
#include "TGo4FitModel.h"
#include "TGo4FitData.h"
#include "TGo4FitParameter.h"

TGo4FitAmplEstimation::TGo4FitAmplEstimation() : TGo4FitterAction(), fiNumIters(1) {
}

TGo4FitAmplEstimation::TGo4FitAmplEstimation(const char* Name, Int_t NumIters) :
  TGo4FitterAction(Name,"Estimate amplitude action"), fiNumIters(NumIters) {
}

TGo4FitAmplEstimation::~TGo4FitAmplEstimation() {
}

void TGo4FitAmplEstimation::DoAction(TGo4FitterAbstract* Fitter) {
   TGo4Fitter* f = dynamic_cast<TGo4Fitter*> (Fitter);
   if(f==0) return;
   if (!CalculateWithBuffers(f))
     CalculateWithIterators(f);
}

Double_t TGo4FitAmplEstimation::PointWeight(Int_t niter, Int_t FFtype, Double_t value, Double_t modelvalue, Double_t standdev) {
   switch (FFtype) {
      case TGo4Fitter::ff_least_squares: return 1.;
      case TGo4Fitter::ff_chi_square: if (standdev<=0.) return 0.; else return 1./standdev;
      case TGo4Fitter::ff_chi_Pearson:
      case TGo4Fitter::ff_ML_Poisson:
        if (niter==0) if (value<0.) return 0.; else return 1./(value+1.);
                 else if (modelvalue<=0.) return 0.; else return 1./modelvalue;
      case TGo4Fitter::ff_chi_Neyman: if (value<1.) return 1.; else return 1./value;
      case TGo4Fitter::ff_chi_gamma: if (value<0.) return 0.; else return 1./(value+1.);
      case TGo4Fitter::ff_user: return 1.;
      default: return 1.;
   }
}


Bool_t TGo4FitAmplEstimation::CalculateWithBuffers(TGo4Fitter* fitter) {
   if (!fitter->IsInitialized()) return kFALSE;

   for(Int_t n=0;n<fitter->GetNumData();n++)
     if (!fitter->DataBuffersAllocated(fitter->GetData(n))) return kFALSE;

   for(Int_t n=0;n<fitter->GetNumModel();n++)
     if (!fitter->ModelBuffersAllocated(fitter->GetModel(n))) return kFALSE;

   TObjArray comps, fixedcomps;

   for(Int_t n=0;n<fitter->GetNumModel();n++) {
     TGo4FitModel *fModel = fitter->GetModel(n);

     Bool_t assigned = kFALSE;
     for (Int_t na=0;na<fModel->NumAssigments();na++)
       if (fitter->FindData(fModel->AssignmentName(na))) {
          assigned = kTRUE;
          break;
       }
     if (!assigned) continue;

     if ( (fModel->GetAmplPar()!=0) &&
          (!fModel->GetAmplPar()->GetFixed()) ) comps.Add(fModel);
                                           else fixedcomps.Add(fModel);
   }

   Int_t ncomp = comps.GetLast()+1;
   if (ncomp<=0) return kFALSE;

   TMatrixD matr(ncomp,ncomp);
   TVectorD vector(ncomp);

   Double_t** DopData = new Double_t* [fitter->GetNumData()];
   for(Int_t n=0;n<fitter->GetNumData();n++)
      DopData[n] = new Double_t[fitter->GetDataBinsSize(fitter->GetData(n))];

   Double_t** Weights = new Double_t* [fitter->GetNumData()];
   for(Int_t n=0;n<fitter->GetNumData();n++)
      Weights[n] = new Double_t[fitter->GetDataBinsSize(fitter->GetData(n))];


   Int_t niter = 0;
   Int_t FFtype = fitter->GetFitFunctionType();
   Bool_t changed = kFALSE;

   do {

       fitter->RebuildAll();

       for(Int_t ndata=0;ndata<fitter->GetNumData();ndata++) {
           TGo4FitData* data = fitter->GetData(ndata);

           Int_t size = fitter->GetDataBinsSize(data);
           Double_t ampl = data->GetAmplValue();

           Double_t* values = fitter->GetDataBinsValues(data);
           Double_t* result = fitter->GetDataBinsResult(data);
           Double_t* devs = fitter->GetDataBinsDevs(data);
           Double_t* weight = Weights[ndata];

          for (Int_t nbin=0;nbin<size;nbin++)
             weight[nbin] = PointWeight(niter,FFtype, ampl*values[nbin], result[nbin], ampl*ampl*devs[nbin]);
       }

       for(Int_t n1=0;n1<ncomp;n1++)
          for(Int_t n2=n1;n2<ncomp;n2++) {
             Double_t fSum = 0.;
             for(Int_t ndata=0;ndata<fitter->GetNumData();ndata++) {
                Double_t* bins1 = fitter->GetModelBinsValues((TGo4FitModel*) comps[n1], fitter->GetDataName(ndata));
                Double_t* bins2 = fitter->GetModelBinsValues((TGo4FitModel*) comps[n2], fitter->GetDataName(ndata));
                if ((bins1!=0) && (bins2!=0)) {
                   Int_t size = fitter->GetDataBinsSize(fitter->GetData(ndata));
                   Double_t* weight = Weights[ndata];
                   for(Int_t nbin=0;nbin<size;nbin++)
                     fSum+=bins1[nbin]*bins2[nbin]*weight[nbin];
                }
             }
             matr(n1,n2) = fSum;
             matr(n2,n1) = fSum;
          }

        // caluclating difference between data and fixed components
        for(Int_t ndata=0;ndata<fitter->GetNumData();ndata++) {
           TGo4FitData* data = fitter->GetData(ndata);
           Int_t size = fitter->GetDataBinsSize(data);
           Double_t dampl = data->GetAmplValue();
           Double_t* values = fitter->GetDataBinsValues(data);
           Double_t* bins = DopData[ndata];

           for(Int_t nbin=0;nbin<size;nbin++)
             bins[nbin] = dampl*values[nbin];

           for(Int_t n=0;n<=fixedcomps.GetLast();n++) {
              TGo4FitModel *model = (TGo4FitModel*) fixedcomps[n];
              Double_t* mbins = fitter->GetModelBinsValues(model,data->GetName());
              if (mbins!=0) {
                 Double_t mampl = model->GetAmplValue();
                 for(Int_t nbin=0;nbin<size;nbin++)
                   bins[nbin] -= mampl*mbins[nbin];
              }
           }
        }

        // filling vector

        for(Int_t n=0;n<ncomp;n++) {
           Double_t fSum = 0.;
           for(Int_t ndata=0;ndata<fitter->GetNumData();ndata++) {
               Double_t* bins1 = fitter->GetModelBinsValues((TGo4FitModel*) comps[n], fitter->GetDataName(ndata));
               if (bins1==0) continue;
               Int_t size = fitter->GetDataBinsSize(fitter->GetData(ndata));
               Double_t* bins = DopData[ndata];
               Double_t* weight = Weights[ndata];
               for(Int_t nbin=0;nbin<size;nbin++)
                  fSum+=bins[nbin]*bins1[nbin]*weight[nbin];

           }
           vector(n) = fSum;
        }

        if (matr.Determinant()==0.) {
           cout << "  Amplitude estimation algorithm failed. " << endl;
           cout << "  Determinant of matrice == 0.; This may be due to equiavalent model components or zero model at all" << endl;
           break;
        }

        matr.Invert();
        vector*=matr;

        for(Int_t n=0;n<ncomp;n++) {
           TGo4FitModel *model = (TGo4FitModel*) comps[n];
           model->SetAmplValue(vector(n));
           if (matr(n,n)>0.)
             model->SetAmplError(TMath::Sqrt(matr(n,n)));
        }

        changed = kFALSE;
        niter++;
        if (niter<fiNumIters)
          if ((FFtype==TGo4Fitter::ff_chi_Pearson) ||
              (FFtype==TGo4Fitter::ff_ML_Poisson)) changed=kTRUE;

    } while (changed);

   for(Int_t n=0;n<fitter->GetNumData();n++) delete[] Weights[n];
   delete[] Weights;

   for(Int_t n=0;n<fitter->GetNumData();n++) delete[] DopData[n];
   delete[] DopData;

   return kTRUE;
}


Bool_t TGo4FitAmplEstimation::CalculateWithIterators(TGo4Fitter* fitter) {
   if (fitter==0) return kFALSE;

   Int_t nmodel = fitter->GetNumModel();
   TArrayI refindex(fitter->GetNumModel()); refindex.Reset(-1);
   TArrayI fixedindex(fitter->GetNumModel()); fixedindex.Reset(-1);

   Int_t ncomp = 0;
   Int_t nfixed = 0;

   for(Int_t n=0;n<nmodel;n++) {
     TGo4FitModel *model = fitter->GetModel(n);
     Bool_t assigned = kFALSE;
     for (Int_t na=0;na<model->NumAssigments();na++)
       if (fitter->FindData(model->AssignmentName(na))) {
          assigned = kTRUE;
          break;
       }
     if (!assigned) continue;

     if ( (model->GetAmplPar()!=0) &&
          (!model->GetAmplPar()->GetFixed()) ) refindex[ncomp++] = n;
                                          else fixedindex[nfixed++] = n;
   }

   if (ncomp<=0) return kFALSE;

   TMatrixD matr(ncomp,ncomp);
   TVectorD vector(ncomp);

   TArrayC Usage(nmodel);
   TArrayD ModelValues(nmodel);
   TArrayD ModelAmpls(nmodel);

   Int_t niter = 0;
   Int_t FFtype = fitter->GetFitFunctionType();
   Bool_t changed = kFALSE;

   do {
       matr = 0.; vector = 0.;

       for(Int_t ndata=0;ndata<fitter->GetNumData();ndata++) {
          TGo4FitData* data = fitter->GetData(ndata);
          TGo4FitDataIter* iter = data->MakeIter();
          if (iter==0) return kFALSE;

          Double_t dampl = data->GetAmplValue();

          if (iter->Reset()) {
             Usage.Reset(0); ModelAmpls.Reset(0.); ModelValues.Reset(0.);
             for(Int_t n=0;n<nmodel;n++) {
                 TGo4FitModel* model = fitter->GetModel(n);
                 if (model->IsAssignTo(data->GetName())) {
                   Usage[n] = 1;
                   ModelAmpls[n] = model->GetAmplValue() * model->GetRatioValueFor(data->GetName());
                   model->BeforeEval(iter->IndexesSize());
                 }
             }

             do {
               Double_t result = 0.;
               for (Int_t n=0; n<nmodel;n++)
                 if (Usage[n]) {
                   ModelValues[n] = fitter->GetModel(n)->EvaluateAtPoint(iter);
                   result += ModelAmpls[n] * ModelValues[n];
                 }
               Double_t weight = PointWeight(niter, FFtype, dampl*iter->Value(), result, dampl*dampl*iter->StandardDeviation());
               if (weight<=0.) continue;

               for (Int_t i1=0;i1<ncomp;i1++)
                 for(Int_t i2=i1;i2<ncomp;i2++) {
                    Double_t zn = matr(i1,i2);
                    zn += ModelValues[refindex[i1]] * ModelValues[refindex[i2]] * weight;
                    matr(i1,i2) = zn;
                    if (i1!=i2) matr(i2,i1) = zn;
                 }

               Double_t dvalue = dampl*iter->Value();
               for(Int_t nf=0;nf<nfixed;nf++)
                 dvalue -= ModelAmpls[fixedindex[nf]] * ModelValues[fixedindex[nf]];

               for (Int_t i1=0;i1<ncomp;i1++) {
                 Double_t zn = vector(i1);
                 zn += ModelValues[refindex[i1]] * dvalue * weight;
                 vector(i1) = zn;
               }

             } while (iter->Next());

             for(Int_t n=0;n<nmodel;n++)
               if (Usage[n]) fitter->GetModel(n)->AfterEval();
          }
          delete iter;
       }

       if (matr.Determinant()==0.) {
           cout << "  Amplitude estimation algorithm failed. " << endl;
           cout << "  Determinant of matrice == 0.; This may be due to equiavalent model components or zero model at all" << endl;
           return kFALSE;
        }

        matr.Invert();
        vector*=matr;

        for(Int_t n=0;n<ncomp;n++) {
           TGo4FitModel *model = fitter->GetModel(refindex[n]);
           model->SetAmplValue(vector(n));
           if (matr(n,n)>0.)
             model->SetAmplError(TMath::Sqrt(matr(n,n)));
        }

        changed = kFALSE;
        niter++;
        if (niter<fiNumIters)
          if ((FFtype==TGo4Fitter::ff_chi_Pearson) ||
              (FFtype==TGo4Fitter::ff_ML_Poisson)) changed=kTRUE;

    } while (changed);

   return kTRUE;
}

void TGo4FitAmplEstimation::Print(Option_t* option) const {
   TGo4FitterAction::Print(option);
   cout << "  number of iterations " << fiNumIters << endl;
}
