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

#ifndef TGO4FITDATARIDGE_H
#define TGO4FITDATARIDGE_H

#include "TGo4FitData.h"
#include "TGo4FitSlot.h"

/**
 * Special object to approxiamte ridges on multi-dimensional histograms.
 *
 * Ridge means that some coordinate in multidimensional histograms can be represented as function of another coordinate(s) like y = f(x). If one has histogram, build in (x,y) space, one can try to approximate function f by several way.
 *
 * First simplest way, which works for two-dimensional way is to build TProfile from TH2 object, averaging data along y axis. Then TGo4FitDataProfile can be used to approximate TProfile by any kind of functional dependency.
 *
 * Another way is usage of TGo4FitDataRidge object. It selects one of histogram coordinate and use histogram as a set of measured points y(x) with different statistical weights, reverse proportinal to bin content of histogram at (x,y). Then this set of points can be again approximate of any kind of functional dependency from one coordinate. This simplest case can be extend to N-dimensions. In this case one of coordinate is a functions of the rest. This means, that this corrdinate can be approximate by function with N-1 coordinates.
 *
 * To obtain data, TGo4FitDataRidge object internally uses another TGo4FitData, which should provide histogram. Currently only TGo4FitDataHistogram object can be used for this purposes.
 */
class TGo4FitDataRidge : public TGo4FitData {
   public:

      /**
       * Default constructor.
       */
      TGo4FitDataRidge();

      /**
       * Creates TGo4FitDataRidge object with given name.
       * Pointer on TGo4FitData object, which should provide histogramic data, can be specified.
       * Also axis number, which uses as ridge coordinate, can be specified. By default y-axis is used as ridge.
       */
      TGo4FitDataRidge(const char* iName, TGo4FitData* Data = 0, Int_t SelectedAxis = 1);

      /**
       * Destroys TGo4FitDataRidge object
       */
      virtual ~TGo4FitDataRidge();

      /**
       * Sets pointer on TGo4FitData object and ownership flag.
       */
      void SetData(TGo4FitData* iData, Bool_t Owned) { fxData.SetObject(iData,Owned); }

      /**
       * Returns pointer on TGo4FitData object.
       */
      TGo4FitData* GetData() const { return dynamic_cast<TGo4FitData*> (fxData.GetObject()); }

      /**
       * Sets number of axis, which selected as "ridge altitude".
       */
      void SetSelectedAxis(Int_t naxis) { fiSelectedAxis = naxis; }

      /**
       * Returns number of selected axis.
       */
      Int_t GetSelectedAxis() { return fiSelectedAxis; }

      /**
       * Create TGo4FitDataRidgeIter iterator, assosiated with given data object..
       */
      TGo4FitDataIter* MakeIter();

      /**
       * Add pointer on slot, which should contain TGo4FitData object, to list of slots.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Print information to standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * Slot for TGo4FitData object.
       */
      TGo4FitSlot fxData;

      /**
       * Number of selected as ridge axis.
       */
      Int_t fiSelectedAxis;

   ClassDef(TGo4FitDataRidge,1)
};

// *****************************************************************************

class TGo4FitDataRidgeIter : public TGo4FitDataIter {
  public:
    TGo4FitDataRidgeIter();
    TGo4FitDataRidgeIter(TGo4FitDataRidge* data);
    ~TGo4FitDataRidgeIter();

  protected:
    virtual TGo4FitData* GetData() const { return fxData; }
    virtual Bool_t StartReset();
    virtual Bool_t ReadCurrentPoint();
    virtual Bool_t ShiftToNextPoint();

  private:
    TGo4FitDataRidge* fxData;           //!
    TGo4FitDataIter* iter;              //!
    TArrayD fxOwnScales;                //!

  ClassDef(TGo4FitDataRidgeIter,1)
};

#endif // TGO4FITDATARIDGE_H
