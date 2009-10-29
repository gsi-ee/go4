// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


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
