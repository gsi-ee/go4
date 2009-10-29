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

#include "QFitModelGaussNWidget.h"

#include "TGo4FitModelGaussN.h"
#include "TGo4FitGuiTypes.h"

QFitModelGaussNWidget::QFitModelGaussNWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{
  setupUi(this);
}

TGo4FitModelGaussN * QFitModelGaussNWidget::GetGaussN()
{
   return dynamic_cast<TGo4FitModelGaussN*> (GetObject());
}

void QFitModelGaussNWidget::FillSpecificData()
{
    QFitNamedWidget::FillSpecificData();
    if (GetGaussN()) {
       AxisNumberSpin->setValue(GetGaussN()->GetAxisNumbers());
       SetAxisTable();
    }
}

void QFitModelGaussNWidget::SetAxisTable()
{
   if (GetGaussN()==0) return;
   int num = GetGaussN()->GetAxisNumbers();
   AxisTable->setRowCount(num);
   for(int n=0;n<num;n++)
     AxisTable->setItem(n,0,
      new QTableWidgetItem(QString::number(GetGaussN()->GetAxisForIndex(n))));
}

void QFitModelGaussNWidget::AxisNumberSpin_valueChanged( int num)
{
  if(!fbFillWidget && GetGaussN())
    if (GetGaussN()->SetAxisNumbers(num)) {
      SetAxisTable();
      UpdateItemsOfType(FitGui::ot_parslist, false);
    }
}

void QFitModelGaussNWidget::AxisTable_valueChanged( int nrow, int ncol)
{
   if(!fbFillWidget && GetGaussN()) {
      bool ok = FALSE;
      int value = AxisTable->item(nrow,ncol)->text().toInt(&ok);
      if(ok) {
          GetGaussN()->SetAxisForIndex(nrow,value);
          if (GetGaussN()->ResortIndexes(nrow)) SetAxisTable();
      }
   }
}
