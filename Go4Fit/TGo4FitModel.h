#ifndef TGO4FITMODEL_H
#define TGO4FITMODEL_H

#include "TNamed.h"
#include "TGo4FitComponent.h"

class TGo4FitParameter;
class TGo4FitData;
class TGo4FitDataIter;

/**
 * Internal class, used for assignment of model component to data
 */
class TGo4FitAssignment : public TNamed {
    public:

       /**
        * Default constructor.
        */
       TGo4FitAssignment();

       /**
        * Create TGo4FitAssignment object, which assign model to DataName.
        */
       TGo4FitAssignment(const char* DataName);

       /**
        * Destroys TGo4FitAssignment object.
        */
       virtual ~TGo4FitAssignment();

       Double_t RatioValue();

       /**
        * Print information about object on standard output.
        */
       virtual void Print(Option_t* option) const;

       /**
        * Parameter for ratio value
        */
       TGo4FitParameter* fxRatio;

       /**
        * Pointer on assigned data.
        */
       TGo4FitData* fxData;         //!

       /**
        * Array of boolean values, selected model bins.
        */
       Char_t* fxModelMask;         //!

       /**
        * Array of model bins for assigned data.
        * Size of models and data bins are the same.
        * According to mask values some of models bins not used for calculation (always 0).
        */
       Double_t* fxModelBins;       //!

       ClassDef(TGo4FitAssignment,1);
};


/**
 * Basic abstract class for represnting model components of fitted data.
 */
class TGo4FitModel : public TGo4FitComponent {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModel();

      /**
       * Creates TGo4FitModel object with given name.
       * Standard constrcutor. Add amplitude parameter, if specified.
       * TGo4FitModel object should not be used directly. It is only basic class for concrete implementations like gaussians, polynoms and so on.
       */
      TGo4FitModel(const char* iName, const char* iTitle, Bool_t MakeAmplitude = kFALSE);

      /**
       * Delete TGo4FitModel object.
       */
      virtual ~TGo4FitModel();

      /**
       * Set group index of model.
       * Models, having same group index, can be displayed together
       * If index=-1, models are not belongs to any group
       * index=0 reserved for "Background" group.
       * All polynoms gets background group index.
       * By default index = -1
       */
      void SetGroupIndex(Int_t index = -1) { fiGroupIndex = index; }

      /**
       * Set group index to background (index=0).
       */
      void SetBackgroundGroupIndex() { fiGroupIndex = 0; }

      /**
       * Returns group index of model.
       */
      Int_t GetGroupIndex() const { return fiGroupIndex; }

      /**
       * Returns number of assignment for this model.
       */
      Int_t NumAssigments() const { return fxAssigments.GetLast()+1; }

      /**
       * Returns name of data, to which model object is assigned.
       */
      const char* AssignmentName(Int_t n) { return GetAssigment(n) ? GetAssigment(n)->GetName() : 0; }

      /**
       * Assign model to specified data object.
       * When model assigns to more then one data, additional "Ratio1", "Ratio2" and so on parameters will be created.
       * They means ratio in amplitude of model component in this data to ampltute of this component in data, to wich model was assigned first. By default, this ratio is 1. and not fixed.
       */
      void AssignToData(const char* DataName, Double_t RatioValue = 1., Bool_t FixRatio = kFALSE);

      /**
       * Checks, if model assigned to given data.
       */
      Bool_t IsAssignTo(const char* DataName) const { return FindAssigment(DataName) != 0; }

      /**
       * Change name of data in assignments.
       */
      void ChangeDataNameInAssignments(const char* oldname, const char* newname);

      /**
       * Remove assignment to given data (if exists).
       */
      void ClearAssignmentTo(const char* DataName);
      void ClearAssigmentTo(const char* DataName) { ClearAssignmentTo(DataName); }

      /**
       * Remove all assignments.
       */
      void ClearAssignments();

      /**
       * Check, if model assigned to such a data (via name) and store pointer on this data object.
       */
      void ConnectToDataIfAssigned(TGo4FitData* data);

      /**
       * Returns ratio value for specified data object.
       * Return  1., if model not assign to data.
       */
      Double_t GetRatioValueFor(const char* DataName);

      /**
       * Print information about model object on standard output.
       */
      virtual void Print(Option_t* option) const;

