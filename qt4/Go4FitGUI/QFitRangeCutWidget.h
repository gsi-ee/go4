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

#ifndef QFITRANGECUTWIDGET_H
#define QFITRANGECUTWIDGET_H

#include "QFitNamedWidget.h"
#include "ui_QFitRangeCutWidget.h"


class TGo4FitComponent;
class TCutG;


class QFitRangeCutWidget : public QFitNamedWidget, public Ui::QFitRangeCutWidget
 {

    Q_OBJECT

public:
    QFitRangeCutWidget( QWidget* parent = 0, const char* name = 0 );

public slots:
    virtual TGo4FitComponent * GetComp();
    virtual int GetCutIndex();
    virtual TCutG * GetCut();
    virtual void FillSpecificData();
    virtual void FillXYPointsTable();
    virtual void NumPointsSpin_valueChanged( int );
    virtual void ExcludeCutChk_toggled( bool );
    virtual void XYTable_valueChanged( int, int );


};

#endif // QFITRANGECUTWIDGET_H

