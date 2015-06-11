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

#include "TGo4Fitter.h"

#include <stdlib.h>

#include "Riostream.h"
#include "TClass.h"
#include "TMath.h"
#include "TH1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "THStack.h"
#include "TArrayD.h"

#include "TGo4FitData.h"
#include "TGo4FitDataHistogram.h"
#include "TGo4FitDataGraph.h"
#include "TGo4FitModel.h"
#include "TGo4FitModelGauss1.h"
#include "TGo4FitModelGauss2.h"
#include "TGo4FitModelPolynom.h"
#include "TGo4FitAmplEstimation.h"
#include "TGo4FitSlot.h"
#include "TGo4FitMinuit.h"

TGo4Fitter::TGo4Fitter() :
   TGo4FitterAbstract(),
   fxDatas(), fxModels(), fiFitFunctionType(0), fiMemoryUsage(0),
   fxUserFitFunction(0), fxDrawObjs(0)
{
}

TGo4Fitter::TGo4Fitter(const char* iName, const char* iTitle) :
   TGo4FitterAbstract(iName, iTitle),
   fxDatas(), fxModels(), fiFitFunctionType(0), fiMemoryUsage(0), fxUserFitFunction(0), fxDrawObjs(0)
{
   fxDatas.SetOwner(kTRUE);
   fxModels.SetOwner(kTRUE);
}

TGo4Fitter::TGo4Fitter(const char* iName, Int_t iFitFunctionType, Bool_t IsAddStandardActions) :
   TGo4FitterAbstract(iName,"TGo4Fitter object"),
   fxDatas(), fxModels(), fiFitFunctionType(0), fiMemoryUsage(100), fxUserFitFunction(0), fxDrawObjs(0)
{
   fxDatas.SetOwner(kTRUE);
   fxModels.SetOwner(kTRUE);
   SetFitFunctionType(iFitFunctionType);
   if (IsAddStandardActions) AddStandardActions();
}

TGo4Fitter::~TGo4Fitter()
{
   CheckDuplicatesOnSlot();
   MoveDrawObjectsToROOT();
}

void TGo4Fitter::SetMemoryUsage(Int_t iMemoryUsage)
{
  if (iMemoryUsage<0) fiMemoryUsage=0; else
    if (iMemoryUsage>100) fiMemoryUsage=100; else
      fiMemoryUsage = iMemoryUsage;
}

void TGo4Fitter::CollectAllPars()
{
   TGo4FitterAbstract::CollectAllPars();

   for(Int_t ndata=0;ndata<GetNumData();ndata++)
     GetData(ndata)->CollectParsTo(*this);

   for(Int_t nmodel=0;nmodel<GetNumModel();nmodel++) {
     TGo4FitModel* model = GetModel(nmodel);
     for (Int_t n=0;n<model->NumAssigments();n++)
       if (FindData(model->AssignmentName(n))) {
          model->CollectParsTo(*this);
          break;
       }
   }
}

void TGo4Fitter::Clear(Option_t* option)
{
   TGo4FitterAbstract::Clear(option);
   DeleteAllData();
   DeleteAllModels();
}

TGo4FitData* TGo4Fitter::GetData(Int_t n)
{
   return (n>=0) && (n<GetNumData()) ? dynamic_cast<TGo4FitData*> (fxDatas[n]) : 0;
}

const char* TGo4Fitter::GetDataName(Int_t n)
{
   return GetData(n) ? GetData(n)->GetName() : 0;
}

TGo4FitData* TGo4Fitter::FindData(const char* DataName)
{
   return (DataName==0) ? 0 : dynamic_cast<TGo4FitData*> (fxDatas.FindObject(DataName));
}

TGo4FitData* TGo4Fitter::AddData(TGo4FitData* data)
{
   fxDatas.Add(data);
   SetParsListChange();
   SetUpdateSlotList();
   return data;
}

TGo4FitDataHistogram* TGo4Fitter::AddH1(const char* DataName, TH1* histo, Bool_t Owned, Double_t lrange, Double_t rrange)
{
   TGo4FitDataHistogram *data = new TGo4FitDataHistogram(DataName, histo, Owned);
   if ((lrange<rrange) || (rrange!=0.)) data->SetRange(0,lrange,rrange);
   AddData(data);
   return data;
}

TGo4FitDataHistogram* TGo4Fitter::SetH1(const char* DataName, TH1* histo, Bool_t Owned)
{
   TGo4FitDataHistogram* data = dynamic_cast<TGo4FitDataHistogram*> (FindData(DataName));
   if (data!=0) data->SetHistogram(histo, Owned);
   return data;
}


TGo4FitDataGraph* TGo4Fitter::AddGraph(const char* DataName, TGraph* gr, Bool_t Owned, Double_t lrange, Double_t rrange)
{
   TGo4FitDataGraph *data = new TGo4FitDataGraph(DataName, gr, Owned);
   if ((lrange<rrange) || (rrange!=0.)) data->SetRange(0,lrange,rrange);
   AddData(data);
   return data;
}

TGo4FitDataGraph* TGo4Fitter::SetGraph(const char* DataName, TGraph* gr, Bool_t Owned)
{
   TGo4FitDataGraph *data = dynamic_cast<TGo4FitDataGraph*> (FindData(DataName));
   if (data!=0) data->SetGraph(gr, Owned);
   return data;
}

