// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef QFITSLOTWIDGET_H
#define QFITSLOTWIDGET_H

class TGo4FitSlot;

#include "QFitNamedWidget.h"
#include "ui_QFitSlotWidget.h"

class QFitSlotWidget : public QFitNamedWidget, public Ui::QFitSlotWidget {

public:
    QFitSlotWidget(QWidget *parent = nullptr, const char *name = nullptr);
    void FillSpecificData() override;

public slots:
    virtual TGo4FitSlot * GetSlot();
    virtual void RequirementChk_toggled( bool req );
    virtual void SaveCmb_activated( int typ );
};

#endif
