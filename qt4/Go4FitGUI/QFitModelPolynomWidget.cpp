#include "QFitModelPolynomWidget.h"

#include "TGo4FitModelPolynom.h"
#include "TGo4FitGuiTypes.h"

QFitModelPolynomWidget::QFitModelPolynomWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}


TGo4FitModelPolynom * QFitModelPolynomWidget::GetPolynom()
{
  return dynamic_cast<TGo4FitModelPolynom*> (GetObject());
}

void QFitModelPolynomWidget::FillSpecificData()
{
   QFitNamedWidget::FillSpecificData();
   if(GetPolynom())
      AxisSpin->setValue(GetPolynom()->GetMaxNumAxis());
}

void QFitModelPolynomWidget::AxisSpin_valueChanged( int num )
{
  if(!fbFillWidget && GetPolynom())
    if(GetPolynom()->SetMaxNumAxis(num))
      UpdateItemsOfType(FitGui::ot_parslist, false);
}
