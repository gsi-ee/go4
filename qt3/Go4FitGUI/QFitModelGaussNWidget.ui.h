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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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
   AxisTable->setNumRows(num);
   for(int n=0;n<num;n++)
     AxisTable->setText(n,0,QString::number(GetGaussN()->GetAxisForIndex(n)));
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
      int value = AxisTable->text(nrow,ncol).toInt(&ok);
      if(ok) {
          GetGaussN()->SetAxisForIndex(nrow,value);
          if (GetGaussN()->ResortIndexes(nrow)) SetAxisTable();
      }
   }
}
