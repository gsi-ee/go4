//Author : Denis Bertini 01.11.2000

/**************************************************************************
* Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI           *
*                     Planckstr. 1, 64291 Darmstadt, Germany              *
*                     All rights reserved.                                *
* Contact:            http://go4.gsi.de                                   *
*                                                                         *
* This software can be used under the license agreements as stated in     *
* Go4License.txt file which is part of the distribution.                  *
***************************************************************************/

#ifndef TQCANVASIMP_H
#define TQCANVASIMP_H


/**
*
* TQCanvasImp
* ABC describing Qt GUI independent main window (with menubar, scrollbars
* and a drawing area).
*
* @short ABC describing Qt GUI independent main window (with menubar, scrollbars
* and a drawing area).
*
* @see TCanvasImp
*
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*/

#include "Riostream.h"

#include "TCanvasImp.h"
#include "TInterpreter.h"
#include "TPad.h"
#include "TVirtualPad.h"

#include "qmainwindow.h"
#include "tqrootcanvas.h"
#include "qvbox.h"
#include "qtframe.h"

class TQCanvasImp :  public TCanvasImp {
protected:
  TQRootCanvas *fQCanvas;
  QTFrame* fdialbox;

  void build(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
public:

  /**
   QCanvasImp ctor
   @li creates a QMainWindow as a dialog box
   @li puts inside a QRootCanvas as a central Widget

   */
  TQCanvasImp(TCanvas* = 0) : TCanvasImp() { }
  /**
   TQCanvasImp ctor
   @li creates a QMainWindow as a dialog box
   @li puts inside a QRootCanvas as a central Widget

   @param c (ptr to ROOT TCanvas)
   @param name (title for canvas)
   @param width
   @param height
   */

  TQCanvasImp(TCanvas *c, const char *name, UInt_t width, UInt_t height);
  /**
   QCanvasImp ctor
   @li creates a QMainWindow as a dialog box
   @li puts inside a QRootCanvas as a central Widget

   @param c (ptr to ROOT TCanvas)
   @param name (title for canvas)
   @param x
   @param y
   @param width
   @param height
   */
  TQCanvasImp(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
  virtual ~TQCanvasImp() {
    //    printf("~QCanvasImp \n");
  }

  /**
  InitWindow here do nothing else but giving
  the ROOT internal window Id
  @return int wid
   */
  virtual Int_t  InitWindow() { return fQCanvas->getRootWid(); }
  /**
   Force an update of the dialog box

   */
  virtual void   ForceUpdate() { fdialbox->update(); }
  /**
    Get a specification of the window geometry

   */
//// remove this due to changes in root 4.00/06 JA
//  virtual void   GetWindowGeometry(Int_t &ax, Int_t &ay, UInt_t &w, UInt_t &h) {
//    //    ax=fdialbox->x(); ay=fdialbox->y(); w=fdialbox->width(); h=fdialbox->height();
//    ax=ay=w=h=0;
//  }
  /**
   Iconify (not implemented yet)
   */

  virtual void   Iconify() { cout << "Not implemented\n"; }
  /**
   set a status text (not implemented yet)
   */
  virtual void   SetStatusText(const char* = 0, Int_t = 0) {
    //cout << "statustext Not implemented\n";
  }
  /**
  Move the dialog box window to a specified position in the local
  coordinates
   */
  virtual void   SetWindowPosition(Int_t x, Int_t y) { fdialbox->move(x,y); }
  /**
   Set the Windows size to width=w heigth=h
   */
  virtual void   SetWindowSize(UInt_t w, UInt_t h) { fdialbox->resize(w,h); }
  /**
   Set the window title
   */
  virtual void   SetWindowTitle(const char* newTitle) {
    cout << "title Not implemented" << newTitle << endl;
  }
  /**
   Set the Canvas size
   */
  virtual void   SetCanvasSize(UInt_t w, UInt_t h) {fdialbox->resize(w,h);}
  /**
   Show the menu bar
   */
  virtual void   ShowMenuBar(Bool_t = kTRUE) {
    cout << "menubar Not implemented\n";
  }
  /**
   Show status bar
   */
  virtual void   ShowStatusBar(Bool_t = kTRUE) {
    cout << "statusbar Not implemented\n";
  }
  /**
   Do a Show of the dialog box
   */
  virtual void   Show() { fdialbox->show();}
  /**
   Close the dialog box
   */
  virtual void   Close() {
    // printf(" closing the dialbox \n");
     fQCanvas->close( true );
     fdialbox->close( true );
  }
};
#endif