void TGo4Fitter::CheckSlotsBeforeDelete(TGo4FitComponent* comp)
{
  if (comp)
    for (Int_t n=0;n<comp->NumSlots();n++) {
       TGo4FitSlot* slot = comp->GetSlot(n);
       ClearSlot(slot, kFALSE);
       for(Int_t n2=0;n2<NumSlots();n2++) {
         TGo4FitSlot* slot2 = GetSlot(n2);
         if (slot2->GetConnectedSlot()==slot)
           slot2->ClearConnectionToSlot();
       }

    }
}

TGo4FitData* TGo4Fitter::RemoveData(const char* DataName, Bool_t IsDel)
{
   TGo4FitData* dat = FindData(DataName);
   if (dat) {
      SetParsListChange();
      SetUpdateSlotList();
      fxDatas.Remove(dat);
      if(IsDel) { CheckSlotsBeforeDelete(dat); delete dat; dat = 0; }
      fxDatas.Compress();
   }
   return dat;
}

void TGo4Fitter::DeleteAllData()
{
   fxDatas.Delete();
   fxDatas.Compress();
   SetParsListChange();
   SetUpdateSlotList();
}

TGo4FitModel* TGo4Fitter::GetModel(Int_t n)
{
   return (n>=0) && (n<GetNumModel()) ? dynamic_cast<TGo4FitModel*> (fxModels[n]) : 0;
}

TGo4FitModel* TGo4Fitter::FindModel(const char* ModelName)
{
   return (ModelName==0) ? 0 : dynamic_cast<TGo4FitModel*> (fxModels.FindObject(ModelName));
}

TGo4FitModel* TGo4Fitter::AddModel(TGo4FitModel* model)
{
   fxModels.Add(model);
   SetParsListChange();
   SetUpdateSlotList();
   return model;
}

TGo4FitModel* TGo4Fitter::AddModel(const char* DataName, TGo4FitModel* model)
{
   model->AssignToData(DataName);
   fxModels.Add(model);
   SetParsListChange();
   SetUpdateSlotList();
   return model;
}

TGo4FitComponent* TGo4Fitter::GetComp(Int_t n)
{
   if(n<GetNumData()) return GetData(n); else return GetModel(n-GetNumData());
}

void TGo4Fitter::AddPolynomX(const char* DataName, const char* NamePrefix, Int_t MaxOrder, Int_t GroupIndex, Double_t lrange, Double_t rrange)
{
   if (DataName==0) return;

   Bool_t flag = kFALSE;
   Int_t NumTry = 0;
   Bool_t createmodel = kFALSE;

   do {
       TString Prefix(NamePrefix);
       if (NumTry>0) Prefix+=NumTry;
       flag = kFALSE;
       Bool_t findsame = kFALSE;

       for(Int_t Order=0; Order<=MaxOrder; Order++) {
         TString Name(Prefix);
         Name += "_";
         Name += Order;
         findsame = FindModel(Name.Data());
         if (findsame && !createmodel) break;

         if (createmodel) {
           TGo4FitModelPolynom* comp = new TGo4FitModelPolynom(Name, Order);
           comp->SetGroupIndex(GroupIndex);
           if ((lrange<rrange) || (rrange!=0.)) comp->SetRange(0,lrange,rrange);
           AddModel(DataName, comp);
         }
       }

       flag = kTRUE;
       if (findsame) { NumTry++; createmodel = kFALSE; } else
         if (createmodel) flag = kFALSE;
                     else createmodel = kTRUE;

   } while (flag);
}

void TGo4Fitter::AddPolynomX(const char* DataName, const char* NamePrefix, TArrayD& Coef, Int_t GroupIndex)
{
   if (DataName==0) return;

   Bool_t flag = kFALSE;
   Int_t NumTry = 0;
   Bool_t createmodel = kFALSE;

   do {
       Bool_t findsame = kFALSE;

       for (Int_t n=0;n<Coef.GetSize();n++) {
          TString Name(NamePrefix);
          if (NumTry>0) Name+=NumTry;
          Name+="_";
          Name+=n;
          findsame = FindModel(Name.Data());
          if (findsame && !createmodel) break;

          if (createmodel) {
             TGo4FitModelPolynom* model = new TGo4FitModelPolynom(Name, n);
             model->SetAmplValue(Coef[n]);
             model->SetGroupIndex(GroupIndex);
             AddModel(DataName, model);
          }
       }

       flag = kTRUE;
       if (findsame) { NumTry++; createmodel = kFALSE; } else
         if (createmodel) flag = kFALSE;
                     else createmodel = kTRUE;

   } while(flag);
}

void TGo4Fitter::AddPolynoms(const char* DataName, const char* NamePrefix, Int_t MaxOrder, Int_t NumAxis, Int_t GroupIndex)
{
   if (DataName==0) return;
   TArrayD Orders(NumAxis);
   Orders.Reset(0.);

   Bool_t flag = kFALSE;
   Int_t NumTry = 0;
   Bool_t createmodel = kFALSE;

   do {

       TString Prefix(NamePrefix);
       if (NumTry>0) Prefix+=NumTry;
       flag = kFALSE;
       Bool_t findsame = kFALSE;

       do {
         TString Name(Prefix);
         for(Int_t n=0; n<NumAxis; n++) {
            Name+="_";
            Name+=Int_t(Orders[n]);
         }
         findsame = FindModel(Name.Data());
         if (findsame && !createmodel) break;

         if (createmodel) {
           TGo4FitModelPolynom* comp = new TGo4FitModelPolynom(Name, Orders);
           comp->SetGroupIndex(GroupIndex);
           AddModel(DataName, comp);
         }

         Int_t nn = 0;
         do {
           Orders[nn] += 1.;
           if (Orders[nn]<=MaxOrder) break;
           Orders[nn]=0;
           nn++;
         } while (nn<NumAxis);
         flag = (nn<NumAxis);
       } while (flag);

       flag = kTRUE;
       if (findsame) { NumTry++; createmodel = kFALSE; } else
         if (createmodel) flag = kFALSE;
                     else createmodel = kTRUE;

   } while (flag);
}

