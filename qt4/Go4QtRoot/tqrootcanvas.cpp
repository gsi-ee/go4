/****************************************************************************
** Copyright ( C ) 2000 denis Bertini.  All rights reserved.
*****************************************************************************/

#include "tqrootcanvas.h"

#include <QtCore/QEvent>
#include <QtCore/QSignalMapper>

#include <QtGui/qpainter.h>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QInputDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QMenu>
#include <QtGui/QAction>

#include "TPad.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TString.h"
#include "TH1.h"
#include "TClass.h"
#include "TDataType.h"
#include "TDataMember.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TMethodArg.h"
#include "TColor.h"
#include "TLatex.h"
#include "Riostream.h"

#include "TGo4LockGuard.h"

#include "tqrootdialog.h"

TQRootCanvas::TQRootCanvas( QWidget *parent, const char *name, TCanvas *c ) :
   QWidget(parent),
   fResizeFlag(1),
   fMaskDoubleClick(false)
{
   setObjectName( name ? name : "QRootCanvas");

  TGo4LockGuard threadlock;
  // set defaults
  setUpdatesEnabled( true );
  setMouseTracking(true);

  setFocusPolicy( Qt::TabFocus );
  setCursor( Qt::CrossCursor );
  //setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_PaintUnclipped);

  // add the Qt::WinId to TGX11 interface
  xid=winId();
  wid=gVirtualX->AddWindow(xid,100,30);
  if (c==0){
    isCanvasOwned = true;
    fCanvas=new TCanvas(name,width(),height(),wid);
     }else{
    isCanvasOwned= false;
    fCanvas=c;
  }
  // create the context menu
  fMousePosX = 0;
  fMousePosY = 0;
  fMenuMethods = 0;
  fMenuObj = 0;

  // test here all the events sent to the QWidget
  // has a parent widget
  // then install filter
  if ( parent ){
    parent->installEventFilter( this );
    fParent = parent;
  } else fParent=0;

    setAcceptDrops(TRUE);

}

TQRootCanvas::TQRootCanvas( QWidget *parent, QWidget* tabWin, const char *name, TCanvas *c ) :
   QWidget(tabWin),
   fResizeFlag(1),
   fMaskDoubleClick(false)
{
   setObjectName( name ? name : "QRootCanvas");

   TGo4LockGuard threadlock;
   setUpdatesEnabled( true );
   setMouseTracking(true);
   setFocusPolicy( Qt::TabFocus );
   setCursor( Qt::CrossCursor );

   setAttribute(Qt::WA_NoSystemBackground);
   //setAttribute(Qt::WA_PaintOnScreen);
   // add the Qt::WinId to TGX11 interface
   xid=winId();
   wid=gVirtualX->AddWindow(xid,100,30);
   if (c==0){
      isCanvasOwned = true;
      fCanvas=new TCanvas(name,width(),height(),wid);
   }else{
      isCanvasOwned= false;
      fCanvas=c;
   }
   // create the context menu
   fMousePosX = 0;
   fMousePosY = 0;
   fMenuMethods = 0;
   fMenuObj = 0;

   // test here all the events sent to the QWidget
   // has a parent widget
   // then install filter
   if ( parent ){
      parent->installEventFilter( this );
      fParent = parent;
   } else fParent=0;

   if ( tabWin ) fTabWin = tabWin;
   setAcceptDrops(TRUE);
}

TQRootCanvas::~TQRootCanvas()
{
   if( isCanvasOwned && fCanvas ) {
     delete fCanvas;
     fCanvas = 0;
   }
   delete fMenuMethods;
   fMenuMethods = 0;
}

void TQRootCanvas::setResizeFlag(int flag)
{
      fResizeFlag = flag;
}

bool TQRootCanvas::checkResizeFlag(int level)
{
   //cout <<"----- TQRootCanvas::checkResizeFlag, level="<<level<<", flag="<<fResizeFlag << endl;
   if (level>=fResizeFlag){
      performResize();
     return true;
   }
   return false;
}

