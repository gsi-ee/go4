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


#include "TGo4ConditionEditor.h"

#include <QMessageBox>
#include <QMenu>

#include "TH1.h"
#include "TMath.h"
#include "TCutG.h"
#include "TClass.h"
#include "TPad.h"
#include "TArrayD.h"
#include "TGo4Slot.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4ShapedCond.h"
#include "TGo4ListCond.h"
#include "TGo4CondArray.h"
#include "TGo4ViewPanel.h"
#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"


TGo4ConditionEditor::TGo4ConditionEditor(QWidget *parent, const char* name) :
   QGo4Widget(parent, name)
{
   setupUi(this);

   QObject::connect(CloseWin, SIGNAL(clicked()), this, SLOT(CloseMDIParentSlot()));
   QObject::connect(UpdateCon, SIGNAL(clicked()), this, SLOT(UpdatePressed()));
   QObject::connect(ArrayElements, SIGNAL(valueChanged(int)), this, SLOT(SelectedCond(int)));
   QObject::connect(ResultCombo, SIGNAL(activated(int)), this, SLOT(SetResultMode(int)));
   QObject::connect(InvertCombo, SIGNAL(activated(int)), this, SLOT(SetInvertMode(int)));
   QObject::connect(ClearCondCount, SIGNAL(clicked()), this, SLOT(ClearCounters()));
   QObject::connect(DisplayB, SIGNAL(clicked()), this, SLOT(DisplayPressed()));
   QObject::connect(RefreshButton, SIGNAL(clicked()), this, SLOT(RefreshClicked()));
   QObject::connect(Win1_low, SIGNAL(returnPressed()), this, SLOT(LimitsReturnPressed()));
   QObject::connect(Win1_up, SIGNAL(returnPressed()), this, SLOT(LimitsReturnPressed()));
   QObject::connect(Win2_low, SIGNAL(returnPressed()), this, SLOT(LimitsReturnPressed()));
   QObject::connect(Win2_up, SIGNAL(returnPressed()), this, SLOT(LimitsReturnPressed()));
   QObject::connect(Win1_low, SIGNAL(textChanged(QString)), this, SLOT(LimitsChanged(QString)));
   QObject::connect(Win1_up, SIGNAL(textChanged(QString)), this, SLOT(LimitsChanged(QString)));
   QObject::connect(Win2_low, SIGNAL(textChanged(QString)), this, SLOT(LimitsChanged(QString)));
   QObject::connect(Win2_up, SIGNAL(textChanged(QString)), this, SLOT(LimitsChanged(QString)));
   QObject::connect(CondLimits, SIGNAL(toggled(bool)), this, SLOT(SetDrawLimits(bool)));
   QObject::connect(CondLabel, SIGNAL(toggled(bool)), this, SLOT(SetLabel(bool)));
   QObject::connect(IntBox, SIGNAL(toggled(bool)), this, SLOT(SetIntDraw(bool)));
   QObject::connect(MaxXBox, SIGNAL(toggled(bool)), this, SLOT(SetXMaxDraw(bool)));
   QObject::connect(MaxYBox, SIGNAL(toggled(bool)), this, SLOT(SetYMaxDraw(bool)));
   QObject::connect(MaxCBox, SIGNAL(toggled(bool)), this, SLOT(SetCMaxDraw(bool)));
   QObject::connect(ArrayAllButton, SIGNAL(clicked()), this, SLOT(ArrayAll()));
   QObject::connect(PrintLog, SIGNAL(clicked()), this, SLOT(PrintConditionLog()));
   QObject::connect(SaveCon, SIGNAL(clicked()), this, SLOT(SaveCondition()));
   QObject::connect(Win1_low, SIGNAL(returnPressed()), Win1_up, SLOT(selectAll()));
   QObject::connect(Win1_up, SIGNAL(returnPressed()), Win2_low, SLOT(selectAll()));
   QObject::connect(Win2_low, SIGNAL(returnPressed()), Win2_up, SLOT(selectAll()));
   QObject::connect(Win2_up, SIGNAL(returnPressed()), Win1_low, SLOT(selectAll()));
   QObject::connect(Win1_low, SIGNAL(returnPressed()), Win1_up, SLOT(setFocus()));
   QObject::connect(Win1_up, SIGNAL(returnPressed()), Win2_low, SLOT(setFocus()));
   QObject::connect(Win2_low, SIGNAL(returnPressed()), Win2_up, SLOT(setFocus()));
   QObject::connect(Win2_up, SIGNAL(returnPressed()), Win1_low, SLOT(setFocus()));
   QObject::connect(XMeanBox, SIGNAL(toggled(bool)), this, SLOT(SetXMeanDraw(bool)));
   QObject::connect(YMeanBox, SIGNAL(toggled(bool)), this, SLOT(SetYMeanDraw(bool)));
   QObject::connect(XRMSBox, SIGNAL(toggled(bool)), this, SLOT(SetXRMSDraw(bool)));
   QObject::connect(YRMSBox, SIGNAL(toggled(bool)), this, SLOT(SetYRMSDraw(bool)));
   QObject::connect(CondVisibleChk, SIGNAL(toggled(bool)), this, SLOT(SetCondVisible(bool)));
   QObject::connect(ModifyButton, SIGNAL(clicked()), this, SLOT(ModifyButton_clicked()));
   QObject::connect(NPointsSpin, SIGNAL(valueChanged(int)), this, SLOT(NPointsSpin_valueChanged(int)));
   QObject::connect(CutTable, &QTableWidget::cellChanged, this, &TGo4ConditionEditor::CutTable_valueChanged);
   QObject::connect(CutTable, &QTableWidget::customContextMenuRequested, this, &TGo4ConditionEditor::CutTable_contextMenuRequested);
   QObject::connect(EllipseTiltDial, SIGNAL(valueChanged(int)), this, SLOT(EllipseTheta_valueChanged(int)));
   QObject::connect(AutoRefreshBox, SIGNAL(toggled(bool)), this, SLOT(EllipseRefreshBox_toggled(bool)));
   QObject::connect(EllipseA1Spinbox, SIGNAL(valueChanged(double)), this, SLOT(EllipseA1_valueChanged(double)));
   QObject::connect(EllipseA2Spinbox, SIGNAL(valueChanged(double)), this, SLOT(EllipseA2_valueChanged(double)));
   QObject::connect(EllipseCxSpinbox, SIGNAL(valueChanged(double)), this, SLOT(EllipseCx_valueChanged(double)));
   QObject::connect(EllipseCySpinbox, SIGNAL(valueChanged(double)), this, SLOT(EllipseCy_valueChanged(double)));
   QObject::connect(EllipseTiltEdit, SIGNAL(returnPressed()), this, SLOT(EllipseTheta_returnPressed()));
   QObject::connect(EllipseNptsSpin, SIGNAL(valueChanged(int)), this, SLOT(EllipseNPoints_valueChanged(int)));
   QObject::connect(CircleBox, SIGNAL(toggled(bool)), EllipseA2Spinbox, SLOT(setDisabled(bool)));
   QObject::connect(CircleBox, SIGNAL(toggled(bool)), EllipseTiltDial, SLOT(setDisabled(bool)));
   QObject::connect(CircleBox, SIGNAL(toggled(bool)), EllipseTiltEdit, SLOT(setDisabled(bool)));
   QObject::connect(BoxshapeBox, SIGNAL(toggled(bool)), EllipseNptsSpin, SLOT(setDisabled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(toggled(bool)), EllipseCxSpinbox, SLOT(setDisabled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(toggled(bool)), EllipseCySpinbox, SLOT(setDisabled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(toggled(bool)), EllipseA1Spinbox, SLOT(setDisabled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(toggled(bool)), EllipseA2Spinbox, SLOT(setDisabled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(toggled(bool)), EllipseTiltDial, SLOT(setDisabled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(toggled(bool)), EllipseTiltEdit, SLOT(setDisabled(bool)));
   QObject::connect(CircleBox, SIGNAL(clicked(bool)), this, SLOT(EllipseCircleBox_toggled(bool)));
   QObject::connect(BoxshapeBox, SIGNAL(clicked(bool)), this, SLOT(EllipseCircleBox_toggled(bool)));
   QObject::connect(EllipseBox, SIGNAL(clicked(bool)), this, SLOT(EllipseCircleBox_toggled(bool)));
   QObject::connect(FreeshapeBox, SIGNAL(clicked(bool)), this, SLOT(EllipseCircleBox_toggled(bool)));

   LCDCounts->setDigitCount(10);
   LCDTrueCounts->setDigitCount(10);

   setWindowTitle("Condition editor");
   ResetWidget();
   fiSelectedIndex = -1;
   parentWidget()->adjustSize();
   fbDrawOnNextRefresh = false;
   fbEllipseAutoRefresh = false;
   fiLastChangeValue = -1;
   CutTable->setContextMenuPolicy(Qt::CustomContextMenu);
   fbNewWindow=true;
}


bool TGo4ConditionEditor::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   if (!cl) return false;
   bool res = cl->InheritsFrom(TGo4Condition::Class());
//   res = res || cl->InheritsFrom(TH1::Class());
   return res;
}

void TGo4ConditionEditor::DropItem(const char* itemname, TClass* cl, int kind)
{
   if (!cl) return;

   if (cl->InheritsFrom(TGo4Condition::Class()))
      WorkWithCondition(itemname);

// uncomment this to enable histogram drop on condition editor

//   else
//   if (cl->InheritsFrom(TH1::Class()) && (fiSelectedIndex==-1)) {
//      RemoveLink("Histogram");
//      AddLink(itemname, "Histogram");
//      GetLinked("Histogram", 1);
//
//      auto cond = GetLinkedCast<TGo4Condition>("Condition");
//      if (cond) {
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
   if (strcmp(linkname,"Condition") == 0) {
      TGo4Condition* cond = dynamic_cast<TGo4Condition*> (obj);
      if (cond) cond->SetChanged(kFALSE);
   }

   RefreshWidget(false);
}

void TGo4ConditionEditor::linkedObjectRemoved(const char* linkname)
{
   if (strcmp(linkname,"Condition") == 0)
      ShootCloseWidget(true);
}

void TGo4ConditionEditor::WorkWithCondition(const char* itemname)
{
   const char* conditemname = GetLinkedName("Condition");

   auto con = GetLinkedCast<TGo4Condition>("Condition");
   if (con && (con->IsChanged() != 0) && (strcmp(conditemname,itemname) != 0)) {
      QMessageBox msgBox(QMessageBox::Question, "Condition editor",
                         QString("Current condition %1 is modified!\nNew condition %2 is selected.").arg(conditemname).arg(itemname));

      auto btnContinue = msgBox.addButton("Continue with current", QMessageBox::ActionRole);
      auto btnNew = msgBox.addButton("Start with new", QMessageBox::ActionRole);
      msgBox.setDefaultButton(btnContinue);

      msgBox.exec();
      if (msgBox.clickedButton() == btnContinue)
         return;
   }

   if (!conditemname || (strcmp(conditemname,itemname) != 0)) {
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

   if (!panel) {
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

   //CondTabs->setTabEnabled(1, false);
   //CondTabs->setTabEnabled(2, false);
}

void TGo4ConditionEditor::RefreshWidget(bool checkindex)
{
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (!cond) return;
   const char* conditemname = GetLinkedName("Condition");
   TGo4ViewPanel* panel = WhereItemDrawn(conditemname);
   TPad* pad = !panel ? nullptr : panel->FindPadWithItem(conditemname);

   TGo4BrowserProxy* br = Browser();
   TGo4ServerProxy* serv = br ? br->DefineServerObject(conditemname) : nullptr;
   UpdateCon->setEnabled(!serv || serv->CanSubmitObjects());

   PleaseUpdateLabel->setVisible(cond->IsChanged() != 0);
   fiLastChangeValue = cond->IsChanged();

   QString hitemname;
   TH1* histo = nullptr;
   if (panel && pad) {
      histo = panel->GetPadHistogram(pad);
      hitemname = panel->GetDrawObjectLinkName(pad, histo);
      if (checkindex) {
         QString selname = panel->GetSelectedMarkerName(pad);
         int selindex = panel->GetSelectedMarkerIndex(pad);
         if (selname == cond->GetName())
           fiSelectedIndex = selindex;
      }
   }
   if (!histo) histo = GetLinkedCast<TH1>("Histogram");

   fbTypingMode = false;

   TGo4CondArray* arr = dynamic_cast<TGo4CondArray*> (cond);

   ArrayAllButton->setVisible(arr != nullptr);
   ArrayElements->setVisible(arr != nullptr);
   if (!arr) {
      fiSelectedIndex = -1;
   } else {
      ArrayElements->setMaximum(arr->GetNumber()-1);
      if (fiSelectedIndex >= arr->GetNumber()-1)
        fiSelectedIndex = arr->GetNumber()-1;
      ArrayElements->setValue(fiSelectedIndex);

      cond = SelectedCondition();
   }

   ModifyButton->setVisible(panel && (!arr || (fiSelectedIndex >= 0)));

   TGo4WinCond* wcond = dynamic_cast<TGo4WinCond*> (cond);
   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (cond);
   TGo4ShapedCond* econd = dynamic_cast<TGo4ShapedCond*> (cond);
   TGo4ListCond* lcond = dynamic_cast<TGo4ListCond*> (cond);

   if (wcond) {
      if (wcond->GetDimension() == 2)
         CondClassLbl->setText("Win 2-D  ");
      else
         CondClassLbl->setText("Win 1-D  ");
   } else if (econd) {
      if (econd->IsEllipse())
         CondClassLbl->setText("Ellipse  ");
      else if (econd->IsCircle())
         CondClassLbl->setText("Circle  ");
      else if (econd->IsBox())
         CondClassLbl->setText("Box  ");
      else
         CondClassLbl->setText("Free shape ");
   } else if (pcond)
      CondClassLbl->setText("Polygon  ");
   else if (lcond)
      CondClassLbl->setText("Value list  ");
   else
      CondClassLbl->setText("");

   QString infolbl;

   const char* hname = cond->GetLinkedHistogram();
   if (!hname || (*hname == 0)) {
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

   if (panel) {
     if (infolbl.length() > 0)
        infolbl += "  ";
     infolbl += "Drawn: ";
     infolbl += panel->objectName();
     if (pad && (pad != (TPad*)panel->GetCanvas())) {
        infolbl += ", ";
        infolbl += pad->GetName();
     }
   }
   DrawInfoLbl->setText(infolbl);

   CondTabs->setEnabled(true);

   ResultCombo->setEnabled(true);
   InvertCombo->setEnabled(true);

   if (cond->IsEnabled())
      ResultCombo->setCurrentIndex(0);
   else if (cond->FixedResult())
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
      sratio = QString("%1%2").arg(ratio*100,4,'f',2).arg("%");
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

   if (wcond) {
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

   ShowEllipseWidget(econd != nullptr); // hide all elements on shape tab to reduce minimum window size
   int oldindex = CondTabs->currentIndex();
   CondTabs->setCurrentIndex(2); // JAM: need this trick to retrieve actual tab limits with hidden icons?
   CondTabs->setCurrentIndex(oldindex);

  if (!pcond && ((CondTabs->currentIndex()==1) || (CondTabs->currentIndex()==2)))
     CondTabs->setCurrentIndex(0); // switch to wincond defaults when changing from polycond type

  if (!econd && pcond && (CondTabs->currentIndex()==2))
      CondTabs->setCurrentIndex(1); // switch to polycond defaults when changing from shape type

  if(lcond)
    CondTabs->setCurrentIndex(1);

   CondTabs->setTabEnabled(1, pcond || lcond);
   CondTabs->setTabEnabled(2, econd != nullptr);

   CondTabs->setTabEnabled(3, !lcond);
   CondTabs->setTabEnabled(4, !lcond);
   CondTabs->setTabEnabled(5, !lcond);

   if (pcond) {
      FillCutWidget(pcond->GetCut(kFALSE));
      CondTabs->setTabText(1, "Cut");
      if (econd) {
         FillEllipseWidget(econd);
         if (fbNewWindow)
            CondTabs->setCurrentIndex(2);
      } else {
         if (fbNewWindow)
            CondTabs->setCurrentIndex(1);
      }
   } else if (lcond) {
      FillListWidget(lcond);
      CondTabs->setTabText(1, "Values");
      if (fbNewWindow)
         CondTabs->setCurrentIndex(1);
   } else {
      if (fbNewWindow)
         CondTabs->setCurrentIndex(0);
   }

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
      if (!panel && HistogramChk->isChecked() &&
         Browser()->DefineRelatedObject(conditemname, hname, hitemname))
           DrawCondition(false);
   }
   fbNewWindow = false;
}

TGo4Condition* TGo4ConditionEditor::SelectedCondition()
{
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (!cond) return nullptr;

   TGo4CondArray* arr = dynamic_cast<TGo4CondArray*> (cond);

   if (!arr || (fiSelectedIndex==-1)) {
     if(arr) arr->SetMultiEdit(kTRUE);
     return cond;
   }
   if (fiSelectedIndex<arr->GetNumber()) {
     arr->SetMultiEdit(kFALSE);
     arr->SetCurrentIndex(fiSelectedIndex);
     return arr->At(fiSelectedIndex);
   }
   return cond;
}

void TGo4ConditionEditor::PleaseUpdateSlot()
{
   // GetLinked("Condition", 0);
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
   if (!fbTypingMode || !cond) return;
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
   if (!fbTypingMode || !cond) return;
   cond->Invert(mode==1);
   PleaseUpdateSlot();
}

void TGo4ConditionEditor::ClearCounters()
{
   const char* conditemname = GetLinkedName("Condition");
   auto con = GetLinkedCast<TGo4Condition>("Condition");
   if (!con || !conditemname) return;

   TString objname;
   TGo4ServerProxy* an = Browser()->DefineAnalysisObject(conditemname, objname);
   if (an) {
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
   if (!fbTypingMode || !cond) return false;

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
   if (!fbTypingMode || !cond) return;
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

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
void TGo4ConditionEditor::enterEvent(QEvent *)
#else
void TGo4ConditionEditor::enterEvent(QEnterEvent *)
#endif
{
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (cond && (fiLastChangeValue != cond->IsChanged()))
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
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (cond) cond->SetChanged(changed);
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
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (!cond) return;

   const char* conditemname = GetLinkedName("Condition");
   if (!conditemname) return;

   TGo4ViewPanel* panel = nullptr;

   if (useactive)
      panel = LastActivePanel();
   if (panel) {
      DrawItem(conditemname, panel, nullptr, false, 0);
      RedrawCondition();
      RefreshWidget(false);
      panel->setFocus();
      setFocus();
      return;
   }

   panel = WhereItemDrawn(conditemname);
   TPad* pad = panel ? panel->FindPadWithItem(conditemname) : nullptr;

   if (panel) {
      if (pad)
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

   const char* hname = nullptr;
   if (cond->IsHistogramLink())
      hname = cond->GetLinkedHistogram();
   TString hitemname;

   TGo4BrowserProxy* br = Browser();

   if (hname && br->DefineRelatedObject(conditemname, hname, hitemname)) {
      panel = DrawItem(hitemname.Data(), nullptr);
      RemoveLink("Histogram");
      AddLink(hitemname.Data(), "Histogram");
      DrawItem(conditemname, panel, nullptr, false, 0);
      TPad *pad = panel->FindPadWithItem(conditemname);
      RefreshWidget(false);
      RedrawCondition();
      panel->setFocus();
      setFocus();
   }
}

void TGo4ConditionEditor::RedrawCondition()
{
   const char* conditemname = GetLinkedName("Condition");
   if (!conditemname) return;

   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (!cond) return;

   TGo4ViewPanel* panel = WhereItemDrawn(conditemname);
   if (!panel) return;

   TPad* pad = panel->FindPadWithItem(conditemname);
   if (!pad) return;

   panel->SetSelectedMarker(pad, cond->GetName(), fiSelectedIndex);
   panel->MarkPadModified(pad);
   panel->ShootRepaintTimer(pad);
}

void TGo4ConditionEditor::PrintConditionLog()
{
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (cond) {
      cond->PrintCondition(kFALSE); // JAM want additional output of specific infos, no poly coords
      cond->Print("go4log-limits-stats");
   }
}

bool TGo4ConditionEditor::PrepareForAnalysis()
{
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (!cond) return false;

   SetChangeFlag(false);

   const char* hname = cond->GetLinkedHistogram();
   if (hname && (strlen(hname) != 0)) {
      TString foldername, objname;
      TGo4Slot::ProduceFolderAndName(hname, foldername, objname);
      cond->SetHistogram(objname.Data());
   }
   return true;
}

void TGo4ConditionEditor::ModifyButton_clicked()
{
   auto cond = GetLinkedCast<TGo4Condition>("Condition");
   if (!cond) return;

   const char* conditemname = GetLinkedName("Condition");
   TGo4ViewPanel* panel = WhereItemDrawn(conditemname);
   TPad* pad = !panel ? nullptr : panel->FindPadWithItem(conditemname);

   if (!panel || !pad) return;

   panel->SetActivePad(pad);

   panel->setFocus();

   panel->SetSelectedMarker(pad, cond->GetName(), fiSelectedIndex);

   panel->StartConditionEditing(pad);
}

void TGo4ConditionEditor::FillCutWidget(TCutG* cut)
{
   bool old = fbTypingMode;
   fbTypingMode = false;
   CutTable->setColumnCount(2);
   if (!cut) {
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

void TGo4ConditionEditor::FillEllipseWidget(TGo4ShapedCond* elli)
{
   if(!elli) return;
   bool old = fbTypingMode;
   fbTypingMode = false;
   double a1=0,a2=0,x=0,y=0;
   int theta=0;

   elli->GetRadius(a1,a2);
   elli-> GetCenter(x,y);
   theta= (int) elli->GetTheta();

   EllipseCxSpinbox->setValue(x);
   EllipseCySpinbox->setValue(y);
   EllipseA1Spinbox->setValue(a1);
   EllipseA2Spinbox->setValue(a2);
   EllipseTiltDial->setValue(theta);
   EllipseTiltEdit->setText(QString::number(theta));

   CircleBox->setChecked(elli->IsCircle());
   EllipseBox->setChecked(elli->IsEllipse());
   BoxshapeBox->setChecked(elli->IsBox());
   FreeshapeBox->setChecked(elli->IsFreeShape());
   EllipseNptsSpin->setValue(elli->GetResolution());
   fbTypingMode = old;
}

void TGo4ConditionEditor::ShowEllipseWidget(bool show)
{
    EllipseCxSpinbox->setVisible(show);
    EllipseCySpinbox->setVisible(show);
    EllipseA1Spinbox->setVisible(show);
    EllipseA2Spinbox->setVisible(show);
    EllipseTiltDial->setVisible(show);
    EllipseTiltEdit->setVisible(show);

    ShapegroupBox->setVisible(show);
    CircleBox->setVisible(show);
    EllipseBox->setVisible(show);
    BoxshapeBox->setVisible(show);
    FreeshapeBox->setVisible(show);
    EllipseNptsSpin->setVisible(show);

    AutoRefreshBox->setVisible(show);
    EllipseCenterLabel->setVisible(show);
    EllipseTitlLabel->setVisible(show);
    EllipseRadiusLabel->setVisible(show);
    EllipseNptsLabel->setVisible(show);
    EllipsCxLabel->setVisible(show);
    EllipsCyLabel->setVisible(show);
    EllipsA1Label->setVisible(show);
    EllipsA2Label->setVisible(show);

    verticalLayout_2->setEnabled(show);
    verticalLayout_5->setEnabled(show);
    horizontalLayout_4->setEnabled(show);

}

void TGo4ConditionEditor::FillListWidget(TGo4ListCond* lcon)
{
   bool old = fbTypingMode;
   fbTypingMode = false;
   CutTable->setColumnCount(1);
   if (!lcon) {
      CutTable->setRowCount(0);
      NPointsSpin->setValue(0);
   } else {
      int points = lcon->GetNumValues();
      CutTable->setRowCount(points);
      NPointsSpin->setValue(points);
      for (int n=0;n<points;n++) {
         int val=lcon->GetValue(n);
         CutTable->setItem(n, 0, new QTableWidgetItem(QString::number(val)));
         //CutTable->setItem(n, 1, new QTableWidgetItem(QString::number(y)));
         CutTable->setVerticalHeaderItem(n, new QTableWidgetItem(QString::number(n)));
      }
   }

   fbTypingMode = old;
}


void TGo4ConditionEditor::NPointsSpin_valueChanged(int npoint)
{
   if (!fbTypingMode) return;
   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TGo4ListCond* lcond = dynamic_cast<TGo4ListCond*> (SelectedCondition());
   if (pcond) {
      TCutG *cut = pcond->GetCut(kFALSE);

      if (!cut) {
         cut = new TCutG("conditioncut", npoint);
         Double_t x = 0., y = 0.;
         for (int n = 0; n < npoint - 1; n++)
            cut->SetPoint(n, n * 10, n * 10);
         cut->GetPoint(0, x, y);
         cut->SetPoint(npoint - 1, x, y);
         pcond->SetValues(cut);
      } else {
         if (npoint == 0) {
            pcond->SetValues(0);
            cut = 0;
         } else {
            int old = cut->GetN();
            Double_t x = 0., y = 0.;
            if (old > 1)
               cut->GetPoint(old - 2, x, y);
            cut->Set(npoint);
            for (int n = (old - 1 >= 0 ? old - 1 : 0); n < npoint - 1; n++)
               cut->SetPoint(n, x, y + (n - old + 2) * 10);
            cut->GetPoint(0, x, y);
            cut->SetPoint(npoint - 1, x, y);
         }
      }
      PleaseUpdateSlot();
      FillCutWidget(cut);

   } else if (lcond) {
      lcond->Resize(npoint);
      PleaseUpdateSlot();
      FillListWidget(lcond);
   }
   RedrawCondition();
}

void TGo4ConditionEditor::CutTable_valueChanged( int nrow, int ncol)
{
   if (!fbTypingMode) return;
   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TGo4ListCond* lcond = dynamic_cast<TGo4ListCond*> (SelectedCondition());
   bool ok = false;
   double zn = CutTable->item(nrow, ncol)->text().toDouble(&ok);
   if (!ok) return;

   if (pcond) {
      TCutG *cut = pcond->GetCut(kFALSE);
      if (!cut)
         return;
      if (ncol == 0)
         cut->GetX()[nrow] = zn;
      else
         cut->GetY()[nrow] = zn;
      if ((nrow == 0) || (nrow == cut->GetN() - 1)) {
         int nrow1 = (nrow == 0) ? cut->GetN() - 1 : 0;
         fbTypingMode = false;
         CutTable->setItem(nrow1, ncol, new QTableWidgetItem(CutTable->item(nrow, ncol)->text()));
         if (ncol == 0)
            cut->GetX()[nrow1] = zn;
         else
            cut->GetY()[nrow1] = zn;
         fbTypingMode = true;
      }
   } else if (lcond) {
      lcond->SetValue(nrow, zn);
      int numvals = lcond->GetNumValues();
      if ((nrow == 0) || (nrow == numvals - 1)) {
         int nrow1 = (nrow == 0) ? numvals - 1 : 0;
         fbTypingMode = false;
         CutTable->setItem(nrow1, ncol, new QTableWidgetItem(CutTable->item(nrow, ncol)->text()));
         lcond->SetValue(nrow1, zn);
         fbTypingMode = true;
      }
   } else {
      return;
   }

   PleaseUpdateSlot();

   RedrawCondition();
}

void TGo4ConditionEditor::CutTable_contextMenuRequested( const QPoint & pos )
{
   if (!fbTypingMode) return;
   QTableWidgetItem* item = CutTable->itemAt (pos);
   if (!item) return;

   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TGo4ListCond* lcond = dynamic_cast<TGo4ListCond*> (SelectedCondition());

   if(!pcond && !lcond) return;

   int maxn = 0;
   if (pcond) {
      TCutG *cut = pcond->GetCut(kFALSE);
      if (!cut)
         return;
      maxn = cut->GetN();
      // only internal points should be allowed to deleted
   } else if (lcond) {
      maxn = lcond->GetNumValues();
   } else {
      return;
   }
   int nrow = CutTable->row(item);

   QMenu menu;

   auto actInsert = new QAction("Insert point", &menu);
   menu.addAction(actInsert);
   QObject::connect(actInsert, &QAction::triggered, [this, nrow]() { CutTable_contextMenuHandler(nrow, false); });

   auto actDelete = new QAction("Delete point", &menu);
   menu.addAction(actDelete);
   QObject::connect(actDelete, &QAction::triggered, [this, nrow]() { CutTable_contextMenuHandler(nrow, true); });
   actDelete->setEnabled((nrow > 0) && (nrow < maxn - 1));

   menu.exec(CutTable->mapToGlobal(pos));
}

void TGo4ConditionEditor::CutTable_contextMenuHandler(int id, bool is_delete)
{
   TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (SelectedCondition());
   TGo4ListCond* lcond = dynamic_cast<TGo4ListCond*> (SelectedCondition());

   if (pcond) {
      TCutG *cut = pcond->GetCut(kFALSE);
      if (!cut)
         return;

      if (is_delete) {
         cut->RemovePoint(id);
      } else { // insert point
         Int_t npoints = cut->GetN();
         if (id >= npoints)
            return;
         cut->Set(npoints + 1);

         Double_t x, y;
         for (int n = npoints; n > id; n--) {
            cut->GetPoint(n - 1, x, y);
            cut->SetPoint(n, x, y);
         }

         if (id > 0) {
            Double_t x1, y1;
            cut->GetPoint(id - 1, x1, y1);
            cut->SetPoint(id, (x1 + x) / 2., (y + y1) / 2.);
         }
      }

      FillCutWidget(cut);

   } else if (lcond) {
      if (is_delete) {
         lcond->RemoveValue(id);
      } else { // insert point
         Int_t npoints = lcond->GetNumValues();
         if (id >= npoints)
            return;
         int x1 = lcond->GetValue(id);
         int x2 = lcond->GetValue(id - 1);
         lcond->InsertValue(id, ((x1 + x2) / 2));
      }
      FillListWidget(lcond);
   }
   PleaseUpdateSlot();
   RedrawCondition();
}

void TGo4ConditionEditor::EllipseTheta_valueChanged(int deg)
{
   if (!fbTypingMode)
      return;
   EllipseTiltEdit->setText(QString::number(deg));
   if (fbEllipseAutoRefresh) {
      TGo4ShapedCond *econd = dynamic_cast<TGo4ShapedCond *>(SelectedCondition());
      if (econd)
         econd->SetTheta((double)deg);
      PleaseUpdateSlot();
      RedrawCondition();
   }
}

void TGo4ConditionEditor::EllipseCx_valueChanged(double x)
{
   if (!fbTypingMode)
      return;
   if (fbEllipseAutoRefresh) {
      TGo4ShapedCond *econd = dynamic_cast<TGo4ShapedCond *>(SelectedCondition());
      if (econd) {
         double y = EllipseCySpinbox->value();
         econd->SetCenter(x, y);
         PleaseUpdateSlot();
         RedrawCondition();
      }
   }
}
void TGo4ConditionEditor::EllipseCy_valueChanged(double y)
{
   if (!fbTypingMode)
      return;
   if (fbEllipseAutoRefresh) {
      TGo4ShapedCond *econd = dynamic_cast<TGo4ShapedCond *>(SelectedCondition());
      if (econd) {
         double x = EllipseCxSpinbox->value();
         econd->SetCenter(x, y);
      }
      PleaseUpdateSlot();
      RedrawCondition();
   }
}

void TGo4ConditionEditor::EllipseA1_valueChanged(double r1)
{
   if (!fbTypingMode)
      return;
   if (fbEllipseAutoRefresh)
      UpdateEllipse();
}

void TGo4ConditionEditor::EllipseA2_valueChanged(double r2)
{
   if (!fbTypingMode)
      return;
   if (fbEllipseAutoRefresh)
      UpdateEllipse();
}

void TGo4ConditionEditor::EllipseRefreshBox_toggled(bool on)
{
  if (!fbTypingMode) return;
  fbEllipseAutoRefresh=on;
  if(fbEllipseAutoRefresh) UpdateEllipse();
}


void TGo4ConditionEditor::EllipseCircleBox_toggled(bool on)
{
  if (!fbTypingMode) return;
  if(fbEllipseAutoRefresh) UpdateEllipse();
}


void TGo4ConditionEditor::EllipseTheta_returnPressed()
{
  if (!fbTypingMode) return;
  bool ok = false;
  Int_t theta = EllipseTiltEdit->text().toInt(&ok);
  if(theta < 0) theta = 0;
  theta = theta % 360;
  if(ok)
    {
      EllipseTiltDial->setValue(theta);
    }
  if(fbEllipseAutoRefresh) UpdateEllipse();

}

void TGo4ConditionEditor::EllipseNPoints_valueChanged( int npoint )
{
  if (!fbTypingMode) return;
  if(fbEllipseAutoRefresh) UpdateEllipse();
}

void TGo4ConditionEditor::UpdateEllipse()
{
   TGo4ShapedCond *econd = dynamic_cast<TGo4ShapedCond *>(SelectedCondition());
   if (!econd) {
      std::cout << "UpdateEllipse did not find ellipse condition!!!" << std::endl;
      return;
   }

   if (FreeshapeBox->isChecked()) {
      econd->SetFreeShape();
   } else {
      double cx = EllipseCxSpinbox->value();
      double cy = EllipseCySpinbox->value();
      double a1 = EllipseA1Spinbox->value();
      double a2 = EllipseA2Spinbox->value();
      double th = EllipseTiltDial->value();
      if (econd->IsFreeShape()) {
         // transition from free shape to other form: use polygon geometry to init pars:
         cx = (econd->GetXUp() + econd->GetXLow()) / 2; // mean of coordinates is center
         cy = (econd->GetYUp() + econd->GetYLow()) / 2;
         // a1=(econd->GetXUp() - econd->GetXLow())/2; // half diameter is radius
         // a2=(econd->GetYUp() - econd->GetYLow())/2;
         //  better: use minimum and maximum radial distance
         TCutG *cut = econd->GetCut(false);
         Int_t n = cut->GetN();
         Double_t *xarr = cut->GetX();
         Double_t *yarr = cut->GetY();
         TArrayD rarr(n);
         for (int i = 0; i < n; ++i) {
            rarr[i] = TMath::Sqrt(TMath::Power((xarr[i] - cx), 2) + TMath::Power((yarr[i] - cy), 2));
         }
         Int_t nrmax = TMath::LocMax(n, rarr.GetArray());
         Int_t nrmin = TMath::LocMin(n, rarr.GetArray());
         a1 = rarr[nrmax];
         a2 = rarr[nrmin];
         // now evaluate angle:
         th = 0;
         if (a1) {
            th = TMath::ACos((econd->GetXUp() - cx) / a1) * 180 / TMath::Pi();
         }
      }

      if (CircleBox->isChecked())
         econd->SetCircle(cx, cy, a1, EllipseNptsSpin->value());
      else if (EllipseBox->isChecked())
         econd->SetEllipse(cx, cy, a1, a2, th, EllipseNptsSpin->value());
      else if (BoxshapeBox->isChecked()) {
         if (econd->IsFreeShape()) {
            // transition to box will use different defaults for extensions, otherwise we always get increasing box
            a1 = (econd->GetXUp() - econd->GetXLow()) / 2; // half diameter is radius
            a2 = (econd->GetYUp() - econd->GetYLow()) / 2;
            th = 0; // todo: correctly conserve tilt angle when switching to/fro between free and box
         }

         econd->SetBox(cx, cy, a1, a2, th);
      }
   } // no freeshape selected
   PleaseUpdateSlot();
   RedrawCondition();
   FillEllipseWidget(econd);
}
