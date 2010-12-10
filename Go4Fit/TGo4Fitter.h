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

#ifndef TGO4FITTER_H
#define TGO4FITTER_H

#include "TGo4FitterAbstract.h"

class TH1;
class TGraph;

#include "TArrayD.h"
#include "TObjArray.h"

class TGo4FitComponent;
class TGo4FitData;
class TGo4FitDataHistogram;
class TGo4FitDataGraph;
class TGo4FitModel;
class TGo4FitModelGauss1;

typedef Double_t (*TUserFitFunction)(Double_t, Double_t, Double_t);

/**
 * Central class of Go4Fit package.
 */
class TGo4Fitter : public TGo4FitterAbstract {
   public:
      enum  { ff_least_squares = 0,
              ff_chi_square    = 1,
              ff_chi_Pearson   = 2,
              ff_chi_Neyman    = 3,
              ff_chi_gamma     = 4,
              ff_ML_Poisson    = 5,
              ff_user          = 6 };

      /**
       * Default constructor.
       */
      TGo4Fitter();

      /**
       * Creates TGo4Fitter object and sets name and title.
       */
      TGo4Fitter(const char* iName, const char* iTitle);

      /**
       * Creates TGo4Fitter object and set type of fit function.
       * If AddStandardActions flag is set,
       */
      TGo4Fitter(const char * iName, Int_t iFitFunctionType, Bool_t IsAddStandardActions);

      /**
       * Destructor. Delete TGo4Fitter object.
       */
      virtual ~TGo4Fitter();

      /**
       * Set fitted function type for minimization. Possible values are:
       *    TGo4Fitter:ff_least_squares  = 0   sum of difference squares between model and data;
       *    TGo4Fitter:ff_chi_square     = 1  standard chi-square sum
       *    TGo4Fitter:ff_chi_Pearson    = 2  Pearson chi-square for Poisson statistic
       *    TGo4Fitter:ff_chi_Neyman     = 3  modified Neyman chi-square for Poisson statistic
       *    TGo4Fitter:ff_chi_gamma      = 4  modified chi-square for Poisson statistic
       *    TGo4Fitter:ff_ML_Poisson     = 5  -2Ln from maximum likelihood function for Poisson statistic
       *    TGo4Fitter:ff_user           = 6  user function
       * User function type should not be specified directly, but using SetUserFitFunction() method.
       * More informtation about fit function types can be found in Go4Fit tutorial.
       */
      void SetFitFunctionType(Int_t iFitFunctionType) { fiFitFunctionType = iFitFunctionType; }

      /**
       * Return type of fitted function. For possible values see SetFitFunctionType() method.
       */
      Int_t GetFitFunctionType() { return fiFitFunctionType; }

      /**
       * Set user-defined fitted function.
       * Function should be global and has following signature:
       *    Double_t (*TUserFitFunction)(Int_t, Double_t*, Double_t*)
       * First parameter - number of points,
       * Second - array of data points,
       * Third - array of model points.
       * Function should return value, which will be MINIMIZED by fitter.
       * If necessary maximization, fit function should return negative value.
       */
      void SetUserFitFunction(TUserFitFunction iFunc) { fxUserFitFunction = iFunc; fiFitFunctionType = ff_user; }

      /**
       *  Set value of memory usage.
       *  This parameter can be:
       *  0 - no aditional memory will be allocated in buffers
       *  1 - buffers will be allocated only for data objects
       *  2 - buffers will be used for all data and models objects
       *  3 - individual setup for data and model objects will be used for memory allocaions
       */
      void SetMemoryUsage(Int_t iMemoryUsage);

      /**
       * Retrurn value of memory usage parameter.
       * See SetMemoryUsage for detailed description.
       */
      Int_t GetMemoryUsage() { return fiMemoryUsage; }

      /**
       * Remove all data, all models and all actions
       */
      virtual void Clear(Option_t* option = 0);

      /**
       * Return number of data objects in fitter.
       */
      Int_t GetNumData() const { return fxDatas.GetLast()+1; }

