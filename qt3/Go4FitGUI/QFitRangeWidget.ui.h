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

TGo4FitComponent* QFitRangeWidget::GetComp()
{
   return dynamic_cast<TGo4FitComponent*> (GetObject());
}

int QFitRangeWidget::GetNumRange()
{
   return GetItem()->Tag();
}

void QFitRangeWidget::FillSpecificData() {
   int num = GetNumRange();
   if ((num<0) || (GetComp()==0)) return;

   Int_t typ, naxis;
   Double_t left, right;
   GetComp()->GetRangeCondition(num,typ,naxis,left,right);

   RangeTypCmb->setCurrentItem(typ);
   AxisNumSpin->setValue(naxis);

   if (typ!=3) LeftEdt->setText(QString::number(left));
   if (typ!=2) RightEdt->setText(QString::number(right));

   EnableEditsForRangeType(typ);
}

void QFitRangeWidget::EnableEditsForRangeType( int num )
{
  LeftEdt->setEnabled((num==0) || (num==1) || (num==2));
  RightEdt->setEnabled((num==0) || (num==1) || (num==3));
}

void QFitRangeWidget::GetRangeData()
{
  if (fbFillWidget || (GetComp()==0)) return;

  int num = GetNumRange();
  if(num<0) return;

  Int_t typ = RangeTypCmb->currentItem();
  Int_t naxis = AxisNumSpin->value();

  Double_t left=0., right=0.;
  bool isok1 = TRUE, isok2 = TRUE;

  if (typ!=3) left = LeftEdt->text().toDouble(&isok1);
  if (typ!=2) right = RightEdt->text().toDouble(&isok2);

  if (isok1 && isok2) {
     GetComp()->SetRangeCondition(num,typ,naxis,left,right);
     SetWidgetItemText(true);
  }
}

void QFitRangeWidget::AxisNumSpin_valueChanged( int )
{
  GetRangeData();
}

void QFitRangeWidget::RangeTypCmb_activated( int num )
{
  if (!fbFillWidget) {
    EnableEditsForRangeType(num);
    GetRangeData();
  }
}

void QFitRangeWidget::LeftEdt_textChanged( const QString & )
{
  GetRangeData();
}

void QFitRangeWidget::RightEdt_textChanged( const QString & )
{
  GetRangeData();
}
