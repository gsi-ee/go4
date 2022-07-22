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

#include "QWebCanvas.h"

#include "TCanvas.h"
#include "TROOT.h"
#include "TClass.h"
#include "RVersion.h"
#include "THttpServer.h"

#include "TGo4Log.h"

#include <QGridLayout>
#include <QApplication>
#include <QTimer>
#include <QDropEvent>

#include <cstdlib>
#include <cstdio>

// starting from version 6.23 TWebCanvas includes all required functionality
#include "TWebCanvas.h"

QWebCanvas::QWebCanvas(QWidget *parent) : QWidget(parent)
{
   // JAM the following is pure empiric. hopefully default denominator won't change in future qt?
   fQtScalingfactor = (double) metric(QPaintDevice::PdmDevicePixelRatioScaled)/65536.;

   setObjectName( "QWebCanvas");

   setSizeIncrement( QSize( 100, 100 ) );

   setUpdatesEnabled( true );
   setMouseTracking(true);

   setFocusPolicy( Qt::TabFocus );
   setCursor( Qt::CrossCursor );

   setAcceptDrops(true);

   fRepaintTimer = new QTimer;
   fRepaintTimer->setSingleShot(true);
   QObject::connect(fRepaintTimer, &QTimer::timeout, this, &QWebCanvas::processRepaintTimer);

   // disable option that at least background is redrawn immediately
   // and canvas content after 100 ms timeout
   //setAttribute(Qt::WA_NoSystemBackground);
   //setAttribute(Qt::WA_PaintOnScreen);
   //setAttribute(Qt::WA_PaintUnclipped);

   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(10);
   gridLayout->setContentsMargins(1,1,1,1);

   //fView = new RootWebView(this);
   //QObject::connect(fView, SIGNAL(drop(QDropEvent*)), this, SLOT(dropView(QDropEvent*)));
   //gridLayout->addWidget(fView);

   //static TQt5Timer *qt5timer = nullptr;

   //if (!qt5timer) {
   //   qt5timer = new TQt5Timer(10, kTRUE);
   //   qt5timer->TurnOn();
   //}

   static int wincnt = 1;

   fCanvas = new TCanvas(kFALSE);
   fCanvas->SetName(TString::Format("Canvas%d", wincnt++).Data());
   fCanvas->SetTitle("title");
   fCanvas->ResetBit(TCanvas::kShowEditor);
   fCanvas->SetCanvas(fCanvas);
   fCanvas->SetBatch(kTRUE); // mark canvas as batch

   gPad = fCanvas;

   TWebCanvas *web = new TWebCanvas(fCanvas, "title", 0, 0, 800, 600, kFALSE);

   // this is Go4-special part to provide support of custom classes
   static std::string go4script;

   if (go4script.empty()) {
      TString fname = TGo4Log::subGO4SYS("html/go4canvas.js");
      go4script = THttpServer::ReadFileContent(fname.Data());
   }

   web->SetCustomScripts(go4script);
   web->AddCustomClass("TGo4Marker");
   web->AddCustomClass("TGo4Condition", true);
   web->AddCustomClass("TGo4CondArray");
   // this is end of Go4-special part

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,26,0)
   web->SetAsyncMode(kTRUE); // avoid blocking mode
#endif

   fCanvas->SetCanvasImp(web);

   SetPrivateCanvasFields(true);

   web->SetCanCreateObjects(kFALSE); // not yet create objects on server side

   web->SetUpdatedHandler([this]() { ProcessCanvasUpdated(); });

   web->SetActivePadChangedHandler([this](TPad *pad){ ProcessActivePadChanged(pad); });

   web->SetPadClickedHandler([this](TPad *pad, int x, int y) { ProcessPadClicked(pad, x, y); });

   web->SetPadDblClickedHandler([this](TPad *pad, int x, int y) { ProcessPadDblClicked(pad, x, y); });

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
   ROOT::Experimental::RWebDisplayArgs args("qt6");
#else
   ROOT::Experimental::RWebDisplayArgs args("qt5");
#endif
   args.SetDriverData(this); // it is parent widget for created QWebEngineView element
   args.SetUrlOpt("noopenui");

   web->ShowWebWindow(args);

   fView = findChild<QWebEngineView*>("RootWebView");
   if (!fView) {
      printf("FAIL TO FIND QWebEngineView - ROOT Qt5Web plugin does not work properly !!!!!\n");
      exit(11);
   }

   gridLayout->addWidget(fView);

   QObject::connect(fView, SIGNAL(drop(QDropEvent*)), this, SLOT(dropView(QDropEvent*)));

   fCanvas->SetCanvasSize(fView->width(), fView->height());
}