void TQRootCanvas::performResize()
{
   TGo4LockGuard threadlock;
   UInt_t nxid=winId();
   if(nxid!=xid) {
      // Qt has changed xid for this widget (e.g. at QWorkspace::addWindow())
      // need to adjust the ROOT X access:
      //cout <<"----- TQRootCanvas::performResize finds changed xwinid:"<<  setbase(10) <<nxid<<endl;
      delete fCanvas; // should also remove old x windows!
      wid=gVirtualX->AddWindow(nxid,width(),height());
      fCanvas=new TCanvas(objectName().toAscii(), width(),height(),wid);
      xid=nxid;
   }
   Resize();
   Update();
}

void TQRootCanvas::mouseMoveEvent(QMouseEvent *e)
{
  TGo4LockGuard threadlock;
  if (fCanvas!=0) {
     if (e->buttons() & Qt::LeftButton)
        fCanvas->HandleInput(kButton1Motion, e->x(), e->y());
     else
        fCanvas->HandleInput(kMouseMotion, e->x(), e->y());
  }
}


void TQRootCanvas::mousePressEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;
   //cout <<"----- TQRootCanvas::mousePressEvent" << endl;
   TObjLink* pickobj = 0;
   TPad* pad = fCanvas->Pick(e->x(), e->y(), pickobj);
   TObject *selected = fCanvas->GetSelected();

   // fCanvas->cd();

   switch(e->button()) {
     case Qt::LeftButton :
        fCanvas->HandleInput(kButton1Down, e->x(), e->y());
        emit PadClicked(pad);
        break;
     case Qt::RightButton : {
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
         pad->cd();
        }
        fCanvas->SetSelectedPad(pad);
        gROOT->SetSelectedPrimitive(selected);
        fMousePosX = gPad->AbsPixeltoX(gPad->GetEventX());
        fMousePosY = gPad->AbsPixeltoY(gPad->GetEventY());

        QMenu menu(this);
        QSignalMapper map;
        connect(&map, SIGNAL(mapped(int)), this, SLOT(executeMenu(int)));

        fMenuObj = selected;
        fMenuMethods = new TList;
        TClass *cl = fMenuObj->IsA();
        int curId = -1;

        QString buffer = Form("%s::%s", cl->GetName(), fMenuObj->GetName());
        addMenuAction(&menu, &map, buffer, curId++);

        cl->GetMenuItems(fMenuMethods);
        menu.addSeparator();

        if(!cl->InheritsFrom(TLatex::Class())) {
           addMenuAction(&menu, &map, "Insert Latex", 100 );
           menu.addSeparator();
        }

        if(cl->InheritsFrom(TH1::Class())) {
          addMenuAction(&menu, &map, "Qt Hist Line Color ", 101 );
          addMenuAction(&menu, &map, "Qt Hist Fill Color ", 102 );
          menu.addSeparator();
        }

        TIter iter(fMenuMethods);
        TMethod *method=0;
        while ( (method = dynamic_cast<TMethod*>(iter())) != 0) {
           buffer = method->GetName();
           addMenuAction(&menu, &map, buffer, curId++);
        }

        if (menu.exec(e->globalPos())==0) {
           fMenuObj = 0;
           delete fMenuMethods;
           fMenuMethods = 0;
        }

        break;
     }
     case Qt::MidButton :
        fCanvas->HandleInput(kButton2Down, e->x(), e->y());
        emit SelectedPadChanged(pad);   //   that inform the Qt-world that tha pad is changed
                                        // and give the pointer to the new pad as argument
                                       // of the signal (M. Al-Turany)
        break;
     case  Qt::NoButton :
        break;
     default:
        break;
   }
}

void TQRootCanvas::mouseReleaseEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;
   switch(e->button()) {
      case Qt::LeftButton :
         fCanvas->HandleInput(kButton1Up, e->x(), e->y());
         break;
      case Qt::RightButton :
         fCanvas->HandleInput(kButton3Up, e->x(), e->y());
         break;
      case Qt::MidButton :
         fCanvas->HandleInput(kButton2Up, e->x(), e->y());
         break;
      case  Qt::NoButton :
         break;
      default:
         break;
  }
}

