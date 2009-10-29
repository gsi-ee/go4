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

/****************************************************************************
** Copyright ( C ) 2000 denis Bertini.  All rights reserved.
*****************************************************************************/

#include "tqrootcanvas.h"

#include "Riostream.h"

#include "qpainter.h"
#include "qdragobject.h"

#include "TPad.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TString.h"
#include "TH1.h"

#include "tqcanvasmenu.h"
#include "TGo4LockGuard.h"

TQRootCanvas::TQRootCanvas( QWidget *parent, const char *name, TCanvas *c ) :
   QWidget( parent, name ,WRepaintNoErase | WResizeNoErase ),
   fResizeFlag(2),
   fMaskDoubleClick(false)
{
  TGo4LockGuard threadlock;
  // set defaults
  setUpdatesEnabled( true );
  setMouseTracking(true);

  //  setBackgroundMode( NoBackground );
  setFocusPolicy( TabFocus );
  setCursor( Qt::crossCursor );

  // add the Qt::WinId to TGX11 interface
  wid=gVirtualX->AddWindow(winId(),100,30);
  if (c==0){
    isCanvasOwned = true;
    fCanvas=new TCanvas(name,width(),height(),wid);
     }else{
    isCanvasOwned= false;
    fCanvas=c;
  }
  // create the context menu
  fContextMenu = new TQCanvasMenu( parent, fCanvas );

  // test here all the events sent to the QWidget
  // has a parent widget
  // then install filter
  if ( parent ){
    parent->installEventFilter( this );
    fParent = parent;
  } else fParent=0;

    // drag and drop suppurt  (M. Al-Turany)
    setAcceptDrops(TRUE);

}

TQRootCanvas::TQRootCanvas( QWidget *parent, QWidget* tabWin, const char *name, TCanvas *c ) :
   QWidget( tabWin, name ,WRepaintNoErase | WResizeNoErase ),
   fResizeFlag(2),
   fMaskDoubleClick(false)
{
   TGo4LockGuard threadlock;
  // set defaults
     setUpdatesEnabled( true );
     setMouseTracking(true);
//   QSizePolicy CanvasSizePolicy = new QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//   setSizePolicy(CanvasSizePolicy);
  //  setBackgroundMode( NoBackground );
  setFocusPolicy( TabFocus );
  setCursor( Qt::crossCursor );

  // add the Qt::WinId to TGX11 interface
  wid=gVirtualX->AddWindow(winId(),100,30);
  if (c==0){
    isCanvasOwned = true;
    fCanvas=new TCanvas(name,width(),height(),wid);
  }else{
    isCanvasOwned= false;
    fCanvas=c;
  }
  // create the context menu
  fContextMenu = new TQCanvasMenu( parent, tabWin, fCanvas );

  // test here all the events sent to the QWidget
  // has a parent widget
  // then install filter
  if ( parent ){
    parent->installEventFilter( this );
    fParent = parent;
  } else fParent=0;

  if ( tabWin ) fTabWin = tabWin;

  // drag and drop suppurt  (M. Al-Turany)
    setAcceptDrops(TRUE);
}

TQRootCanvas::~TQRootCanvas()
{
   if (fContextMenu){
      delete fContextMenu;
      fContextMenu = 0;
   }
   if( isCanvasOwned && fCanvas ) {
     delete fCanvas;
     fCanvas = 0;
   }
}

void TQRootCanvas::setResizeFlag(int flag)
{
   if (flag>=fResizeFlag) fResizeFlag = flag;
}

bool TQRootCanvas::checkResizeFlag(int level)
{
   if (level<=fResizeFlag) {
     performResize();
     return true;
   }
   return false;
}

void TQRootCanvas::performResize()
{
   fResizeFlag = 0;
   Resize();
   Update();
}

void TQRootCanvas::mouseMoveEvent(QMouseEvent *e)
{
  TGo4LockGuard threadlock;
  if (fCanvas!=0) {
     if (e->state() & LeftButton)
       fCanvas->HandleInput(kButton1Motion, e->x(), e->y());
     else
        fCanvas->HandleInput(kMouseMotion, e->x(), e->y());
  }
}


