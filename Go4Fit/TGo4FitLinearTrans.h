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

#ifndef TGO4FITLINEARTRANS_H
#define TGO4FITLINEARTRANS_H

#include "TGo4FitAxisTrans.h"

#include "TGo4FitParameter.h"

/**
 * @brief The linear transformation of selected data axis.
 *
 * @details It has two coefficients to transfrom axis values like:
 * ~~~
 *   scale[naxis] = CoefK * scale[naxis] + CoefB
 * ~~~
 *
 * After linear transformation object is constructed, the coefficients can be defined by three ways:
 *
 * - SetCalibration() - just setting these coefficients;
 *
 * - SetCalibrationByRange() - coefficients defines from range and number of points in this range;
 *
 * - SetCalibrationByPoints() - coefficients calculates from coordinates of two points, where first coordinate is channel number and second - axis value.
 *
 * @ingroup go4_fit
 */
class TGo4FitLinearTrans : public TGo4FitAxisTrans {
   public:
      TGo4FitLinearTrans();
      TGo4FitLinearTrans(const char *iName, const char *iTitle, Int_t iAxis = 0);
      virtual ~TGo4FitLinearTrans();

      Int_t GetAxis() const { return fiAxis; }
      void SetAxis(Int_t iAxis) { fiAxis = iAxis; }
      Double_t GetCoefK() { return GetPar(0)->GetValue(); }
      void SetCoefK(Double_t k) { GetPar(0)->SetValue(k); }
      Double_t GetCoefB() { return GetPar(1)->GetValue(); }
      void SetCoefB(Double_t b) { GetPar(1)->SetValue(b); }

      void SetCoef(Double_t iCoefK, Double_t iCoefB)
        { GetPar(0)->SetValue(iCoefK); GetPar(1)->SetValue(iCoefB); }
      void SetCoefByRange(Int_t nbins, Double_t y1, Double_t y2);
      void SetCoefByPoints(Double_t n1, Double_t y1, Double_t n2, Double_t y2);

      void Transformation(Double_t *scales, Int_t naxis) override;

      void Print(Option_t *option = "") const override;

   protected:
      Int_t fiAxis{0};
      Double_t fdCoefK{0};
      Double_t fdCoefB{0};

   ClassDefOverride(TGo4FitLinearTrans,1)
};

#endif // TGO4FITLINEARTRANS_H
