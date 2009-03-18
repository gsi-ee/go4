#ifndef TGO4FITTERCONFIG_H
#define TGO4FITTERCONFIG_H

#include "TGo4FitterAction.h"

#include "TObjArray.h"
#include "TGo4FitParsList.h"

class TGo4FitterAbstract;
class TGo4FitDependency;

/**
 * Fitter configuration action
 * By default all fitter parameters are used in optimization as independent from each other. But there are a lot of situation, then one would like to introduce some kind of dependency between parameters. For instance, two lines have constant difference in positions. In other cases some of the parameter properties should be redefined without touching parameter object itself. For such a cases configuration class TGo4FitterConfig was introduced. There are several routines of TGo4FitterConfig class, which provide useful fitter configurations:
 * SetParFixed() - fix value of given parameter;
 * SetParRange() - fix range for given parameter;
 * SetParEpsilon() - set initial error for given parameter.
 * SetParInit() - set initial value for parameter (can be double value or expression);
 * SetParDepend() - set dependency of given parameter via expression;
 * AddParNew() - create new parameter, which can be used in expressions.
 * Several configuration objects can be added to actions list. It may be useful, if several minimization routines are used. Then before each minimization action new configuration can be applied.
 */
class TGo4FitterConfig : public TGo4FitterAction {
   public:

      /**
       * Default constructor.
       */
      TGo4FitterConfig();

      /**
       * Creates TGo4FiierConfig object with given name and Title.
       */
      TGo4FitterConfig(const char* iName, const char* iTitle);

      /**
       * Destroys TGo4FitterConfig object.
       */
      virtual ~TGo4FitterConfig();

      /**
       * Add new parameter with given name and initial value.
       * This parameter can be used later in dependency calculations in minimizations.
       */
      void AddParNew(const char* ParName, Double_t iValue = 0) { fxParsNew.CreatePar(ParName,"config",iValue); }

      /**
       * Sets, that parameter with ParName should be fixed or not.
       * ParName may be standard or full name of parameter.
       */
      virtual Bool_t SetParFixed(const char* ParName, Bool_t iFixed = kTRUE);

      /**
       * Sets valid range for parameter ParName.
       * ParName may be standard or full name of parameter.
       */
      virtual Bool_t SetParRange(const char* ParName, Double_t RangeMin, Double_t RangeMax);

      /**
       * Sets epsilon for parameter ParName.
       * ParName may be standard or full name of parameter.
       */
      virtual Bool_t SetParEpsilon(const char* ParName, Double_t Epsilon);

      /**
       * Return kTRUE, if parameter ParName should be fixed in minimization.
       * ParName may be standard or full name of parameter.
       */
      virtual Bool_t GetParFixed(const char* ParName);

      /**
       * Return kTRUE, if parameter ParName has explicit configuration for valid range in minimization.
       * ParName may be standard or full name of parameter.
       */
      virtual Bool_t GetParRange(const char * ParName, Double_t & RangeMin, Double_t & RangeMax);

      /**
       * Return kTRUE, if parameter ParName has explicit configuration for epsilon value in minimization.
       * ParName may be standard or full name of parameter.
       */
      virtual Bool_t GetParEpsilon(const char* ParName, Double_t& Epsilon);

      /**
       * Set initial value for parameter.
       * Init value sets to parameter once when configuration activating.
       * FullName should be full name of parameter.
       */
      void SetParInit(const char* FullName, Double_t iValue);

      /**
       * Set initial value for parameter, calculated from expression.
       * Expression may use fitter parameters like
       *      Gauss2.Pos+Gaus3.Pos/10
       * Initializing expression calculated once when config activated.
       * FullName should be full name of parameter.
       */
      void SetParInit(const char* FullName, const char* iExpression);

      /**
       * Set dependency expression for parameter.
       * Expression may use fitter parameters like
       *    Gauss2.Pos+Gaus3.Pos/10
       * Dependency expression calculated every time before fit function calculations.
       * FullName should be full name of parameter.
       */
      void SetParDepend(const char* FullName, const char* iExpression);

      /**
       * Add expression for calculating result value.
       */
      void AddResult(const char* Expression);

      /**
       * Add constant value as result value.
       */
      void AddResult(Double_t Value);

      /**
       * Get list of TGo4FitParameter objects, used as explicit configuration for minimizing.
       */
      TGo4FitParsList& GetParsCfg() { return fxParsCfg; }

      /**
       * Get list of TGo4FitParameter objects, which can be used in minimization and dependency calculations.
       */
      TGo4FitParsList& GetParsNew() { return fxParsNew; }

      /**
       * Returns list of TGo4FitDependency objects, used for parameters initialization.
       */
      TObjArray& GetParsInit() { return fxParsInit; }

      /**
       * Returns list of TGo4FitDependency objects, used for dependencies calculations.
       */
      TObjArray& GetParsDepend() { return fxParsDepend; }

      /**
       * Returns list of TGo4FitDependency objects, used for calculating result values for fitter.
       */
      TObjArray& GetResults() { return fxResults; }

      /**
       * Do configuration action.
       * Calls TGoFitterAbstract::ApplyConfig(this) method.
       */
      virtual void DoAction(TGo4FitterAbstract* Fitter);

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:
      TGo4FitParameter* MakeParForProperties(const char* ParName);

      TGo4FitDependency* FindDepen(const char* FullName, TObjArray* list);

      /**
       * List of parameters objects, used for alternative configuration of parameters, which should be fitted.
       */
      TGo4FitParsList fxParsCfg;         // configurations for some of parameters


      /**
       * List of parameters object, which can be used in dependency calculations and minimization.
       */
      TGo4FitParsList fxParsNew;         // newly create parameters


      /**
       * Array of dependencies for parameters initialization.
       */
      TObjArray fxParsInit;              // array of dependencies, runs once


      /**
       * Array of dependencies (TGo4FitDependency) for introducing dependency between fitted parameters.
       */
      TObjArray fxParsDepend;            // array of dependencies, runs every time


      /**
       * Array of TGo4FitDependency objects, which repersent equations to calculate result values in fitter.
       */
      TObjArray fxResults;               // array of dependencies of results calculations


      /**
       * Determine, if all parameters should be fixed by default.
       */
      Bool_t fbFixedByDefault;           // is all parameters fixed by default

private:

      /** @link aggregation
       * @supplierCardinality 0..*
       * @label for pars dependency*/
      /*#  TGo4FitDependency ParsDependency; */

      /** @link aggregation
       * @supplierCardinality 0..*
       * @label TObjArray*/
      /*#  TGo4FitParMinimization ParsMinimization; */

      /** @link aggregation
       * @supplierCardinality 0..*
       * @label for pars init*/
      /*#  TGo4FitDependency lnkTGo4FitterConfig; */

      /** @link aggregation
       * @supplierCardinality 0..**/
      /*#  TGo4FitParsList lnkTGo4FitterConfig1; */
protected:

   ClassDef(TGo4FitterConfig,1)
};
#endif // TGO4FITTERCONFIG_H
