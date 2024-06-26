// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "QFitModelFormulaWidget.h"

#include "TGo4FitModelFormula.h"
#include "TGo4FitGuiTypes.h"

QFitModelFormulaWidget::QFitModelFormulaWidget(QWidget *parent, const char *name)
         : QFitModelWidget(parent, name)
{
   setupUi(this);
   QObject::connect(ExprEdt, &QGo4LineEdit::textChanged, this, &QFitModelFormulaWidget::ExprEdt_textChanged);
   QObject::connect(NumParSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &QFitModelFormulaWidget::NumParSpin_valueChanged);

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
    GetFormula()->SetExpression(str.toLatin1().constData());
}

void QFitModelFormulaWidget::NumParSpin_valueChanged( int num)
{
  if(!fbFillWidget && GetFormula())
    if(GetFormula()->SetNumberOfExprPar(num))
      UpdateItemsOfType(FitGui::ot_parslist);
}
