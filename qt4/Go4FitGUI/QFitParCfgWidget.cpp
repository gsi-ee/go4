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

#include "QFitParCfgWidget.h"

#include "QFitItem.h"
#include "TGo4FitParsList.h"
#include "TGo4FitParameter.h"


QFitParCfgWidget::QFitParCfgWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{
   setupUi(this);
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
  bool okmin = false, okmax = false;

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
      if (pars->FindPar(name.toLatin1().constData())) return;
      GetPar()->SetName(name.toLatin1().constData());
      GetItem()->setText(0,name.toLatin1().constData());
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
          bool res = false;
          double zn = value.toDouble(&res);
          if(res) GetPar()->SetEpsilon(zn);
      }
}
