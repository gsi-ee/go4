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

#ifndef QFITDEPENDENCYWIDGET_H
#define QFITDEPENDENCYWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitDependencyWidget.h"

class TGo4FitDependency;

class QFitDependencyWidget : public QFitWidget, public Ui::QFitDependencyWidget
 {
     Q_OBJECT

public:
    QFitDependencyWidget(QWidget *parent = nullptr, const char *name = nullptr);
    void FillSpecificData() override;

public slots:
    virtual TGo4FitDependency * GetDepen();
    virtual void ParamEdit_textChanged(const QString &);
    virtual void ExpressionEdt_textChanged(const QString &);

};

#endif
