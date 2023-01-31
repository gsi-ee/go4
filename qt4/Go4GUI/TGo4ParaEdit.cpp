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

#include "TGo4ParaEdit.h"

#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>

#include "TROOT.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TClass.h"
#include "TGo4LockGuard.h"
#include "TGo4Parameter.h"
#include "TGo4ParameterMember.h"
#include "TGo4ParameterStatus.h"
#include "TGo4Fitter.h"
#include "TGo4Slot.h"
#include "TGo4BrowserProxy.h"
#include "TGo4ServerProxy.h"

TGo4ParaEdit::TGo4ParaEdit(QWidget *parent, const char *name) :
   QGo4Widget(parent,name)
{
   setupUi(this);

   QObject::connect(MemberTable, &QTableWidget::cellChanged, this, &TGo4ParaEdit::ChangedTable);
   QObject::connect(MemberTable, &QTableWidget::cellDoubleClicked, this, &TGo4ParaEdit::TableDoubleClick);
   QObject::connect(MemberTable, &QTableWidget::customContextMenuRequested, this, &TGo4ParaEdit::TableContextMenu);
   QObject::connect(CancelButton, &QPushButton::pressed, this, &TGo4ParaEdit::CloseMDIParentSlot);
   QObject::connect(ApplyButton, &QPushButton::pressed, this, &TGo4ParaEdit::ApplyClicked);
   QObject::connect(RefreshButton, &QPushButton::pressed, this, &TGo4ParaEdit::RefreshClicked);
   QObject::connect(ClearButton, &QPushButton::pressed, this, &TGo4ParaEdit::clearTextFields);
   QObject::connect(SavePar, &QPushButton::clicked, this, &TGo4ParaEdit::saveFile);

   fItems = nullptr;
   PleaseUpdateLabel->setVisible(false);
   parentWidget()->adjustSize();

   ParamNameLbl->setText("");
   ParamClassLbl->setText("");

   fItemName = "";
   fFillingTable = false;
   MemberTable->setContextMenuPolicy(Qt::CustomContextMenu);
   MemberTable->horizontalHeader()->setStretchLastSection(true);
}

TGo4ParaEdit::~TGo4ParaEdit()
{
   if (fItems) {
      delete fItems;
      fItems = nullptr;
   }
}

bool TGo4ParaEdit::IsAcceptDrag(const char *itemname, TClass *cl, int kind)
{
   if (kind==TGo4Access::kndGo4Param) return true;

   return !cl ? false : cl->InheritsFrom(TGo4Parameter::Class());
}

void TGo4ParaEdit::DropItem(const char *itemname, TClass *cl, int kind)
{
   if ((kind==TGo4Access::kndGo4Param) ||
       (cl && cl->InheritsFrom(TGo4Parameter::Class())))
         WorkWithParameter(itemname, false);
}

void TGo4ParaEdit::linkedObjectUpdated(const char *linkname, TObject *obj)
{
   if (strcmp(linkname,"Parameter") == 0)
      RefreshWidget(dynamic_cast<TGo4Parameter*> (obj));
   else
   if (strcmp(linkname,"ParStatus") == 0)
      RefreshWidget(dynamic_cast<TGo4ParameterStatus*> (obj));
}

void TGo4ParaEdit::linkedObjectRemoved(const char *linkname)
{
   // if link removed while parameter is disappear, just close parameter editor

   if (strcmp(linkname,"Fitter") != 0)
      ShootCloseWidget(true);
}

