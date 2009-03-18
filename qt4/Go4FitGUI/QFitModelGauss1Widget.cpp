#include "QFitModelGauss1Widget.h"

#include "TGo4FitModelGauss1.h"


QFitModelGauss1Widget::QFitModelGauss1Widget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}

TGo4FitModelGauss1 * QFitModelGauss1Widget::GetGauss1()
{
   return dynamic_cast<TGo4FitModelGauss1*> (GetObject());
}

void QFitModelGauss1Widget::FillSpecificData()
{
    QFitNamedWidget::FillSpecificData();
    if (GetGauss1())
      AxisNumSpin->setValue(GetGauss1()->GetNumAxis());
}


void QFitModelGauss1Widget::AxisNumSpin_valueChanged( int num )
{
    if(!fbFillWidget && GetGauss1())
      GetGauss1()->SetNumAxis(num);
}
