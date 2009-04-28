#include "QFitAmplEstimWidget.h"
#include "TGo4FitAmplEstimation.h"

QFitAmplEstimWidget::QFitAmplEstimWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitAmplEstimation * QFitAmplEstimWidget::GetAmplEstim() {
   return dynamic_cast<TGo4FitAmplEstimation*> (GetObject());
}

void QFitAmplEstimWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   if (GetAmplEstim())
      Iterations->setValue(GetAmplEstim()->GetNumIterations());
}


void QFitAmplEstimWidget::Iterations_valueChanged( int num )
{
  if (!fbFillWidget && GetAmplEstim())
    GetAmplEstim()->SetNumIterations(num);
}