      /**
       * Return data object with given index. If index out of valid range, return 0.
       */
      TGo4FitData* GetData(Int_t n);

      /**
       * Return name of data object with given index. If index not valid, return 0.
       */
      const char* GetDataName(Int_t n);

      /**
       * Return data object with given name. if no corresponding objects, return 0.
       */
      TGo4FitData* FindData(const char* DataName);

      /**
       * Add data object to fitter. Data object owned by fitter and will be destroyed together with fitter.
       */
      TGo4FitData* AddData(TGo4FitData* d);

      /**
       * Create TGo4FitDataHistogram object and adds its to fitter.
       */
      TGo4FitDataHistogram* AddH1(const char* DataName, TH1* histo, Bool_t Owned = kFALSE, Double_t lrange = 0., Double_t rrange = 0.);

      /**
       * Set histogram to existing TGo4FitDataHistogram object
       */
      TGo4FitDataHistogram* SetH1(const char* DataName, TH1* histo, Bool_t Owned = kFALSE);

      /**
       * Create TGo4FitDataGraph object and adds its to fitter.
       */
      TGo4FitDataGraph* AddGraph(const char* DataName, TGraph* gr, Bool_t Owned = kFALSE, Double_t lrange = 0., Double_t rrange = 0.);

      /**
       * Set graph to existing TGo4FitDataGraph object
       */
      TGo4FitDataGraph* SetGraph(const char* DataName, TGraph* gr, Bool_t Owned = kFALSE);

      /**
       * Remove data object from fitter. If IsDel flag is set,  object will be destroyed, otherwise (by default) function return pointer on this object.
       */
      TGo4FitData* RemoveData(const char* DataName, Bool_t IsDel = kFALSE);

      /**
       * Delete all data objects from fitter.
       */
      void DeleteAllData();

      /**
       * Return number of model component in fitter.
       */
      Int_t GetNumModel() const { return fxModels.GetLast()+1; }

      /**
       * Return model component with given index. If index out of valid range, return 0.
       */
      TGo4FitModel* GetModel(Int_t n);

      /**
       * Return model component with given name. If no such model name exists, return 0.
       */
      TGo4FitModel* FindModel(const char* ModelName);

      /**
       * Add model component to fitter. Model already should be assigned to at least one data object.
       */
      TGo4FitModel* AddModel(TGo4FitModel* m);

      /**
       * Add model component to fitter and assign it to specified data object.
       */
      TGo4FitModel* AddModel(const char* DataName, TGo4FitModel* m);

      /**
       * Construct 1-dim polynom for specified data object for x scale.
       * NamePrefix sets prefix of model name for each polynom component.
       * MaxOrder sets maximum order of polynom (default =1 )
       * NumAxis sets number of axis, for which polynom will be sets
       * GroupIndex sets index of models group for all polyn components (default = 0 - background group)
       * lbound and rbound sets range, where polynom will be calculated
       */
      void AddPolynomX(const char* DataName, const char* NamePrefix, Int_t MaxOrder = 1, Int_t GroupIndex = 0, Double_t lrange = 0., Double_t rrange = 0. );

      /**
       * Construct 1-dim polynom with specified polynom coefficients
       * NamePrefix sets prefix of model name for each polynom component.
       * GroupIndex sets index of models group for all polyn components (default = 0 - background group)
       */
       void AddPolynomX(const char* DataName, const char* NamePrefix, TArrayD& Coef, Int_t GroupIndex = 0);

      /**
       * Construct full polynom for specified data object.
       * NamePrefix sets prefix of model name for each polynom component.
       * MaxOrder sets maximum order of polynom (default =1 )
       * NumAxis sets number of axis, for which polynom will be sets
       * GroupIndex sets index of models group for all polyn components (default = 0 - background group)
       */
      void AddPolynoms(const char* DataName, const char* NamePrefix, Int_t MaxOrder = 1, Int_t NumAxis = 1, Int_t GroupIndex = 0);

      /**
       * Add 1-dim gaussian model to fitter.
       */
      TGo4FitModelGauss1* AddGauss1(const char* DataName, const char* ModelName, Double_t iPosition, Double_t iWidth, Double_t iAmpl = 1., Int_t Axis = 0);

