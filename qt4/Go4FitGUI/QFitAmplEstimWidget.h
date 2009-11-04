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

#ifndef QFITAMPLESTIMWIDGET_H
#define QFITAMPLESTIMWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitAmplEstimWidget.h"

class TGo4FitAmplEstimation;

class QFitAmplEstimWidget : public QFitNamedWidget, public Ui::QFitAmplEstimWidget
 {
     Q_OBJECT

 public:
    QFitAmplEstimWidget( QWidget* parent = 0, const char* name = 0 );

 public slots:
    virtual TGo4FitAmplEstimation * GetAmplEstim();
    virtual void FillSpecificData();
    virtual void Iterations_valueChanged( int );

 };

#endif
