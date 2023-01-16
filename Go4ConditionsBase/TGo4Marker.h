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

#ifndef TGO4MARKER_H
#define TGO4MARKER_H

#include "TMarker.h"
#include "TString.h"

class TGo4MarkerPainter;
class TH1;
class TVirtualPad;

/**
 * Extended graphical marker class for go4 condition editor and
 * viewpanel. Offers coordinate label updated from pad. Scans
 * work histogram for bin numbers and contents.
 * @author Joern Adamczewski
 * @since 06/2004
 */

class TGo4Marker : public TMarker {

public:
   TGo4Marker();
   TGo4Marker(Double_t x, Double_t y, Int_t style);
   virtual ~TGo4Marker();

   /** Implement standard root print.
     *  If option is "go4log", print to TGo4Log::Message. Otherwise use cout */
   void Print(Option_t* opt="") const  override;

   /** Display marker with plain root canvas and in go4 viewpanel */
   void Paint(Option_t* opt="")  override;

   /** Draw this marker on current pad */
   void Draw(Option_t* opt="")  override;

   /** Erase view of this marker from the pad */
   virtual void UnDraw(Option_t* opt="");

    /** Re-implement TObject method to pop all our views to front*/
    void Pop() override;

   /** Reimplemented to pop our labels to front when selected */
   void ExecuteEvent(Int_t event, Int_t px, Int_t py) override;

   /** Factory method to generate (optional subclass) implementation for painter */
   virtual TGo4MarkerPainter *CreatePainter();

   void DeletePainter();

   void SetVisible(Bool_t on) { fbVisible = on; }
   Bool_t IsVisible() const { return fbVisible; }
   virtual void SetName(const char *name) { fxName = name; } // *MENU*
   // void DeleteMarker(); // *MENU*

   /** Set marker to position according to work histogram axis.  */
   void SetToBin(Int_t xbin, Int_t ybin = 0);        // *MENU*
   void SetLabelDraw(Bool_t on) { fbHasLabel = on; } // // *TOGGLE* *GETTER=HasLabel
   Bool_t HasLabel() const { return fbHasLabel; }
   void SetLineDraw(Bool_t on) { fbHasConnector = on; } // *TOGGLE* *GETTER=HasConnector
   Bool_t HasConnector() const { return fbHasConnector; }
   void SetXDraw(Bool_t on) { fbXDraw = on; } // *TOGGLE* *GETTER=IsXDraw
   Bool_t IsXDraw() const { return fbXDraw; }
   void SetYDraw(Bool_t on) { fbYDraw = on; } // *TOGGLE* *GETTER=IsYDraw
   Bool_t IsYDraw() const { return fbYDraw; }
   void SetXbinDraw(Bool_t on) { fbXbinDraw = on; } // *TOGGLE* *GETTER=IsXbinDraw
   Bool_t IsXbinDraw() const { return fbXbinDraw; }
   void SetYbinDraw(Bool_t on) { fbYbinDraw = on; } // *TOGGLE* *GETTER=IsYbinDraw
   Bool_t IsYbinDraw() const { return fbYbinDraw; }
   void SetContDraw(Bool_t on) { fbContDraw = on; } // *TOGGLE* *GETTER=IsContDraw
   Bool_t IsContDraw() const { return fbContDraw; }

   const char *GetNumFormat() const { return fxNumFormat.Data(); }
   void SetNumFormat(const char *fmt="%.4E"){ fxNumFormat = fmt; }// *MENU*

   /** This will save draw flags into  static default setup */
   void SaveLabelStyle(); // *MENU*

   /** Will reset label position to defaults */
   void ResetLabel(); // *MENU*

   /** Initialize label setup from static default variables */
   void InitLabelStyle();

   void SetHistogram(TH1* histo);
   TH1* GetHistogram() const { return fxHisto; }

   /** TMarker is not TNamed, so we implement name ourself*/
   const char *GetName() const override { return fxName.Data(); }

