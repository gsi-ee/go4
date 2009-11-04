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

#ifndef QFITMINUITRESWIDGET_H
#define QFITMINUITRESWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitMinuitResWidget.h"

class TGo4FitMinuitResult;

class QFitMinuitResWidget : public QFitNamedWidget, public Ui::QFitMinuitResWidget
 {
    Q_OBJECT

public:
    QFitMinuitResWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitMinuitResult * GetResult();
    virtual void FillSpecificData();


};

#endif // QFITMINUITRESWIDGET_H








