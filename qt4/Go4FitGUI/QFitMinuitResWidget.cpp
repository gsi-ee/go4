#include "QFitMinuitResWidget.h"

#include "TGo4FitMinuitResult.h"

QFitMinuitResWidget::QFitMinuitResWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}

TGo4FitMinuitResult * QFitMinuitResWidget::GetResult() {
   return dynamic_cast<TGo4FitMinuitResult*> (GetObject());
}

void QFitMinuitResWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   if (GetResult()) InfoLabel->setText("this is minuit result object");
}