   void SetXY(Double_t x, Double_t y)
   {
      SetX(x);
      SetY(y);
   }

   /** Deliver x axis bin number of work histogram at marker position */
   Int_t GetXbin() const;
   /** Deliver y axis bin number of work histogram at marker position */
   Int_t GetYbin() const;
   /** Deliver bin content of work histogram at marker position */
   Int_t GetCont() const;
   /** Calculate initial label coordinate from marker position */
   Double_t GetLabelX() const;
   /** Calculate initial label coordinate from marker position */
   Double_t GetLabelY() const;

   static Int_t GetInstances();

   TVirtualPad *GetDrawPad() const { return fxDrawPad; }

   /** True if current draw pad is still existing */
   Bool_t CheckDrawPad() const;
   /** True if current histogram  is still existing */
   Bool_t CheckHistogram() const;

   /** default setting for all markers of label on/off flag. */
   static Bool_t fgbHASLABEL;

   /** default setting for all markers of connector line flag. */
   static Bool_t fgbHASCONNECTOR;

   /** default setting for all markers of xdraw flag. */
   static Bool_t fgbXDRAW;

   /** default setting for all markers of ydraw flag. */
   static Bool_t fgbYDRAW;

   /** default setting for all markers of xbindraw flag. */
   static Bool_t fgbXBINDRAW;

   /** default setting for all markers of ybindraw flag. */
   static Bool_t fgbYBINDRAW;

   /** default setting for all markers of bin content flag. */
   static Bool_t fgbCONTDRAW;

   /** default setting for all conditions with format string for numbers. */
   static TString fgxNUMFORMAT;

   static void SetGlobalStyle(Bool_t HASLABEL, Bool_t HASCONNECTOR, Bool_t XDRAW, Bool_t YDRAW,
                              Bool_t XBINDRAW,Bool_t YBINDRAW,Bool_t CONTDRAW, const char *NUMFORMAT);

   static void GetGlobalStyle(Bool_t &HASLABEL, Bool_t &HASCONNECTOR, Bool_t &XDRAW, Bool_t &YDRAW,
                              Bool_t &XBINDRAW, Bool_t &YBINDRAW, Bool_t &CONTDRAW, TString &NUMFORMAT);

protected:

   /** Painter instance to display the condition in root pad. To
     * be used in Paint() which is called on TPad::Update automatically for
     * all TObjects appended to the pad. **/
   TGo4MarkerPainter* fxPainter{nullptr};

   void SetPainted(Bool_t on) { fbIsPainted=on; }
   Bool_t IsPainted() const { return fbIsPainted; }

   void SetDrawPad(TVirtualPad* pad);


private:

   /** since TMarker is not TNamed, we provide identifier string here: */
   TString fxName;

   /** Flag to prevent Paint() method to redraw marker views after UnDraw.
     * Problem with Paint() called from ROOT event loop. */
   Bool_t fbIsPainted{kFALSE}; //!

   /** Visibility of this marker on pad. For marker array in editor */
   Bool_t fbVisible{kTRUE};

   /** Switch marker label on or off. */
   Bool_t fbHasLabel{kTRUE};

   /** Switch connection line between marker and label on or off. */
   Bool_t fbHasConnector{kTRUE};

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbXDraw{kTRUE};

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbYDraw{kTRUE};

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbXbinDraw{kTRUE};

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbYbinDraw{kTRUE};

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbContDraw{kTRUE};

   /** format string for all label numbers. */
   TString fxNumFormat;

   /** Reference to currently used histogram (for bin contents). */
   TH1* fxHisto{nullptr}; //!

   /** Remember last pad used for Draw. Needed for condition editor to
    * prevent displaying marker on pad other than the working pad. */
   TVirtualPad* fxDrawPad{nullptr}; //!

   /** use this counter for automatic naming and coloring*/
   static UInt_t fguInstanceCounter;

   ClassDefOverride(TGo4Marker,1)
};

#endif //TGO4MARKER_H

