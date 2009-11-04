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

#include "QFitTableWidget.h"

#include "QFitItem.h"
#include "TGo4Fitter.h"
#include "TGo4FitGuiTypes.h"

#include "Riostream.h"

QFitTableWidget::QFitTableWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{
   setupUi(this);
}

void QFitTableWidget::FillSpecificData()
{
   Table->setColumnCount(0);
   Table->setRowCount(0);
   Table->setEnabled(false);
   if (GetItem())
     if (GetItem()->ObjectType()==FitGui::ot_results) {
        TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (GetObject());
        if (fitter==0) return;
        Table->setColumnCount(1);
        Table->setHorizontalHeaderItem(0, new QTableWidgetItem("Value"));
        Table->setColumnWidth(0,150);
        Table->setRowCount(fitter->GetNumResults()+2);
        Table->setVerticalHeaderItem(0,  new QTableWidgetItem("Fit function"));
        Table->setItem(0,0, new QTableWidgetItem(QString::number(fitter->GetResultFF())));
        Table->setVerticalHeaderItem(1, new QTableWidgetItem("NDF"));
        Table->setItem(1,0, new QTableWidgetItem(QString::number(fitter->GetResultNDF())));

        for (Int_t n=0;n<fitter->GetNumResults();n++) {
           Table->setVerticalHeaderItem(n+2, new QTableWidgetItem(QString("Result") + QString::number(n)));
           Table->setItem(n+2, 0, new QTableWidgetItem(QString::number(fitter->GetResultValue(n))));
        }
     }
}
