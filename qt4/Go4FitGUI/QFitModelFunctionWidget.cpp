#include "QFitModelFunctionWidget.h"

//#include "QGo4LineEdit.h"
#include "TGo4FitModelFunction.h"
#include "TGo4FitGuiTypes.h"


QFitModelFunctionWidget::QFitModelFunctionWidget(QWidget *parent, const char* name)
         : QFitModelWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}

TGo4FitModelFunction * QFitModelFunctionWidget::GetFunction() {
   return dynamic_cast<TGo4FitModelFunction*> (GetObject());
}

void QFitModelFunctionWidget::FillSpecificData() {
    QFitNamedWidget::FillSpecificData();
    if (GetFunction()) {
       LibNameEdt->setText(GetFunction()->GetLibraryName());
       FuncNameEdt->setText(GetFunction()->GetFunctionName());
       NumParSpin->setValue(GetFunction()->GetNumberOfFuncPar());
    }
}

void QFitModelFunctionWidget::LibNameEdt_textChanged( const QString & name)
{
   if(!fbFillWidget && GetFunction())
     GetFunction()->SetLibraryName(name);
}

void QFitModelFunctionWidget::FuncNameEdt_textChanged( const QString & name)
{
   if(!fbFillWidget && GetFunction())
      GetFunction()->SetFunctionName(name);
}

void QFitModelFunctionWidget::NumParSpin_valueChanged( int num)
{
  if(!fbFillWidget && GetFunction())
     if(GetFunction()->SetNumberOfFuncPar(num))
       UpdateItemsOfType(FitGui::ot_parslist, false);
}
