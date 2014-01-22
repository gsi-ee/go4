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


#include "TGo4ConditionEditor.h"

#include <QMessageBox>
#include <QMenu>
#include <QtCore/QSignalMapper>
#include <QToolTip>

#include "TH1.h"
#include "TCutG.h"
#include "TClass.h"
#include "TPad.h"
#include "TGo4Log.h"
#include "TGo4Condition.h"
#include "TGo4Slot.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4ViewPanel.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"


TGo4ConditionEditor::TGo4ConditionEditor(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
   setupUi(this);

   setWindowTitle("Condition editor");
   ResetWidget();
   fiSelectedIndex = -1;
   parentWidget()->adjustSize();
   fbDrawOnNextRefresh = false;
   fiLastChangeValue = -1;
   CutTable->setContextMenuPolicy(Qt::CustomContextMenu);
}


bool TGo4ConditionEditor::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   if (cl==0) return false;
   bool res = cl->InheritsFrom(TGo4Condition::Class());
//   res = res || cl->InheritsFrom(TH1::Class());
   return res;
}

void TGo4ConditionEditor::DropItem(const char* itemname, TClass* cl, int kind)
{
   if (cl==0) return;

   if (cl->InheritsFrom(TGo4Condition::Class()))
      WorkWithCondition(itemname);

// uncomment this to enable histogram drop on condition editor

//   else
//   if (cl->InheritsFrom(TH1::Class()) && (fiSelectedIndex==-1)) {
//      RemoveLink("Histogram");
//      AddLink(itemname, "Histogram");
//      GetLinked("Histogram", 1);
//
//      TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
//      if (cond!=0) {
//         cond->SetHistogram(itemname);
//         cond->SetHistogramLink(kTRUE);
//      }
//      CondTabs->setCurrentIndex(1);
//      PleaseUpdateSlot();
//      RefreshWidget();
//   }

}

void TGo4ConditionEditor::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname,"Condition")==0) {
      TGo4Condition* cond = dynamic_cast<TGo4Condition*> (obj);
      if (cond!=0) cond->SetChanged(kFALSE);
   }

   RefreshWidget(false);
}

void TGo4ConditionEditor::linkedObjectRemoved(const char* linkname)
{
   if (strcmp(linkname,"Condition")==0)
      ShootCloseWidget(true);
}

void TGo4ConditionEditor::WorkWithCondition(const char* itemname)
{
   const char* conditemname = GetLinkedName("Condition");

   TGo4Condition* con = dynamic_cast<TGo4Condition*> (GetLinked("Condition",0));
   if ((con!=0) && (con->IsChanged()!=0) && (strcmp(conditemname,itemname)!=0)) {

      int res = QMessageBox::warning(this, "Condition editor",
        QString("Current condition ")+conditemname+" is modified!\n"+
                "New condition " + itemname+ " selected.",
        QString("Continue with current"),
        QString("Start with new"), QString::null, 0);
//        (BrowserItemRemote(conditemname) ? QString("Update in analysis and start new") : QString::null), 0);
      if (res==0) return;
//      if (res==2) UpdateItemInAnalysis(conditemname);
   }

   if ((conditemname==0) || (strcmp(conditemname,itemname)!=0)) {
      ResetWidget();
      CondNameLbl->setText(itemname);
      AddLink(itemname, "Condition");
   }

   QString iconname = ":/icons/right.png";
   QString tooltip = "Refresh condition from analysis";
   if (!BrowserItemRemote(itemname)) {
      iconname = ":/icons/refresh.png";
      tooltip = "Refresh condition from source";
   }
   RefreshButton->setIcon( QIcon(iconname) );
   RefreshButton->setToolTip(tooltip);

   TGo4ViewPanel* panel = WhereItemDrawn(itemname);

   if (panel==0) {
      GetLinked("Condition", 2);
      fbDrawOnNextRefresh = true;
      fiSelectedIndex = -1;
   } else {
      TPad* pad = panel->FindPadWithItem(itemname);
      fiSelectedIndex = panel->GetSelectedMarkerIndex(pad);
      panel->setFocus();
   }

   RefreshWidget(false);

   setFocus();
}

