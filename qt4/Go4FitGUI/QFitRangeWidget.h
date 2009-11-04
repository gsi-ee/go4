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

#ifndef QFITRANGEWIDGET_H
#define QFITRANGEWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitRangeWidget.h"


class TGo4FitComponent;

class QFitRangeWidget : public QFitWidget, public Ui::QFitRangeWidget
{
    Q_OBJECT

public:
    QFitRangeWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitComponent * GetComp();
    virtual int GetNumRange();
    virtual void FillSpecificData();
    virtual void EnableEditsForRangeType( int );
    virtual void GetRangeData();
    virtual void AxisNumSpin_valueChanged( int );
    virtual void RangeTypCmb_activated( int );
    virtual void LeftEdt_textChanged( const QString & );
    virtual void RightEdt_textChanged( const QString & );


};

#endif // QFITRANGEWIDGET_H


