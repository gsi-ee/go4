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

#ifndef TGO4FITCOMPONENT_H
#define TGO4FITCOMPONENT_H

#include "TGo4FitParsList.h"
#include "TGo4FitSlot.h"

#include "TArrayD.h"
#include "TArrayI.h"
#include "TObjArray.h"

class TCutG;

/**
 * Basic absrtact class, combining common properties of data and model.
 */
class TGo4FitComponent : public TGo4FitParsList, public TGo4FitSlotList {
   public:

      /**
       * Default constructor.
       */
      TGo4FitComponent();

      /**
       * Creates TGo4FitComponent object.
       */
      TGo4FitComponent(const char* iName, const char* iTitle);

      /**
       * Destroys TGo4FitComponent object.
       */
      virtual ~TGo4FitComponent();

      /**
       * Return ampltide parameter object.
       * If amplitude not exists, return 0.
       */
      TGo4FitParameter* GetAmplPar();

      /**
       * Returns name of amplitude parameter.
       * If none, return 0.
       */
      const char* GetAmplName();

      /**
       * Returns full name of amplitude parameter.
       * If none, return 0.
       */
      const char* GetAmplFullName();

      /**
       * Return value of amplitude parameter.
       * If none, return 1.
       */
      Double_t GetAmplValue();

      /**
       * Set value of amplitude parameter.
       */
      void SetAmplValue(Double_t iAmpl);

      /**
       * Return error of amplitude parameter.
       * If none, return 0.
       */
      Double_t GetAmplError();

      /**
       * Set error of amplitude parameter.
       */
      void SetAmplError(Double_t iError);

      /**
       * Create amplitude parameter and adds it to parameters list.
       * Typically amplitude is situated in the beggining of parameters list.
       */
      Bool_t MakeAmpl(Bool_t isFixed = kFALSE);

      /**
       * Remove amplitude parameter from parameters list.
       */
      Bool_t RemoveAmpl();

      /**
       * Signals, if amplitude parameter can be removed from or introduced to object.
       */
      virtual Bool_t CanAmplTouch() { return kFALSE; }

      /**
       * Includes axis range in consideration.
       * Number of axis and minium and maximum values should be specified.
       */
      void SetRange(Int_t naxis, Double_t min, Double_t max);

      /**
       * Exclude axis range from consideration.
       */
      void ExcludeRange(Int_t naxis, Double_t min, Double_t max);

      /**
       * Set minimum axis value, taken into consideration.
       * All smaller values will be removed from consideration independently from other range conditions.
       */
      void SetRangeMin(Int_t naxis, Double_t value);

      /**
       * Set maximum axis value, taken into consideration.
       * All lager values will be removed from consideration independently from other range conditions.
       */
      void SetRangeMax(Int_t naxis, Double_t value);

      /**
       * Add TCutG object as range condition for two-dimensional case.
       * Region, defined by TCutG object, can be included or excluded from consideration according value of exclude flag.
       */
      void AddRangeCut(TCutG* cut, Bool_t exclude = kFALSE);

      /**
       * Clear all range conditions for given axis.
       * If naxis=-1, all range conditions will be removed.
       */
      void ClearRanges(Int_t naxis = -1);

      /**
       * Return kTRUE, if any range conditions were introduced.
       */
      Bool_t IsAnyRangeLimits();

      /**
       * Defines minimum allowed value for given range.
       * Returns kFALSE, if no range condition were specified for given axis.
       */
      Bool_t GetRangeMin(Int_t naxis, Double_t& value);

      /**
       * Defines maximum allowed value for given range.
       * Returns kFALSE, if no range condition were specified for given axis.
       */
      Bool_t GetRangeMax(Int_t naxis, Double_t& value);

      /**
       * Returns number of range condition (exclude range cuts).
       */
      Int_t GetNumRangeCondition() const { return fxRangeAxis.GetSize()/2; }

      /**
       * Return value for specified range condition.
       *
       * Following procedures produces correspondent values:
       * typ=0: SetRange()
       * typ=1: ExcludeRange()
       * typ=2: SetRangeMin()    right not defined
       * typ=3: SetRangeMax()    left not defined
       */
      void GetRangeCondition(Int_t n, Int_t& typ, Int_t& naxis, Double_t& left, Double_t& right) const;

      /**
       * Change specified range condition.
       * Range condition should already exists before calling of these procedure.
       * For exact meaning of parameters see GetRangeCondition() method.
       */
      void SetRangeCondition(Int_t n, Int_t typ, Int_t naxis, Double_t left, Double_t right);

      /**
       * Add new range condition.
       * For exact meaning of parameters see GetRangeCondition() method.
       */
      void AddRangeCondition(Int_t typ, Int_t naxis, Double_t left, Double_t right);

      /**
       * Removes specified range condition.
       */
      void RemoveRangeCondition(Int_t n);

      /**
       * Returns number of range cuts, assigned to object.
       */
      Int_t GetNumRangeCut() const;

      /**
       * Returns specified range cut.
       */
      TCutG* GetRangeCut(Int_t n) const;

      /**
       * Return kTRUE if cut eclude range
       */
      Bool_t IsRangeCutExcluding(Int_t n);

      /**
       * Set type of range cut (excluding or including)
       */
      void SetRangeCutExcluding(Int_t n, Bool_t exclude = kTRUE);

      /**
       * Delete range cut with specified index.
       */
      void RemoveRangeCut(Int_t n);

      /**
       * Check all range conditions for specified point.
       */
      Bool_t CheckRangeConditions(const Double_t* values, Int_t numaxis);

      /**
       * Collect all parameters to provided parameters list object.
       * Not only parameters of TGo4FitComponent copies to list.
       * All parameters, which are situated in incpsulated objects, also will be copied to this list.
       */
      virtual void CollectParsTo(TGo4FitParsList& list);

            /**
       * Sets usage of additional buffers.
       * Buffers can only be created in Initialize procedure..
       */
      void SetUseBuffers(Bool_t iUse) { fbUseBuffers = iUse; }

      /**
       * Returns flag of usage of additional buffers.
       * Buffers can only be created in Initialize procedure..
       */
      Bool_t GetUseBuffers() { return fbUseBuffers; }

      /**
       * Print info about object on standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:

      /**
       * Create new parameter with provided properties and add to parameters list.
       */
      TGo4FitParameter* NewParameter(const char* Name, const char* Title, Double_t iValue = 0., Bool_t Fixed = kFALSE, Int_t AtIndx = -1);

      /**
       * Create amplitude parameter with specified properties.
       */
      TGo4FitParameter* NewAmplitude(const char* Name = 0, Double_t iValue = 0., Bool_t IsFixed = kFALSE, Int_t AtIndx = 0);

      /**
       * Returns index of amplitude parameter.
       */
      Int_t GetAmplIndex() { return fiAmplIndex; }

      /**
       * Set index of amplitude parameter.
       */
      void SetAmplIndex(Int_t iAmplIndex = -1) { fiAmplIndex = iAmplIndex; }

   private:

      /**
       * Array for range conditions storage (axis indexes)
       */
      TArrayI fxRangeAxis;

      /**
       * Array for range values storaging.
       */
      TArrayD fxRangeValue;

      /**
       * Array of TCutG objects, used for range selection.
       */
      TObjArray fxCuts;

      /**
       * Index of amplitude parameter (-1 if none)
       */
      Int_t fiAmplIndex;

      /**
       * Specify usage of buffers after initialization
       */
      Bool_t fbUseBuffers;

   ClassDef(TGo4FitComponent,1)
};

#endif // TGO4FITCOMPONENT_H
