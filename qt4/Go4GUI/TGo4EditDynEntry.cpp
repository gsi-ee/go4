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

#include "TGo4EditDynEntry.h"

#include <QMessageBox>
#include <QToolTip>

#include "TClass.h"
#include "TH1.h"
#include "TTree.h"
#include "QGo4LineEdit.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4HistogramEntry.h"
#include "TGo4DynamicEntry.h"
#include "TGo4Condition.h"
#include "TGo4Slot.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"


TGo4EditDynEntry::TGo4EditDynEntry(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
   setupUi(this);

   fbTypingMode = true;
   fiSelectedType = entry_None;
   ResetWidget();
}

bool TGo4EditDynEntry::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   if (cl==0) return false;

   return cl->InheritsFrom(TGo4DynamicEntry::Class()) ||
          cl->InheritsFrom(TH1::Class()) ||
          cl->InheritsFrom(TTree::Class()) ||
          cl->InheritsFrom(TGo4Condition::Class());
}

void TGo4EditDynEntry::DropItem(const char* itemname, TClass* cl, int kind)
{
   if (cl==0) return;

   if (cl->InheritsFrom(TGo4DynamicEntry::Class()))
      WorkWithEntry(itemname);
   else
   if (cl->InheritsFrom(TH1::Class()))
      DropHistogram(itemname);
   else
   if (cl->InheritsFrom(TGo4Condition::Class()))
      DropCondition(itemname);
   else
   if (cl->InheritsFrom(TTree::Class()))
      DropTree(itemname);
}

void TGo4EditDynEntry::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname, "Entry")==0)
     SetPleaseUpdate(false);

   RefreshWidget();
}

void TGo4EditDynEntry::RefreshWidget()
{
   TGo4DynamicEntry* entry =
      dynamic_cast<TGo4DynamicEntry*>(GetLinked("Entry", 0));

   TGo4BrowserProxy* br = Browser();

   if ((entry==0) || (br==0)) return;

   fiSelectedType = entry_None;

   fbTypingMode = false;

   EntryBox->setTitle(QString("Entry: ") + entry->ClassName());
   EntryEnabledButton->setChecked(entry->IsEnabledProcessing());

   EntryBox->setEnabled(TRUE);
   HistogramBox->setEnabled(TRUE);
   EntryTabs->setEnabled(TRUE);

   TGo4HistogramEntry* hentry = dynamic_cast<TGo4HistogramEntry*> (entry);
   TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (entry);

   const char* entryitem = GetLinkedName("Entry");

   if (hentry!=0) {
       fiSelectedType = entry_Histogram;

       EntryTabs->setTabEnabled(EntryTabs->indexOf(EventTab), TRUE);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(ConditionTab), TRUE);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(TreeTab), FALSE);
       EntryTabs->setCurrentIndex(EntryTabs->indexOf(EventTab));

       TString hitemname;
       if (!br->DefineRelatedObject(entryitem,
         hentry->GetHistogramName(), hitemname))
           hitemname = hentry->GetHistogramName();
       HistoNameLbl->setText(hitemname.Data());
       DrawButton->setVisible(hitemname.Length()>0);
       ClearButton->setVisible(hitemname.Length()>0);

       QString fname = hentry->GetHistEventName(0);
       if (fname.length()>0) fname += "/";
       fname += hentry->GetHistVarName(0);
       EvXnameEdit->setText(fname);

       fname = hentry->GetHistEventName(1);
       if (fname.length()>0) fname += "/";
       fname += hentry->GetHistVarName(1);
       EvYnameEdit->setText(fname);

       fname = hentry->GetHistEventName(2);
       if (fname.length()>0) fname += "/";
       fname += hentry->GetHistVarName(2);
       EvZnameEdit->setText(fname);

       TString citemname;
       if (!br->DefineRelatedObject(entryitem,
         hentry->GetConditionName(), citemname))
           citemname = hentry->GetConditionName();
       CondNameLbl->setText(citemname.Data());

       fname = hentry->GetConEventName(0);
       if (fname.length()>0) fname += "/";
       fname += hentry->GetConVarName(0);
       ConXnameEdit->setText(fname);

       fname = hentry->GetConEventName(1);
       if (fname.length()>0) fname += "/";
       fname += hentry->GetConVarName(1);
       ConYnameEdit->setText(fname);
   } else
   if (tentry!=0) {
       fiSelectedType = entry_Tree;

       EntryTabs->setTabEnabled(EntryTabs->indexOf(EventTab), FALSE);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(ConditionTab), FALSE);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(TreeTab), TRUE);
       EntryTabs->setCurrentIndex(EntryTabs->indexOf(TreeTab));

       TString hitemname;
       if (!br->DefineRelatedObject(entryitem,
         tentry->GetHistogramName(), hitemname))
           hitemname = tentry->GetHistogramName();
       HistoNameLbl->setText(hitemname.Data());
       DrawButton->setVisible(hitemname.Length()>0);
       ClearButton->setVisible(hitemname.Length()>0);

       TString titemname;
       if (!br->DefineRelatedObject(entryitem,
         tentry->GetTreeName(), titemname))
           titemname = tentry->GetTreeName();
       TreeNameLbl->setText(titemname.Data());

       DrawExprEdit->setText(tentry->GetVarExp());
       CutExprEdit->setText(tentry->GetCutExp());
       DynIntervalSpin->setValue(tentry->GetDynListInterval());
   }

   fbTypingMode = true;
}

