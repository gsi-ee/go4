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

#ifndef TGO4CONDITION_H
#define TGO4CONDITION_H

#include "TNamed.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include "go4iostream.h"

#define kCOPYCOUNTS true
#define kNOCOPYCOUNTS false
#define kWINDOW 1
#define kPOLYGON 2

class TH1;
class TVirtualPad;
class TCutG;

class TGo4HistogramEntry;
class TGo4EventElement;
class TGo4ConditionPainter;



class

#ifndef __CINT__
#ifdef WIN32
#ifndef BUILDING_GO4BASE_DLL
__declspec(dllimport)
#endif
#endif
#endif


TGo4Condition : public TNamed, public TAttLine, public TAttFill {

  friend class TGo4CondArray;

  public:
    TGo4Condition();

    TGo4Condition(const char* name, const char* title = "Go4 Condition");

    virtual ~TGo4Condition();

    /** Display condition with plain root canvas and in go4 viewpanel */
    virtual void Paint(Option_t* opt="");

    /** Draw this condition on current pad */
    virtual void Draw(Option_t* opt="");

    /** Erase view of this condition from the pad */
    virtual void UnDraw(Option_t* opt="");

    /** Re-implement TObject method to pop all our views to front*/
    virtual void Pop();

    /** Factory method to generate the subclass implentation for painter */
    virtual TGo4ConditionPainter* CreatePainter();

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
    virtual Bool_t Test(Bool_t result) { return true; }

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
     * between the chained conditions (e.g. a locical "and",
     * "or", "xor" relationships are all possible). Depends on the
     * implementation of Test() method. The chained conditions are
     * _not_ owned by the top level condition and are not deleted
     * by dtor, thus one condition instance may be chained to
     * several others.
     */
    virtual void AddCondition(TGo4Condition* next);

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
    virtual void Print(Option_t* opt="") const;

    /** Can be used in Test to store the result. Is retrieved by GetLast()
      * without further test. */
    void SetLast(Bool_t value) { fbLastResult=value; }

    /** Returns result of last Test. */
    Bool_t GetLast() { return fbLastResult; }

    /** Copy values from cond to this. When counts is true, copy also counters. */
    virtual Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts);
    virtual void GetValues(Int_t & dim, Double_t & x1, Double_t & y1, Double_t & x2, Double_t & y2);
    virtual Double_t GetXLow();
    virtual Double_t GetXUp();
    virtual Double_t GetYLow();
    virtual Double_t GetYUp();
    virtual TCutG* GetCut(Bool_t owner);

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

    /** To be overwritten for condition array. By default, it returns this. */
    virtual TGo4Condition* GetActiveCondition();

    /** defines if condition counters shall be reset in UpdateFrom method*/
    virtual void MarkReset(Bool_t on);

    virtual void Clear(Option_t* opt="");

    /** Sets values of external boolean flags to internal variables.
      * Overwritten in TGo4CondArray. */
    virtual void SetFlags(Bool_t enabled, Bool_t lastresult, Bool_t markreset,
                   Bool_t result, Bool_t truevalue, Bool_t falsevalue);

    /** Sets values of internal boolean flags to external variables.
      * Overwritten in TGo4CondArray. */
    virtual void GetFlags(Bool_t* enabled, Bool_t* lastresult, Bool_t* markreset,
                   Bool_t* result, Bool_t* truevalue, Bool_t* falsevalue);

    /** Set/Unset visibility. Overloaded for subclasses!*/
    virtual void SetVisible(Bool_t on);

    /** Visibility property for gui display in editor */
    virtual Bool_t IsVisible();

    /** Define if this condition has association with an analysis histogram */
    void SetHistogramLink(Bool_t on);

    /** True if the histogram of given name is linked to this*/
    Bool_t IsHistogramLink();

    /** Define the name of the associated histogram. */
    void SetHistogram(const char* name);

    /** Access name of the associated histogram. */
    const char* GetLinkedHistogram();

    /** Set reference to work histogram for statistics functions. Note
      * that this method does not change the histogram link definitions! */
    void SetWorkHistogram(TH1* histo);

    /** access work histogram reference */
    TH1* GetWorkHistogram(){return fxHisto;}

    /** Replace default painter of this condition by external one.
      * if different views of this condition shall exist for different TPads,
      * the painter (i.e. view) can be exchanged before pad is updated. */
    virtual void SetPainter(TGo4ConditionPainter* painter);

    /** Configure graphical update tolerance for this condition individually.
      * by default, update tolerance is set to static value fgdUPDATEEPSILON */
    void SetUpdateEpsilon(Double_t val){fdUpdateEpsilon=val;}

    Double_t GetUpdateEpsilon(){return fdUpdateEpsilon;}

    /** default value for graphical update tolerance. */
    static const Double_t fgdUPDATEEPSILON;

     /** This will save draw flags into  static default setup */
    void SaveLabelStyle();

    /** Initialize label setup from static default variables */
    void InitLabelStyle();

    /** Will reset label position to defaults
      * with other option than reset, just pop label to front*/
    void ResetLabel(Option_t* opt="reset");

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

    virtual void SetChanged(Bool_t on) { if (on) fiIsChanged++; else fiIsChanged = 0; }
    virtual Int_t IsChanged() { return fiIsChanged; }

    void SetMultiEdit(Bool_t on){fbMultiEdit=on;}
    virtual Bool_t IsMultiEdit(){return fbMultiEdit;}

    void SetDimension(Int_t d) { fiDim=d; }
    Int_t GetDimension() { return fiDim; }

    void SetOwnedByEditor(Bool_t on){fbOwnedByEditor=on;}
    Bool_t IsOwnedByEditor(){return fbOwnedByEditor;}

    /** set index for array type subclasses */
    virtual void SetCurrentIndex(Int_t ix);

    /** get index for array type subclasses */
    virtual Int_t GetCurrentIndex();

    /** for condition aggregates: return total number of conditions,
      * i.e.size of condition array */
    virtual Int_t GetNumberOfConditions();

    virtual Int_t GetMemorySize();

    virtual Bool_t IsPolygonType();
    virtual Bool_t IsArrayType();

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

  protected:

    /** Painter instance to display the condition in root pad. To
      * be used in Paint() which is called on TPad::Update automatically for
      * all TObjects appended to the pad. */
    TGo4ConditionPainter* fxPainter;

    /** temporary histogram used to calculate statistics on
      * polygon cuts. Will be recalculated only if boundaries
      * habe changed.*/
    TH1* fxCutHis;   //!

    void SetPainted(Bool_t on) { fbIsPainted=on; }
    Bool_t IsPainted() const { return fbIsPainted; }

    const char* MakeScript(ostream& out, const char* varname, Option_t* opt = "", const char* arrextraargs = 0);

  private:

    /** Dimension (1 or 2). */
    Int_t fiDim;

    Bool_t fbMarkReset;

    /** This can be used to store the last result by SetLast(bool).
      * It can then be retrieved by GetLast() without new testing. */
    Bool_t fbLastResult;

    /** These are inverted by Invert(true), and restored by Invert(false) */
    Bool_t fbTrue;

    /** These are inverted by Invert(true), and  restored by Invert(false). */
    Bool_t fbFalse;

    /** Counts the number of calls of the Test method with true result. */
    Int_t fiTrueCounts;

    /** Counts the number of calls of the Test method. */
    Int_t fiCounts;

    /** Flag to enable/disable this condition. Disabled condition will
      * always return fbResult.
      * Counters are incremented even if disabled.
      * After instantiation, fbEnabled will be false and fbResult true. */
    Bool_t fbEnabled;

    /** If fbEnabled is false, return this as result. */
    Bool_t fbResult;

    /** Flag to indicate that properties of all conditions in array are edited/used.
      * for use in TGo4CondArray or other multi-condition implementations*/
    Bool_t fbMultiEdit;

    /** Property to store visibility of condition on display */
    Bool_t fbVisible;

    /** is true if this condition has link to a histogram name */
    Bool_t fbHistogramLink;

    /** contains associatied histogram name */
    TString fxHistoName;

    /** Precision for the check if graphical representation change requires update */
    Double_t fdUpdateEpsilon;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbLabelDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbLimitsDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbIntDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbXMeanDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbXRMSDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbYMeanDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbYRMSDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbXMaxDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbYMaxDraw;

    /** If true, draw corresponding value as label on working pad */
    Bool_t fbCMaxDraw;

    /** Reference to currently used histogram (for statistics boxes). This
      * is _not_ necessarily the same histogram as specified in the linked histogram! */
    TH1* fxHisto;   //!

    /** Counter indicate how many times condition was changed from graphical view */
    Int_t fiIsChanged; //!

    /** Flag to prevent Paint() method to redraw condition views after UnDraw.
      * Problem with Paint() called from ROOT event loop. */
    Bool_t fbIsPainted; //!

    /** Flag to suppress interactive deletion of condition. */
    Bool_t fbOwnedByEditor; //!

    /** This allows to treat conditions streamed into pad */
    Bool_t fbStreamedCondition;//!

   ClassDef(TGo4Condition,6)
};

#endif //TGO4CONDITION_H
