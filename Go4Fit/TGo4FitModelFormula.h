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

#ifndef TGO4FITMODELFORMULA_H
#define TGO4FITMODELFORMULA_H

#include "TGo4FitModel.h"

class TFormula;

/**
 * Model object, which uses ROOT TFormula class facility.
 *
 * Any kind of one-line expression can be analyzed by TFormula object and evaluated for given set of axis values and set of parameters. TGo4FitModelFormula in constructor creates additional parameters with names "Par0", "Par1" and so on, which can be used in equation and can be optimized. Optionally amplitude parameters with name "Ampl" can be created. In constructor expression, number of additional parameters and using of amplitude parameter should be specified. Fort instance, equation with 3 parameters and amplitude:
 *
 *   TGo4FitModelFormula *f = new TGo4FitModelFormula("Form","(x-Par0)*(y-Par1)*(z-Par2)", 3, kTRUE);
 */
class TGo4FitModelFormula : public TGo4FitModel {
   public:

      /**
       * Default constructor
       */
      TGo4FitModelFormula();

      /**
       * Create TGo4FitModelFormula object with given names.
       * Formula expression and number of parameters can be sets up. All using of amplitude parameter can be invoked.
       */
      TGo4FitModelFormula(const char* iName, const char* iExpressionStr = "", Int_t iNPars = 0, Bool_t AddAmplitude = kFALSE);

      /**
       * Destroys TGo4FitModelFormula object.
       */
      virtual ~TGo4FitModelFormula();

      /**
       * Signals that ampltide parameter can be removed from or introdeced to object.
       */
      virtual Bool_t CanAmplTouch() { return kTRUE; }

      /**
       * Return formula expression.
       */
      const char* GetExpression() { return fxExpression.Data(); }

      /**
       * Sets formula expression.
       */
      void SetExpression(const char* expr) { fxExpression = expr; }

      /**
       * Get number of parameters, which can be used in exprssion.
       */
      Int_t GetNumberOfExprPar();

      /**
       * Sets number of parameters, which can be used in expression.
       */
      Bool_t SetNumberOfExprPar(Int_t num);

      void SetPosParIndex(Int_t naxis, Int_t indx = -1);
      void SetWidthParIndex(Int_t naxis, Int_t indx = -1);

      virtual Bool_t BeforeEval(Int_t ndim);
      virtual Double_t EvalN(const Double_t* v);
      virtual void AfterEval();

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      virtual Bool_t Initialize(Int_t UseBuffers = -1);
      virtual void Finalize();

      TGo4FitParameter* GetExprPar(Int_t n);
      TString GetExprParName(Int_t n);

      Bool_t CompileFormula();
      void CloseFormula();

      virtual Int_t GetPosParIndex(Int_t naxis);
      virtual Int_t GetWidthParIndex(Int_t naxis);

      /**
       * String, containig formula expression.
       */
      TString fxExpression;

      TArrayI fxPosIndex;

      TArrayI fxWidthIndex;

      /**
       * formula for evaluation expression in EvalN() method.
       */
      TFormula *fxFormula;               //!
   private:

      /**
       * temporary variable for formula calculation in EvalN() method.
       */
      Int_t Par_ndim;                  //!

   ClassDef(TGo4FitModelFormula,1)
};
#endif // TGO4FITMODELFORMULA_H
