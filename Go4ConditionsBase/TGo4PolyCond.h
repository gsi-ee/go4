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

#ifndef TGO4POLYCOND_H
#define TGO4POLYCOND_H

#include "TGo4Condition.h"

class TH1;
class TH2;

class TGo4PolyCond : public TGo4Condition {
   public:
      TGo4PolyCond();

      TGo4PolyCond(const char *name, const char *title = "Go4 polygon condition");

      virtual ~TGo4PolyCond();

      /** Set reference to work histogram for statistics functions */
      void SetWorkHistogram(TH1* histo) override;

      /** Factory method to generate the subclass implementation for painter */
      TGo4ConditionPainter* CreatePainter() override;

      /** Replace default painter of this condition by external one.
        * if different views of this condition shall exist for different TPads,
        * the painter (i.e. view) can be exchanged before pad is updated. */
      void SetPainter(TGo4ConditionPainter* painter) override;

      using TGo4Condition::SetValues;

      /** Delete old cut and create a new cut with X,Y values. */
      void SetValues(Double_t *x, Double_t *y, Int_t len) override;

      /** Delete old cut and create new one by cloning newcut. */
      void SetValues(TCutG *newcut) override;

      /** Delete old cut and get ownership over newcut. */
      void SetValuesDirect(TCutG *newcut);

      using TGo4Condition::Test;

      /** Test if X,Y are inside. */
      Bool_t Test(Double_t x, Double_t y) override;

      /** minimum extension of polygon in x axis */
      Double_t GetXLow() override;

      /** maximum extension of polygon in x axis */
      Double_t GetXUp() override;

      /** minimum extension of polygon in y axis */
      Double_t GetYLow() override;

      /** maximum extension of polygon in y axis */
      Double_t GetYUp() override;

      /** Used to return the cut pointer of a TGo4PolyCond.
        * In source, pointer is set to 0, when changeowner is true.
        * Thus source object can be deleted without deleting the cut
        * which is now owned by the caller.
        * When changeowner is false, the pointer can only be used to
        * clone a new cut. The cut must not be deleted! */
      TCutG *GetCut(Bool_t changeowner) override;

      /** Printout values. If points is true, printout polygon points. */
      void PrintCondition(Bool_t points = kTRUE) override;

      /** Copy values from cond to this.
        * Get a clone cut from cond by CloneCut as new cut. If counts is true, copy counters too. */
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

      Bool_t IsPolygonType() override;

      Int_t GetMemorySize() override;

      /** Standard way to store parameter in form of macro */
      void SavePrimitive(std::ostream& fs, Option_t* = "") override;

      /** Remove all references to any TCutGs from ROOT list of specials.
        * To be used after loading condition from file, updating contition, or setting of cut. */
      static void CleanupSpecials();

      static TString NextAvailableName();


      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_NPOINTS;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_XPRE;
      /** web condition editor keyword used in UpdateFromUrl. */
      static TString fgxURL_YPRE;

  private:
      /** Returns pointer to a cloned cut from source.
        * Name is set to callers name.
        * Used internally by UpdateFrom. */
      TCutG *CloneCut(TGo4PolyCond * source);

      /** Set new cut histogram */
      void ClearCutHis();

      /** Returns true if fxCutHis can be used, create if possible */
      Bool_t IsCutHis(TH1* source);

      /** Cut pointer. */
      TCutG *fxCut{nullptr};

      /** temporary histogram used to calculate statistics on
        * polygon cuts. Will be recalculated only if boundaries
        * have changed.*/
      TH2 *fxCutHis{nullptr};   //!


   ClassDefOverride(TGo4PolyCond,7)
};

#endif //TGO4POLYCOND_H
