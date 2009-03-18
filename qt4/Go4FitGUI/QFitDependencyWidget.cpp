#include "QFitDependencyWidget.h"

#include "QFitItem.h"
#include "TObjArray.h"
#include "TGo4FitDependency.h"
#include "QFitItem.h"
#include "TObjArray.h"
#include "TGo4FitDependency.h" 

QFitDependencyWidget::QFitDependencyWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}

TGo4FitDependency * QFitDependencyWidget::GetDepen() {
  return dynamic_cast<TGo4FitDependency*> (GetObject());
}

void QFitDependencyWidget::FillSpecificData() {
   if(GetDepen()) {
       if (GetDepen()->IsResultDepend()) {
          TObjArray* lst = dynamic_cast<TObjArray*> (GetItem()->Parent()->Object());
          ParamEdit->setText(QString("Result ")+QString::number(lst->IndexOf(GetObject())));
          ParamEdit->setReadOnly(TRUE);
          ParamEdit->setEnabled(FALSE);
       } else {
          ParamEdit->setText(GetDepen()->GetParameter().Data());
          ParamEdit->setReadOnly(FALSE);
          ParamEdit->setEnabled(TRUE);
       }
       if (GetDepen()->IsInitValue()) ExpressionEdt->setText(QString::number(GetDepen()->GetInitValue()));
                                 else ExpressionEdt->setText(GetDepen()->GetExpression().Data());
   }
}



void QFitDependencyWidget::ParamEdit_textChanged( const QString & param)
{
  if(!fbFillWidget && GetDepen() && (param.length()>0)) {
     GetDepen()->SetParameter(param);
     SetWidgetItemText(true);
  }
}


void QFitDependencyWidget::ExpressionEdt_textChanged( const QString & value)
{
  if(!fbFillWidget && GetDepen() && (value.length()>0)) {
     bool ok = FALSE;
     double zn = value.toDouble(&ok);
     if (ok) GetDepen()->SetInitValue(zn);
        else GetDepen()->SetExpression(value);
     SetWidgetItemText(true);
  }
}