      /**
       * Counts models associated with specific data.
       */
      Int_t NumModelsAssosiatedTo(const char* DataName);

      /**
       * Clones specified model.
       */
      TGo4FitModel* CloneModel(const char* ModelName, const char* NewName = 0);

      /**
       * Remove model component from fitter.
       * If IsDel flag specified, component is delete, otherwise (by default)
       * function return pointer on this model component.
       */
      TGo4FitModel* RemoveModel(const char* ModelName, Bool_t IsDel = kFALSE);

      /**
       * Remove models associated with specific data.
       */
      void DeleteModelsAssosiatedTo(const char* DataName);

      /**
       * Delete all model objects from fitter.
       */
      void DeleteAllModels();

      /**
       * Assign model to specified data object.
       * When model assigns to more then one data, additional "Ratio1", "Ratio2" and so on parameters will be created.
       * They means ratio in amplitude of model component in this data to ampltute of this component in data, to wich model was assigned first. By default, this ratio is 1. and not fixed.
       */
      void AssignModelTo(const char* ModelName, const char* DataName, Double_t RatioValue = 1., Bool_t FixRatio = kFALSE);

      /**
       * Remove assignment to given data (if exists).
       * If DataName==0, all assignments will be cleared.
       */
      void ClearModelAssignmentTo(const char* ModelName, const char* DataName = 0);

      /**
       * Change data name in model component assignments.
       * This function should be used, if data name is changing out of the fitter.
       */
      void ChangeDataNameInAssignments(const char* oldname, const char* newname);

      /**
       * Return total number of TGo4FitComponent (data and model) objects in fitter.
       * This is just a sum GetNumData()+GetNumModel().
       */
      Int_t GetNumComp() const { return GetNumData()+GetNumModel(); }

      /**
       * Return TGo4FitComponent object with given index.
       * If index < GetNumData() return data object, else return model component.
       * If index not valid, return 0.
       */
      TGo4FitComponent* GetComp(Int_t n);

      /**
       * Estimate amplitude of all model components.
       * This routine typically should be executed before general minimization routine.
       * To obtain good estimation of amplitudes, rest model parameters (like position and widths) should has good estimation, otherwise amplitude estimation routine gives unacceptable values.
       */
      void EstimateAmplitudes(Int_t NumIters = 1);

      /**
       * Add amplitude estimation to actions list.
       */
      void AddAmplEstimation(Int_t NumIters = 1);

      /**
       * Add list of standard actions to fitter.
       * Now : AmplEstimate action and Minuit minimizer with "MIGRAD 500 1" command.
       */
      void AddStandardActions();

      /**
       * Calculate value of fit function for given set of parameters and specified type of fit function (it can not correspond to minimized fit function type). If DataName not specified (default), fitfunction calculates for all data objects.
       */
      Double_t CalculateFitFunction(Double_t* pars = 0, Int_t FitFunctionType = -1, const char* DataName = 0);

      /**
       * Calculates number of degree of freedom (NDF).
       * If DataName not specified (default), NDF calculates for all data objects.
       * NDF defined as total number of bins in data object minus number of variable parameters.
       */
      Int_t CalculateNDF(const char* DataName = 0);

      /**
       * Collect all TGo4FitSlot objects, situated in fitter, data and models to givven TObjArray.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Print containment of fitter.
       * If option = "**", print also containment of all associated objects like histogram, calibrations and so on.
       */
      virtual void Print(Option_t* option) const;

      /**
       * Calculates first and second momentum for specified data
       * Usage of ranges and subtraction of model can be specified
       */
      Bool_t CalculatesMomentums(const char* DataName, Bool_t UseRanges, Bool_t SubstractModels, Double_t& first, Double_t& second);

      /**
       * Calculates integral for data or model (if ModelName is specified)
       * if OnlyCounts specified, only sum of values in bins position are calculated
       */
      Double_t CalculatesIntegral(const char* DataName, const char* ModelName = 0, Bool_t OnlyCounts = kFALSE);