TGo4FitModelGauss1* TGo4Fitter::AddGauss1(const char* DataName, const char* ModelName, Double_t iPosition, Double_t iWidth, Double_t iAmpl, Int_t Axis)
{
   TGo4FitModelGauss1* gauss = new TGo4FitModelGauss1(ModelName, iPosition, iWidth, Axis);
   gauss->SetAmplValue(iAmpl);
   AddModel(DataName, gauss);
   return gauss;
}

TGo4FitModel* TGo4Fitter::CloneModel(const char* ModelName, const char* NewName)
{
   TGo4FitModel* mod = FindModel(ModelName);
   if (mod==0) return 0;

   TString newname;
   int cnt = 0;

   do {
      newname = NewName ? NewName : mod->GetName();
      if (cnt>0) {
         newname += "_";
         newname += cnt;
      }
      cnt++;
   } while (FindModel(newname.Data()));

   TGo4FitModel* newmod = (TGo4FitModel*) mod->Clone(newname.Data());

   return AddModel(newmod);
}

TGo4FitModel* TGo4Fitter::RemoveModel(const char * ModelName, Bool_t IsDel)
{
   TGo4FitModel* mod = FindModel(ModelName);
   if (mod) {
      SetParsListChange();
      SetUpdateSlotList();
      fxModels.Remove(mod);
      if(IsDel) { CheckSlotsBeforeDelete(mod); delete mod; mod = 0; }
      fxModels.Compress();
   }
   return mod;
}

Int_t TGo4Fitter::NumModelsAssosiatedTo(const char* DataName)
{
   Int_t res = 0;
   for (Int_t n=0;n<GetNumModel();n++)
      if (GetModel(n)->IsAssignTo(DataName)) res++;
   return res;
}

void TGo4Fitter::DeleteModelsAssosiatedTo(const char* DataName)
{
   Int_t n=0;
   while (n<GetNumModel()) {
      TGo4FitModel* model = GetModel(n++);
      if (model->IsAssignTo(DataName)){
        if (model->NumAssigments()==1) {
           SetParsListChange();
           SetUpdateSlotList();
           fxModels.Remove(model);
           delete model;
           fxModels.Compress();
           n--;
        } else model->ClearAssignmentTo(DataName);
      }
   }
}

void TGo4Fitter::DeleteAllModels()
{
   fxModels.Delete();
   fxModels.Compress();
   SetParsListChange();
   SetUpdateSlotList();
}

void TGo4Fitter::AssignModelTo(const char* ModelName, const char* DataName, Double_t RatioValue, Bool_t FixRatio)
{
  TGo4FitModel* model = FindModel(ModelName);
  if (model) {
     if (DataName!=0)
        model->AssignToData(DataName, RatioValue, FixRatio);
     else {
        model->ClearAssignments();
        for (int n=0; n<GetNumData(); n++)
           model->AssignToData(GetData(n)->GetName(), RatioValue, FixRatio);
     }
     SetParsListChange();
  }
}

void TGo4Fitter::ClearModelAssignmentTo(const char* ModelName, const char* DataName)
{
   TGo4FitModel* model = FindModel(ModelName);
   if (model==0) return;
   if (DataName!=0) model->ClearAssignmentTo(DataName);
               else model->ClearAssignments();
   SetParsListChange();
}

void TGo4Fitter::ChangeDataNameInAssignments(const char* oldname, const char* newname)
{
   for(Int_t n=0;n<GetNumModel();n++)
     GetModel(n)->ChangeDataNameInAssignments(oldname,newname);
}

Bool_t TGo4Fitter::InitFitterData()
{
   Int_t dbuf = -1, mbuf = -1;
   switch (GetMemoryUsage()) {
      case 0: dbuf = 0; mbuf = 0; break;
      case 1: dbuf = 1; mbuf = 0; break;
      case 2: dbuf = 1; mbuf = 1; break;
      default: dbuf = -1; mbuf = -1;
   }

   for(Int_t i1=0;i1<GetNumData();i1++) {
      TGo4FitData *data = GetData(i1);
      if (!data->Initialize(dbuf)) return kFALSE;
      for(Int_t i2=0;i2<GetNumModel();i2++)
        GetModel(i2)->ConnectToDataIfAssigned(data);
   }

   for(Int_t i2=0;i2<GetNumModel();i2++) {
      TGo4FitModel *fModel = GetModel(i2);
      if (!fModel->Initialize(mbuf)) return kFALSE;
   }

   if( (fiFitFunctionType == ff_user) && (fxUserFitFunction==0) ) {
      std::cout << " User fit function not set. Switch to least squares " << std::endl;
      fiFitFunctionType = ff_least_squares;
   }

   return kTRUE;
}

