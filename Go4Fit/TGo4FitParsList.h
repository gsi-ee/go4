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

#ifndef TGO4FITPARSLIST_H
#define TGO4FITPARSLIST_H

#include "TGo4FitNamed.h"

#include "TOrdCollection.h"

class TGo4FitParameter;

/**
 * List of TGo4FitParameter objects.
 * Contain list of parameters and provides access to parameters properties via names.
 * Parameters objects may owned or not owned by TGo4FitParsList object.
 */
class TGo4FitParsList: public TGo4FitNamed {
   public:

      /**
       * Default constructor.
       */
      TGo4FitParsList();

      /**
       * Create TGo4FitParsList object and set flag of ownership of parameters object.
       */
      TGo4FitParsList(Bool_t IsParsOwned);

      /**
       * Create TGo4FitParsList object, set it's name and title, and ownership flag of parameters pbjects.
       */
      TGo4FitParsList(const char* Name, const char* Title, Bool_t IsParsOwned);

      /**
       * Destroy TGo4FitParsList object.
       */
      virtual ~TGo4FitParsList();

      /**
       * Return number of parameters in list.
       */
      virtual Int_t NumPars() { return fxPars.GetSize(); }

      /**
       * Return parameter according given index.
       * If index out of valid range, return 0.
       */
      TGo4FitParameter* GetPar(Int_t n) { return Get(n);  }

      /**
       * Return index of given parameter in list.
       * If parameter not in list, return -1.
       */
      Int_t GetParIndex(const TGo4FitParameter* par);

      /**
       * Find parameter object with given name.
       * Name can be both name or full name of TGo4FitParameter object. If no such parameters exists, return 0.
       */
      TGo4FitParameter* FindPar(const char* ParName) { return Find(ParName); }

      /**
       * Find parameter with given owner full name and parameter name.
       * If no such parameters exists, return 0.
       */
      TGo4FitParameter* FindPar(const char* OwnerFullName, const char* ParName) { return Find(OwnerFullName,ParName); }

      /**
       * Find parameter in list.
       * If no such parameters exists, return 0.
       */
      TGo4FitParameter* FindPar(TGo4FitParameter* par) { return Find(par); }

      /**
       * Return value of parameter with given name.
       * If no such parameter, return 0.
       */
      Double_t GetParValue(const char* ParName);

      /**
       * Set value of parameter with given name.
       * Return kTRUE in case of success, otherwise 0.
       */
      Bool_t SetParValue(const char* ParName, Double_t iValue);

      /**
       * Copy values of all parameters in list to provided array.
       * Array should have at lest NumPars() elements, otherwise routine can cause error.
       */
      void GetParsValues(Double_t* pars);

      /**
       * Set value for all parameters from array.
       */
      void SetParsValues(Double_t* pars);

      /**
       * Get error value for parameter with given name.
       * If no such parameters, return 0.
       */
      Double_t GetParError(const char* ParName);

      /**
       * Set error value for parameter with given name.
       * If such parameters exists, return kTRUE, otherwise kFALSE.
       */
      Bool_t SetParError(const char* ParName, Double_t iError);

      /**
       * Set for parameter with given name, is it fixed or not.
       */
      virtual Bool_t SetParFixed(const char* ParName, Bool_t iFixed = kTRUE);

      /**
       * Return status for parameter with given name, is it fixed or not.
       * If parameter not exist, return kFALSE.
       */
      virtual Bool_t GetParFixed(const char* ParName);

      /**
       * Return number of fixed pars
       */
      Int_t NumFixedPars();

      /**
       * Return number of free pars
       */
      Int_t NumFreePars();

      /**
       * Set range limits for parameter with given name.
       * Return kTRUE, if parameter exist.
       */
      virtual Bool_t SetParRange(const char* ParName, Double_t RangeMin, Double_t RangeMax);

      /**
       * Get range limits for parameter with given name.
       * If succesfull, return kTRUE.
       */
      virtual Bool_t GetParRange(const char* ParName, Double_t& RangeMin, Double_t& RangeMax);

      /**
       * Set epsilon value for parameter with given name.
       * Return kTRUE, if parameter exist.
       */
      virtual Bool_t SetParEpsilon(const char* ParName, Double_t Epsilon);

      /**
       * Get epsilon value for parameter with given name.
       * Return kTRUE, is succesfull.
       */
      virtual Bool_t GetParEpsilon(const char* ParName, Double_t& Epsilon);

