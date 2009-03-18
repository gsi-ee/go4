/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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
