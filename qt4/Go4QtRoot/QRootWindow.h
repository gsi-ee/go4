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

#ifndef QROOTWINDOW_H
#define QROOTWINDOW_H

#include <QWidget>

#include "Rtypes.h"

class QMouseEvent;
class QPaintEvent;
class TGLayoutHints;
class Event_t;
class TQRootFrame;
class TGFrame;
class TGCompositeFrame;

/** Qt widget wrapper for ROOT TGWindow class.
  * @author J.Adamczewski */

class QRootWindow : public QWidget {
   Q_OBJECT

   public:
      /** real ctor*/
      QRootWindow( QWidget *parent = 0, const char *name = 0, bool designermode=false);

      virtual ~QRootWindow();

      TGCompositeFrame* GetRootFrame();
      int GetRootWid() const { return fiWinid; }

      /** Add root subframe into this widget */
      void AddSubFrame(TGFrame* f, TGLayoutHints* l=0);

      /** make this frame to current root master frame.*/
      void SetEditable(bool on=true);

      /** switch root frame resize mode on paint event:
       true=explicit resize, false=no resize of TQRootFrame*/
      void SetResizeOnPaint(bool on=true) { fbResizeOnPaint=on; }
      
      /** deliver width scaled with Qt5 screen scaling factor*/
      double ScaledWidth();
      
       /** deliver Heightth scaled with Qt5 screen scaling factor*/
      double ScaledHeight();

   protected:

      /** Event filter is used to pass Qt mouse events  root, after
        * translating them in MapQMouseEvent. Note that root grabs gui buttons
        * via x11 directly, thus this mechanism is not used for the regular
        *  root widgets! */
      virtual bool eventFilter( QObject *, QEvent * );
      virtual void paintEvent( QPaintEvent *e );
      virtual void closeEvent( QCloseEvent * e);

      //virtual QPaintEngine * paintEngine () const {return 0;}

      /** translate Qt mouse event into root event structure. */
      Bool_t MapQMouseEvent(QMouseEvent *e, Event_t* rev);

      /** this is the pseudo root window embedded into this widget.
        * is used as parent for subwindows */
      TQRootFrame *fxRootwindow;

      /** ROOT internal window index*/
      Int_t fiWinid;
      /** Qt window id as received from Qt (may change during runtime?)*/
      WId fQtWinId;

      /** switch if paint event does explicit resize of ROOT frame (default)
       * for embedded TGedEditor, we better disable it and resize editor from
       * parent widget*/
      Bool_t fbResizeOnPaint;
      
      double fQtScalingfactor;
};

#endif
