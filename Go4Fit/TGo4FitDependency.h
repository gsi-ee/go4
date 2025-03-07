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

#ifndef TGO4FITDEPENDENCY_H
#define TGO4FITDEPENDENCY_H

#include "TObject.h"
#include "TString.h"

class TFormula;

/**
 * @brief Object, used to store dependency of one parameter from other, calculated via expression.
 *
 * @details It used in dependencies calculations, in parameters initialization and in result calculation.
 * Value can be calculated via expression or be just a double constant.
 * The result parameter is sets via it's index in fitter parameters list.
 *
 * @ingroup go4_fit
 */
class TGo4FitDependency : public TObject {
   public:
      TGo4FitDependency();
      TGo4FitDependency(const char *iParameter, const char *iExpression);
      TGo4FitDependency(const char *iParameter, Double_t InitValue);
      virtual ~TGo4FitDependency();

      void SetParameter(const char *iParameter);
      void SetInitValue(Double_t InitValue);
      void SetExpression(const char *iExpression);

      const TString &GetParameter() const { return fxParameter; }
      const TString &GetExpression() const { return fxExpression; }
      Double_t GetInitValue() const { return fdInitValue; }

      Bool_t IsResultDepend() const { return fxParameter.IsNull(); }
      Bool_t IsInitValue() const { return fxExpression.IsNull(); }

      void Initialize(Int_t iNumPar, const char *iFormula);
      Double_t ExecuteDependency(Double_t *Params);
      void Finalize();

      void Print(Option_t *option = "") const override;

   protected:
      TString fxParameter;
      TString fxExpression;
      Double_t fdInitValue{0.};
      Int_t fiNumPar{0};                  //!
      TFormula *fxFormula{nullptr};       //!

   ClassDefOverride(TGo4FitDependency,1)
};

#endif // TGO4FITDEPENDENCY_H
