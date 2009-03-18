/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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
        Table->setText(0,0,QString::number(fitter->GetResultFF()));
        Table->verticalHeader()->setLabel(1,"NDF");
        Table->setText(1,0,QString::number(fitter->GetResultNDF()));

        for (Int_t n=0;n<fitter->GetNumResults();n++) {
           Table->verticalHeader()->setLabel(n+2,QString("Result") + QString::number(n));
           Table->setText(n+2,0,QString::number(fitter->GetResultValue(n)));
        }
     }
}
