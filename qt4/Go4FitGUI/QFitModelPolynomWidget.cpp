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

#include "QFitModelPolynomWidget.h"

#include "TGo4FitModelPolynom.h"
#include "TGo4FitGuiTypes.h"

QFitModelPolynomWidget::QFitModelPolynomWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{
   setupUi(this);
}


TGo4FitModelPolynom * QFitModelPolynomWidget::GetPolynom()
{
  return dynamic_cast<TGo4FitModelPolynom*> (GetObject());
}

void QFitModelPolynomWidget::FillSpecificData()
{
   QFitNamedWidget::FillSpecificData();
   if(GetPolynom())
      AxisSpin->setValue(GetPolynom()->GetMaxNumAxis());
}

void QFitModelPolynomWidget::AxisSpin_valueChanged( int num )
{
  if(!fbFillWidget && GetPolynom())
    if(GetPolynom()->SetMaxNumAxis(num))
      UpdateItemsOfType(FitGui::ot_parslist, false);
}
