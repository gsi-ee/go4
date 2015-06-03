// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

const char* defMbsEventName = "MbsEvent101";


void TGo4EventInfo::init()
{
   setCaption("Event info");
   ResetWidget();
}

bool TGo4EventInfo::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   return (kind==TGo4Access::kndEventElement);
}

void TGo4EventInfo::DropItem(const char* itemname, TClass* cl, int kind)
{
   if (kind==TGo4Access::kndEventElement)
     WorkWithEvent(itemname);
}

void TGo4EventInfo::WorkWithEvent(const char* itemname)
{
   EventLbl->setText(itemname);

   bool ismbs = strstr(itemname,defMbsEventName)!=0;

   MbsPrintBox->setEnabled(ismbs);
   TString mbsitem = Browser()->FindItemInAnalysis(defMbsEventName);
   MbsButton->setEnabled(!ismbs && (mbsitem.Length()>0));
   setFocus();
}

void TGo4EventInfo::ResetWidget()
{
   QGo4Widget::ResetWidget();
   EventLbl->setText("");
   MbsButton->setEnabled(true);
//   if (Browser()!=0) {
//     TString mbsitem = Browser()->FindItemInAnalysis(defMbsEventName);
//     MbsButton->setEnabled(mbsitem.Length()>0);
//   } else
//     MbsButton->setEnabled(false);
   MbsPrintBox->setEnabled(FALSE);
}

void TGo4EventInfo::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   TTree* tr = dynamic_cast<TTree*>(obj);
   if (tr!=0) {
      tr->Show(0);
      return;
   }

   if (obj!=0) {
      obj->Print();
      return;
   }
}

void TGo4EventInfo::linkedObjectRemoved(const char* linkname)
{
}


void TGo4EventInfo::RefreshClicked()
{
   RemoveAllLinks();

   if (EventLbl->text().length()==0) return;
   QString evname = EventLbl->text();
   bool isremote = RemoteButton->isChecked();
   bool istree = TreeButton->isChecked();

   TGo4Slot* tgtslot = 0;
   if (!isremote)
     tgtslot = AddSlot("Event");

   TGo4BrowserProxy* br = Browser();
   if (br==0) return;

   TString objname;
   TGo4ServerProxy* an = br->DefineAnalysisObject(evname.latin1(), objname);
   if (an!=0) an->RequestEventStatus(objname.Data(), istree, tgtslot);
}

void TGo4EventInfo::PrintEventClicked()
{
   if (EventLbl->text().length()==0) return;

   TString folder, name;
   TGo4Slot::ProduceFolderAndName(EventLbl->text().latin1(), folder, name);

   TGo4AnalysisProxy* anal = GetAnalysis(EventLbl->text().latin1());

   if (anal!=0)
     anal->RemotePrintEvent(name.Data(),
                            PrintEventNum->value(),
                            PrintEventSid->value(),
                            HexBox->isChecked(),
                            LongBox->isChecked());
}

void TGo4EventInfo::MbsButton_clicked()
{
   TString itemname = Browser()->FindItemInAnalysis(defMbsEventName);
   if (itemname.Length()>0)
     WorkWithEvent(itemname.Data());
}
