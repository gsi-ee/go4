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

#ifndef TGO4FITPARAMETER_H
#define TGO4FITPARAMETER_H

#include "TGo4FitNamed.h"

/**
 * Model and data objects parameter.
 * Has value, error, range and epsilon attributes.
 * From TGo4FitNamed class obtains full name, which consist of owner name (name of data object or model component) and name of parameter like "Gauss1.Width".
 */
class TGo4FitParameter : public TGo4FitNamed {
   public:

      /**
       * Default constructor.
       */
      TGo4FitParameter();

      /**
       * Create TGo4FitParameter object with given name, title and initial value.
       */
      TGo4FitParameter(const char* name, const char* title, Double_t iValue);

      /**
       * Create TGo4FitParameter object with given name, title, initial value and given range limits.
       */
      TGo4FitParameter(const char* name, const char* title, Double_t iValue, Double_t iRangeMin, Double_t iRangeMax);

      /**
       * Create TGo4FitParameter object with given name, title, initial value and given epsilon.
       */
      TGo4FitParameter(const char* name, const char* title, Double_t iValue, Double_t iEpsilon);

      /**
       * Create TGo4FitParameter object with given name, title, initial value, given range and epsilon.
       */
      TGo4FitParameter(const char* name, const char* title, Double_t iValue, Double_t iRangeMin, Double_t iRangeMax, Double_t iEpsilon);

      /**
       * Destroy TGo4FitParameter object.
       */
      virtual ~TGo4FitParameter();

      /**
       * Return parameter value.
       */
      Double_t GetValue() const { return fdValue; }

      /**
       * Set parameter value.
       */
      void SetValue(Double_t iValue) { fdValue = iValue; }

      /**
       * Get value of parameter error.
       */
      Double_t GetError() const { return fdError; }

      /**
       * Set value of parameter error.
       */
      void SetError(Double_t iError) { fdError = iError; }

      /**
       * Return status, if parameter fixed or not.
       */
      Bool_t GetFixed() const { return fbFixed || fbBlocked; }

      /**
       * Set status of parameter fixed or not.
       */
      void SetFixed(Bool_t iFixed) { fbFixed = iFixed; }

      /**
       * Blocks parameter value.
       * This means, that parameter can not be changed during minimization routine. If parameter block, GetFixed() method returns kTRUE value.
       * This method is used by fitter, when parameter not fixed, but for it dependency calculation is created. Thus, it can not be changed directly by minimization routine, but changing by fitter itself.
       */
      void SetBlocked() { fbBlocked = kTRUE; }

      /**
       * Release blocking of parameter.
       */
      void ClearBlocked() { fbBlocked = kFALSE; }

      /**
       * Return value of parameter epsilon.
       * If epsilon was not specified (by default), return kFALSE.
       * Epsilon means minimum step of parameter value, which can be used during minimization process.
       */
      Bool_t GetEpsilon(Double_t& eps) const { eps=fdEpsilon; return fbEpsilon; }

      /**
       * Return epsilon value of parameter.
       */
      void SetEpsilon(Double_t iEpsilon) { fbEpsilon = kTRUE; fdEpsilon = iEpsilon; };

      /**
       * Set status, if epsilon value was specified or not.
       * Did not change epsilon value itself. Designed for usage in GUI.
       */
      void SetEpsilonUse(Bool_t use) { fbEpsilon = use; }

      /**
       * Change epsilon value.
       * This is not affect on status of epsilon - if epsilon was not specified before, GetEpsilon() method still return kFALSE. Method should not used in normal program. Designed for usage in GUI.
       */
      void ChangeEpsilon(Double_t iEpsilon) { fdEpsilon = iEpsilon; }

      /**
       * Set status, what epsilon is not specified.
       * Not affect on epsilon value, therefore after SetEpsilonUse(kTRUE) GetEpsilon() method return old epsilon value.
       */
      void ClearEpsilon() { fbEpsilon = kFALSE; }

      /**
       * Return range left (minimum) and right (maximum) values.
       * If range was not specified before, return kFALSE.
       */
      Bool_t GetRange(Double_t& min, Double_t& max) const { min = fdRangeMin; max = fdRangeMax; return fbRange; }

      /**
       * Set range minimum and maximum values.
       */
      void SetRange(Double_t iRangeMin, Double_t iRangeMax);

      /**
       * Change status, is range was specified or not.
       * Not change range values themself. Designed for usage in GUI.
       */
      void SetRangeUse(Bool_t use) { fbRange = use; }

      /**
       * Change value of range minimum.
       */
      void ChangeRangeMin(Double_t iRangeMin) { if (fbRange) fdRangeMin = iRangeMin; }

      /**
       * Cahnge value of range maximum.
       */
      void ChangeRangeMax(Double_t iRangeMax) { if (fbRange) fdRangeMax = iRangeMax; }

      /**
       * Return value of range minimum.
       */
      Double_t GetRangeMin() { return fdRangeMin; }

      /**
       * Return value of range maximum.
       */
      Double_t GetRangeMax() { return fdRangeMax; }

      /**
       * Clear status, that range was psecified.
       * Equivalent to SetRangeUse(kFALSE).
       */
      void ClearRange() { fbRange = kFALSE; fdRangeMin = 0.; fdRangeMax = 0.; }

      /**
       * Memorize value of parameter.
       */
      void MemorizeValue() { fdRememberedValue = fdValue; }

      /**
       * Restore value, which was previosly memorized.
       */
      void RememberValue() { fdValue = fdRememberedValue; }

      /**
       * Print TGo4FitParameter object to standard output.
       */
      virtual void Print(Option_t* option) const;

   protected:
      void Reset();

      Double_t fdValue;
      Double_t fdError;
      Bool_t fbFixed;

      Bool_t fbEpsilon;
      Double_t fdEpsilon;

      Bool_t fbRange;
      Double_t fdRangeMin;
      Double_t fdRangeMax;

   private:
      Bool_t fbBlocked;            //!
      Double_t fdRememberedValue;  //!

    ClassDef(TGo4FitParameter,1)
};

#endif // TGO4FITPARAMETER_H
