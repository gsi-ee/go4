#include "QFitLinearTransWidget.h"
#include "TGo4FitLinearTrans.h"

QFitLinearTransWidget::QFitLinearTransWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitLinearTrans* QFitLinearTransWidget::GetLinearTrans() {
  return dynamic_cast<TGo4FitLinearTrans*> (GetObject());
}

void QFitLinearTransWidget::FillSpecificData() {
  QFitNamedWidget::FillSpecificData();
  TGo4FitLinearTrans* trans = GetLinearTrans();
  if (trans)
    NumAxisSpin->setValue(trans->GetAxis());

}

void QFitLinearTransWidget::NumAxisSpin_valueChanged( int axis)
{
   if (!fbFillWidget && GetLinearTrans())
     GetLinearTrans()->SetAxis(axis);
}
