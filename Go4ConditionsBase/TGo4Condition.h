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

#ifndef TGO4CONDITION_H
#define TGO4CONDITION_H

#include "TNamed.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include <iostream>

class TH1;
class TVirtualPad;
class TCutG;
class TObjArray;

class TGo4HistogramEntry;
class TGo4EventElement;
class TGo4ConditionPainter;

enum ConditionsKind {
   kGO4CONDWINDOW  = 1,
   kGO4CONDPOLYGON = 2
};

/** @brief Go4 condition class */
class TGo4Condition : public TNamed, public TAttLine, public TAttFill {

  friend class TGo4CondArray;

  public:
    TGo4Condition();

    TGo4Condition(const char *name, const char *title = "Go4 Condition");

    virtual ~TGo4Condition();

    /** Display condition with plain root canvas and in go4 viewpanel */
    void Paint(Option_t *opt = "") override;

    /** Draw this condition on current pad */
    void Draw(Option_t *opt = "") override;

    /** Erase view of this condition from the pad */
    virtual void UnDraw(Option_t *opt = "");

    /** Re-implement TObject method to pop all our views to front*/
    void Pop() override;

    /** Factory method to generate the subclass implementation for painter */
    virtual TGo4ConditionPainter *CreatePainter();

    /** Return number of calls of the Test method. */
    virtual Int_t Counts();

    /** Return number of true results of the Test method call. */
    virtual Int_t TrueCounts();

    /** Clear counters */
    virtual void ResetCounts();

    /** Set counters */
    virtual void SetCounts(Int_t truecounts, Int_t counts);

    /** Freeze condition, on Test() return result.
      * This overwrites inverted result, but does not change it */
    virtual void Disable(Bool_t result);

    /** Enable (unfreeze) Test execution */
    virtual void Enable();

    /** Returns true  if condition is enabled */
    Bool_t IsEnabled() const { return fbEnabled; }

    /** Test if condition is true. Result is returned, the internal counters are
      * set appropriately. Should return fbResult if condition is not enabled.
      * the default is increment test counter and return true. */
    virtual Bool_t Test();
    virtual Bool_t Test(Double_t x, Double_t y) { return true; }
    virtual Bool_t Test(Double_t x) { return true; }
    //virtual Bool_t Test(Bool_t result) { return true; }

