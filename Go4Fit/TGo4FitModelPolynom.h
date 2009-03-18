#ifndef TGO4FITMODELPOLYNOM_H
#define TGO4FITMODELPOLYNOM_H

#include "TGo4FitModel.h"

class TArrayD;

/**
 * Model objects, which reproduce component of polynomial function.
 *
 * Expession can be written like:
 *
 * Ampl * x^nx * y^ny * z^nz * ...
 *
 * The order of polynomial function should be sets up in constructor like:
 *
 *   TGo4FitModelPolynom *p1 = new TGo4FitModelPolynom("Pol1",orderx,ordery,orderz);
 *   or
 *   TArrayD orders(5);
 *   Orders[0] = 1.; Orders[1] = 0.; ...
 *   TGo4FitModelPolynom *p2 = new TGo4FitModelPolynom("Pol2",Orders);
 *
 * According to number of parameters in constructor TGo4FitModelPolynom has set of parameters "Order0", "Order1" and so on, representing polynom orders for axis x, y and so on correspondently. By default, these parameters are fixed and not fitted in optimizations. To change this default behavior, use:
 *
 * p1->FindPar("Order0")->SetFixed(kFALSE);
 *
 * TGo4FitModelPolynom class always has amplitude parameter, named "Ampl". It can be accessed by its name, for instance:
 *
 * p1->FindPar("Ampl")->SetValue(1000.);
 *
 * or
 *
 * p1->GetAmplitudePar("Ampl")->SetValue(1000.);
 *
 * GetAmplitudePar() method can be used in other models classes only if they create amplitude parameters, otherwise method returns 0.
 */
class TGo4FitModelPolynom : public TGo4FitModel {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModelPolynom();

      /**
       * Creates TGo4FitModelPolynom object with given name for 1-dim case.
       * The order of polynom for x axis can be specified (0 by default).
       */
      TGo4FitModelPolynom(const char* iName, Double_t iOrderX = 0.);

      /**
       * Creates TGo4FitModelPolynom object with given name for 2-dim case.
       * The order of polynom for x and y axises should be specified.
       */
      TGo4FitModelPolynom(const char* iName, Double_t iOrderX, Double_t iOrderY);

      /**
       * Creates TGo4FitModelPolynom object with given name for 3-dim case.
       * The order of polynom for x, y and z axises should be specified.
       */
      TGo4FitModelPolynom(const char* iName, Double_t iOrderX, Double_t iOrderY, Double_t iOrderZ);

      /**
       * Creates TGo4FitModelPolynom object with given name for n-dim case.
       * An array of polynoms orders for appropriate axis should be specified. The size of array will be a dimensions size of polynom. But the n-dim polynom can be used for m-dim case. If n>m, orders for missing axis will be set 0. If n<m, only necessary orders will be used.
       */
      TGo4FitModelPolynom(const char* iName, const TArrayD& iPolynomOrders);

      /**
       * Destroys TGo4FitModelPolynom object.
       */
      virtual ~TGo4FitModelPolynom();

      /**
       * Get polynom order for specified axis.
       */
      Double_t GetPolynomOrder(Int_t naxis);

      /**
       * Returns maximum axis number (dimensions number) in polynom.
       */
      Int_t GetMaxNumAxis();

      /**
       * Sets number of dimensions (maximum axis numbers) in polynom.
       */
      Bool_t SetMaxNumAxis(Int_t numaxis);

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;

      virtual Bool_t BeforeEval(Int_t NDimension);
      virtual Double_t EvalN(const Double_t* v);
      virtual void AfterEval();


   protected:
      TString GetOrderParName(Int_t naxis);


   private:
      void CreateOrdersPars(const Double_t* orders, Int_t startaxis, Int_t maxaxis, Int_t AtIndx = -1);

      /**
       * Temporary variable for EvalN() function.
       */
      Int_t Par_ndim;                       //!


      /**
       * Temporary variable for EvalN() function.
       */
      TArrayD* fxAllOrders;             //!


      /**
       * Temporary variable for EvalN() function.
       */
      Double_t* Par_orders;                 //!

   ClassDef(TGo4FitModelPolynom,1)
};
#endif // TGO4FITMODELPOLYNOM_H
