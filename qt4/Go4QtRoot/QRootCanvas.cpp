// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/****************************************************************************
** Copyright ( C ) 2000 denis Bertini.  All rights reserved.
*****************************************************************************/

#include "QRootCanvas.h"

#include <iostream>

#include <QtCore/QSignalMapper>
#include <QtCore/QTimer>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QInputDialog>
#include <QColorDialog>
#include <QMenu>
#include <QAction>
#include <QMimeData>
#include <QVBoxLayout>
#include <QStatusBar>

#include "TCanvas.h"
#include "TROOT.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TClass.h"
#include "TDataType.h"
#include "TDataMember.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TMethodArg.h"
#include "TColor.h"
#include "TLatex.h"

#ifndef __NOGO4GED__
#include "TGedEditor.h"
#include "TVirtualPadEditor.h"
#endif

#ifdef __GO4X11__
#include "TGo4LockGuard.h"
#else
#define TGo4LockGuard int
#endif

#include "QRootDialog.h"
#include "QRootWindow.h"
#include "QRootApplication.h"

QRootCanvas::QRootCanvas(QWidget *parent) :
   QWidget(parent),
   fMaskDoubleClick(false),
   fxShowEventStatus(false),
   fQtScalingfactor(1.0)
{
   setObjectName( "QRootCanvas");

   setSizeIncrement( QSize( 100, 100 ) );

   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings
   // set defaults
   setUpdatesEnabled( true );
   setMouseTracking(true);

   setFocusPolicy( Qt::TabFocus );
   setCursor( Qt::CrossCursor );

   // disable option that at least background is redrawn immediately
   // and canvas content after 100 ms timeout
   //setAttribute(Qt::WA_NoSystemBackground);
   setAttribute(Qt::WA_PaintOnScreen);
   setAttribute(Qt::WA_PaintUnclipped);

   // add the Qt::WinId to TGX11 interface
   fQtWindowId = winId();
   fRootWindowId = gVirtualX->AddWindow((ULong_t)fQtWindowId, 100, 30);

   fCanvas = new TCanvas("Canvas", width(), height(), fRootWindowId);

   // JAM the following is pure empiric. hopefully default denominator won't change in future qt?
   fQtScalingfactor = (double) metric(QPaintDevice::PdmDevicePixelRatioScaled)/65536.;
   // create the context menu
   fMousePosX = 0;
   fMousePosY = 0;
   fMenuMethods = nullptr;
   fMenuObj = nullptr;

   setAcceptDrops(true);

   fRepaintMode = 0;
   fRepaintTimer = new QTimer;
   fRepaintTimer->setSingleShot(true);
   QObject::connect(fRepaintTimer, &QTimer::timeout, this, &QRootCanvas::processRepaintTimer);

   fEditorFrame = nullptr;
   fxPeditor = nullptr;
   fxRooteditor = nullptr;
   fDummyHisto = nullptr;

   fStatusBar = nullptr;
}

QRootCanvas::~QRootCanvas()
{
   // cannot call here, must be done before until X11 frame is exists
   // cleanupEditor();

   if (fDummyHisto) {
      delete fDummyHisto;
      fDummyHisto = nullptr;
   }

#ifndef __NOGO4GED__
   if (gTQSender == getCanvas())
      gTQSender = nullptr;

   // prevent problems with root's subeditor cache
   if (fxPeditor) {
      fxPeditor->DeleteEditors();
      delete fxPeditor;
      fxPeditor = nullptr;
   }
#endif

   if(fCanvas) {
      delete fCanvas;
      fCanvas = nullptr;
   }

   if (fMenuMethods) {
      delete fMenuMethods;
      fMenuMethods = nullptr;
   }

   delete fRepaintTimer;
}

QPoint QRootCanvas::scaledMousePoint(QMouseEvent *e)
{
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   int scaledX = scaledPosition(e->x());
   int scaledY = scaledPosition(e->y());
#else
   int scaledX = scaledPosition(e->position().x());
   int scaledY = scaledPosition(e->position().y());
#endif
   return QPoint(scaledX, scaledY);
}

