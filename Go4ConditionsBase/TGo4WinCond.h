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

#ifndef TGO4WINCOND_H
#define TGO4WINCOND_H

#include "TGo4Condition.h"

class TH1;

class TGo4WinCond : public TGo4Condition {
   public:
      TGo4WinCond();

      TGo4WinCond(const char *name, const char *title = "Go4 window condition");

      virtual ~TGo4WinCond();

      /** Factory method to generate the subclass implementation for painter */
      TGo4ConditionPainter* CreatePainter() override;

      /** Replace default painter of this condition by external one.
        * if different views of this condition shall exist for different TPads,
        * the painter (i.e. view) can be exchanged before pad is updated. */
      void SetPainter(TGo4ConditionPainter* painter) override;

      using TGo4Condition::SetValues;

      /** Set limits and internal dimension to 1 */
      void SetValues(Double_t low1, Double_t up1) override;

      /** Set limits and internal dimension to 2 */
      void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2) override;

      using TGo4Condition::Test;

      /** Test v1 versus first limits: includes lower, excludes upper. */
      Bool_t Test(Double_t v1) override;

      /** Test v1 versus first limits and v2 versus second limits
        * Includes lower, excludes upper. */
      Bool_t Test(Double_t v1, Double_t v2) override;

      /** Returns limits. */
      void GetValues(Int_t &dim, Double_t &x1, Double_t &y1, Double_t &x2, Double_t &y2) override;

      Double_t GetXLow() override { return fLow1; }
      Double_t GetXUp() override { return fUp1; }
      Double_t GetYLow() override { return fLow2; }
      Double_t GetYUp() override { return fUp2; }

      void SetXLow(Double_t v) { fLow1 = v; }
      void SetXUp(Double_t v) { fUp1 = v; }
      void SetYLow(Double_t v) { fLow2 = v; }
      void SetYUp(Double_t v) { fUp2 = v; }

      /** Printout. If limits is true, print limits too. */
      void PrintCondition(Bool_t full = kTRUE) override;

      /** Copy values from cond to this. When counts is true, copy also counters. */
      Bool_t UpdateFrom(TGo4Condition * cond, Bool_t counts) override;

      /** Method used by HTTP server to update some fields, specified in URL syntax */
      Bool_t UpdateFromUrl(const char *rest_url_opt) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetIntegral(TH1* histo, Option_t* opt="") override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetMean(TH1* histo, Int_t axis=1) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetRMS(TH1* histo, Int_t axis=1) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetSkewness(TH1* histo, Int_t axis=1) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetCurtosis(TH1* histo, Int_t axis=1) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetXMax(TH1* histo) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetYMax(TH1* histo) override;

      /** Calculate value for histogram inside condition limits. */
      Double_t GetCMax(TH1* histo) override;

      Bool_t IsPolygonType()  override { return kFALSE; }

      Int_t GetMemorySize() override;

      /** Standard way to store parameter in form of macro */
      void SavePrimitive(std::ostream& fs, Option_t* = "") override;

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
      Double_t fLow1{0};

      /** exclusive upper limit */
      Double_t fUp1{0};

      /** inclusive lower limit */
      Double_t fLow2{0};

      /** exclusive upper limit */
      Double_t fUp2{0};

      /** Saved minimum x-axis bin number for active histogram range. */
      Int_t fiSaveXMin{0};

      /** Saved maximum x-axis bin number for active histogram range. */
      Int_t fiSaveXMax{0};

      /** Saved minimum y-axis bin number for active histogram range. */
      Int_t fiSaveYMin{0};

      /** Saved maximum y-axis bin number for active histogram range. */
      Int_t fiSaveYMax{0};

      /** Remember ranges of current histo and set range to active window. */
      void SetHistogramRanges(TH1* histo);

      /** Restore original ranges of current histo. */
      void RestoreHistogramRanges(TH1* histo);

   ClassDefOverride(TGo4WinCond,7)
};

#endif //TGO4WINCOND_H
