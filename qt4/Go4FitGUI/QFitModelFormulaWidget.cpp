#include "QFitModelFormulaWidget.h"

#include "TGo4FitModelFormula.h"
#include "TGo4FitGuiTypes.h"

QFitModelFormulaWidget::QFitModelFormulaWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{
   setupUi(this);
}

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
    GetFormula()->SetExpression(str.toAscii());
}

void QFitModelFormulaWidget::NumParSpin_valueChanged( int num)
{
  if(!fbFillWidget && GetFormula())
    if(GetFormula()->SetNumberOfExprPar(num))
      UpdateItemsOfType(FitGui::ot_parslist, false);
}
