#include "tqrootwindow.h"

#include "Riostream.h"

//#include "TVirtualX.h"
#include "TSystem.h"
#include "GuiTypes.h"
#include "TGFrame.h"

#include "qevent.h"
#include "qpainter.h"
#include "q3dragobject.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QCloseEvent>
#include <QPaintEvent>

#include "lockguard.h"

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

TQRootWindow::TQRootWindow( QWidget *parent, const char *name, bool designermode)
  : QWidget( parent, name)//, WMouseNoMask | WPaintUnclipped | WPaintClever)//|WRepaintNoErase | WResizeNoErase)
   ,fxRootwindow(0),fbResizeOnPaint(kTRUE)
{
  // set defaults
  setUpdatesEnabled( true );
  setMouseTracking(true);

  setFocusPolicy( Qt::TabFocus );
  setCursor( Qt::crossCursor );
  //setAttribute(Qt::WA_NoSystemBackground);
  if(!designermode)
     { 
     // add the Qt::WinId to TGX11 interface
     fiXid=winId();
     fiWinid=gVirtualX->AddWindow(fiXid,145,600);
     //cout <<"TQRootWindow ctor added window for "<<fiXid<<" with ROOT wid:"<<fiWinid<< endl;
     fxRootwindow=new TQRootFrame(fiXid);
     fxRootwindow->Resize();
     if ( parent ) parent->installEventFilter( this );
     }
}




void TQRootWindow::AddSubFrame(TGFrame* f, TGLayoutHints* l)
{
   //cout <<"TQRootWindow::AddSubFrame" << endl;
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
if(e->state() & Qt::LeftButton)
         rev->fState |= kButton1Mask;
if(e->state() & Qt::RightButton)
         rev->fState |= kButton3Mask;
if(e->state() & Qt::MidButton)
         rev->fState |= kButton2Mask;
if(e->state() & Qt::MouseButtonMask)
         rev->fState |= kButton1Mask;
if(e->state() & Qt::ShiftButton)
         rev->fState |= kKeyShiftMask;
if(e->state() & Qt::ControlButton)
         rev->fState |= kKeyControlMask;
if(e->state() & Qt::AltButton)
         rev->fState |= kKeyMod1Mask;
if(e->state() & Qt::MetaButton)
         rev->fState |= kKeyMod1Mask;
//if(e->state() & KeyButtonMask)
//         rev->fState |= ShiftMask;
//if(e->state() & Keypad)
//         rev->fState |= ShiftMask;

rev->fCode      = Qt::NoButton;    // button code
if(e->button() & Qt::LeftButton)
         rev->fCode |= kButton1Mask;
if(e->button() & Qt::RightButton)
         rev->fCode |= kButton3Mask;
if(e->button() & Qt::MidButton)
         rev->fCode |= kButton2Mask;

rev->fUser[0]=0;
rev->fWindow    = gVirtualX->GetWindowID(fiWinid); // we refer signals to this window
rev->fSendEvent = 0;
rev->fTime = 0; // this might cause problems with root doubleclick treatment?



return rev;
}



void TQRootWindow::paintEvent( QPaintEvent * e)
{
//Qtrootlockguard threadlock;
 if(fxRootwindow){
     UInt_t nxid=winId();
     if(fiXid!=nxid)
      {
         Qtrootlockguard threadlock;
         // may happen when this window is embedded to Qt workspace...
         //cout <<"Warning: TQRootWindow::paintEvent finds changed X window id: "<<winId()<<", previous:"<<fiXid << endl;
         delete fxRootwindow; // should also remove old x windows!
         fiWinid=gVirtualX->AddWindow(nxid,width(),height());
         fxRootwindow=new TQRootFrame(fiXid);
         fiXid=nxid;
      }  
     if(fbResizeOnPaint)
      {
         Qtrootlockguard threadlock;
         //cout <<"TQRootWindow::paintEvent does TGCompositeFrame Resize ..." <<endl;
         fxRootwindow->Resize(width(),height());
         gVirtualX->Update(1); // Xsync/flus
      }
  }
QWidget::paintEvent(e); 
}

void TQRootWindow::resizeEvent( QResizeEvent *e )
{
 //Qtrootlockguard threadlock;   
 QWidget::resizeEvent(e);  
   
}


bool TQRootWindow ::eventFilter( QObject *o, QEvent *e )
{
Qtrootlockguard threadlock;
  // Filtering of QWidget Events
  //cout <<"TQRootWindow ::eventFilter" << endl;
  QMouseEvent* me=dynamic_cast<QMouseEvent*>(e);
  if(me)
   {
    //cout <<"TQRootWindow ::eventFilter has mouse event" << endl;
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

