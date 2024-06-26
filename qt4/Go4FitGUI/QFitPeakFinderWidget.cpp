// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "QFitPeakFinderWidget.h"

#include "TGo4FitPeakFinder.h"

QFitPeakFinderWidget::QFitPeakFinderWidget(QWidget *parent, const char *name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
   QObject::connect(DataNameEdt, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::DataNameEdt_textChanged);
   QObject::connect(ClearModelsChk, &QCheckBox::toggled, this, &QFitPeakFinderWidget::ClearModelsChk_toggled);
   QObject::connect(UsePolynChk, &QCheckBox::toggled, this, &QFitPeakFinderWidget::UsePolynChk_toggled);
   QObject::connect(PolynSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &QFitPeakFinderWidget::PolynSpin_valueChanged);
   QObject::connect(FindersTab, &QTabWidget::currentChanged, this, &QFitPeakFinderWidget::FindersTab_currentChanged);
   QObject::connect(ThresholdEdt, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::ThresholdEdt_textChanged);
   QObject::connect(MinWidthEdt, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::MinWidthEdt_textChanged);
   QObject::connect(MaxWidthEdt, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::MaxWidthEdt_textChanged);
   QObject::connect(WidthEdit, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::WidthEdit_textChanged);
   QObject::connect(NoiseFactorEdit, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::NoiseFactorEdit_textChanged);
   QObject::connect(NoiseMinEdit, &QGo4LineEdit::textChanged, this, &QFitPeakFinderWidget::NoiseMinEdit_textChanged);
   QObject::connect(SumUpSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &QFitPeakFinderWidget::SumUpSpin_valueChanged);
}

TGo4FitPeakFinder * QFitPeakFinderWidget::GetPF()
{
  return dynamic_cast<TGo4FitPeakFinder*> (GetObject());
}

void QFitPeakFinderWidget::FillSpecificData()
{
   QFitNamedWidget::FillSpecificData();
   TGo4FitPeakFinder* finder = GetPF();
   if (finder) {
      DataNameEdt->setText(finder->GetDataName());
      ClearModelsChk->setChecked(finder->GetClearModels());

      UsePolynChk->setChecked(finder->GetUsePolynom());
      PolynSpin->setValue(finder->GetPolynomOrder());
      PolynSpin->setEnabled(finder->GetUsePolynom());

      FindersTab->setCurrentIndex(finder->GetPeakFinderType());

      ThresholdEdt->setText(QString::number(finder->Get0MaxAmplFactor()));
      MinWidthEdt->setText(QString::number(finder->Get0MinWidth()));
      MaxWidthEdt->setText(QString::number(finder->Get0MaxWidth()));

      WidthEdit->setText(QString::number(finder->Get1LineWidth()));

      NoiseFactorEdit->setText(QString::number(finder->Get2NoiseFactor()));
      NoiseMinEdit->setText(QString::number(finder->Get2NoiseMinimum()));
      SumUpSpin->setValue(finder->Get2ChannelSum());
   }
}


void QFitPeakFinderWidget::DataNameEdt_textChanged( const QString & name)
{
  if (!fbFillWidget && GetPF())
    GetPF()->SetDataName(name.toLatin1().constData());
}


void QFitPeakFinderWidget::ClearModelsChk_toggled( bool zn)
{
   if (!fbFillWidget && GetPF())
      GetPF()->SetClearModels(zn);
}


void QFitPeakFinderWidget::UsePolynChk_toggled( bool zn)
{
  if (!fbFillWidget && GetPF()) {
    GetPF()->SetUsePolynom(zn);
    PolynSpin->setEnabled(zn);
  }
}


void QFitPeakFinderWidget::PolynSpin_valueChanged( int zn)
{
  if (!fbFillWidget && GetPF())
    GetPF()->SetPolynomOrder(zn);
}


void QFitPeakFinderWidget::FindersTab_currentChanged( int indx)
{
  if (!fbFillWidget && GetPF())
    GetPF()->SetPeakFinderType(indx);
}


void QFitPeakFinderWidget::ThresholdEdt_textChanged( const QString & value)
{
  if (!fbFillWidget && GetPF()) {
    bool ok = false;
    double zn = value.toDouble(&ok);
    if (ok && (zn>0) && (zn<=1.)) GetPF()->Set0MaxAmplFactor(zn);
  }
}


void QFitPeakFinderWidget::MinWidthEdt_textChanged( const QString & value)
{
   if (!fbFillWidget && GetPF()) {
      bool ok = false;
      double zn = value.toDouble(&ok);
      if (ok && (zn >= 0))
         GetPF()->Set0MinWidth(zn);
   }
}

void QFitPeakFinderWidget::MaxWidthEdt_textChanged( const QString & value)
{
   if (!fbFillWidget && GetPF()) {
      bool ok = false;
      double zn = value.toDouble(&ok);
      if (ok && (zn > 0))
         GetPF()->Set0MaxWidth(zn);
   }
}

void QFitPeakFinderWidget::WidthEdit_textChanged( const QString & value )
{
  if (!fbFillWidget && GetPF()) {
    bool ok = false;
    double zn = value.toDouble(&ok);
    if (ok && (zn>0)) GetPF()->Set1LineWidth(zn);
  }
}


void QFitPeakFinderWidget::NoiseFactorEdit_textChanged( const QString & value )
{
  if (!fbFillWidget && GetPF()) {
    bool ok = false;
    double zn = value.toDouble(&ok);
    if (ok && (zn>0)) GetPF()->Set2NoiseFactor(zn);
  }
}


void QFitPeakFinderWidget::NoiseMinEdit_textChanged( const QString & value)
{
  if (!fbFillWidget && GetPF()) {
    bool ok = false;
    double zn = value.toDouble(&ok);
    if (ok && (zn >= 0))
       GetPF()->Set2NoiseMinimum(zn);
  }
}


void QFitPeakFinderWidget::SumUpSpin_valueChanged( int zn)
{
  if (!fbFillWidget && GetPF())
    GetPF()->Set2ChannelSum(zn);
}