void TGo4ConditionEditor::ResetWidget()
{
   QGo4Widget::ResetWidget();
   CondNameLbl->setText("");
   CondClassLbl->setText("");

   PleaseUpdateLabel->setVisible(false);

   ArrayAllButton->setVisible(false);
   ArrayElements->setVisible(false);

   HistogramChk->setVisible(false);
   HistogramChk->setChecked(false);
   HistogramChk->setText("null");
   HistogramChk->setEnabled(false);

   CondVisibleChk->setVisible(false);
   DrawInfoLbl->setText("");
   CondTabs->setEnabled(false);

   LCDCounts->display(0);
   LCDTrueCounts->display(0);

   Win1_low->setDisabled(true);
   Win1_up->setDisabled(true);
   Win2_low->setDisabled(true);
   Win2_up->setDisabled(true);

   ResultCombo->setEnabled(false);
   InvertCombo->setEnabled(false);

   CondTabs->setTabEnabled(1, false);
}

void TGo4ConditionEditor::RefreshWidget(bool checkindex)
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return;

   const char* conditemname = GetLinkedName("Condition");
   TGo4ViewPanel* panel = WhereItemDrawn(conditemname);
   TPad* pad = panel==0 ? 0 : panel->FindPadWithItem(conditemname);

   PleaseUpdateLabel->setVisible(cond->IsChanged()!=0);
   fiLastChangeValue = cond->IsChanged();

   QString hitemname;
   TH1* histo = 0;
   if ((panel!=0) && (pad!=0)) {
      histo = panel->GetPadHistogram(pad);
      hitemname = panel->GetDrawObjectLinkName(pad, histo);
      if (checkindex) {
         QString selname = panel->GetSelectedMarkerName(pad);
         int selindex = panel->GetSelectedMarkerIndex(pad);
         if (selname==cond->GetName())
           fiSelectedIndex = selindex;
      }
   }
   if (histo==0) histo = dynamic_cast<TH1*>(GetLinked("Histogram", 0));

   fbTypingMode = false;

   TGo4CondArray* arr = dynamic_cast<TGo4CondArray*> (cond);

   ArrayAllButton->setVisible(arr!=0);
   ArrayElements->setVisible(arr!=0);
   if (arr==0) {
      fiSelectedIndex = -1;
   } else {
      ArrayElements->setMaximum(arr->GetNumber()-1);
      if (fiSelectedIndex>=arr->GetNumber()-1)
        fiSelectedIndex=arr->GetNumber()-1;
      ArrayElements->setValue(fiSelectedIndex);

      cond = SelectedCondition();
   }

   ModifyButton->setVisible((panel!=0) && ((arr==0) || (fiSelectedIndex>=0)));

   TGo4WinCond* wcond = dynamic_cast<TGo4WinCond*> (cond);
   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (cond);

   if (wcond!=0)
     if (wcond->GetDimension()==2)
        CondClassLbl->setText("Win 2-D  ");
     else
        CondClassLbl->setText("Win 1-D  ");
   else
   if (pcond!=0)
     CondClassLbl->setText("Polygon  ");
   else
     CondClassLbl->setText("");

   QString infolbl;

   const char* hname = cond->GetLinkedHistogram();
   if ((hname==0) || (*hname==0)) {
      HistogramChk->setChecked(false);
      HistogramChk->setText("null");
      HistogramChk->setEnabled(false);
//      DisplayB->setEnabled(false);
   } else {
      HistogramChk->setChecked(cond->IsHistogramLink());
      HistogramChk->setText(hname);
      HistogramChk->setEnabled(true);
      infolbl = "Hist: ";
      if (hitemname.length()>0)
         infolbl += hitemname;
      else
         infolbl += hname;
//      DisplayB->setEnabled(true);
   }

   CondVisibleChk->setVisible(true);
   CondVisibleChk->setChecked(cond->IsVisible());

   if (panel!=0) {
     if (infolbl.length()>0) infolbl+="  ";
     infolbl += "Drawn: ";
     infolbl += panel->objectName();
     if ((pad!=0) && (pad!=(TPad*)panel->GetCanvas())) {
        infolbl += ", ";
        infolbl += pad->GetName();
     }
   }
   DrawInfoLbl->setText(infolbl);

   CondTabs->setEnabled(true);

   ResultCombo->setEnabled(true);
   InvertCombo->setEnabled(true);

   if(cond->IsEnabled())
     ResultCombo->setCurrentIndex(0);
   else
     if (cond->FixedResult())
       ResultCombo->setCurrentIndex(1);
     else
       ResultCombo->setCurrentIndex(2);

   if(cond->IsTrue()) InvertCombo->setCurrentIndex(0);
                 else InvertCombo->setCurrentIndex(1);

   LCDCounts->display(cond->Counts());
   LCDTrueCounts->display(cond->TrueCounts());

   double ratio = 0;
   if (cond->Counts()>0)
     ratio = 1.*cond->TrueCounts() / cond->Counts();
   QString sratio;
   if ((ratio>=0.) && (ratio<=100.))
      sratio.sprintf("%4.2f%s",ratio*100.,"%");
   else
      sratio = "?.?? %";
   PercentLabel->setText(sratio);

   CondLimits->setChecked(cond->IsLimitsDraw());
   CondLabel->setChecked(cond->IsLabelDraw());

   Win1_low->setText(QString::number(cond->GetXLow()));
   Win1_up->setText(QString::number(cond->GetXUp()));

   if (cond->GetDimension()==2) {
      Win2_low->setText(QString::number(cond->GetYLow()));
      Win2_up->setText(QString::number(cond->GetYUp()));
   } else {
      Win2_low->setText("");
      Win2_up->setText("");
   }

   if (wcond!=0) {
      Win1_low->setEnabled(true);
      Win1_up->setEnabled(true);
      Win2_low->setEnabled(wcond->GetDimension()==2);
      Win2_up->setEnabled(wcond->GetDimension()==2);
   } else {
      Win1_low->setDisabled(true);
      Win1_up->setDisabled(true);
      Win2_low->setDisabled(true);
      Win2_up->setDisabled(true);
   }

   CondTabs->setTabEnabled(1, (pcond!=0));
   if ((pcond==0) && (CondTabs->currentIndex()==1))
     CondTabs->setCurrentIndex(0);
   if (pcond!=0) FillCutWidget(pcond->GetCut(kFALSE));

   IntBox->setChecked(cond->IsIntDraw());
   MaxCBox->setChecked(cond->IsCMaxDraw());
   MaxXBox->setChecked(cond->IsXMaxDraw());
   MaxYBox->setChecked(cond->IsYMaxDraw());

   XMeanBox->setChecked(cond->IsXMeanDraw());
   YMeanBox->setChecked(cond->IsYMeanDraw());
   XRMSBox->setChecked(cond->IsXRMSDraw());
   YRMSBox->setChecked(cond->IsYRMSDraw());

   IntBox->setText(QString("Integr: ") + QString::number(cond->GetIntegral(histo,"")));
   MaxXBox->setText(QString("MaxX: ") + QString::number(cond->GetXMax(histo)));
   MaxYBox->setText(QString("MaxY: ") + QString::number(cond->GetYMax(histo)));
   MaxCBox->setText(QString("Max: ") + QString::number(cond->GetCMax(histo)));

   XMeanBox->setText(QString("XMean: ") + QString::number(cond->GetMean(histo, 1)));
   XRMSBox->setText(QString("XRMS: ") + QString::number(cond->GetRMS(histo, 1)));
   YMeanBox->setText(QString("YMean: ") + QString::number(cond->GetMean(histo, 2)));
   YRMSBox->setText(QString("YRMS: ") + QString::number(cond->GetRMS(histo, 2)));

   fbTypingMode = true;

   if (fbDrawOnNextRefresh) {
      fbDrawOnNextRefresh = false;
      TString hitemname;
      if ((panel==0) && HistogramChk->isChecked() &&
         Browser()->DefineRelatedObject(conditemname, hname, hitemname))
           DrawCondition(false);
   }
}

