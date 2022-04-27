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

#ifndef TGO4FITMINUITRESULT_H
#define TGO4FITMINUITRESULT_H

#include "TNamed.h"
#include "TMatrixD.h"

class TMinuit;
class TArrayD;
class TArrayC;

/**
 * Result values, taken from Minuit object.
 * This object used only to store results from minuit like:
 * Minuit status,
 * Current parameters values and errors,
 * Covariation (error) matrix
 * Contour plot.
 * Name of variables corresponds to Minuit documentation.
 */
class TGo4FitMinuitResult : public TNamed {
   public:
      TGo4FitMinuitResult();
      TGo4FitMinuitResult(const char* iName, const char* iTitle);
      virtual ~TGo4FitMinuitResult();

      void CallMNSTAT(TMinuit* fMinuit);
      void CallMNPOUT(TMinuit* fMinuit, Int_t nPars);
      void CallMNERRS(TMinuit* fMinuit, Int_t nPars);
      void CallMNEMAT(TMinuit* fMinuit, Int_t nPars, Bool_t DoTransform = kTRUE);
      void GetContourPlot(TMinuit* fMinuit);

      void Print(Option_t *option = "") const override;

      // by MNSTAT command
      Double_t FMIN{0.};
      Double_t FEDM{0.};
      Double_t ERRDEF{0.};
      Int_t NPARI{0};
      Int_t  NPARX{0};
      Int_t ISTAT{0};

      // by MNPOUT command
      TArrayD* ParValues{nullptr};
      TArrayD* ParError{nullptr};

      // by MNERRS command
      TArrayD* EPLUS{nullptr};
      TArrayD* EMINUS{nullptr};
      TArrayD* EPARAB{nullptr};
      TArrayD* GLOBCC{nullptr};

      // by MNEMAT command
      TMatrixD* ERRORMATRIX{nullptr};

      // contour plot
      TArrayD* CONTOX{nullptr};
      TArrayD* CONTOY{nullptr};
      TArrayC* CONTOCH{nullptr};

   ClassDefOverride(TGo4FitMinuitResult,1)
};

#endif // TGO4FITMINUITRESULT_H
