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

#ifndef QROOTCANVAS_H
#define QROOTCANVAS_H

#include <QWidget>


#ifdef WIN32

#ifdef GO4_EXPORT
#define GO4_CLASS_EXPORT  __declspec(dllexport)
#else
#define GO4_CLASS_EXPORT  __declspec(dllimport)
#endif

#else

#define GO4_CLASS_EXPORT

#endif


#ifdef WIN32
#define QDESIGNER_WIDGET_EXPORT GO4_CLASS_EXPORT
#else
#include <QtDesigner/QDesignerExportWidget>
#endif

#include "Rtypes.h"
#include "Gtypes.h"
#include "Buttons.h"
#include "TVirtualX.h"

class TObject;
class TMethod;
class TObjLink;
class TVirtualPad;
class TPad;
class TCanvas;
class TBrowser;
class TContextMenu;
class TList;

class QSignalMapper;
class QMenu;
class QAction;
class QTimer;

/** This canvas uses Qt eventloop to handle user input
  *   @short Graphic Qt Widget based Canvas
  *
  * @authors Denis Bertini <d.bertini(at)gsi.de>
  * @version 2.0
  */

class QDESIGNER_WIDGET_EXPORT QRootCanvas : public QWidget {

   Q_OBJECT

   enum { act_Update = 1, act_Resize = 2 };

   public:
      QRootCanvas(QWidget *parent = 0);
      virtual ~QRootCanvas();

      TCanvas*          getCanvas() { return fCanvas; }
      int               getRootWid() { return fRootWindowId; }
      void              setMaskDoubleClick(bool on=true) { fMaskDoubleClick = on; }

      bool              showEventStatus() const;
      void              setShowEventStatus(bool s);

   signals:
      /** signal which will be emitted when root selected pad is changed
        * via clicking the mid-mouse button (M. al-Turany) */
      void              SelectedPadChanged(TPad*);

      /** signal emitted when mouse clicks on pad  */
      void              PadClicked(TPad*);

      /** signal emitted when user produce left mouse double-click on pad  */
      void              PadDoubleClicked(TPad*);

      void              MenuCommandExecuted(TObject*, const char*);

      void              CanvasStatusEvent(const char*);

      void              CanvasDropEvent(QDropEvent*, TPad*);

      void              CanvasLeaveEvent();

      void              CanvasUpdated();

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
      void              ForceUpdate();
      const char       *GetDISPLAY();
      TContextMenu     *GetContextMenu();
      Int_t             GetDoubleBuffer();
      Int_t             GetEvent();
      Int_t             GetEventX();
      Int_t             GetEventY();
      Color_t           GetHighLightColor();
      TVirtualPad      *GetPadSave();
      TObject          *GetSelected() ;
      Option_t         *GetSelectedOpt();
      TVirtualPad      *GetSelectedPad();
      Bool_t            GetShowEventStatus();
      Bool_t            GetAutoExec();
      Size_t            GetXsizeUser();
      Size_t            GetYsizeUser();
      Size_t            GetXsizeReal();
      Size_t            GetYsizeReal();
      Int_t             GetCanvasID();
      Int_t             GetWindowTopX();
      Int_t             GetWindowTopY();
      UInt_t            GetWindowWidth();
      UInt_t            GetWindowHeight();
      UInt_t            GetWw();
      UInt_t            GetWh();
      virtual void      GetCanvasPar(Int_t &wtopx, Int_t &wtopy, UInt_t &ww, UInt_t &wh);
      virtual void      HandleInput(EEventType button, Int_t x, Int_t y);
      Bool_t            HasMenuBar();
      void              Iconify();
      Bool_t            IsBatch();
      Bool_t            IsRetained();
      virtual void      ls(Option_t *option="");
      void              Modified(Bool_t=1);
      void              MoveOpaque(Int_t set=1);
      Bool_t            OpaqueMoving() ;
      Bool_t            OpaqueResizing() ;
      virtual void      Paint(Option_t *option="");
      virtual TPad     *Pick(Int_t px, Int_t py, TObjLink *&pickobj);
      virtual TPad     *Pick(Int_t px, Int_t py, TObject *prevSelObj);
      virtual void      Resize(Option_t *option="");
      void              ResizeOpaque(Int_t set=1);
      void              SaveSource(const char *filename="", Option_t *option="");
      virtual void      SetCursor(ECursor cursor);
      virtual void      SetDoubleBuffer(Int_t mode=1);
      void              SetWindowPosition(Int_t x, Int_t y);
      void              SetWindowSize(UInt_t ww, UInt_t wh);
      void              SetCanvasSize(UInt_t ww, UInt_t wh);
      void              SetHighLightColor(Color_t col);
      void              SetSelected(TObject *obj);
      void              SetSelectedPad(TPad *pad);
      void              Show();
      virtual void      Size(Float_t xsizeuser=0, Float_t ysizeuser=0);
      void              SetBatch(Bool_t batch=kTRUE);
      void              SetRetained(Bool_t retained=kTRUE);
      void              SetTitle(const char *title="");
      virtual void      ToggleEventStatus();
      virtual void      ToggleAutoExec();
      virtual void      Update();

      void              executeMenu(int id);
      void              processRepaintTimer();

   protected:

      virtual void      dropEvent( QDropEvent *Event );
      virtual void      dragEnterEvent( QDragEnterEvent *e );
      virtual void      mousePressEvent( QMouseEvent *e );
      virtual void      mouseReleaseEvent( QMouseEvent *e );
      virtual void      resizeEvent( QResizeEvent *e );
      virtual void      paintEvent( QPaintEvent *e );
      virtual void      mouseDoubleClickEvent(QMouseEvent* e );
      virtual void      mouseMoveEvent(QMouseEvent *e);
      virtual void      wheelEvent( QWheelEvent* e);
      virtual void      leaveEvent(QEvent *e);
      virtual void      closeEvent( QCloseEvent * e);

      
      /** returns scaled point coordinate, for high dpi case*/
      double scaledPosition(int p)
        {
            return (double) p * fQtScalingfactor;            
        }
      
      virtual QPaintEngine * paintEngine () const {return 0;}

      void              methodDialog(TObject* object, TMethod* method);
      QAction*          addMenuAction(QMenu* menu, QSignalMapper* map, const QString& text, int id);

      void              activateRepaint(int act);

      TCanvas*          fCanvas;
      Int_t             fRootWindowId;
      WId               fQtWindowId; // current id of embedded canvas (may change in Qt 4.4)
      QTimer*           fRepaintTimer; // do not draw canvas immediately, postpone this on few miliseconds
      int               fRepaintMode; // 0 - inactive, 1 - paint, 2 - resize, -1 - skip first repaint event

   private:
      bool              fMaskDoubleClick;
      double            fMousePosX;    // mouse position in user coordinate when activate menu
      double            fMousePosY;    // mouse position in user coordinate when activate menu

      TObject*          fMenuObj;      // object use to fill menu
      TList*            fMenuMethods;  // list of menu methods
      bool              fxShowEventStatus;
      
      double fQtScalingfactor;
};

#endif






