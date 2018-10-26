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

#include <QGridLayout>

QWebCanvas::QWebCanvas(QWidget *parent) : QWidget(parent)
{
   QGridLayout *gridLayout = new QGridLayout(this);
   gridLayout->setSpacing(1);
   gridLayout->setMargin(1);

   fView = new RootWebView(this);

   gridLayout->addWidget(fView);

   fCanvas = 0;
}

QWebCanvas::~QWebCanvas()
{
}