void QRootCanvas::mouseMoveEvent(QMouseEvent *e)
{
  TGo4LockGuard threadlock;
  (void) threadlock; // suppress compiler warnings
  // JAM use event timestamp for reduction of events (qt5 bug):
  static ulong lastprocesstime = 0;
  static ulong delta = 100; // maybe ms units?
  ulong timestamp = e->timestamp();
  if(timestamp-delta < lastprocesstime) {
      // just eat too old events
      e->accept();
      // std::cout <<"----- EATING timestamp:"<<timestamp<< std::endl;
      return;
  }

  if (fCanvas) {
     QPoint pnt = scaledMousePoint(e);

     if (e->buttons() & Qt::LeftButton)
        fCanvas->HandleInput(kButton1Motion, pnt.x(), pnt.y());
     else
        fCanvas->HandleInput(kMouseMotion, pnt.x(), pnt.y());
  }

  if(fxShowEventStatus) {
     TObject *selected = fCanvas->GetSelected();
     Int_t px = fCanvas->GetEventX();
     Int_t py = fCanvas->GetEventY();
     QString buffer = "";
     if (selected) {
        buffer = selected->GetName();
        buffer += "  ";
        buffer += selected->GetObjectInfo(px, py);
     } else {
        buffer = "No selected object x = ";
        buffer += QString::number(px);
        buffer += "  y = ";
        buffer += QString::number(py);
     }

     emit CanvasStatusEvent(buffer.toLatin1().constData());

     if (fStatusBar) fStatusBar->showMessage(buffer.toLatin1().constData());
  }
   e->accept();
   lastprocesstime = timestamp;
}

void  QRootCanvas::wheelEvent( QWheelEvent* e)
{
   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   if (!fCanvas) return;
   e->accept();

#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
   bool positive = (e->delta() > 0);
   int ex = e->x(), ey = e->y();
#else
   QPoint delta = e->pixelDelta();
   if (delta.isNull()) delta = e->angleDelta() / 8;
   bool positive = delta.x() > 0 || delta.y() > 0;
   int ex = e->position().x(), ey = e->position().y();
#endif

   fCanvas->HandleInput(positive ? kWheelUp : kWheelDown, scaledPosition(ex), scaledPosition(ey));
}


void QRootCanvas::mousePressEvent( QMouseEvent *e )
{
   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   TObjLink* pickobj = nullptr;
   // JAM2016-9 test
//    std::cout <<"QRootCanvas::mousePressEvent at ("<<e->x()<<", "<<  e->y()<<")"<< std::endl;

   QPoint scaled = scaledMousePoint(e);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   QPoint mouse_pnt = e->globalPos();
#else
   QPoint mouse_pnt = e->globalPosition().toPoint();
#endif

  // std::cout <<"      scaledX,scaledY: ("<<scaledX<<", "<<scaledY <<") "<< std::endl;
//      std::cout <<"global from event: ("<<e->globalX()<<", "<<  e->globalY()<< std::endl;
//    QPoint globalp=QWidget::mapToGlobal(e->pos());
//    std::cout <<"global from map: ("<<globalp.x()<<", "<<globalp.y() <<") "<< std::endl;
//    QPoint parentp=QWidget::mapToParent(e->pos());
//    std::cout <<"parent: ("<<parentp.x()<<", "<<parentp.y()<<") " << std::endl;
//    QPoint backfromglobalp=QWidget::mapFromGlobal(globalp);
//     std::cout <<"backglobal: ("<<backfromglobalp.x()<<", "<<backfromglobalp.y()<<") " << std::endl;
//     Int_t destx = 0, desty = 0;
//     Window_t child;
//     Window_t rootwindow=gVirtualX->GetDefaultRootWindow();
//      gVirtualX->TranslateCoordinates(rootwindow, fQtWindowId, globalp.x(), globalp.y(), destx, desty, child);
//      std::cout <<"TGX11 global translated: ("<<destx<<", "<<desty<<") " << std::endl;
//      std::cout <<"TGX11 winids - default root:"<<rootwindow<<", Qt:"<<fQtWindowId<<", child:" <<child<< std::endl;


     /*  int themetric= metric(QPaintDevice::PdmDevicePixelRatio);
      int scaledmetric= metric(QPaintDevice::PdmDevicePixelRatioScaled);
      std::cout <<"metric="<<themetric<<", scaled="<<scaledmetric << std::endl;
     int scaledX=e->x() * scaledmetric/65536; // empiric
     int scaledY=e->y() * scaledmetric/65536; // empiric
     std::cout <<"scaledX,scaledY: ("<<scaledX<<", "<<scaledY <<") "<< std::endl;
    */


   TPad *pad = fCanvas->Pick(scaled.x(), scaled.y(), pickobj);
   TObject *selected = fCanvas->GetSelected();

   switch(e->button()) {
     case Qt::LeftButton :
        fCanvas->HandleInput(kButton1Down, scaled.x(), scaled.y());
        emit PadClicked(pad, scaled.x(), scaled.y());
        break;
     case Qt::RightButton : {
        TString selectedOpt("");
        if (pad) {
           if (!pickobj) {
              fCanvas->SetSelected(pad);
              selected = pad;
           } else if(!selected) {
              selected    = pickobj->GetObject();
              selectedOpt = pickobj->GetOption();
           }
           pad->cd();
        }
        fCanvas->SetSelectedPad(pad);
        gROOT->SetSelectedPrimitive(selected);
        fMousePosX = gPad->AbsPixeltoX(gPad->GetEventX());
        fMousePosY = gPad->AbsPixeltoY(gPad->GetEventY());

        QMenu menu;
        QSignalMapper map;
   #if QT_VERSION < QT_VERSION_CHECK(5,15,0)
        auto signal = QOverload<int>::of(&QSignalMapper::mapped);
   #else
        auto signal = &QSignalMapper::mappedInt;
   #endif

        QObject::connect(&map, signal, this, &QRootCanvas::executeMenu);
        fMenuObj = selected;
        fMenuMethods = new TList;
        TClass *cl = fMenuObj->IsA();
        int curId = -1;

        QString buffer = TString::Format("%s::%s", cl->GetName(), fMenuObj->GetName()).Data();
        addMenuAction(&menu, &map, buffer, curId++);

        cl->GetMenuItems(fMenuMethods);
        menu.addSeparator();

        if(!cl->InheritsFrom(TLatex::Class())) {
           addMenuAction(&menu, &map, "Insert Latex", 100);
           menu.addSeparator();
        }

        if(cl->InheritsFrom(TH1::Class())) {
          addMenuAction(&menu, &map, "Qt Hist Line Color ", 101);
          addMenuAction(&menu, &map, "Qt Hist Fill Color ", 102);
          menu.addSeparator();
        }

        TIter iter(fMenuMethods);
        while (auto method = dynamic_cast<TMethod*>(iter())) {
           buffer = method->GetName();
           addMenuAction(&menu, &map, buffer, curId++);
        }

        if (menu.exec(mouse_pnt) == nullptr) {
           fMenuObj = nullptr;
           delete fMenuMethods;
           fMenuMethods = nullptr;
        }

        break;
     }
     case Qt::MiddleButton :
        fCanvas->HandleInput(kButton2Down, scaled.x(), scaled.y());
        emit SelectedPadChanged(pad);   //   that inform the Qt-world that tha pad is changed
                                        // and give the pointer to the new pad as argument
                                       // of the signal (M. Al-Turany)
        break;
     case  Qt::NoButton :
        break;
     default:
        break;
   }
   e->accept();
}

