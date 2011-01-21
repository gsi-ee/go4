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

#include "TGo4TreeViewer.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <QApplication>
#include <QToolTip>

#include "TH1.h"
#include "TClass.h"
#include "TGo4Slot.h"
#include "TGo4BrowserProxy.h"

TGo4TreeViewer::TGo4TreeViewer(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
   setupUi(this);

   fxTreeName = "";
   TreeDrawBtn->setEnabled(FALSE);
   setAcceptDrops(FALSE);
   setToolTip("Tree is not selected. Drag and drop leaf item from browser to X, Y or Z field");
}

void TGo4TreeViewer::ResetWidget()
{
   QGo4Widget::ResetWidget();
   XFieldEdt->clear();
   YFieldEdt->clear();
   ZFieldEdt->clear();
   HistNameEdt->clear();
   CutEdt->clear();
   TreeDrawBtn->setEnabled(FALSE);
   fxTreeName = "";
   setToolTip("Tree is not selected. Drag and drop leaf item from browser to X, Y or Z field");
}

void TGo4TreeViewer::WorkWithLeaf(const char* itemname)
{
   ResetWidget();
   XFieldEdt->setText(itemname);
   ProcessDropEvent(XFieldEdt, true);
}

void TGo4TreeViewer::XFieldEdt_dropped()
{
   ProcessDropEvent(XFieldEdt, true);
}

void TGo4TreeViewer::YFieldEdt_dropped()
{
   ProcessDropEvent(YFieldEdt, true);
}

void TGo4TreeViewer::ZFieldEdt_dropped()
{
   ProcessDropEvent(ZFieldEdt, true);
}

void TGo4TreeViewer::cutEdit_dropped()
{
   ProcessDropEvent(CutEdt, false);
}

void TGo4TreeViewer::ProcessDropEvent(QGo4LineEdit* edt, bool caninit)
{
   QString value = edt->text();

   TGo4BrowserProxy* br = Browser();
   if (br==0) return;

   if (fxTreeName.length()==0) {
      if (!caninit) {
          StatusMessage("First drop something on X, Y or Z field");
          edt->setText("");
          return;
      }

      TString treename;
      if (!br->DefineTreeName(value.toAscii(), treename)) {
          StatusMessage(QString("Invalid tree ") + value);
          edt->setText("");
          return;
      }
      fxTreeName = treename.Data();
      setToolTip(QString("Selected tree: ") + fxTreeName);

      TreeDrawBtn->setEnabled(TRUE);
      AddLink(treename.Data(), "Tree");
   }

   TString leafname;
   if (!br->DefineLeafName(value.toAscii(), fxTreeName.toAscii(), leafname)) {
      edt->setText("");
      StatusMessage(QString("Invalid leaf name ") + value);
      return;
   }
   // count number of [ and replace by []
   // this means, by default accumulate over all members
   TObjArray* it=leafname.Tokenize("[");
   leafname=((TObjString *)it->First())->GetName();
   for(Int_t i=1;i<it->GetEntriesFast();i++) leafname.Append("[]");
   edt->setText(leafname.Data());
   edt->setFocus();
   it->Delete();
}

void TGo4TreeViewer::TreeDrawBtn_clicked()
{
   if (fxTreeName.isEmpty()) {
      StatusMessage("No tree specified");
      return;
   }

   if(XFieldEdt->text().isEmpty()) {
      StatusMessage("Treedraw without x coordinate expression");
      return;
   }

   TGo4BrowserProxy* br = Browser();
   if (br==0) return;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   TString createdh;

   br->PerformTreeDraw(fxTreeName.toAscii(),
                       XFieldEdt->text().toAscii(),
                       YFieldEdt->text().toAscii(),
                       ZFieldEdt->text().toAscii(),
                       CutEdt->text().toAscii(),
                       HistNameEdt->text().toAscii(),
                       createdh);

    if (createdh.Length()>0) {
       HistNameEdt->setText(createdh.Data());
       DrawItem(createdh.Data());
    } else {
        if (WhereItemDrawn(HistNameEdt->text().toAscii())==0)
           DrawItem(HistNameEdt->text());
        else
           br->RequestBrowserObject(HistNameEdt->text().toAscii());
    }

   QApplication::restoreOverrideCursor();
}

void TGo4TreeViewer::TreeClearBtn_clicked()
{
   ResetWidget();
}

void TGo4TreeViewer::NewHistBtn_clicked()
{
   if (fxTreeName.length()>0)
     AskToCreateObject(TH1::Class(), BrowserItemRemote(fxTreeName.toAscii()) ? 1 : 0);
}

void TGo4TreeViewer::requestedObjectCreated(const char* itemname, TClass* cl)
{
   if ((cl!=0) && cl->InheritsFrom(TH1::Class()))
      HistNameEdt->setText(itemname);
}