void TQRootCanvas::mouseDoubleClickEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;
   switch(e->button()) {
      case Qt::LeftButton : {
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
      case Qt::RightButton :
         fCanvas->HandleInput(kButton3Double, e->x(), e->y());
         break;
      case Qt::MidButton :
         fCanvas->HandleInput(kButton2Double, e->x(), e->y());
         break;
      case Qt::NoButton :
         break;
      default:
         break;
   }
}

void TQRootCanvas::resizeEvent( QResizeEvent *e )
{
   fResizeFlag++; // counter will dynamically disable repaint for continuous resize
}

void TQRootCanvas::paintEvent( QPaintEvent * e)
{
   checkResizeFlag(1); // repaint root graphics only if resizing is not in progress
}

void TQRootCanvas::leaveEvent( QEvent *e )
{
   TGo4LockGuard threadlock;
   if (fCanvas!=0)
      fCanvas->HandleInput(kMouseLeave, 0, 0);
}

bool TQRootCanvas::eventFilter( QObject *o, QEvent *e )
{
TGo4LockGuard threadlock;
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

//   if( e->type() == QEvent::MouseButtonRelease) {
//            return FALSE;
//   }

   if( e->type() == QEvent::Enter) { // On enter event check resize flag
       //cout <<"----- TQRootCanvas::eventFilter: Enter" << endl;
       setResizeFlag(1); // enable repaint when resize is done
       checkResizeFlag(1); // immediately do a repaint on entering window
       return FALSE;
   }

   if( e->type() == QEvent::Leave) {
       fCanvas->FeedbackMode(kFALSE);
       return FALSE;
    }


   if( e->type() == QEvent::Paint) {  // Paint is handled in paintEvent
      return FALSE;
   }

   if( e->type() == QEvent::Move) {
     return FALSE;
   }

   if( (e->type() == QEvent::Show))
// ||
//       (e->type() == QEvent::ShowNormal) ||
//       (e->type() == QEvent::ShowFullScreen) ||
//       (e->type() == QEvent::ShowMaximized) ||
//       (e->type() == QEvent::ShowMinimized))
   {
        //cout <<"----- TQRootCanvas::eventFilter: Show" << endl;
        setResizeFlag(1);
        checkResizeFlag(1);
        return FALSE;
   }

//   if( (e->type() == QEvent::UpdateRequest))
//      {
//        cout <<"----- TQRootCanvas::eventFilter: UpdateRequest" << endl;
//        return FALSE;
//   }

   // standard event processing
   return QWidget::eventFilter( o, e );
}

////////////////////////////////////// drag and drop support

void TQRootCanvas::dragEnterEvent( QDragEnterEvent *e )
{
   if (e->mimeData()->hasText())
        e->acceptProposedAction();
}

