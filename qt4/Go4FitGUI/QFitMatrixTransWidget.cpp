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

#include "QFitMatrixTransWidget.h"

#include "TGo4FitMatrixTrans.h"

QFitMatrixTransWidget::QFitMatrixTransWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitMatrixTrans* QFitMatrixTransWidget::GetMatrixTrans() {
  return dynamic_cast<TGo4FitMatrixTrans*> (GetObject());
}

void QFitMatrixTransWidget::FillSpecificData() {
  QFitNamedWidget::FillSpecificData();
  TGo4FitMatrixTrans* trans = GetMatrixTrans();
  if (trans)
    NumAxisSpin->setValue(trans->GetNumAxis());
}

void QFitMatrixTransWidget::NumAxisSpin_valueChanged( int num ){
   if (!fbFillWidget && GetMatrixTrans()) {
     GetMatrixTrans()->SetNumAxis(num);
     UpdateWidgetItem(true);
   }
}
