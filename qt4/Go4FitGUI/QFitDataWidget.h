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

#ifndef QFITDATAWIDGET_H
#define QFITDATAWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitDataWidget.h"

class TGo4FitData;


class QFitDataWidget : public QFitNamedWidget, public Ui::QFitDataWidget
 {
     Q_OBJECT

 public:
    QFitDataWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitData * GetData();
    virtual void FillSpecificData();
    virtual void NumCalibrSpin_valueChanged( int num );
    virtual void UseBinsChk_toggled( bool zn );
    virtual void SigmaCmb_activated( int zn );
    virtual void SigmaEdt_textChanged( const QString & value );
    virtual void BinsLimitEdt_textChanged( const QString & value );
    virtual void AmplChk_toggled( bool chk );
    virtual void UseBuffersChk_toggled( bool );


 };

#endif