#ifndef QROOTCANVAS_H
#define QROOTCANVAS_H

#include <QtGui/QWidget>

#include <QtDesigner/QDesignerExportWidget>

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
class TControlBar;
class TList;

class QSignalMapper;
class QMenu;
class QAction;
class QTimer;

/** This canvas uses Qt eventloop to handle user input
  *   @short Graphic Qt Widget based Canvas
  *
  * @authors Denis Bertini <d.bertini@gsi.de>
  * @version 2.0
  */

class QDESIGNER_WIDGET_EXPORT QRootCanvas : public QWidget {
   Q_OBJECT

   public:
      QRootCanvas(QWidget *parent = 0);
      virtual ~QRootCanvas();

      TCanvas*          getCanvas() { return fCanvas;}
      int               getRootWid() { return fRootWindowId;}
      void              setMaskDoubleClick(bool on=true) { fMaskDoubleClick = on; }

      bool              showEventStatus() const;
      void              setShowEventStatus(bool s);

   signals:
      /** signal which will be emitted when root selected pad is changed
        * via clicking the mid-mouse button (M. al-Turany) */
      void              SelectedPadChanged(TPad*);

      /** signal emitted when mouse clicks on pad  */
      void              PadClicked(TPad*);

      /** signal emitted when user produce left mouse doubleclick on pad  */
      void              PadDoubleClicked(TPad*);

      void              MenuCommandExecuted(TObject*, const char*);

      void              CanvasStatusEvent(const char*);

      void              CanvasDropEvent(QDropEvent*, TPad*);

      void              CanvasLeaveEvent();

      void              DoCanvasResize();

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
      void              Modified(Bool_t=1);
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
      virtual void      performResize();

      void              executeMenu(int id);
      void              processRepaintTimer();
      void              resetPaintFlag();

   protected:

      virtual void      dropEvent( QDropEvent *Event );
      virtual void      dragEnterEvent( QDragEnterEvent *e );
      virtual void      mousePressEvent( QMouseEvent *e );
      virtual void      mouseReleaseEvent( QMouseEvent *e );
      virtual void      resizeEvent( QResizeEvent *e );
      virtual void      paintEvent( QPaintEvent *e );
      virtual void      mouseDoubleClickEvent(QMouseEvent* e );
      virtual void      mouseMoveEvent(QMouseEvent *e);
      virtual void      leaveEvent(QEvent *e);
      virtual void      closeEvent( QCloseEvent * e);

      virtual QPaintEngine * paintEngine () const {return 0;}

      void              methodDialog(TObject* object, TMethod* method);
      QAction*          addMenuAction(QMenu* menu, QSignalMapper* map, const QString& text, int id);

      void              actiavteRepaint(int mode);

      TCanvas*          fCanvas;
      Int_t             fRootWindowId;
      ULong_t           fQtWindowId; // current id of embedded canvas (may change in Qt 4.4)
      QTimer*           fRepaintTimer; // do not draw canvas immediately, postpone this on few miliseconds
      int               fRepaintMode; // 0 - inactive, 1 - paint, 2 - resize, -1 - skip first repaint event
   private:
      bool              fMaskDoubleClick;
      double            fMousePosX;    // mouse position in user coordinate when activate menu
      double            fMousePosY;    // mouse position in user coordinate when activate menu

      TObject*          fMenuObj;      // object use to fill menu
      TList*            fMenuMethods;  // list of menu methods
      bool              fxShowEventStatus;
};

#endif






