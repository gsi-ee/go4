// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
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

#include "TGo4TreeHistogramEntry.h"
#include "TGo4HistogramEntry.h"
#include "TGo4Condition.h"
#include "TGo4Slot.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"

TGo4EditDynEntry::TGo4EditDynEntry(QWidget *parent, const char *name)
   : QGo4Widget(parent, name)
{
   setupUi(this);

   QObject::connect(ApplyButton, &QPushButton::clicked, this, &TGo4EditDynEntry::ApplyClicked);
   QObject::connect(ClearButton, &QPushButton::clicked, this, &TGo4EditDynEntry::ClearHistogramClicked);
   QObject::connect(PrintListButton, &QPushButton::clicked, this, &TGo4EditDynEntry::PrintDynList);
   QObject::connect(RefreshButton, &QPushButton::clicked, this, &TGo4EditDynEntry::RefreshClicked);
   QObject::connect(SaveEntry, &QPushButton::clicked, this, &TGo4EditDynEntry::SaveEntrySlot);
   QObject::connect(CancelButton, &QPushButton::clicked, this, &TGo4EditDynEntry::CloseMDIParentSlot);
   QObject::connect(EvXnameEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::EvXnameEdit_dropped);
   QObject::connect(EvYnameEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::EvYnameEdit_dropped);
   QObject::connect(EvZnameEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::EvZnameEdit_dropped);
   QObject::connect(ConXnameEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::ConXnameEdit_dropped);
   QObject::connect(ConYnameEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::ConYnameEdit_dropped);
   QObject::connect(DrawExprEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::DrawExprEdit_dropped);
   QObject::connect(CutExprEdit, &QGo4LineEdit::textDropped, this, &TGo4EditDynEntry::CutExprEdit_dropped);
   QObject::connect(HisCreateButton, &QPushButton::clicked, this, &TGo4EditDynEntry::HisCreateButton_clicked);
   QObject::connect(HisInfoButton, &QPushButton::clicked, this, &TGo4EditDynEntry::HisInfoButton_clicked);
   QObject::connect(ConNewButton, &QPushButton::clicked, this, &TGo4EditDynEntry::ConNewButton_clicked);
   QObject::connect(ConInfoButton, &QPushButton::clicked, this, &TGo4EditDynEntry::ConInfoButton_clicked);
   QObject::connect(EntryEnabledButton, &QCheckBox::toggled, this, &TGo4EditDynEntry::EntryEnabledButton_toggled);
   QObject::connect(EvXnameEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::EvXnameEdit_textChanged);
   QObject::connect(EvYnameEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::EvYnameEdit_textChanged);
   QObject::connect(EvZnameEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::EvZnameEdit_textChanged);
   QObject::connect(ConXnameEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::ConXnameEdit_textChanged);
   QObject::connect(ConYnameEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::ConYnameEdit_textChanged);
   QObject::connect(DrawExprEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::DrawExprEdit_textChanged);
   QObject::connect(CutExprEdit, &QGo4LineEdit::textChanged, this, &TGo4EditDynEntry::CutExprEdit_textChanged);
   QObject::connect(DynIntervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TGo4EditDynEntry::DynIntervalSpin_valueChanged);
   QObject::connect(DrawButton, &QPushButton::clicked, this, &TGo4EditDynEntry::DrawButton_clicked);
   QObject::connect(ConRemoveButton, &QPushButton::clicked, this, &TGo4EditDynEntry::ConRemoveButton_clicked);

   fbTypingMode = true;
   fiSelectedType = entry_None;
   ResetWidget();
}

bool TGo4EditDynEntry::IsAcceptDrag(const char *itemname, TClass* cl, int kind)
{
   if (!cl) return false;

   return cl->InheritsFrom(TGo4DynamicEntry::Class()) ||
          cl->InheritsFrom(TH1::Class()) ||
          cl->InheritsFrom(TTree::Class()) ||
          cl->InheritsFrom(TGo4Condition::Class());
}

