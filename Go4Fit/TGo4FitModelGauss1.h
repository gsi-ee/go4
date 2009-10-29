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

#ifndef TGO4FITMODELGAUSS1_H
#define TGO4FITMODELGAUSS1_H

#include "TGo4FitModel.h"

/**
 * One dimensional gaussian peak.
 *
 * Has foolowing parameters :
 *   "Ampl" - amplitude,
 *   "Pos" - position of gaussian peak,
 *   "Width" - width of gaussian.
 * In constructor initial values of these parameter and number of selected axis (0 - x axis, 1 - y axis and so on) should be setup:
 *
 *   TGo4FitModelGauss1 *g = new TGo4FitModelGauss1("Gauss", 10., 5., 1);
 *
 * where "Gauss" - name of model component, "10." - peak position, "5." - peak width, "1" - selected axis (here - y).
 */
class TGo4FitModelGauss1 : public TGo4FitModel {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModelGauss1();

      /**
       * Creates TGo4FitModelGauss1 with given name.
       * Position of gaussian, width and number of selected axis can be specified.
       */
      TGo4FitModelGauss1(const char* iName, Double_t iPosition = 1., Double_t iWidth = 1., Int_t iNaxis = 0);

      /**
       * Destroys TGo4FitModelGauss1 object.
       */
      virtual ~TGo4FitModelGauss1();

      /**
       * Return number of axis, where gauss will be build.
       */
      Int_t GetNumAxis() { return fiNaxis; }

      /**
       * Sets number of selected axis.
       */
      void SetNumAxis(Int_t n) { if(n>=0) fiNaxis = n; }

      /**
       * Print information to standard output.
       */
      virtual void Print(Option_t* option) const;

      virtual Double_t Integral();

      virtual Bool_t BeforeEval(Int_t ndim);
      virtual Double_t EvalN(const Double_t* v);

   protected:
      virtual Int_t GetPosParIndex(Int_t naxis) { return (naxis==fiNaxis) ? 1 : -1; }
      virtual Int_t GetWidthParIndex(Int_t naxis) { return (naxis==fiNaxis) ? 2 : -1; }


      /**
       * Number of selected axis, where gaussian wil be build.
       */
      Int_t fiNaxis;

   private:

      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_x0;            //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_k;             //!

   ClassDef(TGo4FitModelGauss1,1)
};
#endif // TGO4FITMODELGAUSS1_H
