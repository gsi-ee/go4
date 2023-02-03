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

#ifndef QFITMODELGAUSSNWIDGET_H
#define QFITMODELGAUSSNWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelGaussNWidget.h"

class TGo4FitModelGaussN;

class QFitModelGaussNWidget : public QFitModelWidget, public Ui::QFitModelGaussNWidget
 {
   Q_OBJECT

public:
    QFitModelGaussNWidget(QWidget *parent = nullptr, const char *name = nullptr);
    void FillSpecificData() override;

public slots:
   virtual TGo4FitModelGaussN * GetGaussN();
   virtual void SetAxisTable();
   virtual void AxisNumberSpin_valueChanged(int);
   virtual void AxisTable_valueChanged( int, int );

};

#endif
