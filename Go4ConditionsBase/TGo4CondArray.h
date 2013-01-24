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

#ifndef TGO4CONDARRAY_H
#define TGO4CONDARRAY_H

#include "TGo4Condition.h"

class TObjArray;
class TGo4PolyCond;
class TGo4WinCond;

class TGo4CondArray : public TGo4Condition {
  public:
    TGo4CondArray();

   /** Constructor. type can be "TGo4WinCond" or "TGo4PolyCond" */
    TGo4CondArray(const char* name, Int_t elements, const char* type);

    /** Constructor. contype can be kGO4CONDWINDOW or kGO4CONDPOLYGON */
    TGo4CondArray(const char* name, Int_t elements, Int_t contype);

    virtual ~TGo4CondArray();

    /** Factory method to generate the subclass implementation for painter */
    virtual TGo4ConditionPainter* CreatePainter();

    /** Reimplement Draw to set painted flag for all array conditions */
    virtual void Draw(Option_t* opt="");

    /** Replace default painter of this condition by external one.
    * if different views of this condition shall exist for different TPads,
    * the painter (i.e. view) can be exchanged before pad is updated. */
    virtual void SetPainter(TGo4ConditionPainter* painter);

    /** Access to the currently selected condition. */
    virtual TGo4Condition* GetActiveCondition();

   /**
     * Returns condition object i from object array.
     * See also Poly and Win.
     */
    TGo4Condition * At(Int_t i);

   /**
     * Returns window condition object i from object array.
     * If the conditions is no window, returns 0.
     * Must cast for window or polygon condition methods!
     */
    TGo4WinCond* Win(Int_t i);

   /**
     * Returns polygon condition object i from object array.
     * Checks if object is polygon condition, if not return 0.
     */
    TGo4PolyCond* Poly(Int_t i);

   /**
     * Returns condition object i from object array. Same as At().
     * Must cast for window or polygon condition methods!
     */
    TGo4Condition * operator[](Int_t i);

   /** Noop, return true. */
//    virtual Bool_t Test() { return true; }

    /**
     * Test currently selected condition(single edit mode). In multi edit mode,
     * all conditions are tested with values and counters are incremented according result.
     * Here return value is logically anded from all results.
     */
    virtual Bool_t Test(Double_t x, Double_t y);

   /**
     * Test currently selected condition(single edit mode). In multi edit mode,
     * all conditions are tested with values and counters are incremented according result.
     * Here return value is logically anded from all results.
     */
    virtual Bool_t Test(Double_t x);

   /** Noop, return true. */
//    virtual Bool_t Test(Bool_t arg) { return true; }


    virtual Double_t GetXLow();
    virtual Double_t GetXUp();
    virtual Double_t GetYLow();
    virtual Double_t GetYUp();

    /** Noop */
//    virtual void SetValues() {}

    /** Calls SetValues for all conditions (multi edit mode) or for currently selected (single edit mode). Window conditions only. */
    virtual void SetValues(Double_t low1, Double_t up1);

    /** Calls SetValues for all conditions (multi edit mode) or for currently selected (single edit mode). Window conditions only. */
    virtual void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2);

    /** Calls SetValues for all conditions (multi edit mode) or for currently selected (single edit mode). Polygon conditions only. */
    virtual void SetValues(TCutG * newcut);

    /** Noop */
