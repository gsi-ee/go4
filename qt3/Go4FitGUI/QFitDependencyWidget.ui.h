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
