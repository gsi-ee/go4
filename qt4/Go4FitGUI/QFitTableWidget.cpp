#include "QFitTableWidget.h"

#include "QFitItem.h"
#include "TGo4Fitter.h"
#include "TGo4FitGuiTypes.h"

#include "Riostream.h"

QFitTableWidget::QFitTableWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{
   setupUi(this);
   // put slot connections here!
   // note: Qt4 uic will add all existing connections 
   // from ui file to the setupUI
}

void QFitTableWidget::FillSpecificData()
{
   Table->setNumCols(0);
   Table->setNumRows(0);
   if (GetItem())
     if (GetItem()->ObjectType()==FitGui::ot_results) {
        TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (GetObject());
        if (fitter==0) return;
        Table->setReadOnly(TRUE);
        Table->setNumCols(1);
        Table->horizontalHeader()->setLabel(0,"Value");
        Table->setColumnWidth(0,150);
        Table->setNumRows(fitter->GetNumResults()+2);
        Table->verticalHeader()->setLabel(0,"Fit function");
        Table->setText(0,0, QString::number(fitter->GetResultFF()));
        Table->verticalHeader()->setLabel(1,"NDF");
        Table->setText(1,0, QString::number(fitter->GetResultNDF()));

        for (Int_t n=0;n<fitter->GetNumResults();n++) {
           Table->verticalHeader()->setLabel(n+2,QString("Result") + QString::number(n));
           Table->setText(n+2,0,QString::number(fitter->GetResultValue(n)));
        }
     }
}
