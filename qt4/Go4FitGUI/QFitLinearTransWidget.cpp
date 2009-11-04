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

#include "QFitLinearTransWidget.h"
#include "TGo4FitLinearTrans.h"

QFitLinearTransWidget::QFitLinearTransWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitLinearTrans* QFitLinearTransWidget::GetLinearTrans() {
  return dynamic_cast<TGo4FitLinearTrans*> (GetObject());
}

void QFitLinearTransWidget::FillSpecificData() {
  QFitNamedWidget::FillSpecificData();
  TGo4FitLinearTrans* trans = GetLinearTrans();
  if (trans)
    NumAxisSpin->setValue(trans->GetAxis());

}

void QFitLinearTransWidget::NumAxisSpin_valueChanged( int axis)
{
   if (!fbFillWidget && GetLinearTrans())
     GetLinearTrans()->SetAxis(axis);
}