void TQRootCanvas::dropEvent( QDropEvent *Event )
{
   TGo4LockGuard threadlock;

   QString str = Event->mimeData()->text();

   TObject *dragedObject = gROOT->FindObject(str.toAscii());
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
     cout << "object " << str.toStdString() <<  " not found by ROOT" << endl;
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


void TQRootCanvas::methodDialog(TObject* object, TMethod* method)
{
   if ((object==0) || (method==0)) return;

   TGo4LockGuard threadlock;
  // Create dialog object with OK and Cancel buttons. This dialog
   // prompts for the arguments of "method".

   TQRootDialog dlg;

   dlg.setWindowTitle(Form("%s:%s", object->GetName(), method->GetName()));

  // iterate through all arguments and create apropriate input-data objects:
  // inputlines, option menus...
   TMethodArg *argument = 0;
   TIter next(method->GetListOfMethodArgs());

   while ((argument = (TMethodArg *) next())) {
      TString argTitle = Form("(%s)  %s", argument->GetTitle(), argument->GetName());
      TString argDflt = argument->GetDefault() ? argument->GetDefault() : "";
      if (argDflt.Length()>0)
         argTitle += Form(" [default: %s]", argDflt.Data());
      TString type       = argument->GetTypeName();
      TDataType    *datatype   = gROOT->GetType(type);
      TString       basictype;

      if (datatype) {
         basictype = datatype->GetTypeName();
      } else {
         if (strncmp(type.Data(), "enum", 4) != 0)
            cout << "*** Warning in Dialog(): data type is not basic type, assuming (int)\n";
         basictype = "int";
      }

      if (strchr(argument->GetTitle(), '*')) {
         basictype += "*";
         type = "char*";
      }

      TDataMember *m = argument->GetDataMember();
      if (m && m->GetterMethod()) {

         Text_t gettername[256] = "";
         strncpy(gettername, m->GetterMethod()->GetMethodName(),255);
         m->GetterMethod()->Init(object->IsA(), gettername, "");

         // Get the current value and form it as a text:

         Text_t val[256];

         if (basictype == "char*") {
            Text_t *tdefval;
            m->GetterMethod()->Execute(object, "", &tdefval);
            strncpy(val, tdefval, 255);
         } else
         if ((basictype == "float") ||
             (basictype == "double")) {
            Double_t ddefval;
            m->GetterMethod()->Execute(object, "", ddefval);
            snprintf(val, 255, "%g", ddefval);
         } else
         if ((basictype == "char") ||
             (basictype == "int")  ||
             (basictype == "long") ||
             (basictype == "short")) {
            Long_t ldefval;
            m->GetterMethod()->Execute(object, "", ldefval);
            snprintf(val, 255, "%li", ldefval);
         }

         // Find out whether we have options ...

         TList *opt;
         if ((opt = m->GetOptions()) != 0) {
            cout << "*** Warning in Dialog(): option menu not yet implemented " << opt << endl;
            // should stop dialog
            return;
         } else {
            // we haven't got options - textfield ...
            dlg.addArg(argTitle.Data(), val, type.Data());
         }
      } else {    // if m not found ...
         if ((argDflt.Length() > 1) &&
             (argDflt[0]=='\"') && (argDflt[argDflt.Length()-1]=='\"')) {
            // cut "" from the string argument
            argDflt.Remove(0,1);
            argDflt.Remove(argDflt.Length()-1,1);
         }

         dlg.addArg(argTitle.Data(), argDflt.Data(), type.Data());
      }
   }

   if (dlg.exec() != QDialog::Accepted) return;

   Bool_t deletion = kFALSE;

   qDebug("DIAL executeMethod:  simple version\n");
   TVirtualPad *psave =  gROOT->GetSelectedPad();

   qDebug("DIAL saved pad: %s gPad:%s \n",psave->GetName(),gPad->GetName());

   qDebug("DIAL obj:%s meth:%s \n", object->GetName(), method->GetName());

   //cout<< "executeMethod" << fCurMethod->GetName() << endl;

   TObjArray tobjlist(method->GetListOfMethodArgs()->LastIndex() + 1);
   for (int n=0; n<=method->GetListOfMethodArgs()->LastIndex(); n++) {
      QString s = dlg.getArg(n);
      qDebug( "** QString values (first ) :%s \n", (const char*) s.toAscii() );
      TObjString *t = new TObjString( (const char*) s.toAscii() );
      tobjlist.AddLast(t) ;
   }

   // handle command if existing object
   if(strcmp(method->GetName(),"Delete") == 0) {
      // here call explicitely the dtor
      qDebug(" DIAL obj name deleted :%s \n", object->GetName());
      emit MenuCommandExecuted(object, "Delete");
      delete object;
      object = 0;
      deletion = kTRUE;
      qDebug(" DIAL deletion done closing ... \n");
   } else
   if (strcmp(method->GetName(), "SetCanvasSize") == 0) {
      int width = dlg.getArg(0).toInt();
      int height = dlg.getArg(1).toInt();
      qDebug( " do resize with %i %i \n", width, height);
      resize(width, height);
      emit MenuCommandExecuted(fCanvas, "SetCanvasSize");
   } else {
      // here call cint call
      qDebug("TCint::Execute called !\n");

      object->Execute(method, &tobjlist);

      if (object->TestBit(TObject::kNotDeleted))
         emit MenuCommandExecuted(object, method->GetName());
      else {
        deletion = TRUE;
        object = 0;
      }
   }

   if(!deletion ) {
      qDebug("DIAL set saved pad: %s herit:%s gPad:%s\n",
             psave->GetName(), psave->ClassName(), gPad->GetName());
      gROOT->SetSelectedPad(psave);
      gROOT->GetSelectedPad()->Modified();
      gROOT->GetSelectedPad()->Update();
      qDebug("DIAL update done on %s \n", gROOT->GetSelectedPad()->GetName());
   } else {
      gROOT->SetSelectedPad( gPad );
      gROOT->GetSelectedPad()->Update();
   }
}

QAction* TQRootCanvas::addMenuAction(QMenu* menu, QSignalMapper* map, const QString& text, int id)
{
   QAction* act = new QAction(text, menu);
   map->connect (act, SIGNAL(triggered()), map, SLOT(map()));
   menu->addAction(act);
   map->setMapping(act, id);
   return act;
}

void TQRootCanvas::executeMenu(int id)
{
   TGo4LockGuard threadlock;
   QString text("");
   bool ok = FALSE;
   if (id >=100) {
      switch (id){
         case 100: {
            TLatex *fxLatex = new TLatex();
            text = QInputDialog::getText(this, tr( "Qt Root" ),
                                         tr( "Please enter your text" ),
                                         QLineEdit::Normal, QString::null, &ok);
            //if (ok && !text.isEmpty())
            fxLatex->DrawLatex(fMousePosX, fMousePosY, text.toAscii());
            emit MenuCommandExecuted(fxLatex, "DrawLatex");
            break;
        }
        case 101: {
           TH1 *h1 = dynamic_cast<TH1*> (fMenuObj);
           if (h1!=0) {
              QColor col = QColorDialog::getColor();
              if (col.isValid()) {
                 short int C_new =  TColor::GetColor(col.red(), col.green(), col.blue());
                 h1->SetLineColor(C_new);
                 emit MenuCommandExecuted(h1, "SetLineColor");
              }
            }
           break;
        }
        case 102 : {
           TH1 *h1 = dynamic_cast<TH1*> (fMenuObj);
           if (h1!=0) {
              QColor col = QColorDialog::getColor();
              if (col.isValid()) {
                short int C_new =  TColor::GetColor(col.red(), col.green(), col.blue());
                h1->SetFillColor(C_new);
                emit MenuCommandExecuted(h1,"SetFillColor");
              }
           }
        }
      }
      gROOT->GetSelectedPad()->Update();
      gROOT->GetSelectedPad()->Modified();
      fCanvas->Modified();
      fCanvas->ForceUpdate();
      gROOT->SetFromPopUp( kFALSE );
   } else
   if (id >=0) {

      // save global to Pad before calling TObject::Execute()

      TVirtualPad  *psave = gROOT->GetSelectedPad();
      TMethod *method= (TMethod *) fMenuMethods->At(id);

      /// test: do this in any case!
      fCanvas->HandleInput(kButton3Up, gPad->XtoAbsPixel(fMousePosX), gPad->YtoAbsPixel(fMousePosY));

      // change current dir that all new histograms appear here
      gROOT->cd();

      if (method->GetListOfMethodArgs()->First())
         methodDialog(fMenuObj, method);
      else {
         gROOT->SetFromPopUp(kTRUE);
         fMenuObj->Execute(method->GetName(), "");

         if (fMenuObj->TestBit(TObject::kNotDeleted)) {
            emit MenuCommandExecuted(fMenuObj, method->GetName());
         } else
            fMenuObj = 0;
      }
      fCanvas->GetPadSave()->Update();
      fCanvas->GetPadSave()->Modified();
      gROOT->SetSelectedPad(psave);
      ////qDebug("MENU:  gPad:%s gROOT:%s cur:%s \n",
      //gPad->GetName(), gROOT->GetSelectedPad()->GetName(), c->GetName());

      gROOT->GetSelectedPad()->Update();
      gROOT->GetSelectedPad()->Modified();
      fCanvas->Modified();
      fCanvas->ForceUpdate();
      gROOT->SetFromPopUp(kFALSE);
    }

   fMenuObj = 0;
   delete fMenuMethods;
   fMenuMethods = 0;
}