      /**
       * Calculates value of model for given x value.
       */
      virtual Double_t Evaluate(Double_t x);

      /**
       * Calculates value of model for given x,y values.
       */
      virtual Double_t Evaluate(Double_t x, Double_t y);

      /**
       * Calculates value of model for given x,y,z values.
       */
      virtual Double_t Evaluate(Double_t x, Double_t y, Double_t z);

      /**
       * Calculates value of model for given axis values.
       */
      virtual Double_t Evaluate(Double_t* v, Int_t ndim);

      /**
       * Calculates intagral of model component
       */
      virtual Double_t Integral();

      /**
       * Return position of model, if exists.
       */
      virtual Bool_t GetPosition(Int_t naxis, Double_t& pos);

      /**
       * Sets position of model component, if possible.
       */
      virtual Bool_t SetPosition(Int_t naxis, Double_t pos);

      /**
       * Returns with of model component, if exists.
       */
      virtual Bool_t GetWidth(Int_t naxis, Double_t& width);

      /**
       * Sets width of model component, if possible.
       */
      virtual Bool_t SetWidth(Int_t naxis, Double_t width);

      /**
       * Set integration properties.
       * This function make sence, if model assign to histogramic data. In this case each bins defined on some finite range.
       * By default there is no integration and model uses central positions in each range, where data bins is defined.
       * If model changes much in these ranges, it may cause big error in modeling. In this case integration inside each data bin ranges can highly reduce these errors.
       * To make integration, bin range on each axis divides on (2^depth) segemnts and value of model calculates in each segments. Thus, if data bins defined on two-dimensional space and depth=3, each bins range were diveded on 8x8=64 squares and model will be calculated 64 times.
       * MinIntegrDepth and MaxIntegrDepth means minimum and maximum allowed integration depth correspondently. If MaxIntegrDepth not specified, it will be equal to MinIntegrDepth.
       * If MinIntegrDepth less then MaxIntegrDepth, model will try to use minimum value first. Then it checks, if result value changes not very much. If so, it finish calculation, otherwise it increases depth of integration up to MaxIntegrDepth.
       * IntegrEps sets maximum allowed error in result value. It can be absolute (when AbsoluteEps = kTRUE) or relative to previous calculated value (with depth-1).
       * Value of each bin also can be scaled on volume (IntegrScalink = kTRUE, default = kFALSE).
       */
      void SetIntegrationsProperty(Int_t iMinIntegrDepth, Int_t iMaxIntegrDepth = 0, Double_t iIntegrEps = 0., Bool_t iAbsoluteEps = kFALSE, Bool_t iIntegrScaling = kFALSE);

      /**
       * Returns kTRUE, if integration properties were specified.
       */
      Bool_t NeedIntegration() { return (fiMinIntegrDepth>0) || (fiMaxIntegrDepth>0); }

       /**
       * Prepares (if necesary) some intermediate variables to be able calculate values of model via EvalN() function.
       * Number of axis, which will be used in evaluations, should be specified.
       */
      virtual Bool_t BeforeEval(Int_t ndim);

      /**
       * Calculates value of model according current parameters values and provided axes values.
       * BeforeEval(), EvalN() & AfterEval() virtual methods provides general interface, where user-specific code should be situated.
       */
      virtual Double_t EvalN(const Double_t* v);

      /**
       * Clear buffers, which were created by BeforeEval() method.
       */
      virtual void AfterEval() { return; }

      /**
       * Evaluate model value for specified data point.
       * All scales values will be take from data buffers, therefore data should be initialized.
       */
      virtual Double_t EvaluateAtPoint(TGo4FitData* data, Int_t nbin, Bool_t UseRanges = kTRUE);

      /**
       * Evaluate model value for point, specified by current values of iterator.
       */
      virtual Double_t EvaluateAtPoint(TGo4FitDataIter* iter, Bool_t UseRanges = kTRUE);

      /**
       * Evaluate model values for all data point and add them to result buffer.
       * Data should be initialized.
       */
      Bool_t AddModelToDataResult(TGo4FitData* data);

      virtual Int_t NumPars();

   protected:

      friend class TGo4Fitter;


      virtual TGo4FitParameter* Get(Int_t n);

      /**
       * Make integration of model inside given point, if integration specified.
       */
      Double_t EvaluateAndIntegrate(Int_t NumScales, const Double_t* Scales, const Double_t* Widths);

