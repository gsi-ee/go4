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

#ifndef TGO4FITMODELGAUSSN_H
#define TGO4FITMODELGAUSSN_H

#include "TGo4FitModel.h"

#include "TArrayI.h"
#include "TVectorD.h"
#include "TMatrixD.h"

/**
 * @brief N-dimensional gaussian peak.
 *
 * @details Has following parameters:
 *   - "Ampl" - amplitude;
 *   - "Pos0", "Pos1", ... - line positions;
 *   - "Width0", "Width1", ... - line widths;
 *   - "Cov0_1", "Cov0_2", ..., "Cov1_2", "Cov1_3", ... - covariations parameters.
 *
 * To create instance of this model:
 *
 * ~~~~~~{cpp}
 * auto g = new TGo4FitModelGaussN("Gauss", 2);
 * ~~~~~~
 *
 * where first parameter - name of model component, second - number of dimensions.
 *
 * @ingroup go4_fit
 */
class TGo4FitModelGaussN : public TGo4FitModel {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModelGaussN();

      /**
       * Creates TGo4FitModelGaussN model with given name.
       * Number of dimensions also can be specified (default = 3).
       */
      TGo4FitModelGaussN(const char *iName, Int_t iNDimension = 3);

      /**
       * Destroys TGo4FitModelGaussN object.
       */
      virtual ~TGo4FitModelGaussN();

      /**
       * Returns number of dimensions (number of axes), where gaussian will be build.
       */
      Int_t GetAxisNumbers() const { return fxIndexes.GetSize(); }

      /**
       * Sets number of axes (dimensions), where gaussian will be build. Should be more than 0.
       */
      Bool_t SetAxisNumbers(Int_t naxis);

      /**
       * Gets axis number (0 - x-axis, 1 - y-axis) for specified dimension of gaussian.
       * Can be, that first dimension of gaussian assign to z-axis, 2-nd dimension to x-axis, 3-rd dimension to y-axis.
       * Default (created in constructor) is normal numbering, when number of dimension is equal to number of axis.
       */
      Int_t GetAxisForIndex(Int_t indx) const { return fxIndexes[indx]; }

      /**
       * Change axis number for specified index (dimension) of gaussian.
       */
      void SetAxisForIndex(Int_t indx, Int_t axis) { fxIndexes[indx] = axis; }

      /**
       * Checks duplicates in axis numbers, associated to different gaussian dimensions.
       * leaveaixs (if specified) sets dimension number, which will be unchanged.
       */
      Bool_t ResortIndexes(Int_t leaveaxis = -1);

      /**
       * Fills vector of gaussian position from parameters values.
       */
      void FillMuVector(TVectorD &Mu);

      /**
       * Fill covariance matrix from parameters values.
       */
      void FillSigmaMatrix(TMatrixD &Sigma);

      /**
       * Print information on standard output.
       */
      void Print(Option_t *option = "") const override;

      Bool_t BeforeEval(Int_t) override;
      Double_t EvalN(const Double_t *v) override;

   protected:
      TString GetPosParName(Int_t naxis);
      TString GetWidthParName(Int_t naxis);
      TString GetCovarParName(Int_t naxis1, Int_t naxis2);

      Int_t GetPosParIndex(Int_t naxis) override;
      Int_t GetWidthParIndex(Int_t naxis) override;

      void AfterEval() override;

      /**
       * Array of axis indexes, where n-dim gaussian will be build.
       */
      TArrayI fxIndexes;

   private:

      /**
       * Temporary variable for EvalN() function.
       */
      Int_t Par_ndim{0};                 //!

      /**
       * Temporary variable for EvalN() function.
       */
      Int_t *Par_indx{nullptr};                //!

      /**
       * Temporary variable for EvalN() function.
       */
      TVectorD *Vect_mu{nullptr};          //!

      /**
       * Temporary variable for EvalN() function.
       */
      TMatrixD *Matr_sig{nullptr};         //!

      /**
       * Temporary variable for EvalN() function.
       */
      TVectorD *Vect_x{nullptr};           //!

      /**
       * Temporary variable for EvalN() function.
       */
      TVectorD *Vect_dx{nullptr};          //!

   ClassDefOverride(TGo4FitModelGaussN,1)
};
#endif // TGO4FITMODELGAUSSN_H
