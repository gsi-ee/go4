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

// JAM2016: activate this for global ROOT "escape mode" before redraw (NOT RECOMMENDED)
// otherwise, escape mode will reset global arrays of TGraph painter class only
//#define GLOBALESCAPE 1

#include "ExampleWidget.h"

#include "QRootCanvas.h"

ExampleWidget::ExampleWidget(QWidget *parent, const char* name) :
   QWidget(parent)
{
   setupUi(this);

   // TODO: viewpanel does not closed when press close button
   setAttribute(Qt::WA_DeleteOnClose);

   setObjectName(name);
   setWindowTitle("Example");

   QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
   sizePolicy3.setHorizontalStretch(0);
   sizePolicy3.setVerticalStretch(20);

   fxQCanvas = new QRootCanvas(this);
   // fxQCanvas->setObjectName(QStringLiteral("fxQCanvas"));
   fxQCanvas->setMinimumSize(QSize(50, 50));
   sizePolicy3.setHeightForWidth(fxQCanvas->sizePolicy().hasHeightForWidth());
   fxQCanvas->setSizePolicy(sizePolicy3);

   // fxGridLayout->addWidget(fxQCanvas, 1, 1, 1, 1);

   fxQCanvas->setObjectName("example");
   fxQCanvas->getCanvas()->SetName("example");
   // fxQCanvas->setEditorFrame(EditorFrame);

}

ExampleWidget::~ExampleWidget()
{
}
