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
   setObjectName( "QWebCanvas");

   setSizeIncrement( QSize( 100, 100 ) );

   setUpdatesEnabled( true );
   setMouseTracking(true);

   setFocusPolicy( Qt::TabFocus );
   setCursor( Qt::CrossCursor );

   setAcceptDrops(true);

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

   TString url = web->CreateWebWindow(1); // create TWebWindow, which will handle all necessary connections

   TString fullurl = UrlSchemeHandler::installHandler(url, web->GetServer(), false);

   fView->load(QUrl(fullurl.Data()));
}

QWebCanvas::~QWebCanvas()
{
}

void QWebCanvas::resizeEvent(QResizeEvent *event)
{
   printf("Resize width: %d %d height: %d %d\n", (int) width(), (int) fView->width(), (int) height(), (int) fView->height());
}

void QWebCanvas::dropEvent(QDropEvent* event)
{
   emit CanvasDropEvent(event, fCanvas);
}

void QWebCanvas::dropView(QDropEvent* event)
{
   emit CanvasDropEvent(event, fCanvas);
}

