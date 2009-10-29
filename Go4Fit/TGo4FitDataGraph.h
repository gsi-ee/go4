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

#ifndef TGO4FITDATAGRAPH_H
#define TGO4FITDATAGRAPH_H

#include "TGo4FitData.h"
#include "TGo4FitSlot.h"

class TGraph;

/**
 * Data object, which provides access to TGraph and TGraphErrors ROOT objects.
 *
 * TGraph is just N points with X and Y coordinates. This is mean, that it may be only one-dimensional.
 * The TGraph object can be assigned to TGo4FitDataGraph object in constructor, in SetGraph() method or in SetObject() method of fitter. TGraph object may owned, or may not owned by data object.
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
      TGo4FitDataGraph(const char* iName, TGraph* = 0, Bool_t iGraphOwned = kFALSE, Bool_t AddAmpl = kFALSE);

      /**
       * Destroys TGo4FitDataGraph object.
       */
      virtual ~TGo4FitDataGraph();

      /**
       * Return pointer on TGraph object.
       */
      TGraph* GetGraph() { return (TGraph*) fxGraph.GetObject(); }

      /**
       * Set TGraph object.
       * Ownership flag can also be sets up. By default TGraph object is not owned.
       */
      void SetGraph(TGraph *iGraph, Bool_t iGraphOwned = kFALSE);

      /**
       * Create TGo4FitDataGraphIter iterator, assosiated with given data object..
       */
      TGo4FitDataIter* MakeIter();

      /**
       * Add pointer on slot, which should contain TGraph object, to slots list.
       */
      virtual void FillSlotList(TSeqCollection* list);

      /**
       * Print information on stndard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * Slot to for TGraph object.
       * @label fxGraph
       */
      TGo4FitSlot fxGraph;

   ClassDef(TGo4FitDataGraph,1)
};

// **************************************************************************

class TGo4FitDataGraphIter : public TGo4FitDataIter {
   public:
      TGo4FitDataGraphIter();
      TGo4FitDataGraphIter(TGo4FitDataGraph* Data);
      ~TGo4FitDataGraphIter();

   protected:
      virtual TGo4FitData* GetData() const { return fxData; }
      virtual Bool_t StartReset();
      virtual Bool_t ReadCurrentPoint();
      virtual Bool_t ShiftToNextPoint();

   private:
      TGo4FitDataGraph* fxData;
      Int_t fiNumPoints;

   ClassDef(TGo4FitDataGraphIter,1)
};

#endif // TGO4FITDATAGRAPH_H