void TGo4Fitter::FinalizeFitterData()
{
   for(Int_t i=0;i<GetNumData();i++) GetData(i)->Finalize();

   for(Int_t i=0;i<GetNumModel();i++) GetModel(i)->Finalize();
}

Double_t TGo4Fitter::PointFitFunction(Int_t FitFunctionType, Double_t value, Double_t modelvalue, Double_t standdev)
{
   switch (FitFunctionType) {
      case ff_least_squares: {
        Double_t zn1 = (value-modelvalue);
        return zn1*zn1; }
      case ff_chi_square: {
        if (standdev<=0.) return 0.;
        Double_t zn2 = (value-modelvalue);
        return zn2*zn2/standdev; }
      case ff_chi_Pearson: {
        if (modelvalue<=0.) return 0.;
        Double_t zn3 = (value-modelvalue);
        return zn3*zn3/modelvalue; }
      case ff_chi_Neyman: {
        Double_t zn4 = (value-modelvalue);
        return zn4*zn4/((value<1.) ? 1. : value); }
      case ff_chi_gamma: {
        if (value<0.) return 0.;
        Double_t zn5 = (value+((value<1.) ? 0. : 1.)-modelvalue);
        return zn5*zn5/(value+1.); }
      case ff_ML_Poisson:
        if (modelvalue<=0.) return 0.;
        return modelvalue - value*TMath::Log(modelvalue);
      case ff_user:
        if (fxUserFitFunction==0) return 0;
        return fxUserFitFunction(value, modelvalue, standdev);
      default:
        return (value-modelvalue)*(value-modelvalue);
   }
}

Double_t TGo4Fitter::CalculateFCN(Int_t FitFunctionType, TGo4FitData* selectdata)
{
  if (GetMemoryUsage()>0)  RebuildAll();

  Double_t fSum = 0.;

  for(Int_t n=0;n<GetNumData();n++) {
    TGo4FitData* dat = GetData(n);
    if (selectdata && (dat!=selectdata)) continue;
    Double_t DataAmpl = dat->GetAmplValue();

    if (dat->BuffersAllocated()) {
       Int_t size = dat->GetBinsSize();
       Double_t* values = dat->GetBinsValues();
       Double_t* res = dat->GetBinsResult();
       Double_t* devs = dat->GetBinsDevs();

       for(Int_t nbin=0;nbin<size;nbin++)
         fSum += PointFitFunction(FitFunctionType, DataAmpl*values[nbin], res[nbin], DataAmpl*DataAmpl*devs[nbin] );
    } else {

       TGo4FitDataIter* iter = dat->MakeIter();
       if (!iter->Reset()) { delete iter; continue; }

       TObjArray Models;

       TArrayD Ampls(GetNumModel());
       for(Int_t nm=0;nm<GetNumModel();nm++)
         if (GetModel(nm)->IsAssignTo(dat->GetName())) {
           TGo4FitModel* model = GetModel(nm);
           Models.Add(model);
           model->BeforeEval(iter->ScalesSize());
           Ampls[Models.GetLast()] = model->GetAmplValue() * model->GetRatioValueFor(dat->GetName());
         }

       do {
          Double_t value = DataAmpl * iter->Value();
          Double_t deviat = DataAmpl * DataAmpl * iter->StandardDeviation();

          Double_t modelvalue = 0.;
          for(Int_t nm=0;nm<=Models.GetLast();nm++) {
             TGo4FitModel* model = dynamic_cast<TGo4FitModel*> (Models.At(nm));
             modelvalue += Ampls[nm] * model->EvaluateAtPoint(iter);
           }

          fSum += PointFitFunction(FitFunctionType, value, modelvalue, deviat);

       } while (iter->Next());

       for(Int_t nm=0;nm<=Models.GetLast();nm++)
          ((TGo4FitModel*) Models.At(nm))->AfterEval();

       delete iter;
    }
  }

  return fSum;
}

Double_t TGo4Fitter::CalculateFitFunction(Double_t* pars, Int_t FitFunctionType, const char* DataName)
{

   if (FitFunctionType<0) FitFunctionType = GetFitFunctionType();

   if (pars) {
     ExecuteDependencies(pars);
     SetParsValues(pars);
   }

   return CalculateFCN(FitFunctionType, FindData(DataName));
}

Int_t TGo4Fitter::CalculateNDF(const char* DataName)
{
   TGo4FitData* selectdata = FindData(DataName);

   Int_t NDF = 0;

   if (selectdata!=0) {
     NDF = selectdata->DefineBinsSize();
     for(Int_t nm=0;nm<GetNumModel();nm++) {
        TGo4FitModel* model = GetModel(nm);
        if (model->IsAssignTo(selectdata->GetName()))
        NDF -= model->NumFreePars();
     }
   } else {
     for (Int_t n=0;n<GetNumData();n++)
       NDF += GetData(n)->DefineBinsSize();
     NDF -= NumFreePars();
   }

   return NDF;
}

Double_t TGo4Fitter::DoCalculation()
{
   return CalculateFCN(fiFitFunctionType);
}

Int_t TGo4Fitter::DoNDFCalculation()
{
   return CalculateNDF(0);
}

