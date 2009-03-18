/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

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
