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

#ifndef QFITTERWIDGET_H
#define QFITTERWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitterWidget.h"

class TGo4Fitter;

class QFitterWidget : public QFitNamedWidget, public Ui::QFitterWidget
 {
    Q_OBJECT

public:
    QFitterWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4Fitter * GetFitter();
    virtual void FillSpecificData();
    virtual void FitFunctionTypeCmb_activated( int typ );


};

#endif // QFITTERWIDGET_H