void TGo4Fitter::RebuildAll(Bool_t ForceBuild)
{
   for(Int_t i2=0;i2<GetNumModel();i2++)
      GetModel(i2)->RebuildShape(ForceBuild);

   for(Int_t i1=0;i1<GetNumData();i1++) {
      TGo4FitData* data = GetData(i1);
      if (!data->BuffersAllocated()) continue;

      Int_t size = data->GetBinsSize();
      Double_t* result = data->GetBinsResult();
      for (Int_t nbin=0;nbin<size;nbin++) result[nbin] = 0.;

      for(Int_t i2=0;i2<GetNumModel();i2++) {
         TGo4FitModel *model = GetModel(i2);
         if (model->IsAssignTo(data->GetName()))
            model->AddModelToDataResult(data);
      }
   }
}

void TGo4Fitter::EstimateAmplitudes(Int_t NumIters)
{
   TGo4FitAmplEstimation abc("this",NumIters);
   abc.DoAction(this);
}

void TGo4Fitter::AddAmplEstimation(Int_t NumIters)
{
   AddAction(new TGo4FitAmplEstimation("AmplEstim",NumIters));
}

void TGo4Fitter::AddStandardActions()
{
   AddAmplEstimation();
   AddSimpleMinuit();
}

void TGo4Fitter::FillSlotList(TSeqCollection* list)
{
   TGo4FitterAbstract::FillSlotList(list);
   for(Int_t i=0;i<GetNumComp();i++)
     GetComp(i)->FillSlotList(list);
}

Bool_t TGo4Fitter::ModelBuffersAllocated(TGo4FitModel* model)
{
  return model==0 ? kFALSE : model->BuffersAllocated();
}

Bool_t TGo4Fitter::DataBuffersAllocated(TGo4FitData* data)
{
  return data==0 ? kFALSE : data->BuffersAllocated();
}

Int_t TGo4Fitter::GetDataBinsSize(TGo4FitData* data)
{
   return (data==0) ? 0 : data->GetBinsSize();
}

Double_t* TGo4Fitter::GetDataBinsValues(TGo4FitData* data)
{
   return (data==0) ? 0 : data->GetBinsValues();
}

Double_t* TGo4Fitter::GetDataBinsDevs(TGo4FitData* data)
{
   return (data==0) ? 0 : data->GetBinsDevs();
}

Double_t* TGo4Fitter::GetDataBinsResult(TGo4FitData* data)
{
   return (data==0) ? 0 : data->GetBinsResult();
}

Double_t* TGo4Fitter::GetModelBinsValues(TGo4FitModel* model, const char* DataName)
{
   return (model==0) ? 0 : model->GetModelBins(DataName);
}

void TGo4Fitter::Print(Option_t* option) const
{
   TString Opt(option);
   if (Opt=="Ampls") { PrintAmpls(); return; } else
   if (Opt=="Pars") { PrintPars(); return; } else
   if (Opt=="Results") { PrintResults(); return; } else
   if (Opt=="Lines") { PrintLines(); return; }

   TGo4FitterAbstract::Print(option);
   std::cout << "Fitiing function type: ";
   switch (fiFitFunctionType) {
     case ff_chi_square   : std::cout << "ff_chi_square" << std::endl; break;
     case ff_chi_Pearson  : std::cout << "ff_chi_Pearson" << std::endl; break;
     case ff_chi_Neyman   : std::cout << "ff_chi_Neyman" << std::endl; break;
     case ff_chi_gamma    : std::cout << "ff_chi_gamma" << std::endl; break;
     case ff_ML_Poisson   : std::cout << "ff_ML_Poisson" << std::endl; break;
     case ff_user         : std::cout << "user defined" << std::endl; break;
     default: std::cout << "ff_least_squares" << std::endl;
   }
   std::cout << std::endl << "   LIST OF DATA OBJECTS" << std::endl;
   fxDatas.Print(option);
   std::cout << std::endl << "   LIST OF MODEL OBJECTS" << std::endl;
   fxModels.Print(option);
}

Bool_t TGo4Fitter::CalculatesMomentums(const char* DataName, Bool_t UseRanges, Bool_t SubstractModels, Double_t& first, Double_t& second)
{
  TGo4FitData* data = FindData(DataName);
  if (data==0) return kFALSE;

  TGo4FitDataIter* iter = data->MakeIter();
  if (iter==0) return kFALSE;

  Int_t size = iter->CountPoints(UseRanges);
  if (size==0) { delete iter; return kFALSE; }

  TObjArray Models;
  if (SubstractModels)
    for(Int_t nm=0; nm<GetNumModel(); nm++)
      if (GetModel(nm)->IsAssignTo(DataName))
        Models.Add(GetModel(nm));

  TArrayD Ampls(Models.GetLast()+1);
  for(Int_t n=0;n<=Models.GetLast();n++) {
     TGo4FitModel* model = (TGo4FitModel*) Models[n];
     model->BeforeEval(iter->ScalesSize());
     Ampls[n] = model->GetAmplValue() * model->GetRatioValueFor(DataName);
  }

  TArrayD bins(size), scales(size);

  Int_t pnt=0;

  if (iter->Reset(UseRanges)) do {
    Double_t value = iter->Value();
    for(Int_t n=0;n<=Models.GetLast();n++) {
      TGo4FitModel* model = (TGo4FitModel*) Models[n];
      value -= Ampls[n] * model->EvaluateAtPoint(iter);
    }
    value = TMath::Abs(value);
    bins[pnt] = value;
    scales[pnt] = iter->x();
    pnt++;
  } while (iter->Next(UseRanges));

  delete iter;

  for(Int_t n=0;n<=Models.GetLast();n++) {
     TGo4FitModel* model = (TGo4FitModel*) Models[n];
     model->AfterEval();
  }

  Int_t niter=0;

  do {
     Double_t sum00=0.;
     Double_t sum11=0.;
     Double_t sum22=0.;
     for (Int_t pnt=0;pnt<size;pnt++)
       if ((bins[pnt]>0.) && ((niter==0) || (TMath::Abs(scales[pnt]-first)<second*2.))) {
            sum00 += bins[pnt];
            sum11 += bins[pnt]*scales[pnt];
            sum22 += bins[pnt]*scales[pnt]*scales[pnt];
       }

     if (sum00>0.) {
       Double_t mid = sum11/sum00;
       Double_t dev = TMath::Sqrt(sum22/sum00-mid*mid);

       if (niter>0)
         if ((dev/second>0.8) && (dev/second<1.2)) niter=10;

       first = mid; second = dev;

      } else niter=10;

  } while (niter++<8);

  return kTRUE;
}

