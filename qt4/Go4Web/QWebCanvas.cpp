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
#include "ROOT/RWebWindowsManager.hxx"

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

   setObjectName("QWebCanvas");

   setSizeIncrement(QSize(100, 100));

   setUpdatesEnabled(true);
   setMouseTracking(true);

   setFocusPolicy(Qt::TabFocus);
   setCursor(Qt::CrossCursor);

   setAcceptDrops(true);

   fRepaintTimer = new QTimer;
   fRepaintTimer->setSingleShot(true);
   QObject::connect(fRepaintTimer, &QTimer::timeout, this, &QWebCanvas::processRepaintTimer);

   // if ROOT compiled with X11 - batch mode is default, disable it before create web widget
   if (gROOT->IsBatch())
      gROOT->SetBatch(kFALSE);

   // disable option that at least background is redrawn immediately
   // and canvas content after 100 ms timeout
   //setAttribute(Qt::WA_NoSystemBackground);
   //setAttribute(Qt::WA_PaintOnScreen);
   //setAttribute(Qt::WA_PaintUnclipped);

   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(10);
   gridLayout->setContentsMargins(1,1,1,1);

   static int wincnt = 1;

   fCanvas = new TCanvas(kFALSE);
   fCanvas->SetName(TString::Format("Canvas%d", wincnt++).Data());
   fCanvas->SetTitle("title");
   fCanvas->ResetBit(TCanvas::kShowEditor);
   fCanvas->SetCanvas(fCanvas);
   fCanvas->SetBatch(kTRUE); // mark canvas as batch

   gPad = fCanvas;

   auto web = new TWebCanvas(fCanvas, "title", 0, 0, 800, 600, kFALSE);


   #if ROOT_VERSION_CODE >= ROOT_VERSION(6,33,0) && QT_VERSION >= QT_VERSION_CHECK(6,0,0)
   // this is usage of new JS modules functionality, works only with qt6
   ROOT::RWebWindowsManager::AddServerLocation("go4sys", TGo4Log::GO4SYS());
   static std::string go4script = "modules:go4sys/html5/go4canvas.mjs";
   #else
   // old method to load plain JS scripts as is
   static std::string go4script = THttpServer::ReadFileContent(TGo4Log::subGO4SYS("html/go4canvas.js").Data());
   #endif

   // these methods are static in newer ROOT starting from 6.33
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
   const char *kind = "qt6";
#else
   const char *kind = "qt5";
#endif

#if ROOT_VERSION_CODE > ROOT_VERSION(6,29,0)
   ROOT::RWebDisplayArgs args(kind);
#else
   ROOT::Experimental::RWebDisplayArgs args(kind);
#endif

   args.SetDriverData(this); // it is parent widget for created QWebEngineView element
   args.SetUrlOpt("noopenui");

   web->ShowWebWindow(args);

   fView = findChild<QWebEngineView *>("RootWebView");
   if (!fView) {
      printf("FAIL TO FIND QWebEngineView - ROOT %sweb plugin does not work properly !!!!!\n", kind);
      exit(11);
   }

   gridLayout->addWidget(fView);

   // keep symbolic connect while rootwebview class does not exported
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
      Int_t *id = (Int_t *)((char *) fCanvas + offset);
      if (*id == fCanvas->GetCanvasID()) *id = on_init ? 111222333 : -1;
   } else {
      printf("ERROR: Cannot modify TCanvas::fCanvasID data member\n");
   }

   offset = TCanvas::Class()->GetDataMemberOffset("fPixmapID");
   if (offset > 0) {
      Int_t *id = (Int_t *)((char *) fCanvas + offset);
      if (*id == fCanvas->GetPixmapID()) *id = on_init ? 332211 : -1;
   } else {
      printf("ERROR: Cannot modify TCanvas::fPixmapID data member\n");
   }

   offset = TCanvas::Class()->GetDataMemberOffset("fMother");
   if (offset > 0) {
      TPad **moth = (TPad **)((char *) fCanvas + offset);
      if (*moth == fCanvas->GetMother()) *moth = on_init ? fCanvas : nullptr;
   } else {
      printf("ERROR: Cannot set TCanvas::fMother data member\n");
   }

}

void QWebCanvas::resizeEvent(QResizeEvent *event)
{
   fCanvas->SetCanvasSize(fView->width(), fView->height());
}

void QWebCanvas::dropEvent(QDropEvent* event)
{
   // TODO: remove, not needed at all

   TObject *obj = nullptr;
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   QPoint pos = event->pos();
#else
   QPoint pos = event->position().toPoint();
#endif
   auto pad = fCanvas->Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   emit CanvasDropEvent(event, pad ? pad : fCanvas);
}

void QWebCanvas::dropView(QDropEvent* event)
{
   TObject *obj = nullptr;
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   QPoint pos = event->pos();
#else
   QPoint pos = event->position().toPoint();
#endif
   auto pad = fCanvas->Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   emit CanvasDropEvent(event, pad ? pad : fCanvas);
}

void QWebCanvas::activateEditor(TPad *pad, TObject *obj)
{
   auto cimp = dynamic_cast<TWebCanvas *> (fCanvas->GetCanvasImp());
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
   auto cimp = fCanvas->GetCanvasImp();
   if (cimp) cimp->ShowEditor(flag);
}

void QWebCanvas::activateStatusLine()
{
    auto cimp = fCanvas->GetCanvasImp();
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
