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

#ifndef TGO4FITPEAKFINDER_H
#define TGO4FITPEAKFINDER_H

#include "TGo4FitterAction.h"

#include "TArrayD.h"

class TGo4Fitter;
class TGo4FitData;

/**
 * Output action
 * To add some output to actions, TGo4FitterOutput action class should be used. In constructor output command and options (if required) should be specified. Also AddOuputAction() routine of fitter can be used. Now following commands are available:
 * "Print". Possible options are: "*"- print all internal objects, "**" - print also objects in slots, "Pars" - print parameters values, "Ampls" - only amplitude parameters, "Results" - result values.
 * "Draw". Draw of the fitter data and model objects. As option name of data object and its components should be sets up. For instance, "data1,Gauss1,Gauss2". If first character will be "#", Draw command creates new canvas for output.
 */
class TGo4FitPeakFinder : public TGo4FitterAction {
   public:

      /**
       * Default constructor.
       */
      TGo4FitPeakFinder();

      /**
       * Creates TGo4FitPeakFinder action with provided name and, ,
       * (optionally), DataName, clear models mode.
       */
      TGo4FitPeakFinder(const char *Name, const char *DataName = nullptr, Bool_t ClearModels = kFALSE, Int_t PolOrder = -1);

      /**
       * Destroys TGo4FitterOutput object.
       */
      virtual ~TGo4FitPeakFinder();

      void SetDataName(const char *name) { fxDataName = name; }
      const char *GetDataName() const { return fxDataName.Data(); }
      void SetClearModels(Bool_t clear) { fbClearModels = clear; }
      Bool_t GetClearModels() const { return fbClearModels; }


      void SetupPolynomialBackground(Int_t PolynomOrder);

      void SetupForFirst(Double_t MaxAmplFactor, Double_t MinWidth, Double_t MaxWidth);

      void SetupForSecond(Double_t LineWidth);

      void SetupForThird(Double_t NoiseFactor, Double_t NoiseMinimum, Int_t ChannelSum);

      void DoAction(TGo4FitterAbstract *Fitter) override;

      Bool_t CanChangeFitter() const override { return kTRUE; }

      // Following functions directly changes single parameters of peakfinders.
      // Normally you should not use them, because some or all of them can be changed later on
      // You should use Setup... routines to configure specific peakfinder

      void SetUsePolynom(Bool_t use) { fbUsePolynom = use; }
      Bool_t GetUsePolynom() const { return fbUsePolynom; }
      void SetPolynomOrder(Int_t order) { fiPolynomOrder = order; }
      Int_t GetPolynomOrder() const { return fiPolynomOrder; }

      void SetPeakFinderType(Int_t typ) { fiPeakFinderType = typ; }
      Int_t GetPeakFinderType() const { return fiPeakFinderType; }

      void Set0MaxAmplFactor(Double_t factor) { fd0MaxAmplFactor = factor; }
      Double_t Get0MaxAmplFactor() const { return fd0MaxAmplFactor; }
      void Set0MinWidth(Double_t min) { fd0MinWidth = min; }
      Double_t Get0MinWidth() const { return fd0MinWidth; }
      void Set0MaxWidth(Double_t max) { fd0MaxWidth = max; }
      Double_t Get0MaxWidth() const { return fd0MaxWidth; }

      void Set1LineWidth(Double_t width) { fd1LineWidth = width; }
      Double_t Get1LineWidth() const { return fd1LineWidth; }

      void Set2NoiseFactor(Double_t factor) { fd2NoiseFactor = factor; }
      Double_t Get2NoiseFactor() const { return fd2NoiseFactor; }
      void Set2NoiseMinimum(Double_t min) { fd2NoiseMinimum = min; }
      Double_t Get2NoiseMinimum() const { return fd2NoiseMinimum; }
      void Set2ChannelSum(Int_t sum) { fi2ChannelSum = sum; }
      Int_t Get2ChannelSum() const { return fi2ChannelSum; }

      /**
       * Print information on standard output.
       */
      void Print(Option_t *option = "") const override;

    private:

      void ROOTPeakFinder(TGo4Fitter *fitter,
                          TGo4FitData* data,
                          Int_t PolynomOrder,
                          Double_t Sigma);

      /**
       * Perform simple peak finder algorithm.
       * Seeks for gaussians with amplitude more than given threshold (AmplThreshold introduced relative to maximum data value, from 0 to 1),
       * with width in range between MinWidth and MaxWidth. Polynomial approximation can be applied for background approximation(PolOrder >= 0) or not used (PolOrder<0).
       * Only selected by range conditions data part will be used by peak finder.
       * Appropriate number of gaussians will be placed in list of models and associate with data.
       */
      void SergeyLinevPeakFinder(TGo4Fitter *fitter,
                                 TGo4FitData* data,
                                 Int_t PolOrder,
                                 Double_t AmplThreshold,
                                 Double_t MinWidth,
                                 Double_t MaxWidth);

      /**
       * Hans Essel (c) peak finder.
       *
       */
      void HansEsselPeakFinder(TGo4Fitter *fitter,
                               TGo4FitData* data,
                               Int_t MaxNumPeaks = 50,
                               Int_t ChannelSum = 1,
                               Double_t NoiseFactor = 2.,
                               Double_t NoiseMinimum = 10.,
                               Int_t MinimasOrder = -1);

      static void DefinePolynom(Int_t size,                    // number of experimental points
                                Double_t *bins,                // array of bins
                                Double_t *scales,              // array of scale values
                                TArrayD& Coef,                 // array of polynomial coefficients
                                Double_t *weight = nullptr,    // statistical weight of each bin
                                Double_t *backgr = nullptr,    // background values
                                Char_t *use = nullptr);        // usage of each points

      static void DefinePolynomEx(Int_t size, Double_t *bins, Double_t *scales, Double_t *weight, Double_t *backgr,
                                  Int_t lbound, Int_t rbound, TArrayD& Coef);


      static Double_t CalcPolynom(const TArrayD& Coef, Double_t x);


      Int_t fiPeakFinderType{0};   // 0 - by S.Linev,  1 - ROOT TSpectrum, 2 - by H.Essel

      TString fxDataName;

      Bool_t fbClearModels{kFALSE};

      Bool_t fbUsePolynom{kFALSE};
      Int_t  fiPolynomOrder{0};

      Double_t fd0MinWidth{0};
      Double_t fd0MaxWidth{0};
      Double_t fd0MaxAmplFactor{0};

      Double_t fd1LineWidth{0};

      Double_t fd2NoiseFactor{0};
      Double_t fd2NoiseMinimum{0};
      Int_t  fi2ChannelSum{0};

   ClassDefOverride(TGo4FitPeakFinder,1)
};

#endif // TGO4FITPEAKFINDER_H