void QRootCanvas::mouseReleaseEvent( QMouseEvent *e )
{
   QPoint scaled = scaledMousePoint(e);

   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   switch(e->button()) {
      case Qt::LeftButton :
         fCanvas->HandleInput(kButton1Up, scaled.x(), scaled.y());
         break;
      case Qt::RightButton :
         fCanvas->HandleInput(kButton3Up, scaled.x(), scaled.y());
         break;
      case Qt::MiddleButton :
         fCanvas->HandleInput(kButton2Up, scaled.x(), scaled.y());
         break;
      case Qt::NoButton :
         break;
      default:
         break;
   }
   e->accept();
}

void QRootCanvas::mouseDoubleClickEvent( QMouseEvent *e )
{
   QPoint scaled = scaledMousePoint(e);

   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   switch(e->button()) {
      case Qt::LeftButton : {
         if (!fMaskDoubleClick)
            fCanvas->HandleInput(kButton1Double, scaled.x(), scaled.y());
         TObjLink* pickobj = nullptr;
         TPad *pad = fCanvas->Pick(scaled.x(), scaled.y(), pickobj);
         emit PadDoubleClicked(pad, scaled.x(), scaled.y());
         // prevent crash on following release event
         // if new canvas will be created in between
         // fCanvas->SetSelected(0);
         break;
      }
      case Qt::RightButton :
         fCanvas->HandleInput(kButton3Double, scaled.x(), scaled.y());
         break;
      case Qt::MiddleButton :
         fCanvas->HandleInput(kButton2Double, scaled.x(), scaled.y());
         break;
      case Qt::NoButton :
         break;
      default:
         break;
   }
    e->accept();
}

void QRootCanvas::activateRepaint(int mode)
{
   fRepaintMode |= mode;
//   if (fRepaintMode > 0) setUpdatesEnabled( false ); // JAM avoid flicker on Qt5 ?
   fRepaintTimer->setSingleShot(true);
   fRepaintTimer->start(100);
}