TGo4Condition* TGo4ConditionEditor::SelectedCondition()
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return 0;

   TGo4CondArray* arr = dynamic_cast<TGo4CondArray*> (cond);

   if ((arr==0) || (fiSelectedIndex==-1)) return cond;

   if (fiSelectedIndex<arr->GetNumber())
     return arr->At(fiSelectedIndex);

   return cond;
}

void TGo4ConditionEditor::PleaseUpdateSlot()
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*> (GetLinked("Condition",0));
   SetChangeFlag(true);
   PleaseUpdateLabel->setVisible(true);
}

void TGo4ConditionEditor::SelectedCond(int t)
{
   if (!fbTypingMode) return;
   if (fiSelectedIndex==t) return;

   fiSelectedIndex = t;
   RefreshWidget(false);
   RedrawCondition();
}

void TGo4ConditionEditor::ArrayAll()
{
   ArrayElements->setValue(-1);
}

void TGo4ConditionEditor::SetResultMode( int mode )
{
   TGo4Condition* cond = SelectedCondition();
   if (!fbTypingMode || (cond==0)) return;
   switch (mode) {
      case 0: cond->Enable(); break;
      case 1: cond->Disable(kTRUE); break;
      case 2: cond->Disable(kFALSE ); break;
      default: cond->Enable(); break;
   }
   PleaseUpdateSlot();
}