void TGo4ParaEdit::WorkWithParameter(const char *itemname, bool isrefresh)
{
   if (PleaseUpdateLabel->isVisible() && !isrefresh) {
       auto par = GetLinkedCast<TGo4Parameter>("Parameter");
       const char *previtem = GetLinkedName("Parameter");
       if (par && previtem) {
          QMessageBox msgBox(QMessageBox::Question, "Parameter editor",
                             QString("Current parameter %1 is modified!\nNew parameter %2 is selected.").arg(previtem).arg(itemname));

          auto btnContinue = msgBox.addButton("Continue with current", QMessageBox::ActionRole);
          /* auto btnNew = */ msgBox.addButton("Start with new", QMessageBox::ActionRole);
          msgBox.setDefaultButton(btnContinue);

          msgBox.exec();
          if (msgBox.clickedButton() == btnContinue)
             return;
       }
   }

   ResetWidget();

   ParamNameLbl->setText(itemname);

   fItemName = itemname;

   QString iconname = ":/icons/right.png";
   QString tooltip = "Refresh parameter values from analysis";

   if (BrowserItemRemote(itemname)) {
      TGo4Slot *tgtslot = AddSlot("ParStatus");
      TGo4BrowserProxy* br = Browser();
      if (br) br->RequestObjectStatus(itemname, tgtslot);
      // add dummy link to be informed when parameter is disappear
      AddLink(itemname, "ParameterLock");
   } else {
      iconname = ":/icons/refresh.png";
      tooltip = "Refresh parameter values from source";
      AddLink(itemname, "Parameter");
      auto par = GetLinkedCast<TGo4Parameter>("Parameter", 2);
      if (par) RefreshWidget(par);
   }

   RefreshButton->setIcon(QIcon(iconname));
   RefreshButton->setToolTip(tooltip);

   setFocus();
}

void TGo4ParaEdit::ResetWidget()
{
   QGo4Widget::ResetWidget();

   ParamNameLbl->setText("");
   ParamClassLbl->setText("");

   fItemName = "";

   RefreshButton->setEnabled(false);
   ApplyButton->setEnabled(false);

   delete fItems;
   fItems = nullptr;

   ShowVisibleItems();

   PleaseUpdateLabel->setVisible(false);
}

void TGo4ParaEdit::RefreshWidget(TGo4Parameter *par)
{
   RefreshButton->setEnabled(par != nullptr);
   ApplyButton->setEnabled(par != nullptr);

   delete fItems;
   fItems = new TObjArray(100);
   fItems->SetOwner(kTRUE);

   if (par) {
      par->GetMemberValues(fItems);
      ParamClassLbl->setText(QString(" - ")+par->ClassName());
   }

   ShowVisibleItems();

   PleaseUpdateLabel->setVisible(false);
}


void TGo4ParaEdit::RefreshWidget(TGo4ParameterStatus* status)
{
   RefreshButton->setEnabled(status != nullptr);

   TGo4BrowserProxy* br = Browser();
   TGo4ServerProxy* serv = br ? br->DefineServerObject(fItemName.toLatin1().constData()) : nullptr;
   ApplyButton->setEnabled(serv && serv->CanSubmitObjects());

   delete fItems;
   fItems = nullptr;

   if (status) {
      fItems = status->GetMemberValues(kTRUE);
      ParamClassLbl->setText(QString(" - ") + status->GetObjectClass());
   }

   ShowVisibleItems();

   PleaseUpdateLabel->setVisible(false);
}

void TGo4ParaEdit::ShowVisibleItems()
{
   if (!fItems) {
      MemberTable->setRowCount(0);
      return;
   }

   fFillingTable = true;

   MemberTable->hide();

   MemberTable->horizontalHeaderItem(fiColType)->setText("Type");
   MemberTable->horizontalHeaderItem(fiColName)->setText("Name");
   MemberTable->horizontalHeaderItem(fiColValue)->setText("Value");
   MemberTable->horizontalHeaderItem(fiColComment)->setText("Comments");

   int memnum = fItems->GetLast()+1;
   MemberTable->setRowCount(memnum);
   // MemberTable->setLeftMargin(0);
   MemberTable->verticalHeader()->hide();
   TString buf;
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
   int ih = QApplication::fontMetrics().height();
#else
   int ih = QFontMetricsF(QApplication::font()).height();
#endif
   for (int row = 0; row < memnum; row++) {
      MemberTable->setRowHeight(row, ih);
      TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);

      QTableWidgetItem* item = new QTableWidgetItem(info->GetTypeName());
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      MemberTable->setItem(row, fiColType, item);

      item = new QTableWidgetItem(info->GetFullName(buf));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      MemberTable->setItem(row, fiColName, item);

      item = new QTableWidgetItem(info->GetStrValue());
      if (info->CanStrEdit())
         item->setFlags(item->flags() | Qt::ItemIsEditable);
      else
         item->setFlags(item->flags() & ~ Qt::ItemIsEditable);
      MemberTable->setItem(row, fiColValue, item);

      item = new QTableWidgetItem(info->GetTitle());
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      MemberTable->setItem(row, fiColComment, item);

      MemberTable->setRowHidden(row, !info->IsVisible());
   }

   fFillingTable = false;

   MemberTable->show();
   ensurePolished();
   update();
   show();
   raise();
   MemberTable->show();
}