QString TGo4EditDynEntry::GetEventName(const QString& itemname)
{
   TString folder, name;
   TGo4Slot::ProduceFolderAndName(itemname.toAscii().constData(), folder, name);
// SL 2.12.2011 Disable this code while it was used to remove parent class names from event name
// now parent class names could remain while in case of composite event it may be important information
//   int pos = folder.Index("/");
//   if (pos>0) folder.Resize(pos);
   return QString(folder.Data());
}

QString TGo4EditDynEntry::GetEventMember(const QString& itemname)
{
   TString folder, name;
   TGo4Slot::ProduceFolderAndName(itemname.toAscii().constData(), folder, name);
   return QString(name.Data());
}

void TGo4EditDynEntry::WorkWithEntry(const char* itemname)
{
   TGo4DynamicEntry* entry =
      dynamic_cast<TGo4DynamicEntry*>(GetLinked("Entry", 0));
   const char* oldname = GetLinkedName("Entry");

   if ((entry!=0) && (oldname!=0) && PleaseUpdateLabel->isVisible()) {

      int res = QMessageBox::warning(this, "Dynamic list entry editor",
        QString("Current entry ")+oldname+" is modified!\n New entry" +
        itemname+ " selected.",
        QString("Continue with current"),
        QString("Start with new"), QString::null, 0);
//        (BrowserItemRemote(oldname) ? QString("Update current in analysis and work with new") : QString::null), 0);
      if (res==0) return;
//      if (res==2) UpdateItemInAnalysis(oldname);
   }

   ResetWidget();

   AddLink(itemname, "Entry");

   EntryNameLbl->setText(itemname);

   QString iconname = ":/icons/right.png";
   QString tooltip = "Refresh dynamic entry from analysis";
   if (!BrowserItemRemote(itemname)) {
      iconname = ":/icons/refresh.png";
      tooltip = "Refresh dynamic entry from source";
   }
   RefreshButton->setIcon( QIcon(iconname) );
   RefreshButton->setToolTip(tooltip);

   GetLinked("Entry", 2);
   RefreshWidget();

   SetPleaseUpdate(false);
   parentWidget()->adjustSize();
   parentWidget()->setFocus();
}