void QRootCanvas::resizeEvent( QResizeEvent *)
{
   activateRepaint(act_Resize);
}

void QRootCanvas::paintEvent( QPaintEvent *)
{
   // this is workaround a problem, that after drawing canvas in
   // viewpanel there is always 1 event after that
   // therefore fRepaintMode set to -1 to ignore such first event
   // In future behavior may change

   if (fRepaintMode < 0)
      fRepaintMode = 0;
   else
      activateRepaint(act_Update);
}

void QRootCanvas::processRepaintTimer()
{
   if (fRepaintMode == 0) return;
   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   WId newid = winId();
   if(newid != fQtWindowId) {
       //printf("processRepaintTimer - sees changed window id %d \n",newid);
      // Qt has changed id for this widget (e.g. at QWorkspace::addWindow())
      // need to adjust the ROOT X access:
      delete fCanvas; // should also remove old x windows!
      fRootWindowId = gVirtualX->AddWindow((ULong_t)newid, width(), height());
      fCanvas = new TCanvas(objectName().toLatin1().constData(), width(), height(), fRootWindowId);
      fQtWindowId = newid;
   }

   if (fRepaintMode & act_Resize) fCanvas->Resize();
                             else fCanvas->Modified(kTRUE);

   fCanvas->Update();

   fRepaintMode = 0;
   emit CanvasUpdated();
   //setUpdatesEnabled( true ); // JAM avoid flicker on Qt5 ?
}

void QRootCanvas::leaveEvent( QEvent *e )
{
   QWidget::leaveEvent(e);

   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   if (fCanvas)
      fCanvas->HandleInput(kMouseLeave, 0, 0);

   emit CanvasLeaveEvent();
}

void QRootCanvas::setShowEventStatus(bool s)
{
   fxShowEventStatus = s;
}

bool QRootCanvas::showEventStatus() const
{
   return fxShowEventStatus;
}

////////////////////////////////////// drag and drop support

void QRootCanvas::dragEnterEvent( QDragEnterEvent *e )
{
   if (e->mimeData()->hasText())
      e->acceptProposedAction();
}

void QRootCanvas::dropEvent( QDropEvent *event )
{
   TObject *obj = nullptr;

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   QPoint pos = event->pos();
#else
   QPoint pos = event->position().toPoint();
#endif

   TPad *pad = Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   if (pad)
      emit CanvasDropEvent(event, pad);
}

/////////////////////////////////////End Drag and drop Support (Mohammad Al-Turany)

void QRootCanvas::cd(Int_t subpadnumber)
{
   fCanvas->cd(subpadnumber);
}

void QRootCanvas::Browse(TBrowser *b)
{
   fCanvas->Browse(b);
}

void QRootCanvas::Clear(Option_t *option)
{
   fCanvas->Clear(option);
}

void QRootCanvas::Close(Option_t *option)
{
   fCanvas->Close(option);
}

void QRootCanvas::Draw(Option_t *option)
{
   fCanvas->Draw(option);
}

TObject *QRootCanvas::DrawClone(Option_t *option)
{
   return  fCanvas->DrawClone(option);
}

TObject *QRootCanvas::DrawClonePad()
{
   return  fCanvas->DrawClonePad();
}

void QRootCanvas::EditorBar()
{
   fCanvas->EditorBar();
}

void QRootCanvas::EnterLeave(TPad *prevSelPad, TObject *prevSelObj)
{
   fCanvas->EnterLeave(prevSelPad, prevSelObj);
}

void QRootCanvas::FeedbackMode(Bool_t set)
{
   fCanvas->FeedbackMode(set);
}

void QRootCanvas::Flush()
{
   fCanvas->Flush();
}

void QRootCanvas::UseCurrentStyle()
{
   fCanvas->UseCurrentStyle();
}

void QRootCanvas::ForceUpdate()
{
   fCanvas->ForceUpdate();
}

const char  *QRootCanvas::GetDISPLAY()
{
   return fCanvas->GetDISPLAY();
}

TContextMenu  *QRootCanvas::GetContextMenu()
{
   return  fCanvas->GetContextMenu();
}

Int_t QRootCanvas::GetDoubleBuffer()
{
   return fCanvas->GetDoubleBuffer();
}

Int_t QRootCanvas::GetEvent()
{
   return fCanvas->GetEvent();
}

Int_t QRootCanvas::GetEventX()
{
   return fCanvas->GetEventX();
}