void TGo4ParaEdit::clearTextFields()
{
   if (!fItems) return;

   for (int n = 0; n <= fItems->GetLast(); n++) {
      TGo4ParameterMember *info = (TGo4ParameterMember *)fItems->At(n);
      info->SetStrValue("");
   }

   PleaseUpdateLabel->setVisible(true);

   ShowVisibleItems();
}

void TGo4ParaEdit::ChangedTable( int row, int col )
{
   if ((col==fiColValue) && !fFillingTable) {
      QString txt = MemberTable->item(row, col)->text();
      TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);
      info->SetStrValue(txt.toLatin1().constData());
      PleaseUpdateLabel->setVisible(true);
   }
}

void TGo4ParaEdit::TableDoubleClick( int row, int col )
{
   if(col != fiColName) return;

   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);

   if (info->IsArrayItem()) {
      fiCurrentRow = row;
      ExpandShrinkArray();
   } else if (info->IsFitterItem()) {
      fiCurrentRow = row;
      EditFitter();
   }
}

void TGo4ParaEdit::TableContextMenu(const QPoint& pnt)
{
   QTableWidgetItem* item = MemberTable->itemAt (pnt);

   if (!item || (MemberTable->column(item) != fiColName)) return;

   int row = MemberTable->row(item);

   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(row);

   if (info->IsArrayItem()) {
       fiCurrentRow = row;
       QMenu contextMenu;
       contextMenu.addAction("Array View");

       if (row<fItems->GetLast()) {
         TGo4ParameterMember* next = (TGo4ParameterMember*) fItems->At(row+1);
         if ((next->GetMemberId() == info->GetMemberId()) && !next->IsVisible())
            contextMenu.addAction( "++ \t(next index)",  this, &TGo4ParaEdit::IncArrayIndex);
       }

       if (row>0) {
          TGo4ParameterMember* prev = (TGo4ParameterMember*) fItems->At(row-1);
          if ((prev->GetMemberId() == info->GetMemberId()) && !prev->IsVisible())
             contextMenu.addAction( "-- \t(prev index)", this, &TGo4ParaEdit::DecArrayIndex);
       }

       contextMenu.addAction( "Expand/Shrink", this, &TGo4ParaEdit::ExpandShrinkArray);
       contextMenu.exec(QCursor::pos());
    } else

    if (info->IsFitterItem()) {
        fiCurrentRow = row;
        QMenu contextMenu;
        contextMenu.addAction("Modify Fitter");

        contextMenu.addAction( "Edit...",  this, &TGo4ParaEdit::EditFitter);

        TGo4Fitter* fitter = nullptr;
        ServiceCall("GetFitterFromFitPanel", &fitter);
        if (fitter && (info->GetObject() != fitter))
           contextMenu.addAction( "Get from FitPanel",  this, &TGo4ParaEdit::GetFitterFromEditor);
        contextMenu.exec(QCursor::pos());
     }
}

void TGo4ParaEdit::ExpandShrinkArray()
{
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);

   Int_t memberid = info->GetMemberId();
   for (int n = 0; n <= fItems->GetLast(); n++) {
      TGo4ParameterMember *item = (TGo4ParameterMember *)fItems->At(n);
      if (item->GetMemberId()==memberid)
        item->SetVisible(!item->IsVisible());
   }
   info->SetVisible(kTRUE);
   ShowVisibleItems();
}

void TGo4ParaEdit::IncArrayIndex()
{
   if (fiCurrentRow>fItems->GetLast()) return;
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   TGo4ParameterMember* next = (TGo4ParameterMember*) fItems->At(fiCurrentRow+1);
   if (next->GetMemberId() == info->GetMemberId()) {
      info->SetVisible(kFALSE);
      next->SetVisible(kTRUE);
      ShowVisibleItems();
   }
}