void TGo4ConditionEditor::SetInvertMode( int mode )
{
   TGo4Condition* cond = SelectedCondition();
   if (!fbTypingMode || (cond==0)) return;
   cond->Invert(mode==1);
   PleaseUpdateSlot();
}

void TGo4ConditionEditor::ClearCounters()
{
   const char* conditemname = GetLinkedName("Condition");
   TGo4Condition* con = dynamic_cast<TGo4Condition*> (GetLinked("Condition",0));

   if ((con==0) || (conditemname==0)) return;

   TString objname;
   TGo4AnalysisProxy* an = Browser()->DefineAnalysisObject(conditemname, objname);
   if (an!=0) {
       an->ClearAnalysisObject(objname.Data());
       GetLinked("Condition", 2);
   } else {
     con->ResetCounts();
     PleaseUpdateSlot();
     RefreshWidget(false);
   }
}

void TGo4ConditionEditor::LimitsChanged(const QString& )
{
   UpdateLimits();
}

void TGo4ConditionEditor::LimitsReturnPressed()
{
   if (UpdateLimits())
      RedrawCondition();
}

bool TGo4ConditionEditor::UpdateLimits()
{
   TGo4Condition* cond = SelectedCondition();
   if (!fbTypingMode || (cond==0)) return false;

   bool okx1, okx2, res = false;
   Double_t x1 = Win1_low->text().toDouble(&okx1);
   Double_t x2 = Win1_up->text().toDouble(&okx2);

   if (cond->GetDimension()==2) {
      bool oky1, oky2;
      Double_t y1 = Win2_low->text().toDouble(&oky1);
      Double_t y2 = Win2_up->text().toDouble(&oky2);
      if (okx1 && okx2 && oky1 && oky2) {
         cond->SetValues(x1, x2, y1, y2);
         res = true;
      }
   } else
      if (okx1 && okx2) {
         cond->SetValues(x1, x2);
         res = true;
      }
   if (res) PleaseUpdateSlot();
   return res;
}

