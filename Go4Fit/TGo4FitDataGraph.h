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

#ifndef TGO4FITDATAGRAPH_H
#define TGO4FITDATAGRAPH_H

#include "TGo4FitData.h"
#include "TGo4FitSlot.h"

class TGraph;

/**
 * Data object, which provides access to TGraph and TGraphErrors ROOT objects.
 *
 * TGraph is just N points with X and Y coordinates. This is mean, that it may be only one-dimensional.
 * The TGraph object can be assigned to TGo4FitDataGraph object in constructor, in SetGraph() method or in SetObject() method of fitter.
 * TGraph object may owned, or may not owned by data object.
 * TGo4FitDataGraph gets Y values as bins containment. If no calibration object is sets up, X can be used as axis values.
 * If TGraphErrors object is assigned, the error values of Y can be used as sigmas in chi-square calculations (fit-function type should be ff_chi_square).
 */
class TGo4FitDataGraph : public TGo4FitData {
   public:

      /**
       * Default constructor.
       */
      TGo4FitDataGraph();

      /**
       * Create TGo4FitDataGraph object with given name.
       * Pointer on TGraph and ownership flag can be provided.
       * Scaling factor (amplitude) also can be added.
       */
      TGo4FitDataGraph(const char *iName, TGraph *iGraph = nullptr, Bool_t iGraphOwned = kFALSE, Bool_t AddAmpl = kFALSE);

      /**
       * Destroys TGo4FitDataGraph object.
       */
      virtual ~TGo4FitDataGraph();

      /**
       * Return pointer on TGraph object.
       */
      TGraph *GetGraph() { return (TGraph *) fxGraph.GetObject(); }

      /**
       * Set TGraph object.
       * Ownership flag can also be sets up. By default TGraph object is not owned.
       */
      void SetGraph(TGraph *iGraph, Bool_t iGraphOwned = kFALSE);

      /**
       * Create TGo4FitDataGraphIter iterator, associated with given data object..
       */
      std::unique_ptr<TGo4FitDataIter> MakeIter() override;

      /**
       * Add pointer on slot, which should contain TGraph object, to slots list.
       */
      void FillSlotList(TSeqCollection* list) override;

      /**
       * Print information on standard output.
       */
      void Print(Option_t *option = "") const override;

   protected:

      /**
       * Slot to for TGraph object.
       * @label fxGraph
       */
      TGo4FitSlot fxGraph;

   ClassDefOverride(TGo4FitDataGraph,1)
};

// **************************************************************************

class TGo4FitDataGraphIter : public TGo4FitDataIter {
   public:
      TGo4FitDataGraphIter();
      TGo4FitDataGraphIter(TGo4FitDataGraph* Data);
      ~TGo4FitDataGraphIter();

   protected:
      TGo4FitData* GetData() const override { return fxData; }
      Bool_t StartReset() override;
      Bool_t ReadCurrentPoint() override;
      Bool_t ShiftToNextPoint() override;

   private:
      TGo4FitDataGraph* fxData{nullptr};
      Int_t fiNumPoints{0};

   ClassDefOverride(TGo4FitDataGraphIter,1)
};

#endif // TGO4FITDATAGRAPH_H
