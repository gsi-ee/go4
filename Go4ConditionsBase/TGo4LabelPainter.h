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

#ifndef TGO4LABELPAINTER_H
#define TGO4LABELPAINTER_H

#include "TPaveText.h"
#include "TLine.h"
#include "TNamed.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include "TAttText.h"

/**
* Text label that knows its author. Required to suppress
* orphaned copies of marker and condition labels after canvas
* reconstruction by streamer.
* @since 8/2004
* @author J. Adamczewski
*/
class TGo4Label : public TPaveText {

  public:
    TGo4Label(): TPaveText(), fxOwner(0) { SetBit(kMustCleanup); /*SetBit(kCanDelete,kFALSE);*/}
    TGo4Label(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Option_t* option = "br")
    : TPaveText(x1,y1,x2,y2,option),fxOwner(0) { SetBit(kMustCleanup); /*SetBit(kCanDelete,kFALSE);*/}

    virtual ~TGo4Label(){fxLastDeleted=(char*) this;}
    void SetOwner(TObject* ob){fxOwner=ob;}

    virtual void Paint(Option_t* opt="");

    virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);

    /** set this to pointer of last deleted instance before*/
    static char* fxLastDeleted;

  private:
    /** the owner of this label */
    TObject* fxOwner;//!
  ClassDef(TGo4Label,7)
};

/**
* Conncetion line for labels that knows its author. Required to suppress
* orphaned copies of marker and condition label connectors after canvas
* reconstruction by streamer.
* @since 8/2004
* @author J. Adamczewski
*/
class TGo4LabelConnector : public TLine {

  public:
    TGo4LabelConnector(): TLine(), fxOwner(0) { SetBit(kMustCleanup); }
    TGo4LabelConnector(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
    : TLine(x1,y1,x2,y2),fxOwner(0) { SetBit(kMustCleanup); }

    virtual ~TGo4LabelConnector(){fxLastDeleted=(char*) this;};
    void SetOwner(TObject* ob){fxOwner=ob;}

    virtual void Paint(Option_t* opt="");

     /** set this to pointer of last deleted instance before*/
    static char* fxLastDeleted;

  private:
    /** the owner of this connector line */
    TObject* fxOwner;//!
  ClassDef(TGo4LabelConnector,6)
};


/**
* Base class for painters providing a textlabel for
* additional information. To be inherited by
* condition painters and marker painters.
* @since 6/2004
* @author J. Adamczewski
*/

class TGo4LabelPainter : public TNamed, public TAttLine, public TAttFill, public TAttText {
  public:
    TGo4LabelPainter();
    TGo4LabelPainter(const char* name, const char* title = 0);
    virtual ~TGo4LabelPainter();

    /** Draw the label */
    virtual void PaintLabel(Option_t* opt="");

    /** Erase the label */
    virtual void UnPaintLabel(Option_t* opt="");

    /** refresh painting of label with current attributes */
    virtual void RePaintLabel(Option_t* opt="");

    /** pop all components to the front of the pad*/
    virtual void DisplayToFront(Option_t* opt="");

    void SetCaption(const char* txt) { fxCaption=txt; }
    void SetX0(Double_t x=0) { fdX0=x; }
    void SetY0(Double_t y=0) { fdY0=y; }
    void SetWidth(Double_t w=0) { fdWidth=w; }
    void SetHeight(Double_t h=0) { fdHeight=h; }

    void AddToLabel(const char* txt) { if(fxLabel) fxLabel->AddText(txt); }

    /** Initialize the label attributes */
    void InitAttributes();

    /** Reference coordinate x of label box (lower left corner) */
    Double_t GetLabelXlo();

    /** Reference coordinate y of label box (lower left corner) */
    Double_t GetLabelYlo();

    /** Reference coordinate x of label box (upper right corner) */
    Double_t GetLabelXup();

    /** Reference coordinate y of label box (upper right corner) */
    Double_t GetLabelYup();

   private:
     /** the label */
     TGo4Label* fxLabel;

    /** initial reference coordinates x */
    Double_t fdX0;

    /** initial reference coordinates y */
    Double_t fdY0;

    /** initial width relative to whole pad*/
    Double_t fdWidth;

    /** initial height relative to whole pad */
    Double_t fdHeight;

    /** Headline for the label */
    TString fxCaption;

    /** Flag to handle streamed label from file correctly*/
    Bool_t fbIsLabStreamed; //!

    /** Create Label at the window coords. */
    TGo4Label* CreateCurrentLabel(Double_t x, Double_t y);

    /** Evaluate coordinates for new label for given xmin, ymin  with respect to pad size*/
    void LabelCoords(Double_t &xmin, Double_t &ymin, Double_t &xmax, Double_t &ymax);

    /** Check if label is still existing. Uses root list of cleanups. */
    Bool_t CheckLabel();

  ClassDef(TGo4LabelPainter,6)
};

#endif //TGO4LABELPAINTER_H
