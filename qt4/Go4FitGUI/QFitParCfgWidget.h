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

#ifndef QFITPARCFGWIDGET_H
#define QFITPARCFGWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitParCfgWidget.h"

class TGo4FitParameter;

class QFitParCfgWidget : public QFitWidget, public Ui::QFitParCfgWidget
{

    Q_OBJECT

public:
    QFitParCfgWidget(QWidget *parent = nullptr, const char *name = nullptr);
    void FillSpecificData() override;

public slots:
    virtual TGo4FitParameter *GetPar();
    virtual void AnalyzeRangeValues();
    virtual void ParNameEdit_textChanged(const QString &);
    virtual void ParFixedChk_toggled(bool);
    virtual void RangeMinEdit_textChanged(const QString &);
    virtual void RangeMaxEdit_textChanged(const QString &);
    virtual void EpsilonEdit_textChanged(const QString &);
};

#endif
