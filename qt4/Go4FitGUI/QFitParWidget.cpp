#include "QFitParWidget.h"

//#include "QGo4LineEdit.h"
#include "QFitItem.h"
#include "TGo4FitParameter.h"
#include "TGo4FitParsList.h"


QFitParWidget::QFitParWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}




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