Double_t TGo4Fitter::CalculatesIntegral(const char* DataName, const char* ModelName, Bool_t onlycounts)
{
    TGo4FitData* data = FindData(DataName);
    if (data==0) return 0.;

    TGo4FitModel* model = ModelName!=0 ? FindModel(ModelName) : 0;
    if ((ModelName!=0) && (model==0)) return 0.;

    TGo4FitDataIter* iter = data->MakeIter();
    if (iter==0) return 0.;

    double sum = 0.;
    double ampl = 1.;

    if (!iter->Reset(kTRUE)) {
       delete iter;
       return 0.;
    }

    if (model!=0) {
      model->BeforeEval(iter->ScalesSize());
      ampl = model->GetAmplValue() * model->GetRatioValueFor(DataName);
    }

    do {
       double dx = onlycounts ? 1. : iter->xWidths();
       double value = model==0 ? iter->Value() : model->EvaluateAtPoint(iter);
       sum += ampl*value*dx;
    } while (iter->Next(kTRUE));

    if (model!=0)
      model->AfterEval();

    delete iter;

    return sum;
}

Double_t TGo4Fitter::CalculatesModelIntegral(const char* ModelName, Bool_t OnlyCounts)
{
   TGo4FitModel* model = FindModel(ModelName);
   if (model==0) return 0.;
   return CalculatesIntegral(model->AssignmentName(0), ModelName, OnlyCounts);
}

TObject* TGo4Fitter::CreateDrawObject(const char* ResName, const char* DataName, Bool_t IsModel, const char* ModelName)
{
    TGo4FitData* data = FindData(DataName);
    if (data==0) return 0;

    TObjArray Models;
    if (IsModel) {
      TGo4FitModel* model = FindModel(ModelName);
      if (model) Models.Add(model); else {
        Int_t groupindex = -1;

        if (ModelName!=0) {
          TString modelname(ModelName);
          if (modelname=="Background") groupindex = 0; else
          if (modelname.Index("Group",0,TString::kExact)==0) {
             modelname.Remove(0,5);
             char* err = 0;
             groupindex = strtol(modelname.Data(),&err,10);
             if (err && (*err!=0)) groupindex=-1;
          }
        }

        for(Int_t nm=0; nm<GetNumModel(); nm++) {
          TGo4FitModel* model = GetModel(nm);
          if (model->IsAssignTo(DataName))
            if ((groupindex<0) || (model->GetGroupIndex()==groupindex))
              Models.Add(model);
        }
      }
      if (Models.GetLast()<0) return 0;
    }

    TGo4FitDataIter* iter = data->MakeIter();
    if (iter==0) return 0;

    if (!iter->Reset(kFALSE)) { delete iter; return 0; }

    TH1* histo = 0;
    Int_t ndim = 0;
    TGraph* gr = 0;
    Bool_t UseRanges = kTRUE;

    if (iter->HasIndexes() && (iter->IndexesSize()==iter->ScalesSize()) && iter->HasWidths()) {
       histo = iter->CreateHistogram(ResName, kFALSE, !IsModel);

       if (histo) {
          ndim = histo->GetDimension();
          UseRanges = Models.GetLast() >= 0;
       }
    } else {
       gr = iter->CreateGraph(ResName, kTRUE, !IsModel);
       UseRanges = kTRUE;
    }

    if (((histo!=0) || (gr!=0)) && IsModel) {
       TArrayD Ampls(Models.GetLast()+1);

       for(Int_t n=0;n<=Models.GetLast();n++) {
         TGo4FitModel* model = (TGo4FitModel*) Models[n];
         model->BeforeEval(iter->ScalesSize());
         Ampls[n] = model->GetAmplValue() * model->GetRatioValueFor(DataName);
       }

       if (iter->Reset(UseRanges)) do {
          Double_t zn = 0;
          for(Int_t n=0;n<=Models.GetLast();n++) {
             TGo4FitModel* model = (TGo4FitModel*) Models[n];
             zn += Ampls[n] * model->EvaluateAtPoint(iter);
          }
          if (histo)
             switch(ndim) {
               case 1: histo->SetBinContent(iter->Indexes()[0]+1,zn); break;
               case 2: histo->SetBinContent(iter->Indexes()[0]+1,iter->Indexes()[1]+1,zn); break;
               case 3: histo->SetBinContent(iter->Indexes()[0]+1,iter->Indexes()[1]+1,iter->Indexes()[2]+1,zn); break;
             }
          if(gr) {
            (gr->GetX())[iter->Point()] = iter->x();
            (gr->GetY())[iter->Point()] = zn;
          }

       } while (iter->Next(UseRanges));

       for(Int_t n=0;n<=Models.GetLast();n++) {
         TGo4FitModel* model = (TGo4FitModel*) Models[n];
         model->AfterEval();
       }
    }

    delete iter;

    if (gr && IsModel)
     for(Int_t n1=0;n1<gr->GetN()-1;n1++)
       for(Int_t n2=n1+1;n2<gr->GetN();n2++)
         if ((gr->GetX())[n1]>(gr->GetX())[n2]) {
            Double_t xx = (gr->GetX())[n1];
            (gr->GetX())[n1] = (gr->GetX())[n2];
            (gr->GetX())[n2] = xx;
            Double_t yy = (gr->GetY())[n1];
            (gr->GetY())[n1] = (gr->GetY())[n2];
            (gr->GetY())[n2] = yy;
         }

    TNamed* res = 0;
    if (histo) res = histo; else res = gr;
    if (res) {
      TString title;
      if (IsModel)
        if (ModelName) { title = "Draw of model "; title+=ModelName; }
                  else { title = "Draw of full model of "; title+=DataName; }
        else { title = "Draw of data "; title+=DataName; }
      res->SetTitle(title.Data());
    }

    return res;
}


