/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

TGo4FitMinuitResult * QFitMinuitResWidget::GetResult() {
   return dynamic_cast<TGo4FitMinuitResult*> (GetObject());
}

void QFitMinuitResWidget::FillSpecificData() {
   QFitNamedWidget::FillSpecificData();
   if (GetResult()) InfoLabel->setText("this is minuit result object");
}
