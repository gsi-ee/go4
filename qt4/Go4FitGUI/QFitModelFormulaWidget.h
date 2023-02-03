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

#ifndef QFITMODELFORMULAWIDGET_H
#define QFITMODELFORMULAWIDGET_H

#include "QFitModelWidget.h"
#include "ui_QFitModelFormulaWidget.h"

class TGo4FitModelFormula;

class QFitModelFormulaWidget : public QFitModelWidget, public Ui::QFitModelFormulaWidget
 {
    Q_OBJECT

public:
    QFitModelFormulaWidget(QWidget *parent = nullptr, const char *name = nullptr);
    void FillSpecificData() override;

public slots:
    virtual TGo4FitModelFormula * GetFormula();
    virtual void ExprEdt_textChanged(const QString &);
    virtual void NumParSpin_valueChanged(int);

};

#endif