/*   valid format for draw options

   "" - draw all data
   "*" - draw all data with models
   "**" - draw all data with models and all components
   "DataName" - draw select data with model
   "DataName-" - draw select data without model
   "DataName*" - draw select data with model and components
   "DataName-, Background" - draw data and sum of components, belongs to background group (index=0)
   "DataName-, Group1" - draw data and sum of components, belongs to group 1
   "ModelName" - draw model component and correspondent data
*/

void TGo4Fitter::Draw(Option_t* option)
{
   TString opt(option);

   TCanvas *fCanvas = 0;

   if ((opt.Length()>0) && (opt[0]=='#')) {
      opt.Remove(0,1);
      TString CanvasName;
      Int_t n=0;
      do {
        CanvasName = "Canvas";
        CanvasName+=n++;
      } while (gROOT->FindObject(CanvasName.Data()));
      fCanvas = new TCanvas(CanvasName,TString("Draw of fitter ")+GetName()+" "+opt,3);
      fCanvas->cd();
   }

   Bool_t drawdata = kFALSE;
   TGo4FitData *selectdata = 0;
   TObjArray selectmodels;
   selectmodels.SetOwner(kTRUE);
   Bool_t drawcomp = kFALSE;
   Bool_t drawmodel = kFALSE;

   if (opt=="*") { opt = "";  drawdata = kTRUE; }

   while (opt.Length()>0) {
     Int_t len = opt.Index(",",0,TString::kExact);

     if (len<0) len = opt.Length();
     if (len==0) break;

     TString optpart(opt.Data(), len);
     while ((optpart.Length()>0) && (optpart[0]==' ')) optpart.Remove(0,1);

     Bool_t find = kFALSE;

     for(Int_t n=0;n<GetNumData();n++)
       if (optpart.Index(GetDataName(n),0,TString::kExact)==0) {
         selectdata = GetData(n);
         drawdata = kTRUE;
         drawmodel = kTRUE;
         find = kTRUE;
         optpart.Remove(0,strlen(GetDataName(n)));
         if (optpart=="*") drawcomp = kTRUE; else
         if (optpart=="-") drawmodel = kFALSE;
         break;
       }

     if (!find) {
       selectmodels.Add(new TObjString(optpart));
       TGo4FitModel* model = FindModel(optpart.Data());
       if (model && (selectdata==0))
         for(Int_t n=0;n<GetNumData();n++)
           if (model->IsAssignTo(GetDataName(n)))
             selectdata = GetData(n);
     }

     opt.Remove(0,len);
     if (opt.Length()>0) opt.Remove(0,1);
   }

   if ((selectdata==0) && !drawdata)
     if (GetNumData()>0) selectdata = GetData(0);

   MoveDrawObjectsToROOT();
   fxDrawObjs = new TObjArray();

   for(Int_t n=0;n<GetNumData();n++) {
      TGo4FitData* data = GetData(n);
      if (selectdata && (data!=selectdata)) continue;

      if (drawdata) {
          TObject* obj = CreateDrawObject(TString(data->GetName())+"_bins", data->GetName(), kFALSE);

          TAttLine* line = dynamic_cast<TAttLine*> (obj);
          if (line) {
            line->SetLineColor(1);
            line->SetLineWidth(1);
          }
          if (obj) fxDrawObjs->Add(obj);
      }

      if (drawmodel) {
         TObject* mobj = CreateDrawObject(TString(data->GetName())+"_fullmodel", data->GetName(), kTRUE);

         TAttLine* line = dynamic_cast<TAttLine*> (mobj);
         if (line) {
           line->SetLineColor(4);
           line->SetLineWidth(2);
         }
         if (mobj) fxDrawObjs->Add(mobj);
      }

      if (drawcomp)
        for(Int_t nmodel=0;nmodel<GetNumModel();nmodel++) {
           TGo4FitModel *model = GetModel(nmodel);
           if ( !model->IsAssignTo(data->GetName())) continue;

           TObject* cobj = CreateDrawObject(TString(data->GetName())+"_"+model->GetName(), data->GetName(), kTRUE, model->GetName());

           TAttLine* line = dynamic_cast<TAttLine*> (cobj);
           if (line) {
             line->SetLineColor(6);
             line->SetLineWidth(1);
           }
           if (cobj) fxDrawObjs->Add(cobj);
        }

      for (Int_t n=0;n<=selectmodels.GetLast();n++) {
        TString name = ((TObjString*) (selectmodels[n]))->String();

        TObject* cobj = CreateDrawObject(TString(data->GetName())+"_" +name, data->GetName(), kTRUE, name.Data());

        TAttLine* line = dynamic_cast<TAttLine*> (cobj);
        if (line) {
          line->SetLineColor(6);
          line->SetLineWidth(1);
        }
        if (cobj) fxDrawObjs->Add(cobj);
      }
   }

   if (fxDrawObjs->GetLast()==0) fxDrawObjs->At(0)->Draw(); else
   if (fxDrawObjs->GetLast()>0)  {
      Bool_t allhisto = kTRUE;
      for (Int_t n=0;n<=fxDrawObjs->GetLast();n++)
        if (!(fxDrawObjs->At(n)->InheritsFrom(TH1::Class()))) allhisto = kFALSE;
      if (allhisto) {
         THStack* stack = new THStack(TString("Stack")+"_"+fxDrawObjs->At(0)->GetName(),fxDrawObjs->At(0)->GetName());
         for (Int_t n=0;n<=fxDrawObjs->GetLast();n++) stack->Add((TH1*)fxDrawObjs->At(n));
         fxDrawObjs->Clear();
         fxDrawObjs->Add(stack);
         stack->Draw("nostack");
      } else
      for (Int_t n=0;n<=fxDrawObjs->GetLast();n++)
        if (n==0) fxDrawObjs->At(n)->Draw("A*");
             else fxDrawObjs->At(n)->Draw("L");
   }

   if (fCanvas!=0) fCanvas->Update();
}

