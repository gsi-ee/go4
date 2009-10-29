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

#ifndef TGO4FITDATA_H
#define TGO4FITDATA_H

#include "TGo4FitComponent.h"

#include "TArrayI.h"
#include "TArrayD.h"

class TH1;
class TGraph;
class TGo4FitSlot;

class TGo4FitAxisTrans;
class TGo4FitDataIter;

/**
 * Basic abtract class for representing data, which should be fitted.
 */
class TGo4FitData : public TGo4FitComponent {
   public:
      enum { dtHistogram = 1, dtGraph = 2 };

      /**
       * Default constructor.
       */
      TGo4FitData();

      /**
       * Creates TGo4FitData object with given name and title.
       * Type of data (histogramic or points) and usage of amplitude can be specified.
       * TGo4FitData object should not be created directly. Implementation of this class like TGo4FitDataHistogram,
       * TGo4FitDataGraph and so on should be used.
       */
      TGo4FitData(const char* iName, const char* iTitle, Int_t iDataType = dtHistogram, Bool_t AddAmpl = kFALSE);

      /**
       * Destroys TGo4FitData object.
       */
      virtual ~TGo4FitData();

      /**
       * Returns type of data source. 0 - histogramic, 1 - points (scale values have no width)
       */
      const Int_t GetDataType() { return fiDataType; }

      /**
       * Returns kTRUE, if binary numbers used as scale values.
       */
      Bool_t GetUseBinScale() { return fbUseBinScale; }

      /**
       * If kTRUE, binary numbers will be used as scale values.
       */
      void SetUseBinScale(Bool_t iUseBinScale) { fbUseBinScale=iUseBinScale; }

      /**
       * Always return kTRUE.
       * This specify that any data object may has or hasnot amplitude.
       * Amplitude parameter (if exist) will be used to scale bins values.
       */
      virtual Bool_t CanAmplTouch() { return kTRUE; }

      /**
       * Return source of sigma values.
       * Possible values are:
       *   0 - no sigma sources;
       *   1 - sigma should be taken from data source object;
       *   2 - sigma is constant and defined by GetSigmaValue() funcion.
       */
      Int_t GetSigmaSource() const { return fiTakeSigmasFrom; }

      /**
       * Return constant sigma value.
       * Uses, when GetSigmaSource() returns 2.
       */
      Double_t GetSigmaValue() const { return fdSigmaValue; }

      /**
       * Sets source of sigma for data.
       * For description see GetSigmaSource() function.
       */
      void SetSigmaSource(Int_t TakeFrom, Double_t SigmaValue = 1.)
         { fiTakeSigmasFrom = TakeFrom; if ((TakeFrom==2) && (SigmaValue>0)) fdSigmaValue = SigmaValue; }

      /**
       * Set constant sigma value.
       * Value wil be used, if GetSigmaSource()==2.
       */
      void SetSigmaValue(Double_t SigmaValue) { fdSigmaValue = SigmaValue; }

      /**
       * Sets limit to exclude bins, which not less then this limit.
       */
      void SetExcludeLessThen(Double_t limit = 0.) { fdExcludeLessThen = limit; }

      /**
       * Returns limit, which uses to exclude bins, less then this limit.
       */
      Double_t GetExcludeLessThen() const { return fdExcludeLessThen; }

      /**
       * Sets number of slots for scale transformation objects.
       * By default, TGo4FitData has no place (slots) for transformation objects.
       * In this case scale values can be taken from data source (TAxis of TH1) or just bins numbers can be used.
       * To use transformation of these values, inherited from TGo4FitAxisTrans class should be used.
       * Slots fot such objects can be reserved first.
       */
      Bool_t SetNumberOfTransSlots(Int_t nslots);

      /**
       * Return number of slots for scale transformation objects.
       */
      Int_t GetNumberOfTransSlots() { return fxAxisTrans.GetLast()+1; }

      /**
       * Return slot for transformation object.
       */
      TGo4FitSlot* GetAxisTransSlot(Int_t nslot);

     /**
       * Return transformation object for given slot.
       */
      TGo4FitAxisTrans* GetAxisTrans(Int_t nslot);

      /**
       * Sets transformation object for given slot.
       * If not enougth slots was reserved, SetNumberOfTransSlots(nslot+1) will be called first.
       */
      void SetAxisTrans(Int_t nslot, TGo4FitAxisTrans* Trans, Bool_t TransOwned = kFALSE);

      /**
       * Add transformation object to data.
       * First new slot reserved for this object.
       */
      void AddAxisTrans(TGo4FitAxisTrans* Trans, Bool_t TransOwned = kFALSE);

      /**
       * Specified, when iNeeded = kTRUE, that trsnaformation object should always be provided to data.
       */
      void SetAxisTransNeeded(Int_t nslot, Bool_t iNeeded = kFALSE);