void TGo4EditDynEntry::DropHistogram(const char * itemname)
{
   if (itemname==0) return;

   EntryChangedSlot();

   HistoNameLbl->setText(itemname);
   DrawButton->setVisible(true);
   ClearButton->setVisible(true);

   TGo4DynamicEntry* entry =
     dynamic_cast<TGo4DynamicEntry*>(GetLinked("Entry", 0));

   TGo4HistogramEntry* hentry = dynamic_cast<TGo4HistogramEntry*> (entry);
   TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (entry);
   if (hentry!=0)
      hentry->SetHistogramName(itemname);
   if (tentry!=0)
      tentry->SetHistogramName(itemname);
}

void TGo4EditDynEntry::DropCondition( const char * itemname )
{
   if (fiSelectedType != entry_Histogram) return;

   EntryChangedSlot();

   CondNameLbl->setText(itemname);

   EntryTabs->setCurrentIndex(EntryTabs->indexOf(ConditionTab));
   TGo4HistogramEntry* hentry =
     dynamic_cast<TGo4HistogramEntry*> (GetLinked("Entry", 0));
   if (hentry!=0)
     hentry->SetConditionName(itemname);
}

void TGo4EditDynEntry::DropTree( const char * itemname )
{
   if (fiSelectedType != entry_Tree) return;

   EntryChangedSlot();

   TreeNameLbl->setText(itemname);

   TGo4TreeHistogramEntry* tentry =
     dynamic_cast<TGo4TreeHistogramEntry*> (GetLinked("Entry", 0));
   if (tentry!=0)
     tentry->SetTreeName(itemname);
}

void TGo4EditDynEntry::ResetWidget()
{
   QGo4Widget::ResetWidget();

   fiSelectedType = entry_None;

   EntryNameLbl->setText("");
   EntryBox->setTitle(QString("Entry: "));

   EntryBox->setEnabled(FALSE);
   HistogramBox->setEnabled(FALSE);
   EntryTabs->setEnabled(FALSE);

   DrawButton->setVisible(false);

   RefreshButton->setIcon( QIcon(":/icons/right.png" ) );

   SetPleaseUpdate(false);
}

void TGo4EditDynEntry::RefreshClicked()
{
   GetLinked("Entry", 2);
   RefreshWidget();
}

void TGo4EditDynEntry::ApplyClicked()
{
   if (!PrepareEntryForAnalysis()) return;

   if (UpdateItemInAnalysis(GetLinkedName("Entry"))) {
      SetPleaseUpdate(false);
      RefreshWidget();
   }
}

void TGo4EditDynEntry::SetPleaseUpdate( bool on )
{
   PleaseUpdateLabel->setVisible(on);
   ensurePolished();
   update();
   show();
}

void TGo4EditDynEntry::EntryChangedSlot()
{
   if(!PleaseUpdateLabel->isVisible())
      SetPleaseUpdate(true);
}

void TGo4EditDynEntry::SaveEntrySlot()
{
   if (!PrepareEntryForAnalysis()) return;

   if (SaveItemToFile(GetLinkedName("Entry"), "DynamicLists"))
       SetPleaseUpdate(false);
}

void TGo4EditDynEntry::EvXnameEdit_dropped()
{
   ProcessDropEvent(EvXnameEdit);
}

void TGo4EditDynEntry::EvYnameEdit_dropped()
{
   ProcessDropEvent(EvYnameEdit);
}

void TGo4EditDynEntry::EvZnameEdit_dropped()
{
   ProcessDropEvent(EvZnameEdit);
}

void TGo4EditDynEntry::ConXnameEdit_dropped()
{
   ProcessDropEvent(ConXnameEdit);
}

void TGo4EditDynEntry::ConYnameEdit_dropped()
{
   ProcessDropEvent(ConYnameEdit);
}

void TGo4EditDynEntry::DrawExprEdit_dropped()
{
   ProcessTreeDropEvent(DrawExprEdit);
}

void TGo4EditDynEntry::CutExprEdit_dropped()
{
   ProcessTreeDropEvent(CutExprEdit);
}

