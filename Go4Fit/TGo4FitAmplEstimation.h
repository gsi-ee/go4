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

#ifndef TGO4FITAMPLESTIMATION_H
#define TGO4FITAMPLESTIMATION_H

#include "TGo4FitterAction.h"

class TGo4Fitter;

/**
 * Amplitude estimation action
 * In additional to general minimization routine very useful amplitude estimation algorithm can be used. If rest of models parameters have good initial estimation, the amplitude parameter can be defined by solving of system of linear equations, as described in theoretical preface part of tutorial. This algorithm is provided by TGo4FitAmplEstimation class. This action can be added by AddAmplEstimation() routine of fitter. Typically, this action should be added before minimization routine.
 */
class TGo4FitAmplEstimation : public TGo4FitterAction {
   public:

      /**
       * Default constructor.
       */
      TGo4FitAmplEstimation();

      /**
       * Creates TGo4FitAmplEstimation action with given name.
       * Optinally, default number of iterations can be sets up (default 1).
       */
      TGo4FitAmplEstimation(const char* Name, Int_t NumIters = 1);

      /**
       * Destroys TGo4FitAmplEstimation object.
       */
      virtual ~TGo4FitAmplEstimation();

      /**
       * Returns number of iterations.
       */
      Int_t GetNumIterations() { return fiNumIters; }

      /**
       * Sets number of iterations.
       */
      void SetNumIterations(Int_t iNumIters) { fiNumIters = iNumIters; }

      virtual Bool_t NeedBuffers() const { return kTRUE; }

      /**
       * Perform amplitude estimations.
       * See Go4Fit tutorial to more details.
       */
      virtual void DoAction(TGo4FitterAbstract* Fitter);

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;
    private:
       Double_t PointWeight(Int_t niter, Int_t FFtype, Double_t value, Double_t modelvalue, Double_t standdev);
       Bool_t CalculateWithBuffers(TGo4Fitter* fitter);
       Bool_t CalculateWithIterators(TGo4Fitter* fitter);

       /**
        * Number of iterations, used for amplitude estimation.
        */
       Int_t fiNumIters;

   ClassDef(TGo4FitAmplEstimation,1)
};

#endif // TGO4FITAMPLESTIMATION_H