void TGo4ParaEdit::DecArrayIndex()
{
   if (fiCurrentRow == 0) return;
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   TGo4ParameterMember* prev = (TGo4ParameterMember*) fItems->At(fiCurrentRow-1);
   if (prev->GetMemberId() == info->GetMemberId()) {
      info->SetVisible(kFALSE);
      prev->SetVisible(kTRUE);
      ShowVisibleItems();
   }
}

void TGo4ParaEdit::EditFitter()
{
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   TGo4Fitter* fitter = dynamic_cast<TGo4Fitter*> (info->GetObject());

   if(!fitter) {
      fitter = new TGo4Fitter("Fitter","Fitter from parameter editor");
      info->SetObject(fitter, kTRUE);
   }

   TGo4Slot *fitterslot = AddSlot("Fitter");
   fitterslot->AssignObject(fitter, kFALSE);
   EditObjectInSlot(fitterslot);
   PleaseUpdateLabel->setVisible(true);
}

void TGo4ParaEdit::GetFitterFromEditor()
{
   TGo4ParameterMember* info = (TGo4ParameterMember*) fItems->At(fiCurrentRow);
   if (!info || !info->IsFitterItem()) return;

   TGo4Fitter* fitter = nullptr;
   ServiceCall("CloneFitterFromFitPanel", &fitter);

   if (fitter) info->SetObject(fitter, kTRUE);
}

void TGo4ParaEdit::saveFile()
{
   TGo4LockGuard lock;

   if (!fItems) return;

   if (GetLinkedName("Parameter")) {
      auto par = GetLinkedCast<TGo4Parameter>("Parameter");
      if (!par) return;
      if (par->SetMemberValues(fItems))
         if (SaveItemToFile(GetLinkedName("Parameter"), "Parameters"))
            PleaseUpdateLabel->setVisible(false);

   } else if (fItemName.length() > 0) {

      const char *parclass = Browser()->ItemClassName(fItemName.toLatin1().constData());
      if (!parclass) return;
      if (!gROOT->GetClass(parclass)) {
         QMessageBox::warning(this, "Parameter editor",
            QString("Parameter class ") + parclass + " not known to gui");
         return;
      }
      TString foldname, parname;
      TGo4Slot::ProduceFolderAndName(fItemName.toLatin1().constData(), foldname, parname);

      TGo4ParameterStatus status(parname, parclass, (TObjArray*)fItems->Clone());

      TGo4Parameter *par = status.CreateParameter();

      if (!par) {
         QMessageBox::warning(this, "Parameter editor",
            QString("Can not crate parameter object of class ") + parclass);
         return;
      }

      QFileDialog fd(this, QString("Save ") + fItemName + " in root file",
            QString(), "ROOT (*.root);;ROOT XML (*.xml)");
      fd.setFileMode( QFileDialog::AnyFile);
      fd.setAcceptMode(QFileDialog::AcceptSave);
      fd.selectFile("file.root");
      if (fd.exec() != QDialog::Accepted) return;

      QStringList flst = fd.selectedFiles();
      if (flst.isEmpty()) return;

      TFile *f = TFile::Open(flst[0].toLatin1().constData(),"UPDATE");
      if (f) {
         f->WriteTObject(par, par->GetName(), "WriteDelete");
         delete f;
      }
   }
}

void TGo4ParaEdit::RefreshClicked()
{
   WorkWithParameter(ParamNameLbl->text().toLatin1().constData(), true);
}

void TGo4ParaEdit::ApplyClicked()
{
   TGo4LockGuard lock;

   if (fItemName.isEmpty() || !fItems) return;

   TString foldname, parname;
   TGo4Slot::ProduceFolderAndName(fItemName.toLatin1().constData(), foldname, parname);

   const char *parclass = Browser()->ItemClassName(fItemName.toLatin1().constData());
   if (!parclass) return;

   TGo4ParameterStatus status(parname, parclass, (TObjArray*)fItems->Clone());

   if (UpdateItemInAnalysis(fItemName.toLatin1().constData(), &status)) {
     if (BrowserItemRemote(fItemName.toLatin1().constData()))
        RefreshClicked();
     else
        PleaseUpdateLabel->setVisible(false);
   }
}