void TQRootCanvas::mousePressEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;

   TObjLink* pickobj = 0;
   TPad* pad = fCanvas->Pick(e->x(), e->y(), pickobj);
   TObject *selected = fCanvas->GetSelected();

   // fCanvas->cd();

   switch(e->button()) {
     case LeftButton :
        fCanvas->HandleInput(kButton1Down, e->x(), e->y());
        emit PadClicked(pad);
        break;
     case RightButton : {
        TString selectedOpt("");
        if (pad!=0) {
          if (pickobj==0) {
            fCanvas->SetSelected(pad);
            selected = pad;
          } else
          if(selected==0) {
            selected    = pickobj->GetObject();
            selectedOpt = pickobj->GetOption();
          }
        }
        pad->cd();
        fCanvas->SetSelectedPad(pad);
        gROOT->SetSelectedPrimitive(selected);
        fContextMenu->popup(selected, gPad->AbsPixeltoX(gPad->GetEventX()),  gPad->AbsPixeltoY(gPad->GetEventY()), e);
        break;
     }
     case MidButton :
        fCanvas->HandleInput(kButton2Down, e->x(), e->y());
        emit SelectedPadChanged(pad);   //   that inform the Qt-world that tha pad is changed
                                        // and give the pointer to the new pad as argument
                                       // of the signal (M. Al-Turany)
        break;
     case  NoButton :
        break;
     default:
        break;
   }
}

void TQRootCanvas::mouseReleaseEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;
   switch(e->button()) {
      case LeftButton :
         fCanvas->HandleInput(kButton1Up, e->x(), e->y());
         break;
      case RightButton :
         fCanvas->HandleInput(kButton3Up, e->x(), e->y());
         break;
      case MidButton :
         fCanvas->HandleInput(kButton2Up, e->x(), e->y());
         break;
      case  NoButton :
         break;
      default:
         break;
  }
}

void TQRootCanvas::mouseDoubleClickEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;
   switch(e->button()) {
      case LeftButton : {
         if (!fMaskDoubleClick)
            fCanvas->HandleInput(kButton1Double, e->x(), e->y());
         TObjLink* pickobj = 0;
         TPad* pad = fCanvas->Pick(e->x(), e->y(), pickobj);
         emit PadDoubleClicked(pad);
         // prevent crash on following release event
         // if new canvas will be created in between
         // fCanvas->SetSelected(0);
         break;
      }
      case RightButton :
         fCanvas->HandleInput(kButton3Double, e->x(), e->y());
         break;
      case MidButton :
         fCanvas->HandleInput(kButton2Double, e->x(), e->y());
         break;
      case NoButton :
         break;
      default:
         break;
   }
}

void TQRootCanvas::resizeEvent( QResizeEvent *e )
{
   QWidget::resizeEvent( e );
   setResizeFlag(1);
}

void TQRootCanvas::paintEvent( QPaintEvent * )
{
   if(fCanvas) {
      QPainter p;
      p.begin( this);
      p.end();
      if (!checkResizeFlag(2))
        fCanvas->Update();
  }
}

void TQRootCanvas::leaveEvent( QEvent *e )
{
   TGo4LockGuard threadlock;
   if (fCanvas!=0)
      fCanvas->HandleInput(kMouseLeave, 0, 0);
}

bool TQRootCanvas::eventFilter( QObject *o, QEvent *e )
{
   if ( e->type() == QEvent::Close) {  // close
      if (fCanvas && (isCanvasOwned== false) ) {
         delete fCanvas;
         #if DEBUG_LEVEL
         qDebug("TQRootCanvas ::eventFilter canvas Id:%i deleted \n", fCanvas->GetCanvasID()) ;
         #endif
         fCanvas=0;
       }
      return FALSE;
   }

   if ( e->type() == QEvent::ChildRemoved ) {  // child is removed
      #if DEBUG_LEVEL
      qDebug(" TQRootCanvas:QEvent:Child removed called \n" );
      #endif
      return FALSE;                        // eat event
   }


   if ( e->type() == QEvent::Destroy) {  // destroy
      #if DEBUG_LEVEL
      qDebug(" QEvent:Destroy called \n" );
      #endif
      return FALSE;
   }

   if( e->type() == QEvent::MouseButtonRelease) {  // Resize on mouse release
      checkResizeFlag();
      return FALSE;
   }

   if( e->type() == QEvent::Enter) { // On enter event check resize flag
       checkResizeFlag();
       return FALSE;
   }

   if( e->type() == QEvent::Leave) {
       fCanvas->FeedbackMode(kFALSE);
       return FALSE;
    }


   if( e->type() == QEvent::Paint) {  // Paint
      return FALSE;
   }

   if( e->type() == QEvent::Move) {  // Paint
     return FALSE;
   }

   if( (e->type() == QEvent::Show) ||
       (e->type() == QEvent::ShowNormal) ||
       (e->type() == QEvent::ShowFullScreen) ||
       (e->type() == QEvent::ShowMaximized) ||
       (e->type() == QEvent::ShowMinimized)) {
        setResizeFlag(2);
        return FALSE;
   }

   // standard event processing
   return QWidget::eventFilter( o, e );
}

////////////////////////////////////// drag and drop support

void TQRootCanvas::dragEnterEvent( QDragEnterEvent *e )
{
//TGo4LockGuard threadlock;
  if ( QTextDrag::canDecode(e ))
    e->accept();
}

