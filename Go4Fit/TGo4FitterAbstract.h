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

#ifndef TGO4FITTERABSTRACT_H
#define TGO4FITTERABSTRACT_H

#include "TGo4FitParsList.h"
#include "TGo4FitSlot.h"

#include "TObjArray.h"
#include "TArrayD.h"

class TVirtualPad;
class TGo4FitterConfig;
class TGo4FitterAction;
class TGo4FitterOutput;

/**
 * Abstract fitter class.
 * This is abstract class, which implement a general behaviour of fitter.
 * It has a list of all parameters (class inherited from TGo4FitParsList) and function, which should be fitted.
 * It has a list of action, which may be performed on the fitter.
 * The specific things, like list of data and list of models, are implemented in inherited TGo4Fitter class.
 */
class TGo4FitterAbstract : public TGo4FitParsList, public TGo4FitSlotList {
   public:

      /**
       * Default constructor.
       */
      TGo4FitterAbstract();

      /**
       * Create TGo4FitterAbstract object with given name and tiile.
       * TGo4FitterAbstract object should not be create directly, because it is abstract class.
       * TGo4Fitter or other implementation class should be used.
       */
      TGo4FitterAbstract(const char * iName, const char * iTitle);

      /**
       * Destroy TGo4FitterAbstract object.
       */
      virtual ~TGo4FitterAbstract();

      /**
       * Clear fitter.
       * Removes and delete all actions from actions list.
       */
      virtual void Clear(Option_t* option = "");

      /**
       * Return total number of parameters, which should be fitted.
       * Parameters are collects from all associated objects like data or models.
       */
      virtual Int_t NumPars() { CheckParsListChanging(); return TGo4FitParsList::NumPars(); }

      /**
       * Should be used, when number of parameters are changed.
       * Next time, when somebody will access fitter parameter list, fitter automatically recollect parameters from associated objects.
       */
      void SetParsListChange() { fbParsChange = kTRUE; }

      /**
       * If provided fitter has a same list of parameters, their values will be copied to fitter.
       */
      void CopyParsValuesFrom(TGo4FitterAbstract* fitter);

      /**
       * Return kTRUE, if parameter should be fixed.
       * If configuration object assigned to fitter, this flag will be tested there first and only then original parameter configuration will be used.ÿ
       */
      virtual Bool_t GetParFixed(const char* ParName);

      /**
       * Return kTRUE, if range condition specified for parameter and range boundaries.
       * If configuration object assigned to fitter, range values will be tested there first and only then original parameter configuration will be used.
       */
      virtual Bool_t GetParRange(const char* ParName, Double_t& RangeMin, Double_t& RangeMax);

      /**
       * Return kTRUE, if epsilon value specified for parameter.
       * If configuration object assigned to fitter, epsilon value will be tested there first and only then original parameter configuration will be used.
       */
      virtual Bool_t GetParEpsilon(const char* ParName, Double_t& Epsilon);

      /**
       * Checks, if config object corresponds to fitter parameters.
       */
      Bool_t IsSuitableConfig(TGo4FitterConfig* Config);

      /**
       * Set active configuration object.
       * This function calls in TGo4FitterConfig::DoAction() method.
       */
      Bool_t ApplyConfig(TGo4FitterConfig* Config);

      /**
       * Calculates value of fit function.
       * Fitter should be initialized before.
       * Instead of using values of parameters (from parameters list) array of values can be sets directly.
       */
      Double_t CalculateFitFunction(Double_t* pars = 0);

      /**
       * Add action to action list.
       * Action is a object, derived from TGo4FitterAction class.
       * Action object always owned by fitter, thus it will be destroyed when fitter is destroing. Therefore, stack object should not be added to fitter.
       */
      void AddAction(TGo4FitterAction* Action);

      /**
       * Add action to action list at specified index.
       */
      void AddActionAt(TGo4FitterAction* Action, Int_t indx);

      /**
       * Returns number of action in actions list.
       */
      Int_t GetNumActions() { return fxActions.GetLast()+1; }

      /**
       * Returns pointer on action with given index.
       */
      TGo4FitterAction* GetAction(Int_t num);

      /**
       * Remove and delete all actions from actions list.
       */
      void DeleteActions() { fxActions.Delete(); }

      /**
       * Remove action from actions list and destroy it.
       */
      void DeleteAction(TGo4FitterAction* action);

      /**
       * Shift action in action list upward (dir=-1) or downward (dir=+1).
       */
      void ReplaceAction(TGo4FitterAction* action, Int_t dir);

      /**
       * Executes actions list.
       * First Initialize() method will be called to create buffers (if required by actions).
       * Then for each action in actions list will be called DoAction() method.
       * AllowFitterChange flag specify, if action which may change fitter, can be executed.
       * Alternative array with actions can also be specified to function.
       * In the end Finalize() method close all buffers.
       */
      void DoActions(Bool_t AllowFitterChange = kFALSE, TObjArray* Actions = 0);

      /**
       * Executes specified action.
       */
      void DoAction(TGo4FitterAction* Action);

      /**
       * Executes specified action from actions list.
       */
      void DoAction(Int_t indx);


      TObjArray* ProcessObjects(TObjArray* objs, Bool_t CloneFitter = kTRUE, Bool_t OnlyRequired = kTRUE, TObjArray* rownames = 0, TObjArray* colnames = 0);

