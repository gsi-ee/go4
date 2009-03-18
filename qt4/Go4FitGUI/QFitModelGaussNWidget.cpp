#include "QFitModelGaussNWidget.h"

#include "TGo4FitModelGaussN.h"
#include "TGo4FitGuiTypes.h"

QFitModelGaussNWidget::QFitModelGaussNWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
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
   AxisTable->setNumRows(num);
   for(int n=0;n<num;n++)
     AxisTable->setText(n,0,QString::number(GetGaussN()->GetAxisForIndex(n)));
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
      int value = AxisTable->text(nrow,ncol).toInt(&ok);
      if(ok) {
          GetGaussN()->SetAxisForIndex(nrow,value);
          if (GetGaussN()->ResortIndexes(nrow)) SetAxisTable();
      }
   }
}
