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

#include "QFitModelWidget.h"

#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>

#include "TGo4FitPanel.h"

QFitModelWidget::QFitModelWidget( QWidget* parent,  const char* name )
    : QFitNamedWidget( parent, name )
{
    if (!name) setObjectName( "QFitModelWidget" );
    setWindowTitle("QFitModelWidget");

    AmplChk = new QCheckBox("amplitude", this);
    AmplChk->setGeometry( QRect( 10, 110, 80, 25 ) );

    textLabel1 = new QLabel( "group", this );
    textLabel1->setGeometry( QRect( 200, 110, 40, 20 ) );

    BuffersChk = new QCheckBox("use buffers", this);
    BuffersChk->setGeometry( QRect( 100, 110, 90, 25 ) );

    GroupSpin = new QSpinBox( this );
    GroupSpin->setGeometry( QRect( 240, 110, 55, 22 ) );
    GroupSpin->setMinimum( -1 );
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );

    // signals and slots connections
    QObject::connect(AmplChk, &QCheckBox::toggled, this, &QFitModelWidget::AmplChk_toggled);
    QObject::connect(BuffersChk, &QCheckBox::toggled, this, &QFitModelWidget::BuffersChk_toggled);
    QObject::connect(GroupSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &QFitModelWidget::GroupSpin_valueChanged);
}

QFitModelWidget::~QFitModelWidget()
{
}

void QFitModelWidget::AmplChk_toggled(bool chk)
{
  if (!fbFillWidget && fxPanel)
    fxPanel->ChangeModelPar(this, 0, chk ? 1 : 0);
}

void QFitModelWidget::BuffersChk_toggled( bool chk)
{
  if (!fbFillWidget && fxPanel)
    fxPanel->ChangeModelPar(this, 1, chk ? 1 : 0);
}


void QFitModelWidget::GroupSpin_valueChanged( int indx )
{
  if (!fbFillWidget && fxPanel)
    fxPanel->ChangeModelPar(this, 2, indx);
}
