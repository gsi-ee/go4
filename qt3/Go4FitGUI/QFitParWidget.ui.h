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

TGo4FitParameter* QFitParWidget::GetPar() {
   return dynamic_cast<TGo4FitParameter*> (GetObject());
}

void QFitParWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   TGo4FitParameter* fxParameter = GetPar();
   if (fxParameter==0) return;

   ValueEdt->setText(QString::number(fxParameter->GetValue()));
   ErrorEdt->setText(QString::number(fxParameter->GetError()));
   FixedChk->setChecked(fxParameter->GetFixed());

   Double_t min, max;

   bool b = fxParameter->GetRange(min,max);

   RangeChk->setChecked(b);
   RangeMinEdt->setEnabled(b);
   RangeMaxEdt->setEnabled(b);

   RangeMinEdt->setText(QString::number(min));
   RangeMaxEdt->setText(QString::number(max));

   Double_t eps;
   b = fxParameter->GetEpsilon(eps);
   EpsilonChk->setChecked(b);
   EpsilonEdt->setEnabled(b);
   EpsilonEdt->setText(QString::number(eps));
}

void QFitParWidget::ValueEdt_textChanged( const QString & value)
{
  if(!fbFillWidget && (GetPar()!=0)) {
     bool ok;
     double zn = value.toDouble(&ok);
     if(ok) {
       GetPar()->SetValue(zn);
       SetWidgetItemText(true);
     }
  }
}

void QFitParWidget::ErrorEdt_textChanged( const QString & value)
{
  if(!fbFillWidget && (GetPar()!=0)) {
     bool ok;
     double zn = value.toDouble(&ok);
     if(ok) GetPar()->SetError(zn);
  }
}

void QFitParWidget::RangeMinEdt_textChanged( const QString & value)
{
  if(!fbFillWidget && (GetPar()!=0)) {
     bool ok;
     double zn = value.toDouble(&ok);
     if(ok) GetPar()->ChangeRangeMin(zn);
  }
}


void QFitParWidget::RangeMaxEdt_textChanged( const QString & value)
{
  if(!fbFillWidget && (GetPar()!=0)) {
     bool ok;
     double zn = value.toDouble(&ok);
     if(ok) GetPar()->ChangeRangeMax(zn);
  }
}


void QFitParWidget::EpsilonEdt_textChanged( const QString & value)
{
  if(!fbFillWidget && (GetPar()!=0)) {
     bool ok;
     double zn = value.toDouble(&ok);
     if(ok) GetPar()->ChangeEpsilon(zn);
  }

}


void QFitParWidget::FixedChk_toggled( bool zn)
{
  if(!fbFillWidget && (GetPar()!=0))
    GetPar()->SetFixed(zn);
}


void QFitParWidget::RangeChk_toggled( bool zn)
{
   if(!fbFillWidget && (GetPar()!=0)) {
      RangeMinEdt->setEnabled(zn);
      RangeMaxEdt->setEnabled(zn);
      GetPar()->SetRangeUse(zn);
   }
}

void QFitParWidget::EpsilonChk_toggled( bool zn)
{
   if(!fbFillWidget && (GetPar()!=0)) {
      EpsilonEdt->setEnabled(zn);
      GetPar()->SetEpsilonUse(zn);
   }
}
