#ifndef TGO4FITLINEARTRANS_H
#define TGO4FITLINEARTRANS_H

#include "TGo4FitAxisTrans.h"

#include "TGo4FitParameter.h"

/**
 * The linear transfromation of selected data axis.
 *
 * It has two coefficients to transfrom axis values like:
 *
 *   scale[naxis] = CoefK * scale[naxis] + CoefB
 *
 * After linear transformation object is constructed, the coefficients can be defined by three ways:
 *
 * SetCalibration() - just setting these coefficients;
 *
 * SetCalibrationByRange() - coefficients defines from range and number of points in this range;
 *
 * SetCalibrationByPoints() - coefficients calculates from coordinates of two points, where first coordinate is channel number and second - axis value.
 */
class TGo4FitLinearTrans : public TGo4FitAxisTrans {
   public:
      TGo4FitLinearTrans();
      TGo4FitLinearTrans(const char* iName, const char* iTitle, Int_t iAxis = 0);
      virtual ~TGo4FitLinearTrans();

      Int_t GetAxis() { return fiAxis; }
      void SetAxis(Int_t iAxis) { fiAxis = iAxis; }
      Double_t GetCoefK() { return GetPar(0)->GetValue(); }
      void SetCoefK(Double_t k) { GetPar(0)->SetValue(k); }
      Double_t GetCoefB() { return GetPar(1)->GetValue(); }
      void SetCoefB(Double_t b) { GetPar(1)->SetValue(b); }

      void SetCoef(Double_t iCoefK, Double_t iCoefB)
        { GetPar(0)->SetValue(iCoefK); GetPar(1)->SetValue(iCoefB); }
      void SetCoefByRange(Int_t nbins, Double_t y1, Double_t y2);
      void SetCoefByPoints(Double_t n1, Double_t y1, Double_t n2, Double_t y2);

      virtual void Transformation(Double_t* scales, Int_t naxis);

      virtual void Print(Option_t* option) const;

   protected:
      Int_t fiAxis;
      Double_t fdCoefK;
      Double_t fdCoefB;

   ClassDef(TGo4FitLinearTrans,1)
};
#endif // TGO4FITLINEARTRANS_H
