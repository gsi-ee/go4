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

#ifndef TQROOTCANVAS_H
#define TQROOTCANVAS_H

#include "qwidget.h"
#include "Rtypes.h"
#include "Gtypes.h"
#include "Buttons.h"
#include "TVirtualX.h"

class TObject;
class TObjLink;
class TVirtualPad;
class TPad;
class TCanvas;
class TBrowser;
class TContextMenu;
class TControlBar;
class TQCanvasMenu;

/** This canvas uses Qt eventloop to handle user input
  *   @short Graphic Qt Widget based Canvas
  *
  * @authors Denis Bertini <d.bertini@gsi.de>
  * @version 2.0
  */

class TQRootCanvas : public QWidget {
   Q_OBJECT

   public:
      TQRootCanvas( QWidget *parent = 0, const char *name = 0 ,TCanvas *c=0);
      TQRootCanvas( QWidget *parent = 0, QWidget* tabWin = 0 , const char *name = 0 ,TCanvas *c=0);
      virtual ~TQRootCanvas();

      TCanvas*          getCanvas() { return fCanvas;}
      int               getRootWid() { return wid;}
      bool              getCanvasOwner(){ return isCanvasOwned; }
      QWidget*          getParent() { return fParent;}
      QWidget*          getTabWin() { return fTabWin;}
      void              setResizeFlag(int level = 1);
      bool              checkResizeFlag(int level = 1);
      TQCanvasMenu*     getContextMenu() { return fContextMenu; }
      void              setMaskDoubleClick(bool on=true) { fMaskDoubleClick = on; }

   signals:
      /** signal which will be emitted when root selected pad is changed
        * via clicking the mid-mouse button (M. al-Turany) */
      void              SelectedPadChanged(TPad*);

      /** signal emitted when mouse clicks on pad  */
      void              PadClicked(TPad*);

      /** signal emitted when user produce left mouse doubleclick on pad  */
      void              PadDoubleClicked(TPad*);

   public slots:

      void              cd(Int_t subpadnumber=0);
      virtual void      Browse(TBrowser *b);
      void              Clear(Option_t *option="");
      void              Close(Option_t *option="");
      virtual void      Draw(Option_t *option="");
      virtual TObject  *DrawClone(Option_t *option="");
      virtual TObject  *DrawClonePad();
      virtual void      EditorBar();
      void              EnterLeave(TPad *prevSelPad, TObject *prevSelObj);
      void              FeedbackMode(Bool_t set);
      void              Flush();
      void              UseCurrentStyle();
      void              ForceUpdate() ;
      const char       *GetDISPLAY();
      TContextMenu     *GetContextMenu() ;
      Int_t             GetDoubleBuffer() ;
      TControlBar      *GetEditorBar()  ;
      Int_t             GetEvent()  ;
      Int_t             GetEventX() ;
      Int_t             GetEventY() ;
      Color_t           GetHighLightColor() ;
      virtual void      GetPadDivision(Int_t xdivision=1, Int_t ydivision=1);
      TVirtualPad      *GetPadSave() ;
      TObject          *GetSelected() ;
      Option_t         *GetSelectedOpt() ;
      TVirtualPad      *GetSelectedPad()  ;
      Bool_t            GetShowEventStatus() ;
      Bool_t            GetAutoExec() ;
      Size_t            GetXsizeUser()  ;
      Size_t            GetYsizeUser()  ;
      Size_t            GetXsizeReal()  ;
      Size_t            GetYsizeReal()  ;
      Int_t             GetCanvasID()  ;
      Int_t             GetWindowTopX();
      Int_t             GetWindowTopY();
      UInt_t            GetWindowWidth() ;
      UInt_t            GetWindowHeight()  ;
      UInt_t            GetWw() ;
      UInt_t            GetWh() ;
      virtual void      GetCanvasPar(Int_t &wtopx, Int_t &wtopy, UInt_t &ww, UInt_t &wh);
      virtual void      HandleInput(EEventType button, Int_t x, Int_t y);
      Bool_t            HasMenuBar()  ;
      void              Iconify() ;
      Bool_t            IsBatch() ;
      Bool_t            IsRetained() ;
      virtual void      ls(Option_t *option="") ;
      void              MoveOpaque(Int_t set=1);
      Bool_t            OpaqueMoving() ;
      Bool_t            OpaqueResizing() ;
      virtual void      Paint(Option_t *option="");
      virtual TPad     *Pick(Int_t px, Int_t py, TObjLink *&pickobj) ;
      virtual TPad     *Pick(Int_t px, Int_t py, TObject *prevSelObj);
      virtual void      Resize(Option_t *option="");
      void              ResizeOpaque(Int_t set=1) ;
      void              SaveSource(const char *filename="", Option_t *option="");
      virtual void      SetCursor(ECursor cursor);
      virtual void      SetDoubleBuffer(Int_t mode=1);
      void              SetWindowPosition(Int_t x, Int_t y) ;
      void              SetWindowSize(UInt_t ww, UInt_t wh) ;
      void              SetCanvasSize(UInt_t ww, UInt_t wh);
      void              SetHighLightColor(Color_t col);
      void              SetSelected(TObject *obj) ;
      void              SetSelectedPad(TPad *pad) ;
      void              Show() ;
      virtual void      Size(Float_t xsizeuser=0, Float_t ysizeuser=0);
      void              SetBatch(Bool_t batch=kTRUE);
      void              SetRetained(Bool_t retained=kTRUE);
      void              SetTitle(const char *title="");
      virtual void      ToggleEventStatus();
      virtual void      ToggleAutoExec();
      virtual void      Update();

   protected:
      virtual void      performResize();
      virtual void      dropEvent( QDropEvent *Event );
      virtual void      dragEnterEvent( QDragEnterEvent *e );
      virtual bool      eventFilter( QObject *, QEvent * );
      virtual void      mousePressEvent( QMouseEvent *e );
      virtual void      mouseReleaseEvent( QMouseEvent *e );
      virtual void      resizeEvent( QResizeEvent *e );
      virtual void      paintEvent( QPaintEvent *e );
      virtual void      mouseDoubleClickEvent(QMouseEvent* e );
      virtual void      mouseMoveEvent(QMouseEvent *e);
      virtual void      leaveEvent(QEvent *e);
      virtual void      closeEvent( QCloseEvent * e);

      TQCanvasMenu*     fContextMenu;
      TCanvas*          fCanvas;
      Int_t             wid;
      bool              isCanvasOwned;
      QWidget*          fParent;
      QWidget*          fTabWin;
   private:
      int               fResizeFlag;
      bool              fMaskDoubleClick;
};

#endif






