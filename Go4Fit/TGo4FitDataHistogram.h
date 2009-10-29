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

#ifndef TGO4FITDATAHISTOGRAM_H
#define TGO4FITDATAHISTOGRAM_H

#include "TGo4FitData.h"
#include "TGo4FitSlot.h"

class TH1;

/**
 * Data objects, which provides access to generic TH1 ROOT histogram.
 *
 * There are several implementations of TH1 for one, two and three-dimensional histogram. All of them inherited from TH1 class and supported in TGo4FitDataHistogram object.
 * The histogram can be assigned to TGo4FitDataHistogram object in constructor, in SetHistogram() method or in SetObject() method of fitter. Histogram may owned, or may not owned by data object.
 * TGo4FitDataHistogram gets from histogram number of dimensions and number of bins on each axis. The first and last bins on each axis (0 and NBins+1 indexes) are excluded from data analysis. This means, that data object uses only bins, which has indexes from 1 to NBins.
 * If no calibration objects are sets up, TGo4FitDataHistogram can uses axis values, taken from histogram itself. As axis values central position of each bin is using.
 */
class TGo4FitDataHistogram : public TGo4FitData {
   public:

      /**
       * Default constructor.
       */
      TGo4FitDataHistogram();

      /**
       * Creates TGo4FitDataHistogram object with provided name.
       * Pointer on histogram (derived from TH1 class) and ownership flag can be sets up in constructor.
       * Scaling factor for bins (amplitude) can be added.
       */
      TGo4FitDataHistogram(const char* iName, TH1* = 0, Bool_t iHistogramOwned = kFALSE, Bool_t AddAmpl = kFALSE);

      /**
       * Destroys TGo4FitDataHistogram object.
       */
      virtual ~TGo4FitDataHistogram();

      /**
       * Return pointer on assigned histogram.
       */
      TH1* GetHistogram() { return (TH1*) fxHistogram.GetObject(); }

      /**
       * Set histogram to object.
       * Ownership flag also can be sets up. By default histogram is not owned by object.
       */
      void SetHistogram(TH1 *iHistogram, Bool_t iHistogramOwned = kFALSE);

      /**
       * Create TGo4FitDataHistogramIter iterator, assosiated with given data object..
       */
      TGo4FitDataIter* MakeIter();

      /**
       * Add pointer on slot, which should contains histogram, to list.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Print information to standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * Slot for storing pointer on TH1 object
       * @label slot for TH1*
       */
      TGo4FitSlot fxHistogram;

   ClassDef(TGo4FitDataHistogram,1)
};

// *****************************************************************************

class TGo4FitDataHistogramIter : public TGo4FitDataIter {
  public:
    TGo4FitDataHistogramIter();
    TGo4FitDataHistogramIter(TGo4FitDataHistogram* data);
    ~TGo4FitDataHistogramIter();

  protected:
    virtual TGo4FitData* GetData() const { return fxData; }
    virtual Bool_t StartReset();
    virtual Bool_t ReadCurrentPoint();
    virtual Bool_t ShiftToNextPoint();

  private:
    TGo4FitDataHistogram* fxData;       //!
    TArrayI fxLimits;                   //!
    TArrayD fxOwnScales;                //!
    TArrayD fxOwnWidths;                //!

  ClassDef(TGo4FitDataHistogramIter,1)
};

#endif // TGO4FITDATAHISTOGRAM_H