      /**
       * Return kTRUE, if either initial data axis or data bins are transformed by TGo4FitData object.
       * For instance, if SetUseBinScales() is used.
       */
      virtual Bool_t IsAnyDataTransform();

      /**
       * Creates iterator for data object.
       * For detailed description see TGo4FitDataIter class.
       */
      virtual TGo4FitDataIter* MakeIter() { return 0; }

      /**
       * Creates object, which can be drawn on canvas by ROOT.
       * This method creates either TH1 (in case of histogramic data) or TGraph object and fills bins by content of data.
       */
      TObject* CreateDrawObject(const char* ObjName);

      /**
       * Return scales minimum and maximum for specified axis.
       * Can be used before or after Initialize() method.
       */
      Bool_t DefineScaleMinMax(Int_t naxis, Double_t& min, Double_t& max);

      /**
       * Define dimension number of data
       * Create iterator and checks number of dimension
       */
      Int_t DefineDimensions();

      /**
       * Define number of selected bins in data
       * Create iterator and secuantially checks all points
       */
      Int_t DefineBinsSize();

      /**
       * Checks, if data has same dimensions number and size of each dimensions.
       * Used, when one data objects used to be a model component of another data object.
       * Should be used only after Initialize() method for current data object.
       */
      Bool_t IsCompatibleData(TGo4FitData* data);

      /**
       * Exclude points from model according model range conditions.
       * Primary range conditions defines for data. No any model, associated to this data, can exceed it's ranges.
       * But not full data range can be used for explicit model evaluation. This functions fill model mask (array of chars) inside data range, where model should be evaluated.
       * Can be used only after Initialize() method.
       */
      void ApplyRangesForModelMask(TGo4FitComponent* model, Char_t* ModelMask);

     /**
       * Copy pointers on all slots of data object to list.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Display information about data object on standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      friend class TGo4FitModel;
      friend class TGo4Fitter;

      /**
       * Initialize data object.
       * This routine should be used to allocate memory for internal buffers.
       * Its automatically calls in DoActions() routine of fitter.
       * if UseBuffers>0 or ( GetUseBuffers() && UseBuffers<0) internal buffers will be created.
       * Concuming more memory may dramaticaly decrease time for fitting.
       */
      virtual Bool_t Initialize(Int_t UseBuffers = -1);

      /**
       * Removes all buffers, created in initialize() routine.
       */
      virtual void Finalize();

      /**
       * Checks, if buffers allocated for data.
       */
      Bool_t BuffersAllocated() const { return (fxValues!=0) && (fxFullScale!=0); }

      /**
       * Return number of data bins in buffers.
       */
      Int_t GetBinsSize() const { return fiBinsSize; }

      /**
       * Returns number of axis values for each point.
       * Can be used after Initialize() method, otherwise TGo4FitDataIter::ScalesSize() method should be used.
       */
      Int_t GetScalesSize() const { return fiScalesSize; }

      /**
       * Returns pointer on buffer with data bins values.
       * Buffer has GetBinsSize() values. Return 0, if buffers were not allocated.
       */
      Double_t* GetBinsValues() { return fxValues; }

      /**
       * Returns pointer on buffer with standard deviations for each data bins.
       * Buffer has GetBinsSize() values. Return 0, if buffers were not allocated.
       */
      Double_t* GetBinsDevs() { return fxStandDev; }

      /**
       * Returns pointer on buffer with complete model of data bins.
       * Buffer has GetBinsSize() values. Return 0, if buffers were not allocated.
       */
      Double_t* GetBinsResult() { return fxBinsResult; }

      /**
       * Return scale values for specified index from buffer.
       * Can be used only if buffers were allocated, otherwise return 0. nbin should be from 0 to GetBinsSize()-1
       */
      const Double_t* GetScaleValues(const Int_t nbin);

      /**
       * Return scales width values for specified index from buffer.
       * Can be used only if buffers were allocated, otherwise return 0. nbin should be from 0 to GetBinsSize()-1
       */
      const Double_t* GetWidthValues(const Int_t nbin);

      /**
       * Return indexes values for specified data bin from buffer.
       * Can be used only if buffers were allocated, otherwise return 0. nbin should be from 0 to GetBinsSize()-1
       */
      const Int_t* GetFullIndex(Int_t nbin);

      /**
       * Returns dimension of indexes arrays.
       * Can be used after Initialize() method, otherwise TGo4FitDataIter::IndexesSize() method should be used.
       */
      Int_t GetIndexesSize() const { return fiIndexesSize; }

      /**
       * Specified type of data:
       *  0 - Histogram, 1 - graphics, 2 and so on - user defined
       */
      Int_t fiDataType;

      /**
       * Use binary numbers as scale values.
       * if this flag set, bin numbers are used as scale values
       */
      Bool_t fbUseBinScale;

