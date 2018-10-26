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
#include "THttpServer.h"

#include "rooturlschemehandler.h"
#include <QWebEngineProfile>
#include <QGridLayout>

#include <stdio.h>
#include <stdlib.h>

QWebCanvas::QWebCanvas(QWidget *parent) : QWidget(parent)
{
   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(1);
   gridLayout->setMargin(1);

   fView = new RootWebView(this);

   gridLayout->addWidget(fView);

   static bool guifactory = false;
   static int wincnt = 1;
   static int nhandler = 0;
   static THttpServer *last_serv = 0;

   if (!guifactory) {
      guifactory = true;
      gGuiFactory = gBatchGuiFactory = new TWebGuiFactory();
   }

   fCanvas = new TCanvas(Form("Canvas%d", wincnt), 100, 200, wincnt);
   wincnt++;

   // not call Show, but try to extract URL for the
   TWebCanvas *web = dynamic_cast<TWebCanvas *>(fCanvas->GetCanvasImp());
   if (!web) {
      printf("Something went wrong - no web canvas provided\n");
      exit(3);
   }

   TString url = web->CreateWebWindow(1); // create TWebWindow, which will handle all necessary connections
   THttpServer *serv = web->GetServer(); // get http server instance

   char protocol[100], fullurl[2000];
   bool create_handler = false;

   if (last_serv != serv) {
      last_serv = serv;
      create_handler = true;
      nhandler++;
   }

   const char *suffix = url.Index("?") != kNPOS ? "&qt5" : "?qt5";

   snprintf(protocol, sizeof(protocol), "roothandler%d", nhandler);
   snprintf(fullurl, sizeof(fullurl), "%s://dummy:8080%s%s", protocol, url.Data(), suffix);

   printf("Start %s\n", fullurl);

   if (create_handler) {
      const QByteArray protocol_name = QByteArray(protocol);
      UrlSchemeHandler *handler = new UrlSchemeHandler(Q_NULLPTR, serv);
      QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(protocol_name, handler);
   }

   fView->load(QUrl(fullurl));
}

QWebCanvas::~QWebCanvas()
{
}
