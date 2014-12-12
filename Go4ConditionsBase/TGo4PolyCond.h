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

#ifndef TGO4POLYCOND_H
#define TGO4POLYCOND_H

#include "TGo4Condition.h"

class TH1;
class TH2;

class TGo4PolyCond : public TGo4Condition {
   public:
      TGo4PolyCond();

      TGo4PolyCond(const char* name, const char* title = "Go4 polygon condition");

      virtual ~TGo4PolyCond();

      /** Factory method to generate the subclass implementation for painter */
      virtual TGo4ConditionPainter* CreatePainter();

      /** Replace default painter of this condition by external one.
        * if different views of this condition shall exist for different TPads,
        * the painter (i.e. view) can be exchanged before pad is updated. */
      virtual void SetPainter(TGo4ConditionPainter* painter);

//      virtual void SetValues() {}
//      virtual void SetValues(Double_t low1, Double_t up1) {}
//      virtual void SetValues(Double_t low1, Double_t up1, Double_t low2, Double_t up2) {}

      /** Delete old cut and create a new cut with X,Y values. */
      virtual void SetValues(Double_t * x, Double_t * y, Int_t len);

      /** Delete old cut and create new one by cloning newcut. */
      virtual void SetValues(TCutG * newcut);

      /** Delete old cut and get ownership over newcut. */
      void SetValuesDirect(TCutG * newcut);

//      virtual Bool_t Test() { return true; }
//      virtual Bool_t Test(Double_t x) { return true; }
//      virtual Bool_t Test(Bool_t on) { return true; }

      /** Test if X,Y are inside. */
      virtual Bool_t Test(Double_t x, Double_t y);

      /** minimum extension of polygon in x axis */
      virtual Double_t GetXLow();

      /** maximum extension of polygon in x axis */
      virtual Double_t GetXUp();

      /** minimum extension of polygon in y axis */
      virtual Double_t GetYLow();

      /** maximum extension of polygon in y axis */
      virtual Double_t GetYUp();

      /** Used to return the cut pointer of a TGo4PolyCond.
        * In source, pointer is set to 0, when changeowner is true.
        * Thus source object can be deleted without deleting the cut
        * which is now owned by the caller.
        * When changeowner is false, the pointer can only be used to
        * clone a new cut. The cut must not be deleted! */
      TCutG* GetCut(Bool_t changeowner);

      /** Printout values. If points is true, printout polygon points. */
      virtual void PrintCondition(Bool_t points = kTRUE);

      /** Copy values from cond to this.
        * Get a clone cut from cond by CloneCut as new cut. If counts is true, copy counters too. */
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

      /** Remove all references to any TCutGs from ROOT list of specials.
        * To be used after loading condition from file, updating contition, or setting of cut. */
      static void CleanupSpecials();

      static TString NextAvailableName();

  private:
      /** Returns pointer to a cloned cut from source.
        * Name is set to callers name.
        * Used internally by UpdateFrom. */
      TCutG * CloneCut(TGo4PolyCond * source);

      /** Create clone of source histogram that only has
        * contents for bins inside the current TCutG.
        * For calculation of statistical values. */
      TH2* CreateCutHistogram(TH1* source);

      /** Cut pointer. */
      TCutG* fxCut;

   ClassDef(TGo4PolyCond,7)
};

#endif //TGO4POLYCOND_H