void TGo4EditDynEntry::ProcessDropEvent(QGo4LineEdit* edt)
{
   if ((edt==0) || (Browser()==0)) return;

   QString value = edt->text();

   EntryChangedSlot();

   Int_t kind = Browser()->ItemKind(value.toAscii().constData());
   if (kind!=TGo4Access::kndDataMember) {
      edt->setText("");
      return;
   }

   QString serachfor("EventObjects/Events/");
   int pos = value.indexOf(serachfor);
   if (pos<0) {
      edt->setText("");
      return;
   }

   value.remove(0, pos+serachfor.length());

   edt->setText(value);
}

void TGo4EditDynEntry::ProcessTreeDropEvent(QGo4LineEdit* edt)
{
   if ((edt==0) || (Browser()==0)) return;

   QString value = edt->text();

   TGo4BrowserProxy* br = Browser();

   EntryChangedSlot();

   int kind = br->ItemKind(value.toAscii().constData());
   if (kind!=TGo4Access::kndTreeLeaf) {
      edt->setText("");
      return;
   }

   TString treename;
   if (br->DefineTreeName(value.toAscii().constData(), treename))
     DropTree(treename.Data());

   TString lfolder, lname;
   TGo4Slot::ProduceFolderAndName(value.toAscii().constData(), lfolder, lname);
   edt->setText(lname.Data());
}


void TGo4EditDynEntry::HisCreateButton_clicked()
{
   AskToCreateObject(TH1::Class(), 1);
}


void TGo4EditDynEntry::HisInfoButton_clicked()
{
   ShowItemInfo(HistoNameLbl->text());
}

void TGo4EditDynEntry::ConNewButton_clicked()
{
   AskToCreateObject(TGo4Condition::Class(), 1);
}

void TGo4EditDynEntry::ConInfoButton_clicked()
{
   EditItem(CondNameLbl->text());
}

void TGo4EditDynEntry::requestedObjectCreated(const char* itemname, TClass* cl)
{
   if ((cl==0) || (fiSelectedType==entry_None)) return;
   if (cl->InheritsFrom(TH1::Class()))
      DropHistogram(itemname);
   else
   if (cl->InheritsFrom(TGo4Condition::Class()))
      DropCondition(itemname);
}

bool TGo4EditDynEntry::PrepareEntryForAnalysis()
{
   TGo4DynamicEntry* entry =
     dynamic_cast<TGo4DynamicEntry*>(GetLinked("Entry", 0));

   if (entry==0) return false;

   TGo4HistogramEntry* hentry = dynamic_cast<TGo4HistogramEntry*> (entry);
   TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (entry);

   if (hentry!=0) {
       TString folder, name;

       TGo4Slot::ProduceFolderAndName(hentry->GetHistogramName(), folder, name);
       hentry->SetHistogramName(name.Data());

       TGo4Slot::ProduceFolderAndName(hentry->GetConditionName(), folder, name);
       hentry->SetConditionName(name.Data());

       return true;
   }

   if (tentry!=0) {
      TString folder, name;

      TGo4Slot::ProduceFolderAndName(tentry->GetHistogramName(), folder, name);
      tentry->SetHistogramName(name.Data());

      TGo4Slot::ProduceFolderAndName(tentry->GetTreeName(), folder, name);
      tentry->SetTreeName(name.Data());
      return true;
   }

   return false;
}


void TGo4EditDynEntry::EntryEnabledButton_toggled( bool on)
{
   TGo4DynamicEntry* entry =
     dynamic_cast<TGo4DynamicEntry*>(GetLinked("Entry", 0));
   if ((entry==0) || !fbTypingMode) return;
   entry->EnableProcessing(on);
   EntryChangedSlot();
}


void TGo4EditDynEntry::EvXnameEdit_textChanged( const QString & xname)
{
   TGo4HistogramEntry* hentry =
     dynamic_cast<TGo4HistogramEntry*> (GetLinked("Entry", 0));
   if ((hentry==0) || !fbTypingMode) return;

   hentry->SetHisEventName(0, GetEventName(xname).toAscii().constData());
   hentry->SetHisVarName(0, GetEventMember(xname).toAscii().constData());
}


