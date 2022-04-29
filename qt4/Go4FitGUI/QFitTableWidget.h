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

#ifndef QFITTABLEWIDGET_H
#define QFITTABLEWIDGET_H

#include "QFitWidget.h"
#include "ui_QFitTableWidget.h"

class QFitTableWidget : public QFitWidget, public Ui::QFitTableWidget {
   Q_OBJECT

public:
   QFitTableWidget(QWidget *parent = nullptr, const char *name = nullptr);

public slots:
   void FillSpecificData() override;
};

#endif
