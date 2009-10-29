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