      /**
       * Specify sigma source.
       * 0 - no sigmas, 1 -  try take from data, 2 - const value (fdSigmaValue)
       */
      Int_t fiTakeSigmasFrom;

      /**
       * Value of sigma when fiTakeSigmasFrom = 2
       */
      Double_t fdSigmaValue;

      /**
       * Sets limit for exclude bins, which less then this limit
       */
      Double_t fdExcludeLessThen;


      /**
       * Array of slots for scale transformation objects.
       */
      TObjArray fxAxisTrans;

      /**
       * @label Array of slots for calibrations
       * @supplierCardinality MaxDimension
       */

   private:

      /**
       * Clears (sets to 0) all pointers, used for buffer allocations.
       */
      void ResetAllPoinetrs();

      /**
       * Release all memory, allocated for buffers.
       */
      void ReleaseAllPointers();

      /**
       * Number of entries in buffers.
       */
      Int_t fiBinsSize;                       //!

      /**
       * Number of indexes for each point.
       */
      Int_t fiIndexesSize;                    //!

      /**
       * Number of scales values for each point.
       */
      Int_t fiScalesSize;                    //!

      /**
       * Buffer for bins values.
       */
      Double_t* fxValues;                    //!

      /**
       * Buffer for standard devations of bins values.
       */
      Double_t* fxStandDev;                  //!

      /**
       * Buffer for complete model of bins values.
       */
      Double_t* fxBinsResult;                //!

      /**
       * Array of axis values for each bins.
       * Used in case of maximum memory usage.
       * Size [BinsSize * AxisNumber]
       */
      Double_t* fxFullScale;                 //!

      /**
       * Array of width values for each bin.
       * Used in case of maximum memory usage.
       * Size [BinsSize * AxisNumber]
       */
      Double_t* fxFullWidth;                 //!

      /**
       * Store combination of indexes for each data bins.
       * Size of [GetBinsSize()][NumDimension()].
       * Used in case of maximum memory usage.
       */
      Int_t* fxFullIndex;                     //!

      /** @link aggregation
       * @supplierCardinality 0..**/
      /*#  TGo4FitAxisTrans AxisTrans; */

      /** @link aggregation
       * @supplierCardinality 0..*
       * @label TObjArray fxCalibrations*/
      /*#  TGo4FitSlot lnkTGo4FitData; */


    ClassDef(TGo4FitData,1)
};

 /**
  * Iterator calss for data object.
  * The main aim of TGo4FitDataIter class is provide an sequential access to data, containing in appropriate data object.
  * Each new class of data object should provide also it's own implementation for TGo4FitDataIrter class. To create iterator, MakeIter() method of data object should be used.
  * Iterator has two main methods: Reset() and Next(). First method resets all internal buffers of iterator and positioning pointer on first data point. Next() method shifts to next data pointer.
  * As argument of these methods usage of bins selection meachanizm via range conditions or amplitude threshold can be specified.
  * Typical usage of iterator:
  *
  *    TGo4FitDataIter* iter = data->MakeIter();
  *    if (iter->Reset()) do {
  *    // do something with values in iterator
  *    } while (iter->Next());
  *
  * Access to most iterator methods like Values(), IndexesSize() and so on can be done only after first call of Reset() method.
  */

class TGo4FitDataIter : public TObject {
   public:

      /**
       * Default constructor.
       */
      TGo4FitDataIter();

      /**
       * Destroys TGo4FitDataIter object.
       */
      virtual ~TGo4FitDataIter();

      /**
       * Initialize iterator and positioning pointer on first point.
       * If UseRanges = kTRUE (default), selection meachanizm of data points via range conditions or amplitude threshold will be involved.
       * Most of iterator routines should be used only after Reset() command.
       */
      virtual Bool_t Reset(Bool_t UseRanges = kTRUE);

      /**
       * Shift pointer to next data point.
       * Return kTRUE, if next points exist.
       * If UseRanges = kTRUE (default), selection meachanizm of data points via range conditions or amplitude threshold will be involved.
       * Most of iterator routines should be used only after Reset() command.
       */
      virtual Bool_t Next(Bool_t UseRanges = kTRUE);

      /**
       * Return kTRUE, if data object has indexes.
       */
      Bool_t HasIndexes() const { return IndexesSize() > 0; }

      /**
       * Return size (number of dimensions) of data indexes.
       */
      Int_t IndexesSize() const { return fxIndexes.GetSize(); }

      /**
       * Return indexes for current data point.
       */
      const Int_t* Indexes() const { return fxIndexes.GetArray(); }

      /**
       * Iterate over all data points and returns maximum value for indexes.
       */
      Bool_t DefineIndexesLimits(TArrayI& Limits);

      /**
       * Return size (number) of scale values for each data point.
       */
      Int_t ScalesSize() const { return fxScales.GetSize(); }