void TQRootCanvas::dropEvent( QDropEvent *Event )
{
   TGo4LockGuard threadlock;
   QString str;

    if ( QTextDrag::decode( Event, str ) ) {
      TObject *dragedObject = gROOT->FindObject(str);
      QPoint Pos = Event->pos();
      TObject *object=0;
      TPad *pad = fCanvas->Pick(Pos.x(), Pos.y(), object);
      if(dragedObject!=0) {
        if(dragedObject->InheritsFrom(TH1::Class())) {
          pad->cd();
          dragedObject->Draw();
          pad->Update();
        }
      } else
        cout << "object " << str <<  " not found by ROOT" << endl;
    }
}

/////////////////////////////////////End Drag and drop Support (Mohammad Al-Turany)


void      TQRootCanvas::cd(Int_t subpadnumber)
{
   fCanvas->cd(subpadnumber);
}

void      TQRootCanvas::Browse(TBrowser *b)
{
   fCanvas->Browse(b);
}

void      TQRootCanvas::Clear(Option_t *option)
{
   fCanvas->Clear(option);
}
void      TQRootCanvas::Close(Option_t *option)
{
   fCanvas->Close(option);
}
void      TQRootCanvas::Draw(Option_t *option)
{
   fCanvas->Draw(option);
}
TObject  *TQRootCanvas::DrawClone(Option_t *option)
{
   return  fCanvas->DrawClone(option);
}
TObject  *TQRootCanvas::DrawClonePad()
{
   return  fCanvas->DrawClonePad();
}
void      TQRootCanvas::EditorBar()
{
   fCanvas->EditorBar();
}
void      TQRootCanvas::EnterLeave(TPad *prevSelPad, TObject *prevSelObj)
{
   fCanvas->EnterLeave(prevSelPad, prevSelObj);
}
void      TQRootCanvas::FeedbackMode(Bool_t set)
{
   fCanvas->FeedbackMode(set);
}
void      TQRootCanvas::Flush()
{
   fCanvas->Flush();
}
void      TQRootCanvas::UseCurrentStyle()
{
   fCanvas->UseCurrentStyle();
}
void      TQRootCanvas::ForceUpdate()
{
   fCanvas->ForceUpdate() ;
}
const char  *TQRootCanvas::GetDISPLAY()
{
   return fCanvas->GetDISPLAY() ;
}
TContextMenu  *TQRootCanvas::GetContextMenu()
{
   return  fCanvas->GetContextMenu() ;
}
Int_t     TQRootCanvas::GetDoubleBuffer()
{
   return fCanvas->GetDoubleBuffer();
}
TControlBar  *TQRootCanvas::GetEditorBar()
{
   return 0; // not existing anymore for ROOT>4.00/02 JA
   //return fCanvas->GetEditorBar();
}
Int_t     TQRootCanvas::GetEvent()
{
   return fCanvas->GetEvent();
}
Int_t     TQRootCanvas::GetEventX()
{
   return fCanvas->GetEventX() ;
}
Int_t     TQRootCanvas::GetEventY()
{
   return fCanvas->GetEventY() ;
}
Color_t   TQRootCanvas::GetHighLightColor()
{
   return fCanvas->GetHighLightColor() ;
}
void     TQRootCanvas::GetPadDivision(Int_t xdivision, Int_t ydivision)
{
//      fCanvas->GetPadDivision(xdivision, ydivision);
//       do nothing this function was taken out in root 3.0.4

}
TVirtualPad  *TQRootCanvas::GetPadSave()
{
   return fCanvas->GetPadSave();
}
TObject   *TQRootCanvas::GetSelected()
{
   return fCanvas->GetSelected() ;
}
Option_t  *TQRootCanvas::GetSelectedOpt()
{
   return fCanvas->GetSelectedOpt();
}
TVirtualPad *TQRootCanvas::GetSelectedPad()
{
   return fCanvas->GetSelectedPad();
}
Bool_t    TQRootCanvas::GetShowEventStatus()
{
   return fCanvas->GetShowEventStatus() ;
}
Bool_t    TQRootCanvas::GetAutoExec()
{
   return fCanvas->GetAutoExec();
}
Size_t    TQRootCanvas::GetXsizeUser()
{
   return fCanvas->GetXsizeUser();
}
Size_t    TQRootCanvas::GetYsizeUser()
{
   return fCanvas->GetYsizeUser();
}
Size_t    TQRootCanvas::GetXsizeReal()
{
   return fCanvas->GetXsizeReal();
}
Size_t    TQRootCanvas::GetYsizeReal()
{
   return fCanvas->GetYsizeReal();
}
Int_t     TQRootCanvas::GetCanvasID()
{
   return fCanvas->GetCanvasID();
}

