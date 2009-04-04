#include "QFitModelGaussNWidget.h"

#include "TGo4FitModelGaussN.h"
#include "TGo4FitGuiTypes.h"

QFitModelGaussNWidget::QFitModelGaussNWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{
  setupUi(this);
}

TGo4FitModelGaussN * QFitModelGaussNWidget::GetGaussN()
{
   return dynamic_cast<TGo4FitModelGaussN*> (GetObject());
}

void QFitModelGaussNWidget::FillSpecificData()
{
    QFitNamedWidget::FillSpecificData();
    if (GetGaussN()) {
       AxisNumberSpin->setValue(GetGaussN()->GetAxisNumbers());
       SetAxisTable();
    }
}

void QFitModelGaussNWidget::SetAxisTable()
{
   if (GetGaussN()==0) return;
   int num = GetGaussN()->GetAxisNumbers();
   AxisTable->setRowCount(num);
   for(int n=0;n<num;n++)
     AxisTable->setItem(n,0,
   	new QTableWidgetItem(QString::number(GetGaussN()->GetAxisForIndex(n))));
}

void QFitModelGaussNWidget::AxisNumberSpin_valueChanged( int num)
{
  if(!fbFillWidget && GetGaussN())
    if (GetGaussN()->SetAxisNumbers(num)) {
      SetAxisTable();
      UpdateItemsOfType(FitGui::ot_parslist, false);
    }
}

void QFitModelGaussNWidget::AxisTable_valueChanged( int nrow, int ncol)
{
   if(!fbFillWidget && GetGaussN()) {
      bool ok = FALSE;
      int value = AxisTable->item(nrow,ncol)->text().toInt(&ok);
      if(ok) {
          GetGaussN()->SetAxisForIndex(nrow,value);
          if (GetGaussN()->ResortIndexes(nrow)) SetAxisTable();
      }
   }
}