void TGo4EditDynEntry::DropItem(const char *itemname, TClass* cl, int kind)
{
   if (!cl) return;

   if (cl->InheritsFrom(TGo4DynamicEntry::Class()))
      WorkWithEntry(itemname);
   else if (cl->InheritsFrom(TH1::Class()))
      DropHistogram(itemname);
   else if (cl->InheritsFrom(TGo4Condition::Class()))
      DropCondition(itemname);
   else if (cl->InheritsFrom(TTree::Class()))
      DropTree(itemname);
}

void TGo4EditDynEntry::linkedObjectUpdated(const char *linkname, TObject *obj)
{
   if (strcmp(linkname, "Entry") == 0)
     SetPleaseUpdate(false);

   RefreshWidget();
}

void TGo4EditDynEntry::RefreshWidget()
{
   auto entry = GetLinkedCast<TGo4DynamicEntry>("Entry");

   TGo4BrowserProxy* br = Browser();

   if (!entry || !br) return;

   fiSelectedType = entry_None;

   fbTypingMode = false;

   EntryBox->setTitle(QString("Entry: ") + entry->ClassName());
   EntryEnabledButton->setChecked(entry->IsEnabledProcessing());

   EntryBox->setEnabled(true);
   HistogramBox->setEnabled(true);
   EntryTabs->setEnabled(true);

   TGo4HistogramEntry* hentry = dynamic_cast<TGo4HistogramEntry*> (entry);
   TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (entry);

   const char* entryitem = GetLinkedName("Entry");

   if (hentry) {
       fiSelectedType = entry_Histogram;

       EntryTabs->setTabEnabled(EntryTabs->indexOf(EventTab), true);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(ConditionTab), true);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(TreeTab), false);
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

   } else if (tentry) {

       fiSelectedType = entry_Tree;

       EntryTabs->setTabEnabled(EntryTabs->indexOf(EventTab), false);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(ConditionTab), false);
       EntryTabs->setTabEnabled(EntryTabs->indexOf(TreeTab), true);
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
   TGo4Slot::ProduceFolderAndName(itemname.toLatin1().constData(), folder, name);
// SL 2.12.2011 Disable this code while it was used to remove parent class names from event name
// now parent class names could remain while in case of composite event it may be important information
//   int pos = folder.Index("/");
//   if (pos>0) folder.Resize(pos);
   return QString(folder.Data());
}

QString TGo4EditDynEntry::GetEventMember(const QString& itemname)
{
   TString folder, name;
   TGo4Slot::ProduceFolderAndName(itemname.toLatin1().constData(), folder, name);
   return QString(name.Data());
}