Int_t QRootCanvas::GetEventY()
{
   return fCanvas->GetEventY();
}

Color_t QRootCanvas::GetHighLightColor()
{
   return fCanvas->GetHighLightColor();
}

TVirtualPad *QRootCanvas::GetPadSave()
{
   return fCanvas->GetPadSave();
}

TObject *QRootCanvas::GetSelected()
{
   return fCanvas->GetSelected();
}

Option_t *QRootCanvas::GetSelectedOpt()
{
   return fCanvas->GetSelectedOpt();
}

TVirtualPad *QRootCanvas::GetSelectedPad()
{
   return fCanvas->GetSelectedPad();
}

Bool_t QRootCanvas::GetShowEventStatus()
{
   return fCanvas->GetShowEventStatus();
}

Bool_t QRootCanvas::GetAutoExec()
{
   return fCanvas->GetAutoExec();
}

Size_t QRootCanvas::GetXsizeUser()
{
   return fCanvas->GetXsizeUser();
}

Size_t QRootCanvas::GetYsizeUser()
{
   return fCanvas->GetYsizeUser();
}

Size_t QRootCanvas::GetXsizeReal()
{
   return fCanvas->GetXsizeReal();
}

Size_t QRootCanvas::GetYsizeReal()
{
   return fCanvas->GetYsizeReal();
}

Int_t QRootCanvas::GetCanvasID()
{
   return fCanvas->GetCanvasID();
}

Int_t QRootCanvas::GetWindowTopX()
{
   return fCanvas->GetWindowTopX();
}

Int_t QRootCanvas::GetWindowTopY()
{
   return fCanvas->GetWindowTopY();
}

UInt_t QRootCanvas::GetWindowWidth()
{
   return fCanvas->GetWindowWidth();
}

UInt_t QRootCanvas::GetWindowHeight()
{
   return fCanvas->GetWindowHeight();
}

UInt_t QRootCanvas::GetWw()
{
   return fCanvas->GetWw();
}

UInt_t QRootCanvas::GetWh()
{
   return fCanvas->GetWh();
}

void QRootCanvas::GetCanvasPar(Int_t &wtopx, Int_t &wtopy, UInt_t &ww, UInt_t &wh)
{
   fCanvas->GetCanvasPar(wtopx, wtopy, ww, wh);
}

void QRootCanvas::HandleInput(EEventType button, Int_t x, Int_t y)
{
   fCanvas->HandleInput(button, scaledPosition(x), scaledPosition(y));
}

Bool_t QRootCanvas::HasMenuBar()
{
   return fCanvas->HasMenuBar();
}

void QRootCanvas::Iconify()
{
   fCanvas->Iconify();
}

Bool_t QRootCanvas::IsBatch()
{
   return fCanvas->IsBatch();
}

Bool_t QRootCanvas::IsRetained()
{
   return fCanvas->IsRetained();
}

void QRootCanvas::ls(Option_t *option)
{
   fCanvas->ls(option);
}

void QRootCanvas::Modified(Bool_t mod)
{
   fCanvas->Modified(mod);
}

void QRootCanvas::MoveOpaque(Int_t set)
{
   fCanvas->MoveOpaque(set);
}

Bool_t QRootCanvas::OpaqueMoving()
{
   return fCanvas->OpaqueMoving();
}

Bool_t QRootCanvas::OpaqueResizing()
{
   return fCanvas->OpaqueResizing();
}

void QRootCanvas::Paint(Option_t *option)
{
   fCanvas->Paint(option);
}

TPad *QRootCanvas::Pick(Int_t px, Int_t py, TObjLink *&pickobj)
{
   return fCanvas->Pick(px, py, pickobj);
}

TPad *QRootCanvas::Pick(Int_t px, Int_t py, TObject *prevSelObj)
{
   return fCanvas->Pick(px, py, prevSelObj);
}

void QRootCanvas::Resize(Option_t *option)
{
   fCanvas->Resize(option);
}

void QRootCanvas::ResizeOpaque(Int_t set)
{
   fCanvas->ResizeOpaque(set);
}

void QRootCanvas::SaveSource(const char *filename, Option_t *option)
{
   fCanvas->SaveSource(filename, option);
}

void QRootCanvas::SetCursor(ECursor cursor)
{
   fCanvas->SetCursor(cursor);
}

void QRootCanvas::SetDoubleBuffer(Int_t mode)
{
   fCanvas->SetDoubleBuffer(mode);
}

void QRootCanvas::SetWindowPosition(Int_t x, Int_t y)
{
   fCanvas->SetWindowPosition(x, y);
}

