#ifndef TGO4FITMODELGAUSS2_H
#define TGO4FITMODELGAUSS2_H

#include "TGo4FitModel.h"

/**
 * Two dimensional gaussian peak.
 *
 * Has following parameters:
 *   "Ampl" - amplitude;
 *   "Pos0" - line position on first coordinate;
 *   "Pos1" - line position on second coordinate;
 *   "Width0" - line width on first coordinate;
 *   "Width1" - line width on second coordinate;
 *   "Cov0_1" - covariation between first and second coordinate.
 * By default, first coordinate associated with x axis, second - with y axis. To create instance of this model:
 *
 *   TGo4FitModelGauss2 *g = new TGo4FitModelGauss2("Gauss", 5., 5., 1., 1., 0.5);
 *
 * where first parameter - name of model component, then initial value for positions, widths and covariation parameters are defined. To assigned coordinates to another axis, two more parameters should be used in the constructor:
 *
 *   TGo4FitModelGauss2 *g = new TGo4FitModelGauss2("Gauss", 5., 5., 1., 1., 0.5, 1, 2);
 *
 * where 1 - assignment of first coordinate to y axis , 2 - assignment of second coordinate to z axis.
 */
class TGo4FitModelGauss2 : public TGo4FitModel {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModelGauss2();

      /**
       * Creates TGo4FitModelGauss2 with specified name.
       * Initial value for positions, widths and covariation parameters can be specified.
       * Also number of selected axis, where 2-dim gaussian will be build, can be specified (default 0 and 1).
       */
      TGo4FitModelGauss2(const char* iName, Double_t iPos1 = 1., Double_t iPos2 = 1.,
                                       Double_t iWidth1 = 1., Double_t iWidth2 = 1., Double_t iCov12 = 0.,
                                       Int_t iNaxis1 = 0, Int_t iNaxis2 = 1);

      /**
       * Destroys TGo4FitModelGauss2 object.
       */
      virtual ~TGo4FitModelGauss2();

      /**
       * Return number of first selected axis.
       */
      Int_t GetNumAxis1() { return fiNaxis1; }

      /**
       * Return number of second selcted axis.
       */
      Int_t GetNumAxis2() { return fiNaxis2; }

      /**
       * Sets number of first selected axis.
       */
      Int_t SetNumAxis1(Int_t n) { return fiNaxis1 = n; }

      /**
       * Sets number of second selected axis.
       */
      Int_t SetNumAxis2(Int_t n) { return fiNaxis2 = n; }

      /**
       * Prints information to standard output.
       */
      virtual void Print(Option_t* option) const;

      virtual Bool_t BeforeEval(Int_t ndim);
      virtual Double_t EvalN(const Double_t* v);

   protected:
      virtual Int_t GetPosParIndex(Int_t naxis) { return (naxis==fiNaxis1) ? 1 : (naxis==fiNaxis2) ? 2 : -1; }
      virtual Int_t GetWidthParIndex(Int_t naxis) { return (naxis==fiNaxis1) ? 3 : (naxis==fiNaxis2) ? 4 : -1; }


      /**
       * Number of first selected axis.
       */
      Int_t fiNaxis1;

      /**
       * Number of second selected axis.
       */
      Int_t fiNaxis2;

   private:

      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_mu1;                //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_mu2;                //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_sig1;               //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_sig2;               //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_ro;                 //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t Par_mult;               //!

   ClassDef(TGo4FitModelGauss2,1)
};
#endif // TGO4FITMODELGAUSS2_H
