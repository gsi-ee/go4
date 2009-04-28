#include "QFitModelGauss2Widget.h"

#include "TGo4FitModelGauss2.h"


QFitModelGauss2Widget::QFitModelGauss2Widget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{
   setupUi(this);
}

TGo4FitModelGauss2 * QFitModelGauss2Widget::GetGauss2()
{
   return dynamic_cast<TGo4FitModelGauss2*> (GetObject());
}

void QFitModelGauss2Widget::FillSpecificData()
{
    QFitNamedWidget::FillSpecificData();
    if (GetGauss2()) {
       Axis1Spin->setValue(GetGauss2()->GetNumAxis1());
       Axis2Spin->setValue(GetGauss2()->GetNumAxis2());
    }
}

void QFitModelGauss2Widget::Axis1Spin_valueChanged(int num1)
{
  if (!fbFillWidget && GetGauss2()) {
     GetGauss2()->SetNumAxis1(num1);
     int num2 = GetGauss2()->GetNumAxis2();
     if (num2==num1) {
        num2++;
        GetGauss2()->SetNumAxis2(num2);
        fbFillWidget = kTRUE;
        Axis2Spin->setValue(num2);
        fbFillWidget = kFALSE;
     }
  }
}

void QFitModelGauss2Widget::Axis2Spin_valueChanged(int num2)
{
  if (!fbFillWidget && GetGauss2()) {
     GetGauss2()->SetNumAxis2(num2);
     int num1 = GetGauss2()->GetNumAxis1();
     if (num2==num1) {
        if (num1==0) num1 = 1; else num1--;
        GetGauss2()->SetNumAxis1(num1);
        fbFillWidget = kTRUE;
        Axis1Spin->setValue(num1);
        fbFillWidget = kFALSE;
     }
  }
}
