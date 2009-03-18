#ifndef TGO4HISTOGRAMENTRY_H
#define TGO4HISTOGRAMENTRY_H

#include "TGo4DynamicEntry.h"
#include "TString.h"

/* maximum dimension  of histogram processable in dynamic list */
#define __MAXHISDIM__ 3
#define __MAXCONDIM__ 2

class TH1;
class TGo4Condition;
class TDataMember;

/**
 * Entry for the dynamic list, specialized for histogram like objects.
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4HistogramEntry : public TGo4DynamicEntry {

     friend class TGo4DynamicList;

   public:

      TGo4HistogramEntry();

      TGo4HistogramEntry(const char* name);

      virtual ~TGo4HistogramEntry();

      /** Set name of the external histogram to be filled. */
      void SetHistogramName(const char* name) { fxHistogramName = name; }
      /** Get name of the external histogram to be filled. */
      const char* GetHistogramName() const { return fxHistogramName.Data(); }

      /** Set Name of the data member that is to be filled
        * in histogram for axis number ix. ix is 0 for x-axis,
        * 1 for y-axis, etc. */
      void SetHisVarName(Int_t ix, const char* name);
      const char* GetHistVarName(Int_t ix) const;

      /** Set Name of the event object that contains the data to be filled
       * in histogram, for axis number ix. ix is 0 for x-axis,
       * 1 for y-axis, etc. */
      void SetHisEventName(Int_t ix, const char* name);
      const char* GetHistEventName(Int_t ix) const;

      /** Set name of the external condition to be referenced. */
      void SetConditionName(const char* name) { fxConditionName = name; }
      const char* GetConditionName() const { return fxConditionName.Data(); }

      /** Set Name of the data value (eventclass member) that is to be
        * tested by condition for axis number ix. ix is 0 for x-axis,
        * 1 for y-axis, etc. */
      void SetConVarName(Int_t ix, const char* name);
      const char* GetConVarName(Int_t ix) const;

      /** Set Name of the eventstructure object that is to be
        * tested by the condition for axis number ix. ix is 0 for x-axis,
        * 1 for y-axis, etc. */
      void SetConEventName(Int_t ix, const char* name);
      const char* GetConEventName(Int_t ix) const;

      Bool_t NeedInitialisation() const { return fbNeedInitialisation; }
      void SetNeedInitialisation(Bool_t on = kTRUE) { fbNeedInitialisation = on; }

      /** Resets this entry to an initial status. */
      virtual void Reset();

      virtual void Print(Option_t* dummy = "") const;

      virtual void RecursiveRemove(TObject* obj);

      static const char* Get_fgcNOCONDITION();

      static const char* Get_fgcNODATA();

      static const char* Get_fgcNOEVENT();

   protected:

      void InitHistPointer(Int_t ix, TObject* event, TDataMember* member, Long_t offset);

      void InitCondPointer(Int_t ix, TObject* event, TDataMember* member, Long_t offset);

      Double_t GetPtrValue(Int_t type, void* ptr);

      Bool_t TestConditionNew();

      void ProcessNew(Bool_t* evvalid);

   private:

    /** Title of the analysis object, for example a histogram. */
    TString fxHistogramName;

    /** Array of names of the  x,y,z,.. coordinate data members. */
    TString fxHisVarName[__MAXHISDIM__];

    /** Array of names of the  events which contain
      * the data members to be filled into the x,y,z,.. coordinates. */
    TString fxHisEventName[__MAXHISDIM__];

    /** Name of the condition to be applied for this entry. */
    TString fxConditionName;

    /** Name of the eventstructure datamembers that shall be
      * tested with the condition. */
    TString fxConVarName[__MAXCONDIM__];

    /** Array of names of the  events which contain
      * the data members to be tested  with the x,y
      * coordinates of the condition. */
    TString fxConEventName[__MAXCONDIM__];


    /** The (histogram) object kept by this entry. */
    TH1* fxHistogram; //!

    /** Condition to be checked on processing this entry. */
    TGo4Condition* fxCondition;                  //!

    Bool_t fbNeedInitialisation;  //!

    /** Pointer for event objects, class does not known to entry itself */
    TObject* fxHisEvents[__MAXHISDIM__]; //!

    /** Array of pointers to the event structure that
      * contains the data for the condition test. */
    TObject* fxConEvents[__MAXCONDIM__]; //!

    /** Array of pointers and data types to fill histogram */
    Int_t fxHistType[__MAXHISDIM__]; //!
    void* fxHistPtr[__MAXHISDIM__]; //!

    /** Array of pointers and data types to check condition */
    Int_t fxCondType[__MAXCONDIM__]; //!
    void* fxCondPtr[__MAXCONDIM__]; //!

    /** Text to indicate that no condition is used */
    static const char* fgcNOCONDITION;
    /** Text to indicate that no condition is used */
    static const char* fgcNODATA;
    /** Text to indicate that no condition is used */
    static const char* fgcNOEVENT;

  ClassDef(TGo4HistogramEntry,3)
};

#endif //TGO4HISTOGRAMENTRY_H
