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
    TGo4CondArray(const char *name, Int_t elements, const char *type);

    /** Constructor. contype can be kGO4CONDWINDOW or kGO4CONDPOLYGON */
    TGo4CondArray(const char *name, Int_t elements, Int_t contype);

    virtual ~TGo4CondArray();

    /** Factory method to generate the subclass implementation for painter */
    TGo4ConditionPainter* CreatePainter() override;

    /** Reimplement Draw to set painted flag for all array conditions */
    void Draw(Option_t* opt="") override;

    /** Replace default painter of this condition by external one.
    * if different views of this condition shall exist for different TPads,
    * the painter (i.e. view) can be exchanged before pad is updated. */
    void SetPainter(TGo4ConditionPainter* painter) override;

    /** Access to the currently selected condition. */
    const TGo4Condition* GetActiveCondition() const override;

   /**
     * Returns condition object i from object array.
     * See also Poly and Win.
     */
    TGo4Condition *At(Int_t i) const;

   /**
     * Returns window condition object i from object array.
     * If the conditions is no window, returns 0.
     * Must cast for window or polygon condition methods!
     */
    TGo4WinCond *Win(Int_t i) const;

   /**
     * Returns polygon condition object i from object array.
     * Checks if object is polygon condition, if not return 0.
     */
    TGo4PolyCond *Poly(Int_t i) const;

   /**
     * Returns condition object i from object array. Same as At().
     * Must cast for window or polygon condition methods!
     */
    TGo4Condition * operator[](Int_t i) const;

    using TGo4Condition::Test;
    /**
     * Test currently selected condition(single edit mode). In multi edit mode,
     * all conditions are tested with values and counters are incremented according result.
     * Here return value is logically anded from all results.
     */
    Bool_t Test(Double_t x, Double_t y) override;

   /**
     * Test currently selected condition(single edit mode). In multi edit mode,
     * all conditions are tested with values and counters are incremented according result.
     * Here return value is logically anded from all results.
     */
    Bool_t Test(Double_t x) override;

    Double_t GetXLow() const override;
    Double_t GetXUp() const override;
    Double_t GetYLow() const override;
    Double_t GetYUp() const override;

    /** Noop */
//    virtual void SetValues() {}

    using TGo4Condition::SetValues;

    /** Calls SetValues for all conditions (multi edit mode) or for currently selected (single edit mode). Window conditions only. */
    void SetValues(Double_t low1, Double_t up1) override;

    /** Calls SetValues for all conditions (multi edit mode) or for currently selected (single edit mode). Window conditions only. */
    void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2) override;

    /** Calls SetValues for all conditions (multi edit mode) or for currently selected (single edit mode). Polygon conditions only. */
    void SetValues(TCutG * newcut) override;

    /** Noop */
//    virtual void SetValues(Double_t * x, Double_t * y, Int_t len) {}


   /**
     * Calls Print for all conditions in the array.
     * Checks for condition type. If points is true, print Cut or Limits,
     * respectively.
     */
    void PrintCondition(Bool_t points) override;

    /** Prints counters and bar. */
    void PrintBar() override;


    /** for condition aggregates: return total number of conditions,
     * i.e.size of condition array */
    Int_t GetNumberOfConditions() const override;

   /**
     * Returns number of conditions in array.
     * Same as GetNumberOfConditions, but kept for backward compatibility
     */
    Int_t GetNumber() const;

    /** Returns class name of conditions. */
    const char *GetType() const;

    /** Returns condition type ID (kGO4CONDWINDOW=1 or kGO4CONDPOLYGON=2). */
    void GetType(Int_t& type) const;

    /** Calls Enable for all conditions (multi edit mode) or for currently selected (single edit mode) */
    void Enable() override;

    /** Calls Disable for all conditions (multi edit mode) or for currently selected (single edit mode) */
    void Disable(Bool_t result) override;

    /** Calls Invert for all conditions (multi edit mode) or for currently selected (single edit mode) */
    void Invert(Bool_t on) override;

   /** defines if condition counters shall be reset in UpdateFrom method*/
    void MarkReset(Bool_t on) override;

   /** Calls ResetCounts for all conditions (multi edit mode) or for currently selected (single edit mode) */
    void ResetCounts() override;

   /** Return number of calls of the Test method.
     * For complete array (multi edit mode) or for currently selected condition (single edit mode) */
    Int_t Counts() override;

   /** Return number of true results of the Test method call.
     * For complete array (multi edit mode) or for currently selected condition (single edit mode) */
    Int_t TrueCounts() override;

    /** Sets values of internal boolean flags to external variables. */
    void GetFlags(Bool_t* enabled, Bool_t* lastresult, Bool_t* markreset,
                  Bool_t* result, Bool_t* truevalue, Bool_t* falsevalue) override;

    /** we overwrite single condition visibility setter */
    void SetVisible(Bool_t on) override;

    /** we overwrite single condition visibility getter */
    Bool_t IsVisible() const override;

    /** we overwrite single condition changed property */
    void SetChanged(Bool_t on = kTRUE) override;

    /** we overwrite single condition changed property */
    Int_t IsChanged() const override;

    void SetLabelDraw(Bool_t on) override;
    Bool_t IsLabelDraw() const override;
    void SetLimitsDraw(Bool_t on) override;
    Bool_t IsLimitsDraw() const override;
    void SetIntDraw(Bool_t on) override;
    Bool_t IsIntDraw() const override;
    void SetXMeanDraw(Bool_t on) override;
    Bool_t IsXMeanDraw() const override;
    void SetXRMSDraw(Bool_t on) override;
    Bool_t IsXRMSDraw() const override;
    void SetYMeanDraw(Bool_t on) override;
    Bool_t IsYMeanDraw() const override;
    void SetYRMSDraw(Bool_t on) override;
    Bool_t IsYRMSDraw() const override;
    void SetXMaxDraw(Bool_t on) override;
    Bool_t IsXMaxDraw() const override;
    void SetYMaxDraw(Bool_t on) override;
    Bool_t IsYMaxDraw() const override;
    void SetCMaxDraw(Bool_t on) override;
    Bool_t IsCMaxDraw() const override;

   /** Calculate value for histogram inside condition limits.
     * Option may specify "width" to scale channel sum with axes*/
    Double_t GetIntegral(TH1 *histo, Option_t* opt="") override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetMean(TH1 *histo, Int_t axis=1) override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetRMS(TH1 *histo, Int_t axis=1) override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetSkewness(TH1 *histo, Int_t axis=1) override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetCurtosis(TH1 *histo, Int_t axis=1) override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetXMax(TH1 *histo) override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetYMax(TH1 *histo) override;

    /** Calculate value for histogram inside condition limits. */
    Double_t GetCMax(TH1 *histo) override;

    Bool_t IsPolygonType() const override;
    Bool_t IsArrayType() const override { return kTRUE; }

    void SetCurrentIndex(Int_t ix) override;
    Int_t GetCurrentIndex() const override;

    Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts) override;

    Int_t GetMemorySize() const override;

    /** Standard way to store parameter in form of macro */
    void SavePrimitive(std::ostream& fs, Option_t* = "") override;

  private:
    /** The object array of conditions */
    TObjArray *condarr{nullptr};

    /** Number of conditions in array. */
    Int_t fiNumCond{0};

    /** Condition type ID (kGO4CONDWINDOW or kGO4CONDPOLYGON) */
    Int_t fiType{0};

    /** Index of selected condition */
    Int_t fiSelectedCond{0};

  ClassDefOverride(TGo4CondArray,6)
};

#endif //TGO4CONDARRAY_H
