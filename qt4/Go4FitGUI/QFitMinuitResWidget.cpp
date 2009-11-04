// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "QFitMinuitResWidget.h"

#include "TGo4FitMinuitResult.h"

QFitMinuitResWidget::QFitMinuitResWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitMinuitResult * QFitMinuitResWidget::GetResult() {
   return dynamic_cast<TGo4FitMinuitResult*> (GetObject());
}

void QFitMinuitResWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   if (GetResult()) InfoLabel->setText("this is minuit result object");
}