void TGo4ConditionEditor::ChangeConditionProperty(int id, bool on)
{
   TGo4Condition* cond = SelectedCondition();
   if (!fbTypingMode || (cond==0)) return;
   switch(id) {
     case 0: cond->SetLabelDraw(on); break;
     case 1: cond->SetLimitsDraw(on); break;
     case 2: cond->SetIntDraw(on);  break;
     case 3: cond->SetXMeanDraw(on);  break;
     case 4: cond->SetXRMSDraw(on);  break;
     case 5: cond->SetYRMSDraw(on);  break;
     case 6: cond->SetYMeanDraw(on);  break;
     case 7: cond->SetXMaxDraw(on);  break;
     case 8: cond->SetYMaxDraw(on);  break;
     case 9: cond->SetCMaxDraw(on);  break;
     case 10: cond->SetVisible(on); break;
     default: return;
   }
   PleaseUpdateSlot();
   RedrawCondition();
}


void TGo4ConditionEditor::SetLabel( bool on )
{
   ChangeConditionProperty(0, on);
}


void TGo4ConditionEditor::SetDrawLimits( bool on )
{
   ChangeConditionProperty(1, on);
}

void TGo4ConditionEditor::SetIntDraw( bool on )
{
   ChangeConditionProperty(2, on);
}

void TGo4ConditionEditor::SetXMeanDraw( bool on )
{
   ChangeConditionProperty(3, on);
}

void TGo4ConditionEditor::SetXRMSDraw( bool on )
{
   ChangeConditionProperty(4, on);
}

void TGo4ConditionEditor::SetYRMSDraw( bool on )
{
   ChangeConditionProperty(5, on);
}

void TGo4ConditionEditor::SetYMeanDraw( bool on )
{
   ChangeConditionProperty(6, on);
}

void TGo4ConditionEditor::SetXMaxDraw( bool on )
{
   ChangeConditionProperty(7, on);
}

void TGo4ConditionEditor::SetYMaxDraw( bool on )
{
   ChangeConditionProperty(8, on);
}

void TGo4ConditionEditor::SetCMaxDraw( bool on )
{
   ChangeConditionProperty(9, on);
}

void TGo4ConditionEditor::SetCondVisible(bool on)
{
   ChangeConditionProperty(10, on);
}


void TGo4ConditionEditor::enterEvent( QEvent * )
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return;
   if (fiLastChangeValue==cond->IsChanged()) return;

   RefreshWidget(true);
}

void TGo4ConditionEditor::leaveEvent( QEvent * )
{
}

void TGo4ConditionEditor::RefreshClicked()
{
   GetLinked("Condition", 2);
   RefreshWidget(false);
}

void TGo4ConditionEditor::DisplayPressed()
{
   DrawCondition(true);
}

void TGo4ConditionEditor::SetChangeFlag(bool changed)
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*> (GetLinked("Condition",0));
   if (cond!=0) cond->SetChanged(changed);
}

void TGo4ConditionEditor::UpdatePressed()
{
   if (PrepareForAnalysis()) {
      const char* conditem = GetLinkedName("Condition");
      if (UpdateItemInAnalysis(conditem) && BrowserItemRemote(conditem))
         RefreshClicked();
      else {
        SetChangeFlag(false);
        RefreshWidget(false);
      }
   }
}

void TGo4ConditionEditor::SaveCondition()
{
   if (PrepareForAnalysis())
     if (SaveItemToFile(GetLinkedName("Condition"), "Conditions")) {
        SetChangeFlag(false);
        RefreshWidget(false);
     }
}

