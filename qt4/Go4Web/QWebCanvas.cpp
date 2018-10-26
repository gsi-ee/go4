// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "QWebCanvas.h"

#include "TCanvas.h"
#include "TWebCanvas.h"
#include "THttpServer.h"
#include "TTimer.h"
#include "TROOT.h"

#include "rooturlschemehandler.h"
#include <QGridLayout>
#include <QApplication>
#include <QTimer>
#include <QDropEvent>

#include <stdlib.h>


QWebCanvas::QWebCanvas(QWidget *parent) : QWidget(parent)
{
   fQtScalingfactor = 1.0;

#if QT_VERSION > QT_VERSION_CHECK(5,6,0)
   // JAM the following is pure empiric. hopefully default denominator won't change in future qt?
   fQtScalingfactor = (double) metric(QPaintDevice::PdmDevicePixelRatioScaled)/65536.;
#endif

   setObjectName( "QWebCanvas");

   setSizeIncrement( QSize( 100, 100 ) );

   setUpdatesEnabled( true );
   setMouseTracking(true);

   setFocusPolicy( Qt::TabFocus );
   setCursor( Qt::CrossCursor );

   setAcceptDrops(true);

   fRepaintTimer = new QTimer;
   fRepaintTimer->setSingleShot(true);
   connect(fRepaintTimer, SIGNAL(timeout()), this, SLOT(processRepaintTimer()));

   // disable option that at least background is redrawn immediately
   // and canvas content after 100 ms timeout
   //setAttribute(Qt::WA_NoSystemBackground);
   //setAttribute(Qt::WA_PaintOnScreen);
   //setAttribute(Qt::WA_PaintUnclipped);

   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(10);
   gridLayout->setMargin(1);

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
   fCanvas->SetName(Form("Canvas%d", wincnt++));
   fCanvas->SetTitle("title");
   fCanvas->ResetBit(TCanvas::kShowEditor);
   fCanvas->SetCanvas(fCanvas);
   fCanvas->SetBatch(kTRUE); // mark canvas as batch

   gPad = fCanvas;

   TWebCanvas *web = new TWebCanvas(fCanvas, "title", 0, 0, 800, 600);

   fCanvas->SetCanvasImp(web);

   web->SetUpdatedHandler([this]() { ProcessCanvasUpdated(); });

   web->SetActivePadChangedHandler([this](TPad *pad){ ProcessActivePadChanged(pad); });

   web->SetPadClickedHandler([this](TPad *pad, int x, int y) { ProcessPadClicked(pad, x, y); });

   web->SetPadDblClickedHandler([this](TPad *pad, int x, int y) { ProcessPadDblClicked(pad, x, y); });

   web->CreateWebWindow(1); // create TWebWindow, which will handle all necessary connections

   TString where;
   where.Form("qt5 qprnt:%llu layout:%llu url:&noopenui", (long long unsigned) this, (long long unsigned) gridLayout);

   web->ShowWebWindow(where.Data());

   fView = findChild<QWebEngineView*>("RootWebView");
   if (!fView) {
      printf("FAIL TO FIND VIEW!!!!!\n");
      exit(11);
   }

   gridLayout->addWidget(fView);

   QObject::connect(fView, SIGNAL(drop(QDropEvent*)), this, SLOT(dropView(QDropEvent*)));

   fCanvas->SetCanvasSize(fView->width(), fView->height());

   // QString fullurl = UrlSchemeHandler::installHandler(QString(url.Data()), web->GetServer());

   // disable openui completely from canvas - use only graphics
   // fullurl.append("&noopenui");

   // fView->load(QUrl(fullurl));

   // fCanvas->SetCanvasSize(fView->width(), fView->height());
}

QWebCanvas::~QWebCanvas()
{
}

void QWebCanvas::resizeEvent(QResizeEvent *event)
{
   printf("Resize width: %d %d height: %d %d\n", (int) width(), (int) fView->width(), (int) height(), (int) fView->height());

   fCanvas->SetCanvasSize(fView->width(), fView->height());
}

void QWebCanvas::dropEvent(QDropEvent* event)
{
   // TODO: remove, not needed at all

   TObject* obj(0);
   QPoint pos = event->pos();
   TPad* pad = fCanvas->Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   printf("Drop on pad %s\n", pad ? pad->GetName() : "---");

   emit CanvasDropEvent(event, pad ? pad : fCanvas);
}

void QWebCanvas::dropView(QDropEvent* event)
{
   TObject* obj(0);
   QPoint pos = event->pos();
   TPad* pad = fCanvas->Pick(scaledPosition(pos.x()), scaledPosition(pos.y()), obj);

   printf("Drop on view %s\n", pad ? pad->GetName() : "---");

   emit CanvasDropEvent(event, pad ? pad : fCanvas);
}

void QWebCanvas::actiavteEditor(TPad *pad, TObject *obj)
{
   TWebCanvas *cimp = dynamic_cast<TWebCanvas*> (fCanvas->GetCanvasImp());
   if (cimp) {
      printf("QWebCanvas:: Activate editor\n");
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