void QRootCanvas::SetWindowSize(UInt_t ww, UInt_t wh)
{
   fCanvas->SetWindowSize(ww,wh);
}

void QRootCanvas::SetCanvasSize(UInt_t ww, UInt_t wh)
{
   fCanvas->SetCanvasSize(ww, wh);
}

void QRootCanvas::SetHighLightColor(Color_t col)
{
   fCanvas->SetHighLightColor(col);
}

void QRootCanvas::SetSelected(TObject *obj)
{
   fCanvas->SetSelected(obj);
}

void QRootCanvas::SetSelectedPad(TPad *pad)
{
   fCanvas->SetSelectedPad(pad);
}

void QRootCanvas::Show()
{
   fCanvas->Show();
}

void QRootCanvas::Size(Float_t xsizeuser, Float_t ysizeuser)
{
   fCanvas->Size(xsizeuser, ysizeuser);
}

void QRootCanvas::SetBatch(Bool_t batch)
{
   fCanvas->SetBatch(batch);
}

void QRootCanvas::SetRetained(Bool_t retained)
{
   fCanvas->SetRetained(retained);
}

void QRootCanvas::SetTitle(const char *title)
{
   fCanvas->SetTitle(title);
}

void QRootCanvas::ToggleEventStatus()
{
   fCanvas->ToggleEventStatus();
}

void QRootCanvas::ToggleAutoExec()
{
   fCanvas->ToggleAutoExec();
}

void QRootCanvas::Update()
{
   // do not call update directly, use timer instead
   activateRepaint(act_Update);
}

void  QRootCanvas::closeEvent(QCloseEvent * e)
{
    if (fCanvas) {
       delete fCanvas;
       fCanvas = nullptr;
    }

    e->accept();
}

