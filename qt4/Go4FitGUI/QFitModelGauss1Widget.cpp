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

#include "QFitModelGauss1Widget.h"

#include "TGo4FitModelGauss1.h"


QFitModelGauss1Widget::QFitModelGauss1Widget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{
   setupUi(this);
}

TGo4FitModelGauss1 * QFitModelGauss1Widget::GetGauss1()
{
   return dynamic_cast<TGo4FitModelGauss1*> (GetObject());
}

void QFitModelGauss1Widget::FillSpecificData()
{
    QFitNamedWidget::FillSpecificData();
    if (GetGauss1())
      AxisNumSpin->setValue(GetGauss1()->GetNumAxis());
}


void QFitModelGauss1Widget::AxisNumSpin_valueChanged( int num )
{
    if(!fbFillWidget && GetGauss1())
      GetGauss1()->SetNumAxis(num);
}
