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

#ifndef TGO4FITMODELFROMDATA_H
#define TGO4FITMODELFROMDATA_H

#include "TGo4FitModel.h"

#include "TGo4FitSlot.h"

class TGo4FitData;
class TH1;

/**
 * Model object, which is uses TGo4FitData object to produce model bins.
 *
 * In constructor one should just specify data object (it may be TGo4FitDataHistogram or other), which will be used as model. Optionally, amplitude parameter can be created. For instance:
 *
 * ~~~{cpp}
 *   TH1 *histo = GetHistogramSomewhere();
 *   TGo4FitDataHistogram *h = new TGo4FitDataHistogram("hdata", histo, kFALSE);
 *   TGo4FitModelFromData *m = new TGo4FitModelFromData("hmodel", h, kFALSE);
 * ~~~
 *
 * The dimensions and bins number on each axis of data object, used in model, should be absolutely the same, as in data object, which should be fitted. Assigned data object will be owned by TGo4FitModelFromData object. But data source object (histogram "histo" in example) may not be owned by object and may be provided later by SetObject() method of fitter.
 *
 * ~~~{cpp}
 *   TGo4Fitter *fitter = GetFitterSomewhere();
 *   TH1 *histo = GetHistogramSomewhere();
 *   fitter->SetObject(histo, "hdata");
 * ~~~
 *
 * The name of data object "hdata" instead of model object "hmodel" should be used, when assigning data to TGo4FitModelFromData object via SetObject() method of fitter.
 *
 * @ingroup go4_fit
 */
class TGo4FitModelFromData : public TGo4FitModel {
   public:

      /**
       * Default constructor.
       */
      TGo4FitModelFromData();

      /**
       * Creates TGo4FitModelFromData object with given name.
       * Pointer on TGo4FitData object and usage of amplitude can be specified.
       */
      TGo4FitModelFromData(const char *iName, TGo4FitData *iDataAsModel = nullptr, Bool_t Amplitude = kTRUE);

      /**
       * Creates TGo4FitModelFromData object with given name, which uses external histogram as model component.
       * In constructor TGo4FitDataHistogram object will be created, to which histogram will be assigned.
       * Additionally ownership flag for histogram and usage of amplitude can be specified.
       */
      TGo4FitModelFromData(const char *iName, TH1 *, Bool_t iOwned = kFALSE, Bool_t Amplitude = kTRUE);

      /**
       * Destroy TGo4FitModelFromData object
       */
      ~TGo4FitModelFromData();

      /**
       * Return pointer on TGo4FitData object, which is used as model component.
       */
      TGo4FitData *GetDataAsModel() const;

      /**
       * Sets pointer on TGo4FitData object with ownership flag.
       */
      void SetDataAsModel(TGo4FitData *iData, Bool_t iOwned);

      /**
       * Add pointer on slot, which should contain TGo4FitData object, to a slots list.
       */
      void FillSlotList(TSeqCollection *list) override;

      /**
       * Print information on standard output.
       */
      void Print(Option_t *option = "") const override;

      Bool_t BeforeEval(Int_t) override;
      Double_t EvalN(const Double_t*) override { return 0.; }
      void AfterEval() override;

      Double_t EvaluateAtPoint(TGo4FitData *data, Int_t nbin, Bool_t UseRanges = kTRUE) override;
      Double_t EvaluateAtPoint(std::unique_ptr<TGo4FitDataIter> &iter, Bool_t UseRanges = kTRUE) override;

   protected:

      /**
       * Initialize object.
       */
      Bool_t Initialize(Int_t UseBuffers = -1) override;

      Double_t FindDataPoint(Int_t NumIndexes, const Int_t *Indexes);

      /**
       * Slot for TGo4FitData object, which is used as model component.
       */
      TGo4FitSlot fxData;

   private:
      std::unique_ptr<TGo4FitDataIter> fxIter;     //!

   ClassDefOverride(TGo4FitModelFromData,1)
};
#endif // TGO4FITMODELFROMDATA_H
