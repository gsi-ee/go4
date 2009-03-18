#include "QFitOutputActionWidget.h"

//#include "QGo4LineEdit.h"
#include "QFitItem.h"
#include "TGo4FitterOutput.h"


QFitOutputActionWidget::QFitOutputActionWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}


TGo4FitterOutput* QFitOutputActionWidget::GetOutAct()
{
   return dynamic_cast<TGo4FitterOutput*> (GetObject());
}

void QFitOutputActionWidget::FillSpecificData()
{
   QFitNamedWidget::FillSpecificData();
   if (GetOutAct()) {
      if (QString("Draw")==GetOutAct()->GetCommand()) CommandCombo->setCurrentItem(4);
                                                 else CommandCombo->setCurrentItem(0);
      OptionsEdit->setText(GetOutAct()->GetOptions());
   }
}

void QFitOutputActionWidget::OptionsEdit_textChanged( const QString & str)
{
  if(!fbFillWidget && GetOutAct())
    GetOutAct()->SetOptions(str);
}

void QFitOutputActionWidget::CommandCombo_activated( int n)
{
  if(!fbFillWidget && GetOutAct()) {
     if (n==4) GetOutAct()->SetCommand("Draw");
          else GetOutAct()->SetCommand("Print");
     ChangeName(QString(GetOutAct()->GetCommand()));
     switch (n) {
        case 1: OptionsEdit->setText("Pars"); break;
        case 2: OptionsEdit->setText("Ampls"); break;
        case 3: OptionsEdit->setText("Results"); break;
     }
     OptionsEdit->setEnabled((n==0) || (n==4));
  }
}
