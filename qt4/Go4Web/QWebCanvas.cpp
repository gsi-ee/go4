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
#include "TWebGuiFactory.h"
#include "TWebVirtualX.h"
#include "THttpServer.h"
#include "TTimer.h"

#include "rooturlschemehandler.h"
#include <QGridLayout>
#include <QApplication>
#include <QTimer>
#include <QDropEvent>

#include <stdio.h>
#include <stdlib.h>


class TQt5Timer : public TTimer {
public:
   TQt5Timer(Long_t milliSec, Bool_t mode) : TTimer(milliSec, mode)
   {
      // construtor
   }
   virtual ~TQt5Timer()
   {
      // destructor
   }
   virtual void Timeout()
   {
      // timeout handler
      // used to process http requests in main ROOT thread

      QApplication::sendPostedEvents();
      QApplication::processEvents();
   }
};


QWebCanvas::QWebCanvas(QWidget *parent) : QWidget(parent)
{
   fQtScalingfactor = 1.0;

#if QT_VERSION > QT_VERSION_CHECK(5,6,0)
   // JAM the following is pure empiric. hopefully default denominator won't change in future qt?
   fQtScalingfactor=(double) metric(QPaintDevice::PdmDevicePixelRatioScaled)/65536.;
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
   // setAttribute(Qt::WA_PaintOnScreen);
   // setAttribute(Qt::WA_PaintUnclipped);

   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(10);
   gridLayout->setMargin(1);

   fView = new RootWebView(this);

   QObject::connect(fView, SIGNAL(drop(QDropEvent*)), this, SLOT(dropView(QDropEvent*)));

   gridLayout->addWidget(fView);

   static bool guifactory = false;
   static int wincnt = 1;

   if (!guifactory) {
      guifactory = true;
      printf("Create WebGuiFactory\n");
      gGuiFactory = gBatchGuiFactory = new TWebGuiFactory();

      // enable Qt events processing inside ROOT event loop
      TQt5Timer *timer = new TQt5Timer(10, kTRUE);
      timer->TurnOn();
   }

   fCanvas = new TCanvas(Form("Canvas%d", wincnt++), 100, 200, TWebVirtualX::WebId);

   // not call Show, but try to extract URL for the
   TWebCanvas *web = dynamic_cast<TWebCanvas *>(fCanvas->GetCanvasImp());
   if (!web) {
      printf("Something went wrong - no web canvas provided\n");
      exit(3);
   }

   web->SetUpdatedHandler([this]() { ProcessCanvasUpdated(); });

   web->SetActivePadChangedHandler([this](TPad *pad){ ProcessActivePadChanged(pad); });

   web->SetPadClickedHandler([this](TPad *pad, int x, int y) { ProcessPadClicked(pad, x, y); });

   web->SetPadDblClickedHandler([this](TPad *pad, int x, int y) { ProcessPadDblClicked(pad, x, y); });

   TString url = web->CreateWebWindow(1); // create TWebWindow, which will handle all necessary connections

   TString fullurl = UrlSchemeHandler::installHandler(url, web->GetServer(), false);

   fView->load(QUrl(fullurl.Data()));

   fCanvas->SetCanvasSize(fView->width(), fView->height());
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

void QWebCanvas::setStatusBarVisible(bool flag)
{
   if (fCanvas->GetShowEventStatus() != flag)
      fCanvas->ToggleEventStatus();
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
