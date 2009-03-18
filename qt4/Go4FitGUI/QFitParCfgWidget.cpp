#include "QFitParCfgWidget.h"

//#include "QGo4LineEdit.h"
#include "QFitItem.h"
#include "TGo4FitParsList.h"
#include "TGo4FitParameter.h"


QFitParCfgWidget::QFitParCfgWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}


TGo4FitParameter * QFitParCfgWidget::GetPar()
{
   return dynamic_cast<TGo4FitParameter*> (GetObject());
}

void QFitParCfgWidget::FillSpecificData()
{
  if (GetPar()) {
     ParNameEdit->setText(GetPar()->GetName());
     ParFixedChk->setChecked(GetPar()->GetFixed());
     Double_t min,max;
     if (GetPar()->GetRange(min,max)) {
        RangeMinEdit->setText(QString::number(min));
        RangeMaxEdit->setText(QString::number(max));
     } else {
        RangeMinEdit->setText("");
        RangeMaxEdit->setText("");
     }
     Double_t eps;
     if (GetPar()->GetEpsilon(eps)) EpsilonEdit->setText(QString::number(eps));
                               else EpsilonEdit->setText("");
  }
}

void QFitParCfgWidget::AnalyzeRangeValues()
{
  if  ((RangeMinEdit->text().length()==0) && (RangeMaxEdit->text().length()==0))
    { GetPar()->SetRangeUse(kFALSE); return; }
  double min,max;
  bool okmin = FALSE, okmax = FALSE;

  min = RangeMinEdit->text().toDouble(&okmin);
  max = RangeMaxEdit->text().toDouble(&okmax);

  if (RangeMinEdit->text().length()==0) { min = max; okmin = okmax; }
  if (RangeMaxEdit->text().length()==0) { max = min; okmax = okmin; }

  if (okmin && okmax)
    GetPar()->SetRange(min,max);
}


void QFitParCfgWidget::ParNameEdit_textChanged( const QString & name)
{
  if(!fbFillWidget && GetPar() && (name.length()>0)) {
      TGo4FitParsList* pars = dynamic_cast<TGo4FitParsList*> (GetItem()->Parent()->Object());
      if (pars==0) return;
      if (pars->FindPar(name)) return;
      GetPar()->SetName(name);
      GetItem()->setText(0,name);
  }
}


void QFitParCfgWidget::ParFixedChk_toggled( bool chk)
{
  if(!fbFillWidget && GetPar())
    GetPar()->SetFixed(chk);
}


void QFitParCfgWidget::RangeMinEdit_textChanged( const QString &)
{
  if(!fbFillWidget && GetPar())
    AnalyzeRangeValues();
}


void QFitParCfgWidget::RangeMaxEdit_textChanged( const QString &)
{
  if(!fbFillWidget && GetPar())
    AnalyzeRangeValues();
}

void QFitParCfgWidget::EpsilonEdit_textChanged( const QString & value)
{
    if(!fbFillWidget && GetPar())
      if (value.length()==0) GetPar()->SetEpsilonUse(kFALSE); else {
          bool res = FALSE;
          double zn = value.toDouble(&res);
          if(res) GetPar()->SetEpsilon(zn);
      }
}