      /**
       * Initialize model object.
       * If usage of buffers is specified (UseBuffers>0),
       * method creates a number of buffers for representing model bins for each data, to which model was assign to.
       */
      virtual Bool_t Initialize(Int_t UseBuffers = -1);

      /**
       * Deletes all buffers, created during initialization.
       */
      virtual void Finalize();

      /**
       * Return pointer on assigned data.
       */
      TGo4FitData* GetAssignedConnection(Int_t n);

      /**
       * Checks if model allocate buffers for calculations
       */
      Bool_t BuffersAllocated() const;

      /**
       * Recalculates shape of object.
       */
      void RebuildShape(Bool_t ForceBuild = kFALSE);

      /**
       * Get model bins for specified data (if exists)
       */
      Double_t* GetModelBins(const char* DataName) const;

      /**
       * Sets flag, that shape bins should be refilled next time, when RebuildShape() routine will be called.
       */
      void SetNeedToRebuild() { fbNeedToRebuild = kTRUE; }

      /**
       * Another place, where user specific code can be placed for model values calculation.
       * Function gets as parameter array of Coordinates and array of Parameters values.
       * Only user-specific calculation should  be done and result value should be return.
       */
      virtual Double_t UserFunction(Double_t*, Double_t*) { return 0; }

      /**
       * Return index of parameter (if exist), which represent position of model for given axis.
       */
      virtual Int_t GetPosParIndex(Int_t) { return -1; }

      /**
       * Return index of parameter (if exist), which represent width of model component for given axis.
       */
      virtual Int_t GetWidthParIndex(Int_t) { return -1; }

      /**
       * Return parameter (if exist), which represent position of model for given axis.
       */
      virtual TGo4FitParameter* GetPosPar(Int_t naxis = 0)
        { return Get(GetPosParIndex(naxis)); }

      /**
       * Return parameter (if exist), which represent width of model component for given axis.
       */
      virtual TGo4FitParameter* GetWidthPar(Int_t naxis = 0)
        { return Get(GetWidthParIndex(naxis)); }

      const Int_t* GetDataFullIndex(TGo4FitData* data, Int_t nbin);
      Int_t GetDataIndexesSize(TGo4FitData* data);

      /**
       * Minimum depth of integration.
       */
      Int_t fiMinIntegrDepth;

      /**
       * Maximum intergration depth.
       */
      Int_t fiMaxIntegrDepth;

      /**
       * Integration precision.
       */
      Double_t fdIntegrEps;

      /**
       * States, if integration precision absolute or relative.
       */
      Bool_t fbAbsoluteEps;

      /**
       * Scale integral to integration volume.
       */
      Bool_t fbIntegrScaling;

   private:

      /**
       * Clear all buffers, allocated during initialization.
       */
      void RemoveAllPars();

      /**
       * Return TGo4FitAssignment object with given index.
       */
      TGo4FitAssignment* GetAssigment(Int_t n) { return (TGo4FitAssignment*) fxAssigments[n]; }
      TGo4FitAssignment* GetAssigment(Int_t n) const { return (TGo4FitAssignment*) fxAssigments[n]; }

      /**
       * Find assignment to given data.
       */
      TGo4FitAssignment* FindAssigment(const char* DataName) const;

      /**
       * Set name of ratio parameter.
       */
      TString GetRatioName(Int_t n);

      /**
       * List of TGo4FitAssignment objects.
       */
      TObjArray fxAssigments;

      /**
       * Store group index of specified model.
       */
      Int_t fiGroupIndex;

      /**
       * Array of values of parameters.
       */
      TArrayD fxCurrentPars;            //!

      /**
       * Pointer on array of parameters values.
       */
      Double_t* fxCurrentParsArray;     //!

      /**
       * Internal flag. Signals, when object should be rebuild.
       */
      Bool_t fbNeedToRebuild;                 //!

      /**
       * List of all parameters, associated not only with component directly but also with incapsulated objects.
       */
      TGo4FitParsList* fxAllPars;              //!

      TArrayD* fxAllParsValues;                //!

      /** @link aggregation
       * @supplierCardinality 0..**/
      /*#  TGo4FitAssignment lnkTGo4FitModel; */

   ClassDef(TGo4FitModel,1)
};
#endif // TGO4FITMODEL_H
