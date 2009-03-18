/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

TGo4FitModelFormula * QFitModelFormulaWidget::GetFormula()
{
   return dynamic_cast<TGo4FitModelFormula*> (GetObject());
}

void QFitModelFormulaWidget::FillSpecificData()
{
    QFitNamedWidget::FillSpecificData();
    if (GetFormula()) {
       NumParSpin->setValue(GetFormula()->GetNumberOfExprPar());
       ExprEdt->setText(GetFormula()->GetExpression());
    }
}

void QFitModelFormulaWidget::ExprEdt_textChanged( const QString & str)
{
  if(!fbFillWidget && GetFormula())
    GetFormula()->SetExpression(str);
}

void QFitModelFormulaWidget::NumParSpin_valueChanged( int num)
{
  if(!fbFillWidget && GetFormula())
    if(GetFormula()->SetNumberOfExprPar(num))
      UpdateItemsOfType(FitGui::ot_parslist, false);
}
