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

#ifndef QFITPARWIDGET_H
#define QFITPARWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitParWidget.h"

class TGo4FitParameter;


class QFitParWidget : public QFitNamedWidget, public Ui::QFitParWidget
 {
    Q_OBJECT

public:
    QFitParWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitParameter * GetPar();
    virtual void FillSpecificData();
    virtual void ValueEdt_textChanged( const QString & value );
    virtual void ErrorEdt_textChanged( const QString & value );
    virtual void RangeMinEdt_textChanged( const QString & value );
    virtual void RangeMaxEdt_textChanged( const QString & value );
    virtual void EpsilonEdt_textChanged( const QString & value );
    virtual void FixedChk_toggled( bool zn );
    virtual void RangeChk_toggled( bool zn );
    virtual void EpsilonChk_toggled( bool zn );


};

#endif // QFITPARWIDGET_H


