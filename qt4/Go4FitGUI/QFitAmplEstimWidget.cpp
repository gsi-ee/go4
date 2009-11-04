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

#include "QFitAmplEstimWidget.h"
#include "TGo4FitAmplEstimation.h"

QFitAmplEstimWidget::QFitAmplEstimWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitAmplEstimation * QFitAmplEstimWidget::GetAmplEstim() {
   return dynamic_cast<TGo4FitAmplEstimation*> (GetObject());
}

void QFitAmplEstimWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   if (GetAmplEstim())
      Iterations->setValue(GetAmplEstim()->GetNumIterations());
}


void QFitAmplEstimWidget::Iterations_valueChanged( int num )
{
  if (!fbFillWidget && GetAmplEstim())
    GetAmplEstim()->SetNumIterations(num);
}
