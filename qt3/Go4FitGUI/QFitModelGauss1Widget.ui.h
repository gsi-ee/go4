/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


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
