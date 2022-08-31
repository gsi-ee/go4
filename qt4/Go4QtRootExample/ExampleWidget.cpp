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

#include "ExampleWidget.h"

#include "QRootCanvas.h"
#include "TCanvas.h"
#include "TH1.h"

#include <QMessageBox>

ExampleWidget::ExampleWidget(QWidget *parent, const char *name) :
   QWidget(parent)
{
   setupUi(this);

   // TODO: viewpanel does not closed when press close button
   setAttribute(Qt::WA_DeleteOnClose);

   setObjectName(name);

   fxQCanvas->setObjectName("example");
   fxQCanvas->getCanvas()->SetName("example");
   fxQCanvas->setEditorFrame(EditorFrame);
   fxQCanvas->buildEditorWindow();

   fHisto = new TH1F("h1","title", 100, -5, 5);
   fHisto->FillRandom("gaus", 10000);
   fHisto->SetDirectory(nullptr);

   fxQCanvas->getCanvas()->cd();
   fHisto->Draw("colz");
}

ExampleWidget::~ExampleWidget()
{
}


void ExampleWidget::InfoButton_clicked()
{
   QMessageBox::information(this,"QtRoot standalone example","Demo how QRootCanvas can be inserted into the QWidget");
}

void ExampleWidget::EditorButton_clicked()
{
   fxQCanvas->toggleEditor();

   // actiavte in ged some object
   if (fxQCanvas->isEditorVisible()) {
      fxQCanvas->activateEditor(fxQCanvas->getCanvas(), fHisto);
      fxQCanvas->resizeEditor();
   }
}

void ExampleWidget::resizeEvent(QResizeEvent * e)
{
   fxQCanvas->resizeEditor();
}

void ExampleWidget::ExitButton_clicked()
{
   // when widget closed, application automatically exit
   close();
}


