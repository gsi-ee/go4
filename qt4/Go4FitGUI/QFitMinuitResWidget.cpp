#include "QFitMinuitResWidget.h"

#include "TGo4FitMinuitResult.h"

QFitMinuitResWidget::QFitMinuitResWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitMinuitResult * QFitMinuitResWidget::GetResult() {
   return dynamic_cast<TGo4FitMinuitResult*> (GetObject());
}

void QFitMinuitResWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   if (GetResult()) InfoLabel->setText("this is minuit result object");
}