void QRootCanvas::methodDialog(TObject *object, TMethod* method)
{
   if (!object || !method) return;

   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

  // Create dialog object with OK and Cancel buttons. This dialog
   // prompts for the arguments of "method".

   QRootDialog dlg;

   dlg.setWindowTitle(TString::Format("%s:%s", object->GetName(), method->GetName()).Data());

   // iterate through all arguments and create appropriate input-data objects:
   // inputlines, option menus...
   TIter next(method->GetListOfMethodArgs());

   while (auto argument = (TMethodArg *) next()) {
      TString argTitle = TString::Format("(%s)  %s", argument->GetTitle(), argument->GetName());
      TString argDflt = argument->GetDefault() ? argument->GetDefault() : "";
      if (argDflt.Length() > 0)
         argTitle += TString::Format(" [default: %s]", argDflt.Data());
      TString type = argument->GetTypeName();
      TDataType *datatype   = gROOT->GetType(type);
      TString basictype;

      if (datatype) {
         basictype = datatype->GetTypeName();
      } else {
         if (type.CompareTo("enum") != 0)
            std::cout << "*** Warning in Dialog(): data type is not basic type, assuming (int)\n";
         basictype = "int";
      }

      if (TString(argument->GetTitle()).Index("*") != kNPOS) {
         basictype += "*";
         type = "char*";
      }

      TDataMember *m = argument->GetDataMember();
      if (m && m->GetterMethod()) {

         m->GetterMethod()->Init(object->IsA(), m->GetterMethod()->GetMethodName(), "");

         // Get the current value and form it as a text:

         TString val;

         if (basictype == "char*") {
            char *tdefval = nullptr;
            m->GetterMethod()->Execute(object, "", &tdefval);
            if (tdefval) val = tdefval;
         } else
         if ((basictype == "float") ||
             (basictype == "double")) {
            Double_t ddefval = 0.;
            m->GetterMethod()->Execute(object, "", ddefval);
            val = TString::Format("%g", ddefval);
         } else
         if ((basictype == "char") ||
             (basictype == "int")  ||
             (basictype == "long") ||
             (basictype == "short")) {
            Long_t ldefval = 0;
            m->GetterMethod()->Execute(object, "", ldefval);
            val = TString::Format("%ld", ldefval);
         }

         // Find out whether we have options ...

         TList *opt;
         if ((opt = m->GetOptions()) != nullptr) {
            // should stop dialog
            // workaround JAM: do not stop dialog, use textfield (for time display toggle)
            dlg.addArg(argTitle.Data(), val.Data(), type.Data());
            //return;
         } else {
            // we haven't got options - textfield ...
            dlg.addArg(argTitle.Data(), val.Data(), type.Data());
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

   TObjArray tobjlist(method->GetListOfMethodArgs()->LastIndex() + 1);
   for (int n = 0; n <= method->GetListOfMethodArgs()->LastIndex(); n++) {
      QString s = dlg.getArg(n);
      qDebug( "** QString values (first ) :%s \n", s.toLatin1().constData() );
      tobjlist.AddLast(new TObjString(s.toLatin1().constData()));
   }

   // handle command if existing object
   if(strcmp(method->GetName(),"Delete") == 0) {
      // here call explicitly the dtor
      qDebug(" DIAL obj name deleted :%s \n", object->GetName());
      emit MenuCommandExecuted(object, "Delete");
      delete object;
      object = nullptr;
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
        deletion = true;
        object = nullptr;
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
      gROOT->SetSelectedPad(gPad);
      gROOT->GetSelectedPad()->Update();
   }
}

QAction* QRootCanvas::addMenuAction(QMenu* menu, QSignalMapper* map, const QString& text, int id)
{
   bool enabled = QRootApplication::IsRootCanvasMenuEnabled();

   QAction* act = new QAction(text, menu);

   if (!enabled)
      if ((text.compare("DrawClone") == 0) || (text.compare("DrawClass") == 0) || (text.compare("Inspect") == 0) ||
          (text.compare("SetShowProjectionX") == 0) || (text.compare("SetShowProjectionY") == 0) ||
          (text.compare("DrawPanel") == 0) || (text.compare("FitPanel") == 0))
         act->setEnabled(false);

   QObject::connect(act, &QAction::triggered, [id, map]() {
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
      map->mapped(id);
#else
      map->mappedInt(id);
#endif
   });

   menu->addAction(act);
   map->setMapping(act, id);

   return act;
}

void QRootCanvas::executeMenu(int id)
{
   TGo4LockGuard threadlock;
   (void) threadlock; // suppress compiler warnings

   QString text;
   bool ok = false;
   if (id >= 100) {
      switch (id){
         case 100: {
            TLatex *fxLatex = new TLatex();
            text = QInputDialog::getText(this, tr( "Qt Root" ),
                                         tr( "Please enter your text" ),
                                         QLineEdit::Normal, QString(), &ok);
            //if (ok && !text.isEmpty())
            fxLatex->DrawLatex(fMousePosX, fMousePosY, text.toLatin1().constData());
            emit MenuCommandExecuted(fxLatex, "DrawLatex");
            break;
        }
        case 101: {
           TH1 *h1 = dynamic_cast<TH1 *> (fMenuObj);
           if (h1) {
              QColor col = QColorDialog::getColor();
              if (col.isValid()) {
                 short int C_new =  TColor::GetColor(col.red(), col.green(), col.blue());
                 h1->SetLineColor(C_new);
                 emit MenuCommandExecuted(h1, "SetLineColor");
              }
            }
           break;
        }
        case 102: {
           TH1 *h1 = dynamic_cast<TH1 *> (fMenuObj);
           if (h1) {
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
   if (id >= 0) {

      // save global to Pad before calling TObject::Execute()

      TVirtualPad *psave = gROOT->GetSelectedPad();
      TMethod *method = (TMethod *) fMenuMethods->At(id);

      /// test: do this in any case!
      fCanvas->HandleInput(kButton3Up, gPad->XtoAbsPixel(fMousePosX), gPad->YtoAbsPixel(fMousePosY));

      // change current dir that all new histograms appear here
      gROOT->cd();

      if (method->GetListOfMethodArgs()->First()) {
        if (strstr(method->GetName(), "Delete")) {
          // JAM2016: do not allow mouse menu delete in Go4
        } else {
          methodDialog(fMenuObj, method);
        }
      }
    else
      {
         gROOT->SetFromPopUp(kTRUE);
         fMenuObj->Execute(method->GetName(), "");

         if (fMenuObj->TestBit(TObject::kNotDeleted)) {
            emit MenuCommandExecuted(fMenuObj, method->GetName());
         } else {
            fMenuObj = nullptr;
         }

      }

      #ifndef __NOGO4GED__
         TGedEditor* ed = dynamic_cast<TGedEditor*>(TVirtualPadEditor::GetPadEditor(kFALSE));
         if (fMenuObj && ed) ed->SetModel(psave, fMenuObj, kButton1Down);
      #endif

      fCanvas->GetPadSave()->Update();
      fCanvas->GetPadSave()->Modified();

      gROOT->SetSelectedPad(psave);

      gROOT->GetSelectedPad()->Update();
      gROOT->GetSelectedPad()->Modified();

      fCanvas->Modified();
      fCanvas->ForceUpdate();
      gROOT->SetFromPopUp(kFALSE);
    }

   fMenuObj = nullptr;
   delete fMenuMethods;
   fMenuMethods = nullptr;
}

bool QRootCanvas::isEditorVisible()
{
   return fEditorFrame ? fEditorFrame->isVisible() : false;
}


bool QRootCanvas::isEditorAllowed()
{
#ifdef __NOGO4GED__
   return false;
#else
   return QRootApplication::IsRootCanvasMenuEnabled();
#endif
}

void QRootCanvas::toggleEditor()
{
   if (!fEditorFrame) return;

#ifndef __NOGO4GED__
   bool flag = !isEditorVisible();

   fEditorFrame->setVisible(flag);
   if (flag && !fxPeditor) {
      // JAM the following is pure empiric. hopefully default denominator won't change in future qt?
      double scalefactor = (double) metric(QPaintDevice::PdmDevicePixelRatioScaled)/65536.;
      fEditorFrame->setMinimumWidth( fEditorFrame->minimumWidth()/scalefactor);

// JAM 5-2019: this part was moved to buildEditorWindow() because of init problems with Qt 4
//      std::cout<< "QRootCanvas::toggleEditor() will create Rooteditor" <<std::endl;
//      fxRooteditor = new QRootWindow(fEditorFrame, "rootwrapperwindow");
//      QVBoxLayout* gedlayout = new QVBoxLayout(fEditorFrame);
//      gedlayout->setContentsMargins(0, 0, 0, 0);
//      gedlayout->addWidget(fxRooteditor);
//////////////////
      TGo4LockGuard threadlock;
      (void) threadlock; // suppress compiler warnings

      fxRooteditor->SetResizeOnPaint(kFALSE); // disable internal resize on paintEvent, we use ResizeGedEditor
      fxRooteditor->SetEditable(); // mainframe will adopt pad editor window

      fxPeditor = TVirtualPadEditor::LoadEditor();
      fxPeditor->SetGlobal(kFALSE);
      fxRooteditor->SetEditable(kFALSE); // back to window manager as root window
   }
#endif
}

void QRootCanvas::resizeEditor()
{
#ifndef __NOGO4GED__
   TGedEditor* ed = dynamic_cast<TGedEditor*>(fxPeditor);
   if (ed && isEditorVisible() && fxRooteditor)
      ed->Resize(fxRooteditor->ScaledWidth(), fxRooteditor->ScaledHeight());
#endif
}


void QRootCanvas::activateEditor(TPad *pad, TObject *obj)
{
#ifndef __NOGO4GED__
   TGedEditor* ed = dynamic_cast<TGedEditor*>(fxPeditor);
   if (ed && obj && isEditorVisible()) {
      gTQSender = getCanvas();
      ed->SetModel(pad, obj, kButton1Down);
   }
#endif
}

void QRootCanvas::cleanupEditor()
{
#ifndef __NOGO4GED__
   TGedEditor* ed = dynamic_cast<TGedEditor*>(fxPeditor);
   if (!ed) return;
   if (!fDummyHisto) {
      fDummyHisto = new TH1I("dummyhisto", "dummyhisto", 100, -10., 10.);
      fDummyHisto->FillRandom("gaus", 1000);
      fDummyHisto->SetDirectory(nullptr);
      fDummyHisto->SetBit(kCanDelete, kFALSE);
   }
   gTQSender = getCanvas();
   ed->SetModel(nullptr, fDummyHisto, kButton1Down);
   ed->SetModel(nullptr, getCanvas(), kButton1Down);
#endif
}


void QRootCanvas::showStatusMessage(const char *msg)
{
   if (fStatusBar) fStatusBar->showMessage(msg);
}

bool QRootCanvas::isStatusBarVisible()
{
   return fStatusBar ? fStatusBar->isVisible() : false;
}

void QRootCanvas::setStatusBarVisible(bool flag)
{
   if (!fStatusBar) return;

   setShowEventStatus(flag);
   fStatusBar->setVisible(flag);
}


// JAM2019 test order of creation for ged parent window in qt4:
void QRootCanvas::buildEditorWindow()
{
   if (!fxRooteditor) {
      fxRooteditor = new QRootWindow(fEditorFrame, "rootwrapperwindow");
      QVBoxLayout* gedlayout = new QVBoxLayout(fEditorFrame);
      gedlayout->setContentsMargins(0, 0, 0, 0);
      gedlayout->addWidget(fxRooteditor);
   }
}