void TGo4ConditionEditor::DrawCondition(bool useactive)
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return;

   const char* conditemname = GetLinkedName("Condition");
   if (conditemname==0) return;

   TGo4ViewPanel* panel = 0;

   if (useactive) panel = LastActivePanel();
   if (panel!=0) {
      DrawItem(conditemname, panel, 0, false, 0);
      RedrawCondition();
      RefreshWidget(false);
      panel->setFocus();
      setFocus();
      return;
   }

   panel = WhereItemDrawn(conditemname);
   TPad* pad = panel==0 ? 0 : panel->FindPadWithItem(conditemname);

   if (panel!=0) {
      if (pad!=0)
         RedrawCondition();
      else {
         DrawItem(conditemname, panel, panel->GetActivePad(), false, 0);
         panel->SetSelectedMarker(pad, cond->GetName(), fiSelectedIndex);
         panel->MarkPadModified(pad);
         panel->ShootRepaintTimer(pad);
      }
      panel->setFocus();
      setFocus();
      return;
   }

   const char* hname = 0;
   if (cond->IsHistogramLink())
      hname = cond->GetLinkedHistogram();
   TString hitemname;

   TGo4BrowserProxy* br = Browser();

   if (hname!=0)
      if (br->DefineRelatedObject(conditemname, hname, hitemname)) {
        panel = DrawItem(hitemname.Data(), 0);
        RemoveLink("Histogram");
        AddLink(hitemname.Data(), "Histogram");
        DrawItem(conditemname, panel, 0, false, 0);
        RefreshWidget(false);
        RedrawCondition();
        panel->setFocus();
        setFocus();
      }
}

void TGo4ConditionEditor::RedrawCondition()
{
   const char* conditemname = GetLinkedName("Condition");
   if (conditemname==0) return;

   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return;

   TGo4ViewPanel* panel = WhereItemDrawn(conditemname);
   if (panel==0) return;

   TPad* pad = panel->FindPadWithItem(conditemname);
   if (pad==0) return;

   panel->SetSelectedMarker(pad, cond->GetName(), fiSelectedIndex);
   panel->MarkPadModified(pad);
   panel->ShootRepaintTimer(pad);
}

void TGo4ConditionEditor::PrintConditionLog()
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond!=0) cond->Print("go4log-limits-stats");
}

bool TGo4ConditionEditor::PrepareForAnalysis()
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return false;

   SetChangeFlag(false);

   const char* hname = cond->GetLinkedHistogram();
   if ((hname!=0) && (strlen(hname)!=0)) {
      TString foldername, objname;
      TGo4Slot::ProduceFolderAndName(hname, foldername, objname);
      cond->SetHistogram(objname.Data());
   }
   return true;
}

void TGo4ConditionEditor::ModifyButton_clicked()
{
   TGo4Condition* cond = dynamic_cast<TGo4Condition*>(GetLinked("Condition", 0));
   if (cond==0) return;

   const char* conditemname = GetLinkedName("Condition");
   TGo4ViewPanel* panel = WhereItemDrawn(conditemname);
   TPad* pad = panel==0 ? 0 : panel->FindPadWithItem(conditemname);

   if ((panel==0) || (pad==0)) return;

   panel->SetActivePad(pad);

   panel->setFocus();

   panel->SetSelectedMarker(pad, cond->GetName(), fiSelectedIndex);

   panel->StartConditionEditing(pad);
}

void TGo4ConditionEditor::FillCutWidget(TCutG* cut)
{
   bool old = fbTypingMode;
   fbTypingMode = false;

   if (cut==0) {
      CutTable->setRowCount(0);
      NPointsSpin->setValue(0);
   } else {
      CutTable->setRowCount(cut->GetN());
      NPointsSpin->setValue(cut->GetN());
      for (int n=0;n<cut->GetN();n++) {
         Double_t x,y;
         cut->GetPoint(n, x,y);
         CutTable->setItem(n, 0, new QTableWidgetItem(QString::number(x)));
         CutTable->setItem(n, 1, new QTableWidgetItem(QString::number(y)));
         CutTable->setVerticalHeaderItem(n, new QTableWidgetItem(QString::number(n)));
      }
   }

   fbTypingMode = old;
}

