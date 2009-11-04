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

#ifndef QFITPRINTWIDGET_H
#define QFITPRINTWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitPrintWidget.h"

class QFitPrintWidget : public QFitWidget, public Ui::QFitPrintWidget
 {

    Q_OBJECT

public:
    QFitPrintWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual void FillSpecificData();
    virtual void SetDrawOption( const QString & option );

protected:
    QString fxDrawOption;


};

#endif // QFITPRINTWIDGET_H


