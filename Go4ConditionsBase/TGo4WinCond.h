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

#ifndef TGO4WINCOND_H
#define TGO4WINCOND_H

#include "TGo4Condition.h"

class TH1;

class TGo4WinCond : public TGo4Condition {
   public:
      TGo4WinCond();

      TGo4WinCond(const char* name, const char* title = "Go4 window condition");

      virtual ~TGo4WinCond();

      /** Factory method to generate the subclass implementation for painter */
      virtual TGo4ConditionPainter* CreatePainter();

      /** Replace default painter of this condition by external one.
        * if different views of this condition shall exist for different TPads,
        * the painter (i.e. view) can be exchanged before pad is updated. */
      virtual void SetPainter(TGo4ConditionPainter* painter);

//      virtual void SetValues() {}
//      virtual void SetValues(TCutG * newcut) {}
//      virtual void SetValues(Double_t * x, Double_t * y, Int_t len) {}

      /** Set limits and internal dimension to 1 */
      virtual void SetValues(Double_t low1, Double_t up1);

      /** Set limits and internal dimension to 2 */
      virtual void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2);

//      virtual Bool_t Test() { return true; }
//      virtual Bool_t Test(Bool_t on) { return true; }

      /** Test v1 versus first limits: includes lower, excludes upper. */
      virtual Bool_t Test(Double_t v1);

      /** Test v1 versus first limits and v2 versus second limits
        * Includes lower, excludes upper. */
      virtual Bool_t Test(Double_t v1, Double_t v2);

      /** Returns limits. */
      virtual void GetValues(Int_t & dim, Double_t & x1, Double_t & y1, Double_t & x2, Double_t & y2);

      inline virtual Double_t GetXLow(){return fLow1;}
      inline virtual Double_t GetXUp() {return fUp1;}
      inline virtual Double_t GetYLow(){return fLow2;}
      inline virtual Double_t GetYUp() {return fUp2;}

      /** Printout. If limits is true, print limits too. */
      virtual void PrintCondition(Bool_t full = kTRUE);

      /** Copy values from cond to this. When counts is true, copy also counters. */
      Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts);

      /** Method used by HTTP server to update some fields, specified in URL syntax */
      virtual Bool_t UpdateFromUrl(const char* rest_url_opt);


      /** Calculate value for histogram inside condition limits. */
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

      virtual Int_t GetMemorySize();

      /** Standard way to store parameter in form of macro */
      virtual void SavePrimitive(std::ostream& fs, Option_t* = "");


      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_XLOW;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_XUP;

      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_YLOW;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_YUP;


   private:

      /** inclusive lower limit */
      Double_t fLow1;

      /** exclusive upper limit */
      Double_t fUp1;

      /** inclusive lower limit */
      Double_t fLow2;

      /** exclusive upper limit */
      Double_t fUp2;

      /** Saved minimum x-axis bin number for active histogram range. */
      Int_t fiSaveXMin;

      /** Saved maximum x-axis bin number for active histogram range. */
      Int_t fiSaveXMax;

      /** Saved minimum y-axis bin number for active histogram range. */
      Int_t fiSaveYMin;

      /** Saved maximum y-axis bin number for active histogram range. */
      Int_t fiSaveYMax;

      /** Remember ranges of current histo and set range to active window. */
      void SetHistogramRanges(TH1* histo);

      /** Restore original ranges of current histo. */
      void RestoreHistogramRanges(TH1* histo);

   ClassDef(TGo4WinCond,7)
};

#endif //TGO4WINCOND_H
