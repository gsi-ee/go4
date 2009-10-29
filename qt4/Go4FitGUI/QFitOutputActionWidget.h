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

#ifndef QFITOUTPUTACTIONWIDGET_H
#define QFITOUTPUTACTIONWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitOutputActionWidget.h"


class TGo4FitterOutput;


class QFitOutputActionWidget : public QFitNamedWidget, public Ui::QFitOutputActionWidget
 {

    Q_OBJECT

public:
    QFitOutputActionWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitterOutput * GetOutAct();
    virtual void FillSpecificData();
    virtual void OptionsEdit_textChanged( const QString & str );
    virtual void CommandCombo_activated( int );

};

#endif // QFITOUTPUTACTIONWIDGET_H


