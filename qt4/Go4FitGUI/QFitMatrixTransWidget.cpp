#include "QFitMatrixTransWidget.h"

#include "TGo4FitMatrixTrans.h"

QFitMatrixTransWidget::QFitMatrixTransWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitMatrixTrans* QFitMatrixTransWidget::GetMatrixTrans() {
  return dynamic_cast<TGo4FitMatrixTrans*> (GetObject());
}

void QFitMatrixTransWidget::FillSpecificData() {
  QFitNamedWidget::FillSpecificData();
  TGo4FitMatrixTrans* trans = GetMatrixTrans();
  if (trans)
    NumAxisSpin->setValue(trans->GetNumAxis());
}

void QFitMatrixTransWidget::NumAxisSpin_valueChanged( int num ){
   if (!fbFillWidget && GetMatrixTrans()) {
     GetMatrixTrans()->SetNumAxis(num);
     UpdateWidgetItem(true);
   }
}