QWebCanvas::~QWebCanvas()
{
   if (fCanvas) {
      SetPrivateCanvasFields(false);

      gROOT->GetListOfCanvases()->Remove(fCanvas);

      fCanvas->Close();
      delete fCanvas;
      fCanvas = nullptr;
   }
}

void QWebCanvas::SetPrivateCanvasFields(bool on_init)
{
   Long_t offset = TCanvas::Class()->GetDataMemberOffset("fCanvasID");
   if (offset > 0) {
      Int_t *id = (Int_t *)((char*) fCanvas + offset);
      if (*id == fCanvas->GetCanvasID()) *id = on_init ? 111222333 : -1;
   } else {
      printf("ERROR: Cannot modify TCanvas::fCanvasID data member\n");
   }

   offset = TCanvas::Class()->GetDataMemberOffset("fPixmapID");
   if (offset > 0) {
      Int_t *id = (Int_t *)((char*) fCanvas + offset);
      if (*id == fCanvas->GetPixmapID()) *id = on_init ? 332211 : -1;
   } else {
      printf("ERROR: Cannot modify TCanvas::fPixmapID data member\n");
   }

   offset = TCanvas::Class()->GetDataMemberOffset("fMother");
   if (offset > 0) {
      TPad **moth = (TPad **)((char*) fCanvas + offset);
      if (*moth == fCanvas->GetMother()) *moth = on_init ? fCanvas : nullptr;
   } else {
      printf("ERROR: Cannot set TCanvas::fMother data member\n");
   }

}


void QWebCanvas::resizeEvent(QResizeEvent *event)
{
   // printf("Resize width: %d %d height: %d %d\n", (int) width(), (int) fView->width(), (int) height(), (int) fView->height());

   fCanvas->SetCanvasSize(fView->width(), fView->height());
}

void QWebCanvas::dropEvent(QDropEvent* event)
{
   // TODO: remove, not needed at all

   TObject* obj(0);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   QPoint pos = event->pos();
#else
   QPoint pos = event->position().toPoint();
#endif
   TPad* pad = fCanvas->Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   printf("Drop on pad %s\n", pad ? pad->GetName() : "---");

   emit CanvasDropEvent(event, pad ? pad : fCanvas);
}

void QWebCanvas::dropView(QDropEvent* event)
{
   TObject* obj(0);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   QPoint pos = event->pos();
#else
   QPoint pos = event->position().toPoint();
#endif
   TPad* pad = fCanvas->Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   printf("Drop on view %s\n", pad ? pad->GetName() : "---");

   emit CanvasDropEvent(event, pad ? pad : fCanvas);
}

void QWebCanvas::activateEditor(TPad *pad, TObject *obj)
{
   TWebCanvas *cimp = dynamic_cast<TWebCanvas*> (fCanvas->GetCanvasImp());
   if (cimp) {
      cimp->ShowEditor(kTRUE);
      cimp->ActivateInEditor(pad, obj);
   }
}

bool QWebCanvas::isStatusBarVisible()
{
   return fCanvas->GetShowEventStatus();
}

bool QWebCanvas::isEditorVisible()
{
   return fCanvas->GetShowEditor();
}

void QWebCanvas::setStatusBarVisible(bool flag)
{
   if (fCanvas->GetShowEventStatus() != flag)
      fCanvas->ToggleEventStatus();
}

void QWebCanvas::setEditorVisible(bool flag)
{
   TCanvasImp *cimp = fCanvas->GetCanvasImp();
   if (cimp) cimp->ShowEditor(flag);
}

void QWebCanvas::activateStatusLine()
{
   TCanvasImp *cimp = fCanvas->GetCanvasImp();
   if (cimp) cimp->ShowStatusBar(kTRUE);
}

void QWebCanvas::processRepaintTimer()
{
   fCanvas->Update();
}

void QWebCanvas::Modified()
{
   fCanvas->Modified();
}

void QWebCanvas::Update()
{
   fRepaintTimer->setSingleShot(true);
   fRepaintTimer->start(100);
}