void TGo4EditDynEntry::WorkWithEntry(const char *itemname)
{
   auto entry = GetLinkedCast<TGo4DynamicEntry>("Entry");

   const char* oldname = GetLinkedName("Entry");

   if (entry && oldname && PleaseUpdateLabel->isVisible()) {

      QMessageBox msgBox(QMessageBox::Question, "Dynamic list entry editor",
                         QString("Current entry %1 is modified!\nNew entry %2 is selected.").arg(oldname).arg(itemname));

      auto btnContinue = msgBox.addButton("Continue with current", QMessageBox::ActionRole);
      auto btnNew = msgBox.addButton("Start with new", QMessageBox::ActionRole);
      msgBox.setDefaultButton(btnContinue);

      msgBox.exec();
      if (msgBox.clickedButton() == btnContinue)
         return;
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
   if (!itemname) return;

   EntryChangedSlot();

   HistoNameLbl->setText(itemname);
   DrawButton->setVisible(true);
   ClearButton->setVisible(true);

   auto entry = GetLinkedCast<TGo4DynamicEntry>("Entry");
   TGo4HistogramEntry* hentry = dynamic_cast<TGo4HistogramEntry*> (entry);
   TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (entry);
   if (hentry)
      hentry->SetHistogramName(itemname);
   if (tentry)
      tentry->SetHistogramName(itemname);
}

void TGo4EditDynEntry::DropCondition( const char * itemname )
{
   if (fiSelectedType != entry_Histogram) return;

   EntryChangedSlot();

   CondNameLbl->setText(itemname);

   EntryTabs->setCurrentIndex(EntryTabs->indexOf(ConditionTab));
   auto hentry = GetLinkedCast<TGo4HistogramEntry>("Entry");
   if (hentry)
     hentry->SetConditionName(itemname);
}

void TGo4EditDynEntry::DropTree( const char * itemname )
{
   if (fiSelectedType != entry_Tree) return;

   EntryChangedSlot();

   TreeNameLbl->setText(itemname);

   auto tentry = GetLinkedCast<TGo4TreeHistogramEntry>("Entry");
   if (tentry)
      tentry->SetTreeName(itemname);
}

void TGo4EditDynEntry::ResetWidget()
{
   QGo4Widget::ResetWidget();

   fiSelectedType = entry_None;

   EntryNameLbl->setText("");
   EntryBox->setTitle(QString("Entry: "));

   EntryBox->setEnabled(false);
   HistogramBox->setEnabled(false);
   EntryTabs->setEnabled(false);

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
   if (!edt || !Browser()) return;

   QString value = edt->text();

   EntryChangedSlot();

   Int_t kind = Browser()->ItemKind(value.toLatin1().constData());
   if (kind!=TGo4Access::kndDataMember) {
      edt->setText("");
      return;
   }

   QString serachfor("EventObjects/Events/");
   int pos = value.indexOf(serachfor);
   if (pos < 0) {
      edt->setText("");
      return;
   }

   value.remove(0, pos+serachfor.length());

   edt->setText(value);
}

void TGo4EditDynEntry::ProcessTreeDropEvent(QGo4LineEdit* edt)
{
   if (!edt || !Browser()) return;

   QString value = edt->text();

   TGo4BrowserProxy* br = Browser();

   EntryChangedSlot();

   int kind = br->ItemKind(value.toLatin1().constData());
   if (kind!=TGo4Access::kndTreeLeaf) {
      edt->setText("");
      return;
   }

   TString treename;
   if (br->DefineTreeName(value.toLatin1().constData(), treename))
     DropTree(treename.Data());

   TString lfolder, lname;
   TGo4Slot::ProduceFolderAndName(value.toLatin1().constData(), lfolder, lname);
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

void TGo4EditDynEntry::requestedObjectCreated(const char *itemname, TClass* cl)
{
   if (!cl || (fiSelectedType==entry_None)) return;
   if (cl->InheritsFrom(TH1::Class()))
      DropHistogram(itemname);
   else
   if (cl->InheritsFrom(TGo4Condition::Class()))
      DropCondition(itemname);
}

bool TGo4EditDynEntry::PrepareEntryForAnalysis()
{
   auto entry = GetLinkedCast<TGo4DynamicEntry>("Entry");

   if (!entry) return false;

   TGo4HistogramEntry* hentry = dynamic_cast<TGo4HistogramEntry*> (entry);
   TGo4TreeHistogramEntry* tentry = dynamic_cast<TGo4TreeHistogramEntry*> (entry);

   if (hentry) {
       TString folder, name;

       TGo4Slot::ProduceFolderAndName(hentry->GetHistogramName(), folder, name);
       hentry->SetHistogramName(name.Data());

       TGo4Slot::ProduceFolderAndName(hentry->GetConditionName(), folder, name);
       hentry->SetConditionName(name.Data());

       return true;
   }

   if (tentry) {
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
   auto entry = GetLinkedCast<TGo4DynamicEntry>("Entry");
   if (!entry || !fbTypingMode) return;
   entry->EnableProcessing(on);
   EntryChangedSlot();
}

void TGo4EditDynEntry::EvXnameEdit_textChanged( const QString & xname)
{
   auto hentry = GetLinkedCast<TGo4HistogramEntry>("Entry");
   if (!hentry || !fbTypingMode) return;

   hentry->SetHisEventName(0, GetEventName(xname).toLatin1().constData());
   hentry->SetHisVarName(0, GetEventMember(xname).toLatin1().constData());
}


void TGo4EditDynEntry::EvYnameEdit_textChanged( const QString & yname)
{
   auto hentry = GetLinkedCast<TGo4HistogramEntry>("Entry");
   if (!hentry || !fbTypingMode) return;

   hentry->SetHisEventName(1, GetEventName(yname).toLatin1().constData());
   hentry->SetHisVarName(1, GetEventMember(yname).toLatin1().constData());
}


void TGo4EditDynEntry::EvZnameEdit_textChanged( const QString & zname)
{
   auto hentry = GetLinkedCast<TGo4HistogramEntry>("Entry");
   if (!hentry || !fbTypingMode) return;

   hentry->SetHisEventName(2, GetEventName(zname).toLatin1().constData());
   hentry->SetHisVarName(2, GetEventMember(zname).toLatin1().constData());
}


void TGo4EditDynEntry::ConXnameEdit_textChanged( const QString & xname)
{
   auto hentry = GetLinkedCast<TGo4HistogramEntry>("Entry");
   if (!hentry || !fbTypingMode) return;

   hentry->SetConEventName(0, GetEventName(xname).toLatin1().constData());
   hentry->SetConVarName(0, GetEventMember(xname).toLatin1().constData());
}


void TGo4EditDynEntry::ConYnameEdit_textChanged( const QString & yname)
{
   auto hentry = GetLinkedCast<TGo4HistogramEntry>("Entry");
   if (!hentry || !fbTypingMode) return;

   hentry->SetConEventName(1, GetEventName(yname).toLatin1().constData());
   hentry->SetConVarName(1, GetEventMember(yname).toLatin1().constData());
}


void TGo4EditDynEntry::DrawExprEdit_textChanged( const QString & value)
{
   auto tentry = GetLinkedCast<TGo4TreeHistogramEntry>("Entry");
   if (!tentry || !fbTypingMode) return;
   tentry->SetVarExp(value.toLatin1().constData());
}

void TGo4EditDynEntry::CutExprEdit_textChanged( const QString & value)
{
   auto tentry = GetLinkedCast<TGo4TreeHistogramEntry>("Entry");
   if (!tentry || !fbTypingMode) return;
   tentry->SetCutExp(value.toLatin1().constData());
}

void TGo4EditDynEntry::DynIntervalSpin_valueChanged( int value)
{
   auto tentry = GetLinkedCast<TGo4TreeHistogramEntry>("Entry");
   if (!tentry || !fbTypingMode) return;
   tentry->SetDynListInterval(value);
}


void TGo4EditDynEntry::DrawButton_clicked()
{
   DrawItem(HistoNameLbl->text());
}

void TGo4EditDynEntry::ClearHistogramClicked()
{
   TString objname;
   TGo4ServerProxy* an = Browser()->DefineAnalysisObject(HistoNameLbl->text().toLatin1().constData(), objname);
   if (an) {
      an->ClearAnalysisObject(objname.Data());
      if (GetLinkedCast<TGo4TreeHistogramEntry>("Entry"))
         an->ExecuteLine("@ResetBackStores();");
      Browser()->GetBrowserObject(HistoNameLbl->text().toLatin1().constData(), 2);
   }
}

void TGo4EditDynEntry::PrintDynList()
{
   TString objname;
   TGo4ServerProxy* an = Browser()->DefineAnalysisObject(GetLinkedName("Entry"), objname);

   if (an)
     an->PrintDynListEntry(objname.Data());
}

void TGo4EditDynEntry::ConRemoveButton_clicked()
{
   DropCondition("");
}
