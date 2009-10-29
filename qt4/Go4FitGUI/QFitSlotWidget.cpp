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

#include "QFitSlotWidget.h"

#include "TClass.h"
#include "QFitItem.h"
#include "QGo4LineEdit.h"
#include "TGo4FitSlot.h"
#include "TGo4FitPanel.h"

QFitSlotWidget::QFitSlotWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitSlot* QFitSlotWidget::GetSlot()
{
   return dynamic_cast<TGo4FitSlot*> (GetObject());
}

void QFitSlotWidget::FillSpecificData()
{
   QFitNamedWidget::FillSpecificData();
   TGo4FitSlot* slot = GetSlot();
   if (slot) {
     NameEdt->setReadOnly(TRUE);
     NameEdt->setDisabled(TRUE);

     InfoLbl->setText(QString("Slot of: ")+slot->GetOwner()->GetName() +
                       ",   dedicated for:" + slot->GetClass()->GetName() + " class");
     InfoLbl->adjustSize();

     if (slot->GetObject()) {
        ObjectLbl->setText(QString("Object: ")+slot->GetObjectName() +
                           ",   of class: " + slot->GetObject()->ClassName());
        if(slot->GetOwned()) OwnershipLbl->setText("Object owned by slot");
                        else OwnershipLbl->setText("Object not owned by slot");
     } else {
        ObjectLbl->setText("No object in slot");
        OwnershipLbl->setText("");
     }
     ObjectLbl->adjustSize();
     OwnershipLbl->adjustSize();

     RequirementChk->setChecked(slot->GetNeeded());
     SaveCmb->setCurrentIndex(slot->GetSaveFlag());

     if (fxPanel!=0) {
        SourceLbl->setText(QString("Source: ") + fxPanel->Wiz_GetSlotSourceInfo(slot));
        SourceLbl->adjustSize();
     }
   }
}

void QFitSlotWidget::RequirementChk_toggled( bool req)
{
  if(!fbFillWidget && GetSlot())
    GetSlot()->SetNeeded(req);
}


void QFitSlotWidget::SaveCmb_activated( int typ )
{
  if(!fbFillWidget && GetSlot())
    GetSlot()->SetSaveFlag(typ);
}