Int_t     TQRootCanvas::GetWindowTopX()
{
   return fCanvas->GetWindowTopX();
}
Int_t     TQRootCanvas::GetWindowTopY()
{
   return fCanvas->GetWindowTopY();
}
UInt_t    TQRootCanvas::GetWindowWidth()
{
   return fCanvas->GetWindowWidth() ;
}
UInt_t    TQRootCanvas::GetWindowHeight()
{
   return fCanvas->GetWindowHeight();
}
UInt_t    TQRootCanvas::GetWw()
{
   return fCanvas->GetWw();
}
UInt_t    TQRootCanvas::GetWh()
{
   return fCanvas->GetWh() ;
}
void      TQRootCanvas::GetCanvasPar(Int_t &wtopx, Int_t &wtopy, UInt_t &ww, UInt_t &wh)
{
   fCanvas->GetCanvasPar(wtopx, wtopy, ww, wh);
}
void      TQRootCanvas::HandleInput(EEventType button, Int_t x, Int_t y)
{
   fCanvas->HandleInput(button, x, y);
}
Bool_t    TQRootCanvas::HasMenuBar()
{
   return fCanvas->HasMenuBar() ;
}
void      TQRootCanvas::Iconify()
{
   fCanvas->Iconify();
}
Bool_t    TQRootCanvas::IsBatch()
{
   return fCanvas->IsBatch() ;
}
Bool_t    TQRootCanvas::IsRetained()
{
   return fCanvas->IsRetained();
}
void      TQRootCanvas::ls(Option_t *option)
{
   fCanvas->ls(option);
}
void      TQRootCanvas::MoveOpaque(Int_t set)
{
   fCanvas->MoveOpaque(set);
}
Bool_t    TQRootCanvas::OpaqueMoving()
{
   return fCanvas->OpaqueMoving();
}
Bool_t    TQRootCanvas::OpaqueResizing()
{
   return fCanvas->OpaqueResizing();
}
void      TQRootCanvas::Paint(Option_t *option)
{
   fCanvas->Paint(option);
}
TPad     *TQRootCanvas::Pick(Int_t px, Int_t py, TObjLink *&pickobj)
{
   return fCanvas->Pick(px, py, pickobj);
}
TPad     *TQRootCanvas::Pick(Int_t px, Int_t py, TObject *prevSelObj)
{
   return fCanvas->Pick(px, py, prevSelObj);
}
void      TQRootCanvas::Resize(Option_t *option)
{
   fCanvas->Resize(option);
}
void      TQRootCanvas::ResizeOpaque(Int_t set)
{
   fCanvas->ResizeOpaque(set);
}
void      TQRootCanvas::SaveSource(const char *filename, Option_t *option)
{
   fCanvas->SaveSource(filename, option);
}
void      TQRootCanvas::SetCursor(ECursor cursor)
{
   fCanvas->SetCursor(cursor);
}
void      TQRootCanvas::SetDoubleBuffer(Int_t mode)
{
   fCanvas->SetDoubleBuffer(mode);
}
void      TQRootCanvas::SetWindowPosition(Int_t x, Int_t y)
{
   fCanvas->SetWindowPosition(x, y) ;
}
void      TQRootCanvas::SetWindowSize(UInt_t ww, UInt_t wh)
{
   fCanvas->SetWindowSize(ww,wh) ;
}
void      TQRootCanvas::SetCanvasSize(UInt_t ww, UInt_t wh)
{
   fCanvas->SetCanvasSize(ww, wh);
}
void      TQRootCanvas::SetHighLightColor(Color_t col)
{
   fCanvas->SetHighLightColor(col);
}
void      TQRootCanvas::SetSelected(TObject *obj)
{
   fCanvas->SetSelected(obj);
}
void      TQRootCanvas::SetSelectedPad(TPad *pad)
{
   fCanvas->SetSelectedPad(pad);
}
void      TQRootCanvas::Show()
{
   fCanvas->Show() ;
}
void      TQRootCanvas::Size(Float_t xsizeuser, Float_t ysizeuser)
{
   fCanvas->Size(xsizeuser, ysizeuser);
}
void      TQRootCanvas::SetBatch(Bool_t batch)
{
   fCanvas->SetBatch(batch);
}
void      TQRootCanvas::SetRetained(Bool_t retained)
{
   fCanvas->SetRetained(retained);
}
void      TQRootCanvas::SetTitle(const char *title)
{
   fCanvas->SetTitle(title);
}
void      TQRootCanvas::ToggleEventStatus()
{
   fCanvas->ToggleEventStatus();
}
void      TQRootCanvas::ToggleAutoExec()
{
   fCanvas->ToggleAutoExec();
}
void      TQRootCanvas::Update()
{
   fCanvas->Update();
}

void  TQRootCanvas::closeEvent( QCloseEvent * e){
 //   printf("TQRootCanvas: close event called \n");
    if( isCanvasOwned ){
      delete fCanvas; fCanvas = 0;
    }

    e->accept();
    return;
}
