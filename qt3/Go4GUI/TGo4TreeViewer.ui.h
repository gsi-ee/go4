#include "Riostream.h"
#include "TObjArray.h"
#include "TObjString.h"
void TGo4TreeViewer::init()
{
   fxTreeName = "";
   TreeDrawBtn->setEnabled(FALSE);
   setAcceptDrops(FALSE);
   QToolTip::add(this, "Tree is not selected. Drag and drop leaf item from browser to X, Y or Z field");
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
   QToolTip::add(this, "Tree is not selected. Drag and drop leaf item from browser to X, Y or Z field");
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
      if (!br->DefineTreeName(value.latin1(), treename)) {
          StatusMessage(QString("Invalid tree ") + value);
          edt->setText("");
          return;
      }
      fxTreeName = treename.Data();
      QToolTip::add(this, QString("Selected tree: ") + fxTreeName);

      TreeDrawBtn->setEnabled(TRUE);
      cout << "Treeviewer " << treename.Data() << " edt " << edt->text() << endl;
      AddLink(treename.Data(), "Tree");
   }

   TString leafname;
   if (!br->DefineLeafName(value.latin1(), fxTreeName.latin1(), leafname)) {
      edt->setText("");
      StatusMessage(QString("Invalid leaf name ") + value);
      return;
   }

   cout << "Treeviewer leaf " << leafname.Data() << endl;
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

   br->PerformTreeDraw(fxTreeName.latin1(),
                       XFieldEdt->text().latin1(),
                       YFieldEdt->text().latin1(),
                       ZFieldEdt->text().latin1(),
                       CutEdt->text().latin1(),
                       HistNameEdt->text().latin1(),
                       createdh);

    if (createdh.Length()>0) {
       HistNameEdt->setText(createdh.Data());
       DrawItem(createdh.Data());
    } else {
        if (WhereItemDrawn(HistNameEdt->text().latin1())==0)
           DrawItem(HistNameEdt->text().latin1());
        else
           br->RequestBrowserObject(HistNameEdt->text().latin1());
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
     AskToCreateObject(TH1::Class(), BrowserItemRemote(fxTreeName.latin1()) ? 1 : 0);
}

void TGo4TreeViewer::requestedObjectCreated(const char* itemname, TClass* cl)
{
   if ((cl!=0) && cl->InheritsFrom(TH1::Class()))
      HistNameEdt->setText(itemname);
}