      /**
       * Return scale values for current data points.
       */
      const Double_t* Scales() const { return fxScales.GetSize() > 0 ? fxScales.GetArray() : 0; }

      /**
       * Return current x coordinate if exists, otherwise 0
       */
      Double_t x() const { return fxScales.GetSize() > 0 ? fxScales[0] : 0.; }

      /**
       * Return current y coordinate if exists, otherwise 0
       */
      Double_t y() const { return fxScales.GetSize() > 1 ? fxScales[1] : 0.; }

      /**
       * Return current z coordinate if exists, otherwise 0
       */
      Double_t z() const { return fxScales.GetSize() > 2 ? fxScales[2] : 0.; }

      /**
       * Return kTRUE, if scale widths values exists.
       */
      Bool_t HasWidths() const { return fxWidths.GetSize() > 0; }

      /**
       * Return scales widths values.
       */
      const Double_t* Widths() const { return fxWidths.GetSize() > 0 ? fxWidths.GetArray() : 0; }

      /**
       * Return production of all width parameters (1 if no widths)
       */
      Double_t xWidths() const;

      /**
       * Return bin content (Value) for current point.
       */
      Double_t Value() const { return fdValue; }

      /**
       * Return standard deviation of bin content (Value) for current point.
       */
      Double_t StandardDeviation() const { return fdStandardDeviation; }

      /**
       * Return number of current point, starting from 0.
       */
      Int_t Point() const { return fiNumPoint; }

      /**
       * Check range conditions and amplitude threshold for current point.
       */
     Bool_t CheckPointForRange();

      /**
       * Counts total number of points in data object.
       * Uses Reset() and Next() methods, therefore change pointer to the data object
       */
      Int_t CountPoints(Bool_t UseRanges = kTRUE);

      /**
       * Checks, if iterator already reach end of data object.
       * This heppens, when Next() method return kFALSE value.
       */
      Bool_t ReachEnd() const { return fbReachEnd; }

      /**
       * Create histogram (if possible) with appropriate to data object dimensions number and size.
       * If UseRanges specified, data point selection mechanizm will be involved.
       * If SetBins specified, bin content will be copied to histogram.
       */
      TH1* CreateHistogram(const char* HistoName, Bool_t UseRanges = kFALSE, Bool_t SetBins = kFALSE);

      /**
       * Create TGraph object with appropriate to data object size.
       * If more than one scales values exists, only first will be used as X-coordinate of TGraph.
       * Bin content (Value()) will be used as Y-coordinate in TGraph.
       * If UseRanges specified, data point selection mechanizm will be involved.
       * If SetBins specified, bin content will be copied to histogram.
       */
      TGraph* CreateGraph(const char* GraphName, Bool_t UseRanges = kFALSE, Bool_t SetBins = kFALSE);

      /**
       * Create either histogram or graph object.
       */
      TObject* CreateDrawObject(const char* ObjName);

   protected:

      /**
       * Return pointer on correspondent TGo4FitData object, which create iterator.
       * Should be implemented in derived classes.
       */
     virtual TGo4FitData* GetData() const = 0;

      /**
       * Reset pointer and other specific values to the beggining of data.
       * Should be implemented in derived classes.
       */
     virtual Bool_t StartReset() = 0;

      /**
       * Perform specific actions to read all values from data object.
       * Should be implemented in derived classes.
       */
     virtual Bool_t ReadCurrentPoint() = 0;

      /**
       * Move pointer to following data point.
       * Should be implemented in derived classes.
       */
     virtual Bool_t ShiftToNextPoint() = 0;

      /**
       * Reserve buffers for indexes, scales and width values.
       * In case of 3-D histogram they consume only 3 Int_t and 6 Double_t points.
       */
     Bool_t ReserveArrays(Int_t NumDimen, Int_t NumOwnAxis, Bool_t HasWidth);

      /**
       * Transform scales values, using transformation objects in data.
       */
     void TransformScales(Double_t* scales);

      /**
       * Converts scale values.
       * Involved when usage of binary number insted scales or axis calibration objects specified to data.
       */
     Bool_t ProduceScales(const Int_t* index, const Double_t* ownscales, const Double_t* ownwidths);

      /**
       * Producing next indexes set according limits values.
       */
     Bool_t NextIndex(TArrayI& Index, TArrayI& Limits);

      /**
       * Calculates standard deviation from GetSigmaValue() of data object.
       */
     Bool_t GetDeviation();

     TArrayI fxIndexes;                     //!

     TArrayD fxScales;                      //!
     TArrayD fxWidths;                      //!
     Double_t fdValue;                      //!
     Double_t fdStandardDeviation;          //!

     Int_t fiNumPoint;                      //!

     Bool_t fbReachEnd;                     //!

   ClassDef(TGo4FitDataIter,1)
};


#endif // TGO4FITDATA_H