      /**
       * Calculates integral of model (if ModelName is specified)
       * if OnlyCounts specified, only sum of values in bins position are calculated
       */
      Double_t CalculatesModelIntegral(const char* ModelName, Bool_t OnlyCounts = kFALSE);

      /**
       * Create object (TH1 or TGraph), which can be drawn.
       * Object can contain data bins (IsModel = kFALSE), full data model (IsModel = kTRUE, ModelName = 0) or specific model component (IsModel = kTRUE, ModelName specify component name)
       */
      TObject* CreateDrawObject(const char* ObjName, const char* DataName, Bool_t IsModel = kFALSE, const char* ModelName = 0);

      /**
       * Draw fitter on current canvas.
       * As options, data name(s) or component name(s) should be specified, separated by commas.
       * For instance:
       *    Draw("data1") - draw data object with name "data1" and it's full model
       *    Draw("data1,Gauss1,Gauss2") - draw data object "data1", full model and model components with names "Gauss1" and "Gauss2"
       * All objects is created via CreateDrawObject() method and stored in internal list. They will be moved to ROOT when fitter will be destroyed.
       */
      virtual void Draw(Option_t* option);

      /**
       * Copy pointer on drawn object after last command to specified TObjArray.
       * From this moment user should take care about proper destroying of this objects.
       */
      void ProvideLastDrawObjects(TObjArray& lst);

      /**
       * Print value of all amplitude parameters.
       */
      void PrintAmpls() const;

      /**
       * Print amplitude, position and width for each model components
       */
      void PrintLines() const;

      TString FindNextName(const char* Head, Int_t start, Bool_t isModel = kTRUE);

   protected:

      friend class TGo4FitAmplEstimation;

      // a set of function to access data buffers

      Bool_t ModelBuffersAllocated(TGo4FitModel* model);
      Bool_t DataBuffersAllocated(TGo4FitData* data);

      Int_t GetDataBinsSize(TGo4FitData* data);
      Double_t* GetDataBinsValues(TGo4FitData* data);
      Double_t* GetDataBinsDevs(TGo4FitData* data);
      Double_t* GetDataBinsResult(TGo4FitData* data);
      Double_t* GetModelBinsValues(TGo4FitModel* model, const char* DataName);

      /**
       * Update all data objects and model components acording to current parameters values.
       * Only those objects will be recalculated, which parameters were changed after last recalculation.
       * If ForceBuild flag is set (default = kFALSE), all objects will be recalculated anyway.
       */
      void RebuildAll(Bool_t ForceBuild = kFALSE);


      virtual void CollectAllPars();

      virtual Double_t DoCalculation();
      virtual Int_t DoNDFCalculation();
      Double_t CalculateFCN(Int_t FitFunctionType, TGo4FitData* selectdata = 0);
      Double_t PointFitFunction(Int_t FitFunctionType, Double_t value, Double_t modelvalue, Double_t standdev);

      virtual Bool_t InitFitterData();
      virtual void FinalizeFitterData();

      /**
       * Container for data objects.
       */
      TObjArray fxDatas;

      /**
       * Container for model components.
       */
      TObjArray fxModels;

      /**
       * Defines type of fitted function.
       */
      Int_t fiFitFunctionType;

      /**
       * Defines use of memory during actions executions.
       */
      Int_t fiMemoryUsage;

   private:

      void MoveDrawObjectsToROOT();
      void CheckSlotsBeforeDelete(TGo4FitComponent* comp);

      /**
       * Pointer on user fit function.
       */
      TUserFitFunction fxUserFitFunction;    //!

      TObjArray* fxDrawObjs;                 //!


      /** @link aggregation
       * @supplierCardinality 0..*
       * @label TObjArray of models*/
      /*#  TGo4FitModel Models; */

      /** @link aggregation
       * @supplierCardinality 0..*
       * @label TObjArray of data*/
      /*#  TGo4FitData Datas; */
   ClassDef(TGo4Fitter,1)
};


#endif // TGO4FITTER_H