void TGo4EditDynEntry::EvYnameEdit_textChanged( const QString & yname)
{
   TGo4HistogramEntry* hentry =
     dynamic_cast<TGo4HistogramEntry*> (GetLinked("Entry", 0));
   if ((hentry==0) || !fbTypingMode) return;

   hentry->SetHisEventName(1, GetEventName(yname).toAscii().constData());
   hentry->SetHisVarName(1, GetEventMember(yname).toAscii().constData());
}


void TGo4EditDynEntry::EvZnameEdit_textChanged( const QString & zname)
{
   TGo4HistogramEntry* hentry =
     dynamic_cast<TGo4HistogramEntry*> (GetLinked("Entry", 0));
   if ((hentry==0) || !fbTypingMode) return;

   hentry->SetHisEventName(2, GetEventName(zname).toAscii().constData());
   hentry->SetHisVarName(2, GetEventMember(zname).toAscii().constData());
}


void TGo4EditDynEntry::ConXnameEdit_textChanged( const QString & xname)
{
   TGo4HistogramEntry* hentry =
     dynamic_cast<TGo4HistogramEntry*> (GetLinked("Entry", 0));
   if ((hentry==0) || !fbTypingMode) return;

   hentry->SetConEventName(0, GetEventName(xname).toAscii().constData());
   hentry->SetConVarName(0, GetEventMember(xname).toAscii().constData());
}


void TGo4EditDynEntry::ConYnameEdit_textChanged( const QString & yname)
{
   TGo4HistogramEntry* hentry =
     dynamic_cast<TGo4HistogramEntry*> (GetLinked("Entry", 0));
   if ((hentry==0) || !fbTypingMode) return;

   hentry->SetConEventName(1, GetEventName(yname).toAscii().constData());
   hentry->SetConVarName(1, GetEventMember(yname).toAscii().constData());
}


void TGo4EditDynEntry::DrawExprEdit_textChanged( const QString & value)
{
   TGo4TreeHistogramEntry* tentry =
      dynamic_cast<TGo4TreeHistogramEntry*> (GetLinked("Entry", 0));
   if ((tentry==0) || !fbTypingMode) return;
   tentry->SetVarExp(value.toAscii().constData());
}

void TGo4EditDynEntry::CutExprEdit_textChanged( const QString & value)
{
   TGo4TreeHistogramEntry* tentry =
      dynamic_cast<TGo4TreeHistogramEntry*> (GetLinked("Entry", 0));
   if ((tentry==0) || !fbTypingMode) return;
   tentry->SetCutExp(value.toAscii().constData());
}

void TGo4EditDynEntry::DynIntervalSpin_valueChanged( int value)
{
   TGo4TreeHistogramEntry* tentry =
      dynamic_cast<TGo4TreeHistogramEntry*> (GetLinked("Entry", 0));
   if ((tentry==0) || !fbTypingMode) return;
   tentry->SetDynListInterval(value);
}


void TGo4EditDynEntry::DrawButton_clicked()
{
   DrawItem(HistoNameLbl->text());
}

void TGo4EditDynEntry::ClearHistogramClicked()
{
   TString objname;
   TGo4AnalysisProxy* an = Browser()->DefineAnalysisObject(HistoNameLbl->text().toAscii().constData(), objname);
   if (an!=0) {
      an->ClearAnalysisObject(objname.Data());
      if (dynamic_cast<TGo4TreeHistogramEntry*>(GetLinked("Entry",0))!=0)
         an->ExecuteLine("@ResetBackStores();");
      Browser()->GetBrowserObject(HistoNameLbl->text().toAscii().constData(), 2);
   }
}

void TGo4EditDynEntry::PrintDynList()
{
   TString objname;
   TGo4AnalysisProxy* an = Browser()->DefineAnalysisObject(GetLinkedName("Entry"), objname);

   if (an!=0)
     an->PrintDynListEntry(objname.Data());
}

void TGo4EditDynEntry::ConRemoveButton_clicked()
{
   DropCondition("");
}
