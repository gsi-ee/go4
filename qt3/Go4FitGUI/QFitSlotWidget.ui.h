/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

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
     SaveCmb->setCurrentItem(slot->GetSaveFlag());

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
