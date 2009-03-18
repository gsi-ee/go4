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
 *   TH1* histo = GetHistogramSomewhere();
 *   TGo4FitDataHistogram *h = new TGo4FitDataHistogram("hdata", histo, kFALSE);
 *   TGo4FitModelFromData *m = new TGo4FitModelFromData("hmodel", h, kFALSE);
 *
 * The dimensions and bins number on each axis of data object, used in model, should be absolutely the same, as in data object, which should be fitted. Assigned data object will be owned by TGo4FitModelFromData object. But data source object (histogram "histo" in example) may not be owned by object and may be provided later by SetObject() method of fitter.
 *
 *   TGo4Fitter *fitter = GetFitterSomewhere();
 *   TH1* histo = GetHistogramSomewhere();
 *   fitter->SetObject(histo, "hdata");
 *
 * The name of data object "hdata" instead of model object "hmodel" should be used, when assigning data to TGo4FitModelFromData object via SetObject() method of fitter.
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
      TGo4FitModelFromData(const char* iName, TGo4FitData* iDataAsModel = 0, Bool_t Amplitude = kTRUE);

      /**
       * Creates TGo4FitModelFromData object with given name, which uses external histogram as model component.
       * In constructor TGo4FitDataHistogram object will be created, to which histogram will be assigned.
       * Additionally ownership flag for histogram and usage of amplitude can be specified.
       */
      TGo4FitModelFromData(const char* iName, TH1*, Bool_t iOwned = kFALSE, Bool_t Amplitude = kTRUE);

      /**
       * Destroy TGo4FitModelFromData object
       */
      ~TGo4FitModelFromData();

      /**
       * Return pointer on TGo4FitData object, which is used as model component.
       */
      TGo4FitData* GetDataAsModel() const;

      /**
       * Sets pointer on TGo4FitData object with ownership flag.
       */
      void SetDataAsModel(TGo4FitData* iData, Bool_t iOwned);

      /**
       * Add ponter on slot, which should contain TGo4FitData object, to a slots list.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Print information on standard output.
       */
      virtual void Print(Option_t* option) const;

      virtual Bool_t BeforeEval(Int_t);
      virtual Double_t EvalN(const Double_t* v) { return 0; }

      virtual Double_t EvaluateAtPoint(TGo4FitData* data, Int_t nbin, Bool_t UseRanges = kTRUE);
      virtual Double_t EvaluateAtPoint(TGo4FitDataIter* iter, Bool_t UseRanges = kTRUE);
      virtual void AfterEval();

   protected:

      /**
       * Initialize object.
       */
      virtual Bool_t Initialize(Int_t UseBuffers = -1);

      Double_t FindDataPoint(Int_t NumIndexes, const Int_t* Indexes);


      /**
       * Slot for TGo4FitData object, which is used as model component.
       * @label slot for TGo4FitData
       */
      TGo4FitSlot fxData;

   private:
     TGo4FitDataIter* fxIter;       //!

   ClassDef(TGo4FitModelFromData,1)
};
#endif // TGO4FITMODELFROMDATA_H
