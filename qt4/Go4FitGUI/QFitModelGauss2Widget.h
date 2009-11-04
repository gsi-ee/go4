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

#ifndef QFITMODELGAUSS2WIDGET_H
#define QFITMODELGAUSS2WIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelGauss2Widget.h"

class TGo4FitModelGauss2;

class QFitModelGauss2Widget : public QFitModelWidget, public Ui::QFitModelGauss2Widget
 {
    Q_OBJECT

public:
    QFitModelGauss2Widget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitModelGauss2 * GetGauss2();
    virtual void FillSpecificData();
    virtual void Axis1Spin_valueChanged( int );
    virtual void Axis2Spin_valueChanged( int );

};

#endif // QFITMODELGAUSS2WIDGET_H