void TGo4Fitter::PrintAmpls() const
{
   std::cout << std::endl << "*** LIST OF AMPLITUDES VALUE ***" << std::endl;
   for(Int_t n=0;n<GetNumComp();n++) {
       TGo4FitComponent* comp = ((TGo4Fitter*) this)->GetComp(n);
       if (comp->GetAmplPar() != 0)
          std::cout << "    " << comp->GetAmplFullName() << "   " << comp->GetAmplValue() << "   " << comp->GetAmplError() << std::endl;
   }
}

void TGo4Fitter::PrintLines() const
{
  std::cout << std::endl << "    *** LIST OF LINES PARAMETERS ***" << std::endl;

  int MaxAxis = 0;
  for (Int_t n=0; n<GetNumModel();n++) {
     TGo4FitModel* m = ((TGo4Fitter*) this)->GetModel(n);
     if (m==0) continue;
     Double_t zn;
     for (int naxis=0;naxis<3;naxis++)
        if (m->GetPosition(naxis,zn) || m->GetWidth(naxis,zn)) MaxAxis = naxis;
  }

  std::cout << std::setw(10) << "Name" << std::setw(12) << "Ampl";
  for(Int_t naxis=0;naxis<=MaxAxis;naxis++)
    std::cout << std::setw(11) << "Pos" << naxis << std::setw(11) << "Width" << naxis;
  std::cout << std::endl;

  for (Int_t n=0; n<GetNumModel();n++) {
     TGo4FitModel* m = ((TGo4Fitter*) this)->GetModel(n);
     if (m==0) continue;
     std::cout << std::setw(10) << m->GetName() << std::setw(12) << m->GetAmplValue();

     for (int naxis=0;naxis<=MaxAxis;naxis++) {
        Double_t pos, width;
        std::cout << std::setw(12);
        if (m->GetPosition(naxis,pos)) std::cout << pos;
                                  else std::cout << "---";
        std::cout << std::setw(12);
        if (m->GetWidth(naxis,width)) std::cout << width;
                                 else std::cout << "---";
        }
     std::cout << std::endl;
  }
}


void TGo4Fitter::ProvideLastDrawObjects(TObjArray& lst)
{
   if (fxDrawObjs) {
      lst.AddAll(fxDrawObjs);
      delete fxDrawObjs;
      fxDrawObjs = 0;
   }
}

void TGo4Fitter::MoveDrawObjectsToROOT()
{
   if (fxDrawObjs) {
      for(Int_t n=0;n<=fxDrawObjs->GetLast();n++)
         gROOT->Add(fxDrawObjs->At(n));
      delete fxDrawObjs;
      fxDrawObjs = 0;
   }
}

TString TGo4Fitter::FindNextName(const char* Head, Int_t start, Bool_t isModel)
{
   TString name;
   Int_t n = start;
   do {
     name.Form("%s%d", Head, n++);
   } while (isModel ? FindModel(name.Data())!=0 : FindData(name.Data())!=0 );
   return name;
}

void TGo4Fitter::Streamer(TBuffer& b)
{
   if (b.IsReading()) {
      TGo4Fitter::Class()->ReadBuffer(b, this);
      CheckDuplicatesOnSlot();
      SetParsListChange();
      SetUpdateSlotList();
   } else {
      PrepareSlotsForWriting();
      TGo4Fitter::Class()->WriteBuffer(b, this);
   }
}
