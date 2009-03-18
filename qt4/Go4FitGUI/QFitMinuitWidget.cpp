#include "QFitMinuitWidget.h"

#include "TGo4FitMinuit.h"

QFitMinuitWidget::QFitMinuitWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}

TGo4FitMinuit * QFitMinuitWidget::GetMinuit() {
    return dynamic_cast<TGo4FitMinuit*> (GetObject());
}

void QFitMinuitWidget::FillSpecificData() {
  QFitNamedWidget::FillSpecificData();
  TGo4FitMinuit* minuit = GetMinuit();
  if (minuit) {
      QString cmds;
      char s[3]; s[0] = 13; s[1] = 10; s[2]=0;
      for(int i=0;i<minuit->GetNumCommands();i++) {
         cmds+=minuit->GetCommand(i);
         if (i<minuit->GetNumCommands()-1) cmds+=s;
      }
      CommandsEdit->setText(cmds);
   } else CommandsEdit->setText("");
}


void QFitMinuitWidget::CommandsEdit_textChanged()
{
   TGo4FitMinuit* minuit = GetMinuit();
   if (minuit==0) return;

   minuit->ClearCommands();
   for(int n=0;n<CommandsEdit->paragraphs();n++)
     minuit->AddCommand(CommandsEdit->text(n));
}
