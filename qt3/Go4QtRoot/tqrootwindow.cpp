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

#include "tqrootwindow.h"

#include "TVirtualX.h"
#include "TSystem.h"
#include "GuiTypes.h"
#include "TGFrame.h"
#include "Riostream.h"

#include "qevent.h"
#include "qpainter.h"
#include "qdragobject.h"

#include "TGo4LockGuard.h"

/** Pseudo root mainframe to wrap root composite frames like TGedEditor */
class TQRootFrame: public TGCompositeFrame {
   public:
   /** pretend a root frame for the externally created window of id wid */
   TQRootFrame(Window_t wid) :
      TGCompositeFrame(gClient,wid,new TGMainFrame(gClient->GetDefaultRoot(),100,100))
   {
     // we have to offer a real mainframe as top parent for root internal lists:
    // fParent=new TGMainFrame(gClient->GetDefaultRoot(),100,100);
   }

   virtual ~TQRootFrame()
   {
     delete fParent;
   }

   virtual void CloseWindow()
   {
      UnmapWindow();  // capture the close window
   }

   virtual Bool_t HandleEvent(Event_t *event)
   {
      return TGCompositeFrame::HandleEvent(event);
   }

};



///////////////////////////////////////////////////
// This is the widget to be embedded in qt:

TQRootWindow::TQRootWindow( QWidget *parent, const char *name)
  : QWidget( parent, name)//, WMouseNoMask | WPaintUnclipped | WPaintClever)//|WRepaintNoErase | WResizeNoErase)
{
  // set defaults
  setUpdatesEnabled( true );
  setMouseTracking(true);

  //setBackgroundMode( NoBackground );
  setFocusPolicy( TabFocus );
  setCursor( Qt::crossCursor );

  // add the Qt::WinId to TGX11 interface
  fiWinid=gVirtualX->AddWindow(winId(),145,600);
  Window_t handle=gVirtualX->GetWindowID(fiWinid);
  fxRootwindow=new TQRootFrame(handle);
  fxRootwindow->Resize();
  if ( parent ) parent->installEventFilter( this );

//  std::cout <<"visible rect is: "<<visibleRect().width()<<", "<<visibleRect().height() << std::endl;
}


void TQRootWindow::AddSubFrame(TGFrame* f, TGLayoutHints* l)
{
   //std::cout <<"TQRootWindow::AddSubFrame" << std::endl;
   fxRootwindow->AddFrame(f,l);
   fxRootwindow->MapWindow();
   fxRootwindow->MapSubwindows();
   fxRootwindow->Resize();
   polish();
   update();
   show();
}

void TQRootWindow::SetEditable(bool on)
{
   fxRootwindow->SetEditable(on);
}

Event_t* TQRootWindow::MapQMouseEvent(QMouseEvent *e)
{
if(e==0) return 0;
Event_t* rev= new Event_t;

rev->fX=e->x();
rev->fY=e->y();
rev->fXRoot= e->globalX();
rev->fYRoot= e->globalY();

// translate Qt event type:
if(e->type() == QEvent::MouseButtonPress)   rev->fType = kButtonPress;
else if(e->type() == QEvent::MouseButtonRelease) rev->fType = kButtonRelease;
else if(e->type() == QEvent::MouseButtonDblClick) rev->fType = kButtonDoubleClick;
else if(e->type() == QEvent::MouseMove) rev->fType = kMotionNotify;
else if(e->type() == QEvent::KeyPress) rev->fType = kGKeyPress;
else if(e->type() == QEvent::KeyRelease) rev->fType = kKeyRelease;
else rev->fType = kOtherEvent;

// translate Qt state bits:
rev->fState=0;
if(e->state() & LeftButton)
         rev->fState |= kButton1Mask;
if(e->state() & RightButton)
         rev->fState |= kButton3Mask;
if(e->state() & MidButton)
         rev->fState |= kButton2Mask;
if(e->state() & MouseButtonMask)
         rev->fState |= kButton1Mask;
if(e->state() & ShiftButton)
         rev->fState |= kKeyShiftMask;
if(e->state() & ControlButton)
         rev->fState |= kKeyControlMask;
if(e->state() & AltButton)
         rev->fState |= kKeyMod1Mask;
if(e->state() & MetaButton)
         rev->fState |= kKeyMod1Mask;
//if(e->state() & KeyButtonMask)
//         rev->fState |= ShiftMask;
//if(e->state() & Keypad)
//         rev->fState |= ShiftMask;

rev->fCode      = NoButton;    // button code
if(e->button() & LeftButton)
         rev->fCode |= kButton1Mask;
if(e->button() & RightButton)
         rev->fCode |= kButton3Mask;
if(e->button() & MidButton)
         rev->fCode |= kButton2Mask;

rev->fUser[0]=0;
rev->fWindow    = gVirtualX->GetWindowID(fiWinid); // we refer signals to this window
rev->fSendEvent = 0;
rev->fTime = 0; // this might cause problems with root doubleclick treatment?



return rev;
}



void TQRootWindow::paintEvent( QPaintEvent * )
{

//TGo4LockGuard threadlock;
 if(fxRootwindow){
//     QPainter p;
//     p.begin( this);
//     p.end();
//     std::cout <<"TQRootWindow::paintEvent" << std::endl;
//     std::cout <<"visible rect is: "<<visibleRect().width()<<", "<<visibleRect().height() << std::endl;
     fxRootwindow->Resize(width(),height());
  }
}


bool TQRootWindow ::eventFilter( QObject *o, QEvent *e )
{
//TGo4LockGuard threadlock;
  // Filtering of QWidget Events
  //std::cout <<"TQRootWindow ::eventFilter" << std::endl;
  QMouseEvent* me=dynamic_cast<QMouseEvent*>(e);
  if(me)
   {
    //std::cout <<"TQRootWindow ::eventFilter has mouse event" << std::endl;
    Event_t* roote=MapQMouseEvent(me);
    if(fxRootwindow) fxRootwindow->HandleEvent(roote);
    delete roote;
    return FALSE;
   }


  if ( e->type() == QEvent::Close) {  // close
        delete fxRootwindow;
        fxRootwindow=0;
        return FALSE;
       }

  // standard event processing
  return QWidget::eventFilter( o, e );
}



void  TQRootWindow::closeEvent( QCloseEvent * e){

    delete fxRootwindow;
    fxRootwindow=0;
    e->accept();
    return;
}



TQRootWindow::~TQRootWindow()
{
    delete fxRootwindow;
    fxRootwindow=0;
}

TGCompositeFrame* TQRootWindow::GetRootFrame()
{
   return fxRootwindow;
}