      /**
       * Set new name for parameter object with provided index.
       */
      void SetParName(Int_t n, const char* name);

      /**
       * Return name of parameter with given index.
       * If index out of valid range, return 0.
       */
      const char* GetParName(Int_t n);

      /**
       * Return full name of parameter with given index.
       * If index out of valid range, return 0.
       */
      const char* GetParFullName(Int_t n);

      /**
       * Set names of first 10 parameters.
       * For instance:
       *    list->SetParsNames("CoefK","CoefB","Const").
       * If there are more then 3 parameters in list, for the rest parameters standard names will be applied: "Par3", "Par4" and so on.
       */
      void SetParsNames(const char* name0 = "Par0", const char* name1 = "Par1",
                        const char* name2 = "Par2", const char* name3 = "Par3",
                        const char* name4 = "Par4", const char* name5 = "Par5",
                        const char* name6 = "Par6", const char* name7 = "Par7",
                        const char* name8 = "Par8", const char* name9 = "Par9");

      /**
       * Set values for first 10 parameters.
       * For instance,
       *   list->SetParsValues(1,345,23);
       * If more than 3 parameters in list, for rest 0 values will be set.
       */
      void SetParsValues(Double_t par0 = 0., Double_t par1 = 0., Double_t par2 = 0., Double_t par3 = 0., Double_t par4 = 0.,
                         Double_t par5 = 0., Double_t par6 = 0., Double_t par7 = 0., Double_t par8 = 0., Double_t par9 = 0.);

      /**
       * Memorize values of all parameters.
       * This values can be restore later by RemeberedPars().
       */
      virtual void MemorizePars();

      /**
       * Return true if parameters can be safely restored from memorized values;
       */
      virtual bool CanRollbackPars() { return fbCanRollbackPars; }

      /**
       * Restore parameters values, which were stored by MemorizePars() routine;
       */
      virtual void RememberPars();

      /**
       * Returns true, if all parameters in list fixed;
       */
      Bool_t IsAllParsFixed();

      /**
       * Create new TGo4FitParameter object with given name, title and parameter value, and put this object to the list.
       * Return created parameter object.
       */
      TGo4FitParameter* CreatePar(const char* ParName, const char* Title, Double_t iValue = 0);

      /**
       * Remove parameter from list with given name.
       * If parameter owned by list, parameter will be destroyed.
       * Return kTRUE if succesfull.
       */
      Bool_t RemovePar(const char* name);

      /**
       * Remove parameter from list.
       * If parameter owned by list, parameter will be destroyed.
       * Return kTRUE if succesfull.
       */
      Bool_t RemovePar(TGo4FitParameter* par);

      /**
       * Default print method.
       * Call print method for every parameter object in list.
       */
      virtual void Print(Option_t* option) const;

      /**
       * Print only value and error of all parameters in list.
       */
      void PrintPars() const;

      /**
       * Add all parameters to provided TGo4FitParsList object.
       * User should take care, that both source and destination have not ownership flag at same time, otherwise it can cause error during these lists destroyment.
       */
      virtual void CollectParsTo(TGo4FitParsList & list);

      /**
       * Remove all parameters from list.
       * If parameters owned by list, they will be destroyed.
       */
      void ClearPars();

   protected:
      TGo4FitParameter* AddPar(TGo4FitParameter* par);
      TGo4FitParameter* InsertPar(TGo4FitParameter* par, Int_t indx);
      void SetParsOwner(TGo4FitNamed* iOwner);

      void ClearParsBlocking();

      virtual TGo4FitParameter* Get(Int_t n) { return (n>=0) && (n<NumPars()) ? (TGo4FitParameter*) fxPars.At(n) : 0 ; }
      TGo4FitParameter* Find(const char* ParName);
      TGo4FitParameter* Find(const char* OwnerFullName, const char* ParName);
      TGo4FitParameter* Find(TGo4FitParameter* par);
      Bool_t RemoveParByIndex(Int_t indx);

    private:
      TOrdCollection fxPars;              //!

      /**
       * If true, parameters can be safely rollback
       */
      Bool_t fbCanRollbackPars;                        //!

      /** @link aggregation
       * @label TOrdCollection
       * @supplierCardinality 0..**/
      /*#  TGo4FitParameter lnkTGo4FitParsList; */
    ClassDef(TGo4FitParsList,1)
};
#endif // TGO4FITPARSLIST_H
