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

#ifndef QFITLINEARTRANSWIDGET_H
#define QFITLINEARTRANSWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitLinearTransWidget.h"

class TGo4FitLinearTrans;

class QFitLinearTransWidget : public QFitNamedWidget, public Ui::QFitLinearTransWidget
 {
     Q_OBJECT

public:
    QFitLinearTransWidget( QWidget* parent = 0, const char* name = 0 );


public slots:
    virtual TGo4FitLinearTrans * GetLinearTrans();
    virtual void FillSpecificData();
    virtual void NumAxisSpin_valueChanged( int );

 };

#endif










