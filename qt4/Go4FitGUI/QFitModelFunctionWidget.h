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

#ifndef QFITMODELFUNCTIONWIDGET_H
#define QFITMODELFUNCTIONWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelFunctionWidget.h"

class TGo4FitModelFunction;

class QFitModelFunctionWidget : public QFitModelWidget, public Ui::QFitModelFunctionWidget
 {
    Q_OBJECT

public:
    QFitModelFunctionWidget( QWidget* parent = 0, const char* name = 0 );


public slots:
    virtual TGo4FitModelFunction * GetFunction();
    virtual void FillSpecificData();
    virtual void LibNameEdt_textChanged( const QString & );
    virtual void FuncNameEdt_textChanged( const QString & );
    virtual void NumParSpin_valueChanged( int );

};

#endif // QFITMODELFUNCTIONWIDGET_H