      /**
       * Creates and add TGo4FitMinuit object to actions list.
       * TGo4FitMinuit object contain the only command:
       * MIGRAD 500 1
       */
      void AddSimpleMinuit();

      /**
       * Creates and add TGo4FitterOutput action with given parameters to actions list.
       */
      TGo4FitterOutput* AddOutputAction(const char* Action, const char* Option = 0);

      /**
       * Remove and delete all output actions from actions list.
       */
      void DeleteOutputActions();

      /**
       * Returns required number of TPad for actions output.
       * Currently only "Draw" command of TGo4FitterOutput method requires pad for output.
       */
      Int_t NeedPadsNumber();

      /**
       * Set pad, which will be used for output of TGo4FitterOutput object.
       * If pad will not be set or command will not create new pad, current gPad object will be used for output.
       * Thus, if several "Draw" command occur, result of last command will be seen on current pad.
       */
      void SetPad(Int_t indx, TVirtualPad* pad);

      /**
       * Return kTRUE, if parameters were taken as result values.
       */
      Bool_t IsParsAsResults() { return fbParsAsResults; }

      /**
       * Return number of result values.
       */
      Int_t GetNumResults() const { return fxResults.GetSize() > 2 ? fxResults.GetSize() - 2 : 0; }

      /**
       * Return value of result with given index.
       * Normally at the end of execution of actions list values of parameters stores to results array.
       * This array can be accessed later by GetNumResults(), GetResultValue() or GetResults() methods.
       * If configuration was assigned during action, it may provide alternative way to calculating result values, which will be stored in fitter.
       */
      Double_t GetResultValue(Int_t n) const;

      /**
       * Return result value of fit function.
       */
      Double_t GetResultFF() const;

      /**
       * Return result value of number of degree of freedom.
       */
      Int_t GetResultNDF() const;

      /**
       * Returns pointer on array of result values.
       */
      const TArrayD* GetResults() { return &fxResults; }

      /**
       * Print result values to standard output.
       */
      void PrintResults() const;

      /**
       * Print information about fitter to standar output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * Initialize fitter.
       * Called automatically before any actions execution.
       */
      virtual Bool_t Initialize();

      /**
       * Return kTRUE, if Initialize() was done.
       */
      Bool_t IsInitialized() { return fbInitializationDone; }

      /**
       * Remove all buffer, created in Initialize() routine.
       */
      virtual void Finalize();

      /**
       * Should collect parameters from all associated to fitter objects.
       * This function should be extend in inherited classes.
       */
      virtual void CollectAllPars();

      /**
       * Return pointer on parameter of given index.
       * First check, if parameters should be recollected.
       */
      virtual TGo4FitParameter* Get(Int_t n) { CheckParsListChanging(); return TGo4FitParsList::Get(n); }

      /**
       * If appropriate flag sets (via SetParsListChange() method), function recollect all parameters from all objects, associated to fitter.
       */
      void CheckParsListChanging() { if(fbParsChange) CollectAllPars(); }

      /**
       * Calculates value of fit function according current values of parameters.
       * Should be implemented in inherited classes.
       */
      virtual Double_t DoCalculation() { return 0; }

      /**
       * Calculates number of dimensions of freedom;
       * Should be implemented in inherited classes.
       */
      virtual Int_t DoNDFCalculation() { return 0; }

      /**
       * Initialize a list of dependencies (TGo4FitDependecy object), which used in configuration.
       */
      Bool_t InitializeDependencies(TObjArray& Dependencies, Bool_t BlockPars, Bool_t DoInit);

      /**
       * Execute dependency equations.
       */
      void RunDependenciesList(TObjArray& Dependencies, Double_t* pars);

      /**
       * Relese memory, allocated for dependency executions.
       */
      void FinalizeDependencies(TObjArray& Dependencies);

      /**
       * Execute dependenices in currently associated configuration (if exist).
       */
      void ExecuteDependencies(Double_t* pars);

      /**
       * Initialize fitter data.
       * Called during Initialize() method.
       * Should be reimplemented in derived classes.
       */
      virtual Bool_t InitFitterData() { return kTRUE; }

      /**
       * Finalize fitter data.
       * Called during Finalize() method.
       * Should be reimplemented in derived classes.
       */
      virtual void FinalizeFitterData() { return; }

      /**
       * Array of actions onkects (inherited from TGo4FitterAction).
       */
      TObjArray fxActions;

      /**
       * Array of result values.
       * If no specific configuration is used, this just values of fitter parameters.
       * Result array creates in the end of actions execution.
       */
      TArrayD fxResults;

      /**
       * Pointer on currently activated config object.
       */
      TGo4FitterConfig* fxCurrentConfig;               //!


      /**
       * Boolean variable, showing that number of parameters is changed.
       */
      Bool_t fbParsChange;                             //!


      /**
       * Flag showing, that initialization was complete.
       */
      Bool_t fbInitializationDone;                     //!


      /**
       * Flag, showing, that finalize method should be called before fitter will be destroyed.
       */
      Bool_t fbNeedToFinalize;                         //!

      Bool_t fbParsAsResults;                          //!

      /** @link aggregation
       * @supplierCardinality 0..**/
      /*#  TGo4FitterAction lnkTGo4FitterAbstract; */

   ClassDef(TGo4FitterAbstract,1)
};
#endif // TGO4FITTERABSTRACT_H