    /** Set values needed, i.e. limits for windows or cuts for polygons */
    virtual void SetValues() {}
    virtual void SetValues(Double_t low1, Double_t up1) {}
    virtual void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2) {}
    virtual void SetValues(TCutG * newcut) {}
    virtual void SetValues(Double_t * x, Double_t * y, Int_t len) {}

    /**
     * Add another condition to this one. The newly added condition
     * is added as next condition and may be tested together with
     * the current condition. User may specify the logical connection
     * between the chained conditions (e.g. a logical "and",
     * "or", "xor" relationships are all possible). Depends on the
     * implementation of Test() method. The chained conditions are
     * _not_ owned by the top level condition and are not deleted
     * by dtor, thus one condition instance may be chained to
     * several others.
     */
    virtual void AddCondition(TGo4Condition *next);

    /** Increment the test counter. User has to take care
      * that this method is called within his/her Test method. */
    void IncCounts();

    /** Increment the "test was true" counter. User has to take care
      * that this method is called within his/her Test method on success. */
    void IncTrueCounts();

    /** Use this as return, if test was true. Takes into account inversion, i.e.
      * when condition is inverted, IsTrue returns false. */
    Bool_t IsTrue() const { return fbTrue; }

    /** Use this as return, if test was false. Takes into account inversion, i.e.
      * when condition is inverted, IsFalse returns true. */
    Bool_t IsFalse() const { return fbFalse; }

    /** Returns the value set by Disable(value). */
    Bool_t FixedResult() const { return fbResult; }

    /** Inverts return values, when on is true, i.e. isTrue() will return false,
      * and isFalse() will return true. Whn on is false, inversion is reset. */
    virtual void Invert(Bool_t on);

    /** Prints counters and boolean members. */
    virtual void PrintCondition(Bool_t full=kTRUE);

    /** Prints counters and bar. */
    virtual void PrintBar();

    /** Default printout for root object lists */
    void Print(Option_t *opt = "") const override;

    /** Can be used in Test to store the result. Is retrieved by GetLast()
      * without further test. */
    void SetLast(Bool_t value) { fbLastResult=value; }

    /** Returns result of last Test. */
    Bool_t GetLast() const { return fbLastResult; }

    /** Copy values from cond to this. When counts is true, copy also counters. */
    virtual Bool_t UpdateFrom(TGo4Condition *cond, Bool_t counts);

    /** Method used by HTTP server to update some fields, specified in URL syntax */
    virtual Bool_t UpdateFromUrl(const char *rest_url_opt);

    virtual void GetValues(Int_t &dim, Double_t &x1, Double_t &y1, Double_t &x2, Double_t &y2);
    virtual Double_t GetXLow() const { return 0.; }
    virtual Double_t GetXUp() const { return 0.; }
    virtual Double_t GetYLow() const { return 0.; }
    virtual Double_t GetYUp() const { return 0.; }
    virtual TCutG *GetCut(Bool_t owner) { return nullptr; }

    /** Calculate value for histogram inside condition limits.
      * Option may specify "width" to scale channel sum with axes*/
    virtual Double_t GetIntegral(TH1 *, Option_t * = "") { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetMean(TH1 *, Int_t = 1)  { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetRMS(TH1 *, Int_t = 1)  { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetSkewness(TH1 *, Int_t = 1)  { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetCurtosis(TH1 *, Int_t = 1)  { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetXMax(TH1 *) { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetYMax(TH1 *) { return 0.; }

    /** Calculate value for histogram inside condition limits. */
    virtual Double_t GetCMax(TH1 *) { return 0.; }

    /** To be overwritten for condition array. By default, it returns this. */
    virtual const TGo4Condition *GetActiveCondition() const { return this; }

    /** defines if condition counters shall be reset in UpdateFrom method*/
    virtual void MarkReset(Bool_t on);

    void Clear(Option_t *opt = "") override;

    /** Sets values of external boolean flags to internal variables.
      * Overwritten in TGo4CondArray. */
    virtual void SetFlags(Bool_t enabled, Bool_t lastresult, Bool_t markreset,
                   Bool_t result, Bool_t truevalue, Bool_t falsevalue);

    /** Sets values of internal boolean flags to external variables.
      * Overwritten in TGo4CondArray. */
    virtual void GetFlags(Bool_t *enabled, Bool_t *lastresult, Bool_t *markreset,
                   Bool_t *result, Bool_t *truevalue, Bool_t *falsevalue);

    /** Set/Unset visibility. Overloaded for subclasses!*/
    virtual void SetVisible(Bool_t on) { fbVisible = on; }

    /** Visibility property for gui display in editor */
    virtual Bool_t IsVisible() const { return fbVisible; }

    /** Define if this condition has association with an analysis histogram */
    void SetHistogramLink(Bool_t on) { fbHistogramLink = on; }

    /** True if the histogram of given name is linked to this*/
    Bool_t IsHistogramLink() const { return fbHistogramLink; }

    /** Define the name of the associated histogram. */
    void SetHistogram(const char *name);

    /** Access name of the associated histogram. */
    const char *GetLinkedHistogram() const;

    /** Set reference to work histogram for statistics functions. Note
      * that this method does not change the histogram link definitions! */
    virtual void SetWorkHistogram(TH1 *histo) { fxHisto = histo; }

    /** access work histogram reference */
    TH1 *GetWorkHistogram() const { return fxHisto; }

    /** Replace default painter of this condition by external one.
      * if different views of this condition shall exist for different TPads,
      * the painter (i.e. view) can be exchanged before pad is updated. */
    virtual void SetPainter(TGo4ConditionPainter *painter);

    /** Configure graphical update tolerance for this condition individually.
      * by default, update tolerance is set to static value fgdUPDATEEPSILON */
    void SetUpdateEpsilon(Double_t val){fdUpdateEpsilon=val;}

    Double_t GetUpdateEpsilon() const { return fdUpdateEpsilon; }

    /** default value for graphical update tolerance. */
    static const Double_t fgdUPDATEEPSILON;

     /** This will save draw flags into  static default setup */
    void SaveLabelStyle();

    /** Initialize label setup from static default variables */
    void InitLabelStyle();

    /** Will reset label position to defaults
     * with other option than reset, just pop label to front*/
    void ResetLabel(Option_t *opt = "reset");

    virtual void SetLabelDraw(Bool_t on) { fbLabelDraw = on; } // *TOGGLE* *GETTER=IsLabelDraw
    virtual Bool_t IsLabelDraw() const { return fbLabelDraw; }

    virtual void SetLimitsDraw(Bool_t on) { fbLimitsDraw = on; } // *TOGGLE* *GETTER=IsLimitsDraw
    virtual Bool_t IsLimitsDraw() const { return fbLimitsDraw; }

    virtual void SetIntDraw(Bool_t on) { fbIntDraw = on; } // *TOGGLE* *GETTER=IsIntDraw
    virtual Bool_t IsIntDraw() const { return fbIntDraw; }

    virtual void SetXMeanDraw(Bool_t on) { fbXMeanDraw = on; } // *TOGGLE* *GETTER=IsXMeanDraw
    virtual Bool_t IsXMeanDraw() const { return fbXMeanDraw; }

    virtual void SetXRMSDraw(Bool_t on) { fbXRMSDraw = on; } // *TOGGLE* *GETTER=IsXRMSDraw
    virtual Bool_t IsXRMSDraw() const { return fbXRMSDraw; }

    virtual void SetYMeanDraw(Bool_t on) { fbYMeanDraw = on; } // *TOGGLE* *GETTER=IsYMeanDraw
    virtual Bool_t IsYMeanDraw() const { return fbYMeanDraw; }

    virtual void SetYRMSDraw(Bool_t on) { fbYRMSDraw = on; } // *TOGGLE* *GETTER=IsYRMSDraw
    virtual Bool_t IsYRMSDraw() const { return fbYRMSDraw; }

    virtual void SetXMaxDraw(Bool_t on) { fbXMaxDraw = on; } // *TOGGLE* *GETTER=IsXMaxDraw
    virtual Bool_t IsXMaxDraw() const { return fbXMaxDraw; }

    virtual void SetYMaxDraw(Bool_t on) { fbYMaxDraw = on; } // *TOGGLE* *GETTER=IsYMaxDraw
    virtual Bool_t IsYMaxDraw() const { return fbYMaxDraw; }

    virtual void SetCMaxDraw(Bool_t on) { fbCMaxDraw = on; } // *TOGGLE* *GETTER=IsCMaxDraw
    virtual Bool_t IsCMaxDraw() const { return fbCMaxDraw; }

    virtual void SetLabelNumFormat(const char *fmt) { fxNumFormat = fmt; }
    virtual const char *GetLabelNumFormat() const { return fxNumFormat.Data(); }

    virtual void SetChanged(Bool_t on = kTRUE)
    {
       if (on)
          fiIsChanged++;
       else
          fiIsChanged = 0;
    }
    virtual Int_t IsChanged() const { return fiIsChanged; }

    void SetMultiEdit(Bool_t on) { fbMultiEdit = on; }
    virtual Bool_t IsMultiEdit() const { return fbMultiEdit; }

    void SetDimension(Int_t d) { fiDim = d; }
    Int_t GetDimension() const { return fiDim; }

    void SetOwnedByEditor(Bool_t on) { fbOwnedByEditor = on; }
    Bool_t IsOwnedByEditor() const { return fbOwnedByEditor; }

    /** set index for array type subclasses */
    virtual void SetCurrentIndex(Int_t) {}

    /** get index for array type subclasses */
    virtual Int_t GetCurrentIndex() const { return 0; }

    /** for condition aggregates: return total number of conditions,
      * i.e.size of condition array */
    virtual Int_t GetNumberOfConditions() const { return 1; }

    virtual Int_t GetMemorySize() const;

    virtual Bool_t IsPolygonType() const { return kFALSE; }
    virtual Bool_t IsArrayType() const { return kFALSE; }

    void DeletePainter();

    /** default setting for all conditions for label draw on/off flag. */
    static Bool_t fgbLABELDRAW;

    /** default setting for all conditions for label draw limits flag. */
    static Bool_t fgbLIMITSDRAW;

    /** default setting for all conditions for label draw integral flag. */
    static Bool_t fgbINTDRAW;

    /** default setting for all conditions for label draw xmean flag. */
    static Bool_t fgbXMEANDRAW;

    /** default setting for all conditions for label draw xrms flag. */
    static Bool_t fgbXRMSDRAW;

    /** default setting for all conditions for label draw ymean flag. */
    static Bool_t fgbYMEANDRAW;

    /** default setting for all conditions for label draw yrms flag. */
    static Bool_t fgbYRMSDRAW;

    /** default setting for all conditions for label draw xmax flag. */
    static Bool_t fgbXMAXDRAW;

    /** default setting for all conditions for label draw ymax flag. */
    static Bool_t fgbYMAXDRAW;

    /** default setting for all conditions for label draw cmax flag. */
    static Bool_t fgbCMAXDRAW;

    /** default setting for all conditions with format string for numbers. */
   static TString fgxNUMFORMAT;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_RESET;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_RESULT;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_INVERT;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_VISIBLE;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_LABEL;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_LIMITS;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_INTEGRAL;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_XMEAN;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_XRMS;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_YMEAN;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_YRMS;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_XMAX;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_YMAX;
   /** web condition editor keyword used in UpdateFromUrl. */
   static TString fgxURL_CMAX;

   static void SetGlobalStyle(Bool_t LABELDRAW, Bool_t LIMITSDRAW, Bool_t INTDRAW,
                              Bool_t XMEANDRAW, Bool_t YMEANDRAW, Bool_t XRMSDRAW, Bool_t YRMSDRAW,
                              Bool_t XMAXDRAW, Bool_t YMAXDRAW, Bool_t CMAXDRAW, const char *NUMFORMAT);

   static void GetGlobalStyle(Bool_t &LABELDRAW, Bool_t &LIMITSDRAW, Bool_t &INTDRAW,
                              Bool_t &XMEANDRAW, Bool_t &YMEANDRAW, Bool_t &XRMSDRAW, Bool_t &YRMSDRAW,
                              Bool_t &XMAXDRAW, Bool_t &YMAXDRAW, Bool_t &CMAXDRAW, TString &NUMFORMAT);

  protected:

    /** Painter instance to display the condition in root pad. To
      * be used in Paint() which is called on TPad::Update automatically for
      * all TObjects appended to the pad. */
    TGo4ConditionPainter *fxPainter{nullptr};

    /** array with TObjStrings evaluated by UpdateFromUrl*/
    TObjArray *fxUrlOptionArray{nullptr}; //!

    /** re-build the list of url options from string*/
    void BuildUrlOptionArray(const char *rest_url_opt);

    /** returns true if key is present in list of url options. Otherwise false*/
    Bool_t UrlOptionHasKey(const char *key);

    /** Scan list of url options  for key. If found, return TString value.
         * If not found, return default value def_value*/
    TString GetUrlOptionAsString(const char *key, TString def_value);

    /** Scan list of url options  for key. If found, return Integer value.
     * If not found, return default value def_value*/
    Int_t GetUrlOptionAsInt(const char *key, Int_t def_value);

    /** Scan list of url options  for key. If found, return Double value.
     * If not found, return default value def_value*/
    Double_t GetUrlOptionAsDouble(const char *key, Double_t def_value);

    void SetPainted(Bool_t on) { fbIsPainted = on; }
    Bool_t IsPainted() const { return fbIsPainted; }

    const char *MakeScript(std::ostream &out, const char *varname, Option_t *opt = "", const char *arrextraargs = nullptr);

  private:

    /** Dimension (1 or 2). */
    Int_t fiDim{0};

    Bool_t fbMarkReset{kFALSE};

    /** This can be used to store the last result by SetLast(bool).
      * It can then be retrieved by GetLast() without new testing. */
    Bool_t fbLastResult{kFALSE};

    /** These are inverted by Invert(true), and restored by Invert(false) */
    Bool_t fbTrue{kTRUE};

    /** These are inverted by Invert(true), and  restored by Invert(false). */
    Bool_t fbFalse{kFALSE};

    /** Counts the number of calls of the Test method with true result. */
    Int_t fiTrueCounts{0};

    /** Counts the number of calls of the Test method. */
    Int_t fiCounts{0};

    /** Flag to enable/disable this condition. Disabled condition will
      * always return fbResult.
      * Counters are incremented even if disabled.
      * After instantiation, fbEnabled will be false and fbResult true. */
    Bool_t fbEnabled{kFALSE};

    /** If fbEnabled is false, return this as result. */
    Bool_t fbResult{kFALSE};

    /** Flag to indicate that properties of all conditions in array are edited/used.
      * for use in TGo4CondArray or other multi-condition implementations*/
    Bool_t fbMultiEdit{kFALSE};

    /** Property to store visibility of condition on display */
    Bool_t fbVisible{kFALSE};

    /** is true if this condition has link to a histogram name */
    Bool_t fbHistogramLink{kFALSE};

    /** contains associated histogram name */
    TString fxHistoName;

    /** Precision for the check if graphical representation change requires update */
    Double_t fdUpdateEpsilon{0};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbLabelDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbLimitsDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbIntDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbXMeanDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbXRMSDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbYMeanDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbYRMSDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbXMaxDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbYMaxDraw{kFALSE};

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbCMaxDraw{kFALSE};

    /** format string for all label numbers. */
    TString fxNumFormat;

    /** Reference to currently used histogram (for statistics boxes). This
      * is _not_ necessarily the same histogram as specified in the linked histogram! */
    TH1 *fxHisto{nullptr};   //!

    /** Counter indicate how many times condition was changed from graphical view */
    Int_t fiIsChanged{0}; //!

    /** Flag to prevent Paint() method to redraw condition views after UnDraw.
      * Problem with Paint() called from ROOT event loop. */
    Bool_t fbIsPainted{kFALSE}; //!

    /** Flag to suppress interactive deletion of condition. */
    Bool_t fbOwnedByEditor{kFALSE}; //!

    /** This allows to treat conditions streamed into pad */
    Bool_t fbStreamedCondition{kFALSE}; //!

   ClassDefOverride(TGo4Condition,8)
};

#endif //TGO4CONDITION_H
