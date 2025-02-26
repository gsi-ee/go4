// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4FITMODELFUNCTION_H
#define TGO4FITMODELFUNCTION_H

#include "TGo4FitModel.h"

typedef Double_t (*TUserFunction)(Double_t *, Double_t *);

/**
 * @brief Model objects, which uses external user function to calculate model values.
 *
 * @details The function should has such signature:
 *
 * ~~~{cpp}
 * Double_t Func(Double_t *coord, Int_t ncoord, Double_t *pars, Int_t npars) {
 *     // coord - array of axis values, ncoord - number of axis values
 *     // pars - model parameters values, npars - number of parameters
 *    return (coord[0] - pars[0]) * (coord[1] - pars[1]) * (coord[2] - pars[2]);
 * }
 * ~~~
 * In constructor user should define name and title of object, pointer to user function, number of parameters and, optionally, using additional amplitude parameters. For instance, user function with three parameters and amplitude:
 *
 * ~~~{cpp}
 *   auto f = new TGo4FitModelFunction("func", "user function Func",&Func, 3, kTRUE);
 * ~~~
 *
 * In constructor "Par0", "Par1", "Par2" and "Ampl" parameters will be created. They are accessible in usual way from fitter or model object itself.
 * Important notice - this model object can not be saved to file and restored in proper way, because address of user function may change in between. To correctly use this object after saving and restoring routines, user should directly set address of user function to TGo4FitModelFunction object (SetUserFunction() method) before using it. Otherwise, run-time error will occur. To avoid this user should create it's own model class (see example 4) or put function to shared library (example 2).
 * If shared library is created, it can be used in constructor like:
 *
 * ~~~{cpp}
 *   auto f = new TGo4FitModelFunction("Gauss1", "Example8Func.so", "gaussian", 3, kTRUE);
 * ~~~
 *
 * During initialization routine library will be loaded and function will be used for modeling. In this case, if library will be present on the same location, model object can be reused directly after storing to file and reading it back.
 *
 * @ingroup go4_fit
 */
class TGo4FitModelFunction : public TGo4FitModel {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModelFunction();

      /**
       * Creates TGo4FitModelFunction object with specified name.
       * Pointer on function and number of parameters should be specified.
       * Additionally usage of amplitude parameter can be invoked.
       * !!! Pointer on function, specified in this method, can not be correctly restored from streamer. Therefore, object can not be directly reused after streaming. SetUserFunction() should be called before.
       */
      TGo4FitModelFunction(const char *iName, TUserFunction iUserFunction, Int_t iNPars, Bool_t AddAmplitude = kFALSE);

      /**
       * Creates TGo4FitModelFunction object with specified name.
       * Library and function name can be specified. Additionally usage of amplitude parameter can be invoked.
       */
      TGo4FitModelFunction(const char *iName, const char *iLibraryName = "libName.so", const char *iFunctionName = "Funcname", Int_t iNPars = 0, Bool_t AddAmplitude = kFALSE);

      /**
       * Destroys TGo4FitModelFunction object.
       */
      virtual ~TGo4FitModelFunction();

      /**
       * Signals, that amplitude parameter can be removed from or introduced to object.
       */
      Bool_t CanAmplTouch() override { return kTRUE; }

      /**
       * Set address to user function.
       */
      void SetUserFunction(TUserFunction iUserFunction);

      /**
       * Set library file name and function name.
       * Library will be loaded during initialization.
       */
      void SetUserFunction(const char *iLibraryName, const char *iFunctionName);

      /**
       * Returns library name.
       */
      const char *GetLibraryName() const { return fxLibraryName.Data(); }

      /**
       * Sets library name.
       */
      void SetLibraryName(const char *name) { fxLibraryName = name; }

      /**
       * Returns function name in library.
       */
      const char *GetFunctionName() const { return fxFunctionName.Data(); }

      /**
       * Sets function name in library.
       */
      void SetFunctionName(const char *name) { fxFunctionName = name; }

      /**
       * Returns number of parameter, which can be used by function.
       */
      Int_t GetNumberOfFuncPar();

      /**
       * Sets number of parameters, which can be used by function.
       */
      Bool_t SetNumberOfFuncPar(Int_t num);

      void SetPosParIndex(Int_t naxis, Int_t indx = -1);
      void SetWidthParIndex(Int_t naxis, Int_t indx = -1);

      /**
       * Prepare object to evaluations.
       * When usage of function from library is specified, library will be loaded at this time.
       */
      Bool_t BeforeEval(Int_t ndim) override;
      void AfterEval() override;

      void Print(Option_t *option = "") const override;

   protected:

      /**
       * Initialize object.
       * When usage of function from library is specified, library will be loaded at this time.
       */
      Bool_t Initialize(Int_t UseBuffers = -1) override;

      /**
       * Finalize object.
       * Close library, if it was opened.
       */
      void Finalize() override;

      virtual TString GetFuncParName(Int_t n);
      TGo4FitParameter *GetFuncPar(Int_t n);

      Int_t GetPosParIndex(Int_t naxis) override;
      Int_t GetWidthParIndex(Int_t naxis) override;

      Double_t UserFunction(Double_t *Coordinates, Double_t *Parameters) override;

      Bool_t LoadLibrary(Bool_t CloseFirst);
      void CloseLibrary();

      /**
       * String with library name, where function is situated.
       */
      TString fxLibraryName;

      /**
       * Function name.
       */
      TString fxFunctionName;

      TArrayI fxPosIndex;

      TArrayI fxWidthIndex;

   private:

      /**
       * Direct pointer on function.
       */
      TUserFunction fxUserFunction{nullptr};             //!

      /**
       * Pointer on loaded library descriptor.
       */
      void *fxLibrary{nullptr};                          //!

   ClassDefOverride(TGo4FitModelFunction,1)
};

#endif // TGO4FITMODELFUNCTION_H
