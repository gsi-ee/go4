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

#ifndef QFITMODELGAUSS1WIDGET_H
#define QFITMODELGAUSS1WIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelGauss1Widget.h"


class TGo4FitModelGauss1;

class QFitModelGauss1Widget : public QFitModelWidget, public Ui::QFitModelGauss1Widget
 {
    Q_OBJECT

public:
    QFitModelGauss1Widget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitModelGauss1 * GetGauss1();
    virtual void FillSpecificData();
    virtual void AxisNumSpin_valueChanged( int );


};



#endif
