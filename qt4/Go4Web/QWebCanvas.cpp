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

   if (!guifactory) {
      guifactory = true;
      gGuiFactory = gBatchGuiFactory = new TWebGuiFactory();
   }

   fCanvas = new TCanvas(Form("Canvas%d", wincnt), 100, 200, wincnt);

   // not call show, but try to extract URL for the

   TWebCanvas *web = dynamic_cast<TWebCanvas *>(fCanvas->GetCanvasImp());
   if (!web) {
      printf("Something went wrong - no web canvas provided\n");
      exit(3);
   }

   TString url = web->CreateWebWindow(1); // create TWebWindow, which will handle all necessary connections

   THttpServer *serv = web->GetServer(); // get http server instance

   wincnt++;
}

QWebCanvas::~QWebCanvas()
{
}