void TGo4ConditionEditor::NPointsSpin_valueChanged(int npoint)
{
   if (!fbTypingMode) return;

   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   if (pcond==0) return;

   TCutG* cut = pcond->GetCut(kFALSE);

   if (cut==0) {
      cut = new TCutG("conditioncut", npoint);
      Double_t x=0., y=0.;
      for (int n=0;n<npoint-1; n++)
        cut->SetPoint(n, n*10, n*10);
      cut->GetPoint(0, x, y);
      cut->SetPoint(npoint-1, x, y);
      pcond->SetValues(cut);
   } else {
      if (npoint==0) {
         pcond->SetValues(0);
         cut = 0;
      } else {
         int old = cut->GetN();
         Double_t x=0., y=0.;
         if (old>1) cut->GetPoint(old-2, x, y);
         cut->Set(npoint);
         for(int n= (old-1>=0 ? old-1 : 0); n<npoint-1; n++)
           cut->SetPoint(n, x, y+(n-old+2)*10);
         cut->GetPoint(0, x, y);
         cut->SetPoint(npoint-1, x, y);
      }
   }

   PleaseUpdateSlot();

   FillCutWidget(cut);

   RedrawCondition();
}

void TGo4ConditionEditor::CutTable_valueChanged( int nrow, int ncol)
{
   if (!fbTypingMode) return;

   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TCutG* cut = pcond==0 ? 0 : pcond->GetCut(kFALSE);
   if (cut==0) return;

   bool ok;
   double zn = CutTable->item(nrow, ncol)->text().toDouble(&ok);

   if (!ok) return;
   if (ncol==0) cut->GetX()[nrow] = zn;
           else cut->GetY()[nrow] = zn;
   if ((nrow==0) || (nrow==cut->GetN()-1)) {
      int nrow1 = (nrow==0) ? cut->GetN()-1 : 0;
      fbTypingMode = false;
      CutTable->setItem(nrow1, ncol, new QTableWidgetItem(CutTable->item(nrow, ncol)->text()));
      if (ncol==0) cut->GetX()[nrow1] = zn;
              else cut->GetY()[nrow1] = zn;
      fbTypingMode = true;
   }

   PleaseUpdateSlot();

   RedrawCondition();
}


void TGo4ConditionEditor::CutTable_contextMenuRequested( const QPoint & pos )
{
   if (!fbTypingMode) return;

   QTableWidgetItem* item = CutTable->itemAt (pos);

   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TCutG* cut = pcond==0 ? 0 : pcond->GetCut(kFALSE);
   if ((cut==0) || (item==0)) return;

   int nrow = CutTable->row(item);

   QMenu menu(this);
   QSignalMapper map(this);
   AddIdAction(&menu, &map, "Insert point", nrow);
   // only internal points should be allowed to deleted
   AddIdAction(&menu, &map, "Delete point", nrow+1000000, (nrow>0) && (nrow<cut->GetN()-1));
   connect(&map, SIGNAL(mapped(int)), this, SLOT(ContextMenuSlot(int)));
   menu.exec(CutTable->mapToGlobal(pos));
}

void TGo4ConditionEditor::ContextMenuSlot(int id)
{
   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TCutG* cut = pcond==0 ? 0 : pcond->GetCut(kFALSE);
   if (cut==0) return;

   if (id>=1000000) { // delete point
      id-=1000000;
      cut->RemovePoint(id);
   } else {           // insert point
      Int_t npoints = cut->GetN();
      if (id>=npoints) return;
      cut->Set(npoints+1);

      Double_t x,y;

      for(int n=npoints;n>id;n--) {
         cut->GetPoint(n-1, x, y);
         cut->SetPoint(n, x, y);
      }

      if (id>0) {
         Double_t x1, y1;
         cut->GetPoint(id-1, x1, y1);
         cut->SetPoint(id, (x1+x)/2., (y+y1)/2.);
      }
   }

   FillCutWidget(cut);

   PleaseUpdateSlot();

   RedrawCondition();
}