//    virtual void SetValues(Double_t * x, Double_t * y, Int_t len) {}


   /**
     * Calls Print for all conditions in the array.
     * Checks for condition type. If points is true, print Cut or Limits,
     * respectively.
     */
    void PrintCondition(Bool_t points);

    /** Prints counters and bar. */
    virtual void PrintBar();


    /** for condition aggregates: return total number of conditions,
     * i.e.size of condition array */
    virtual Int_t GetNumberOfConditions();

   /**
     * Returns number of conditions in array.
     * Same as GetNumberOfConditions, but kept for backward compatibility
     */
    Int_t GetNumber() const;

    /** Returns class name of conditions. */
    const char* GetType() const;

    /** Returns condition type ID (kGO4CONDWINDOW=1 or kGO4CONDPOLYGON=2). */
    void GetType(Int_t& type) const;

    /** Calls Enable for all conditions (multi edit mode) or for currently selected (single edit mode) */
    virtual void Enable();

    /** Calls Disable for all conditions (multi edit mode) or for currently selected (single edit mode) */
    virtual void Disable(Bool_t result);

    /** Calls Invert for all conditions (multi edit mode) or for currently selected (single edit mode) */
    virtual void Invert(Bool_t on);

   /** defines if condition counters shall be reset in UpdateFrom method*/
    virtual void MarkReset(Bool_t on);

   /** Calls ResetCounts for all conditions (multi edit mode) or for currently selected (single edit mode) */
    virtual void ResetCounts();

   /** Return number of calls of the Test method.
     * For complete array (multi edit mode) or for currently selected condition (single edit mode) */
    virtual Int_t Counts();

   /** Return number of true results of the Test method call.
     * For complete array (multi edit mode) or for currently selected condition (single edit mode) */
    virtual Int_t TrueCounts();

    /** Sets values of internal boolean flags to external variables. */
    virtual void GetFlags(Bool_t* enabled, Bool_t* lastresult, Bool_t* markreset,
                   Bool_t* result, Bool_t* truevalue, Bool_t* falsevalue);

    /** we overwrite single condition visibility setter */
    virtual void SetVisible(Bool_t on);

    /** we overwrite single condition visibility getter */
    virtual Bool_t IsVisible();

    /** we overwrite single condition changed property */
    virtual void SetChanged(Bool_t on);

    /** we overwrite single condition changed property */
    virtual Int_t IsChanged();

    virtual void SetLabelDraw(Bool_t on);
    virtual Bool_t IsLabelDraw();
    virtual void SetLimitsDraw(Bool_t on);
    virtual Bool_t IsLimitsDraw();
    virtual void SetIntDraw(Bool_t on);
    virtual Bool_t IsIntDraw();
    virtual void SetXMeanDraw(Bool_t on);
    virtual Bool_t IsXMeanDraw();
    virtual void SetXRMSDraw(Bool_t on);
    virtual Bool_t IsXRMSDraw();
    virtual void SetYMeanDraw(Bool_t on);
    virtual Bool_t IsYMeanDraw();
    virtual void SetYRMSDraw(Bool_t on);
    virtual Bool_t IsYRMSDraw();
    virtual void SetXMaxDraw(Bool_t on);
    virtual Bool_t IsXMaxDraw();
    virtual void SetYMaxDraw(Bool_t on);
    virtual Bool_t IsYMaxDraw();
    virtual void SetCMaxDraw(Bool_t on);
    virtual Bool_t IsCMaxDraw();

   /** Calculate value for histogram inside condition limits.
     * Option may specify "width" to scale channel sum with axes*/
    virtual Double_t GetIntegral(TH1* histo, Option_t* opt="");

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetMean(TH1* histo, Int_t axis=1);

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetRMS(TH1* histo, Int_t axis=1);

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetXMax(TH1* histo);

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetYMax(TH1* histo);

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetCMax(TH1* histo);

    virtual Bool_t IsPolygonType();
    virtual Bool_t IsArrayType();

    virtual void SetCurrentIndex(Int_t ix);
    virtual Int_t GetCurrentIndex();

    virtual Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts);

    virtual Int_t GetMemorySize();

    /** Standard way to store parameter in form of macro */
    virtual void SavePrimitive(ostream& fs, Option_t* = "");

  private:
    /** The object array of conditions */
    TObjArray* condarr;

    /** Number of conditions in array. */
    Int_t fiNumCond;

    /** Condition type ID (kGO4CONDWINDOW or kGO4CONDPOLYGON) */
    Int_t fiType;

    /** Index of selected condition */
    Int_t fiSelectedCond;

  ClassDef(TGo4CondArray,6)
};

#endif //TGO4CONDARRAY_H
