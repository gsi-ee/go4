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

#ifndef TGO4MARKER_H
#define TGO4MARKER_H

#include "TMarker.h"

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

class

#ifndef __CINT__
#ifdef WIN32
#ifndef BUILDING_GO4BASE_DLL
__declspec(dllimport)
#endif
#endif
#endif

TGo4Marker : public TMarker {

public:
   TGo4Marker();
   TGo4Marker(Double_t x, Double_t y, Int_t style);
   virtual ~TGo4Marker();

   /** Implement standard root print.
     *  If option is "go4log", print to TGo4Log::Message. Otherwise use cout */
   virtual void Print(Option_t* opt="") const;
   /** If option is "go4log", print to TGo4Log::Message. Otherwise use cout */
   void PrintMarker(Option_t* opt="");

   /** Display marker with plain root canvas and in go4 viewpanel */
   virtual void Paint(Option_t* opt="");

   /** Draw this marker on current pad */
   virtual void Draw(Option_t* opt="");

   /** Erase view of this marker from the pad */
   virtual void UnDraw(Option_t* opt="");

    /** Re-implement TObject method to pop all our views to front*/
    virtual void Pop();


   /** Reimplemented to pop our labels to front when selected */
   virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);



   /** Factory method to generate (optional subclass) implementation for painter */
   virtual TGo4MarkerPainter* CreatePainter();


   void SetVisible(Bool_t on){fbVisible=on;}
   Bool_t IsVisible(){return fbVisible;}
   virtual void SetName(const char* name){fxName=name;} // *MENU*
   //void DeleteMarker(); // *MENU*

   /** Set marker to position according to work histogram axis.  */
   void SetToBin(Int_t xbin, Int_t ybin=0); // *MENU*
   void SetLabelDraw(Bool_t on){fbHasLabel=on;} // *MENU*
   Bool_t HasLabel(){return fbHasLabel;}
   void SetLineDraw(Bool_t on){fbHasConnector=on;} // *MENU*
   Bool_t HasConnector(){return fbHasConnector;}
   void SetXDraw(Bool_t on){fbXDraw=on;}// *MENU*
   Bool_t IsXDraw(){return fbXDraw;}
   void SetYDraw(Bool_t on){fbYDraw=on;}// *MENU*
   Bool_t IsYDraw(){return fbYDraw;}
   void SetXbinDraw(Bool_t on){fbXbinDraw=on;}// *MENU*
   Bool_t IsXbinDraw(){return fbXbinDraw;}
   void SetYbinDraw(Bool_t on){fbYbinDraw=on;}// *MENU*
   Bool_t IsYbinDraw(){return fbYbinDraw;}
   void SetContDraw(Bool_t on){fbContDraw=on;}// *MENU*
   Bool_t IsContDraw(){return fbContDraw;}

   /** This will save draw flags into  static default setup */
   void SaveLabelStyle();// *MENU*

   /** Will reset label position to defaults */
   void ResetLabel();// *MENU*

   /** Initialize label setup from static default variables */
   void InitLabelStyle();

   void SetHistogram(TH1* histo);
   TH1* GetHistogram() const { return fxHisto; }

   /** TMarker is not TNamed, so we implement name ourself*/
   virtual const char* GetName() const {return fxName.Data();}

   /** Deliver x axis bin number of work histogram at marker position */
   Int_t GetXbin();
   /** Deliver y axis bin number of work histogram at marker position */
   Int_t GetYbin();
   /** Deliver bin content of work histogram at marker position */
   Int_t GetCont();
   /** Calculate initial label coordinate from marker position */
   Double_t GetLabelX();
   /** Calculate initial label coordinate from marker position */
   Double_t GetLabelY();

   static Int_t GetInstances(){return fguInstanceCounter;}

   TVirtualPad* GetDrawPad(){return fxDrawPad;}

   /** True if current draw pad is still exisiting */
   Bool_t CheckDrawPad();
   /** True if current histogram  is still exisiting */
   Bool_t CheckHistogram();

   void DeletePainter();

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

protected:

   /** Painter instance to display the condition in root pad. To
     * be used in Paint() which is called on TPad::Update automatically for
     * all TObjects appended to the pad. **/
   TGo4MarkerPainter* fxPainter;

   void SetPainted(Bool_t on) { fbIsPainted=on; }
   Bool_t IsPainted() { return fbIsPainted; }

   void SetDrawPad(TVirtualPad* pad);


private:

   /** since TMarker is not TNamed, we provide identifier string here: */
   TString fxName;

   /** Flag to prevent Paint() method to redraw marker views after UnDraw.
     * Problem with Paint() called from ROOT event loop. */
   Bool_t fbIsPainted; //!

   /** Visibility of this marker on pad. For marker array in editor */
   Bool_t fbVisible;

   /** Switch marker label on or off. */
   Bool_t fbHasLabel;

   /** Switch connectionline between marker and label on or off. */
   Bool_t fbHasConnector;

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbXDraw;

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbYDraw;

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbXbinDraw;

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbYbinDraw;

   /** If true, draw corresponding value as label on working pad */
   Bool_t fbContDraw;

   /** Reference to currently used histogram (for bin contents). */
   TH1* fxHisto; //!

   /** Remember last pad used for Draw. Needed for condition editor to
    * prevent displaying marker on pad other than the working pad. */
   TVirtualPad* fxDrawPad; //!

   /** use this counter for automatic naming and coloring*/
   static UInt_t fguInstanceCounter;

ClassDef(TGo4Marker,1)
};

#endif //TGO4MARKER_H

