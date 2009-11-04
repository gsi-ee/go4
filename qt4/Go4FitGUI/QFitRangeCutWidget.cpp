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

#include "QFitRangeCutWidget.h"

#include "QFitItem.h"
#include "TGo4FitComponent.h"
#include "TCutG.h"

QFitRangeCutWidget::QFitRangeCutWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{
   setupUi(this);
}

TGo4FitComponent* QFitRangeCutWidget::GetComp() {
   return dynamic_cast<TGo4FitComponent*> (GetItem()->Parent()->Object());
}

int QFitRangeCutWidget::GetCutIndex() {
  return GetItem()->Tag();
}

TCutG* QFitRangeCutWidget::GetCut() {
  return dynamic_cast<TCutG*> (GetObject());
}

void QFitRangeCutWidget::FillSpecificData() {
  QFitNamedWidget::FillSpecificData();
  TGo4FitComponent* comp = GetComp();
  TCutG* cut = GetCut();
  if (comp && cut) {
     ExcludeCutChk->setChecked(comp->IsRangeCutExcluding(GetCutIndex()));
     NumPointsSpin->setValue(cut->GetN());
     FillXYPointsTable();
  }
}

void QFitRangeCutWidget::FillXYPointsTable() {
   TCutG* cut = GetCut();
   if (cut==0) return;

   XYTable->setRowCount(cut->GetN());

   for (int n=0;n<cut->GetN();n++) {
      Double_t x,y;
      cut->GetPoint(n, x,y);
      XYTable->setItem(n, 0, new QTableWidgetItem(QString::number(x)));
      XYTable->setItem(n, 1, new QTableWidgetItem(QString::number(y)));
      XYTable->setVerticalHeaderItem(n, new QTableWidgetItem(QString::number(n)));
   }
}


void QFitRangeCutWidget::NumPointsSpin_valueChanged( int npoint )
{
  if(!fbFillWidget && (GetCut()!=0)) {
      TCutG* cut = GetCut();
      int old = cut->GetN();
      Double_t x=0., y=0.;
      if (old>1) cut->GetPoint(old-2, x, y);
      cut->Set(npoint);
      for(int n= (old-1>=0 ? old-1 : 0); n<npoint-1; n++)
        cut->SetPoint(n, x, y+(n-old+2)*10);

      cut->GetPoint(0, x, y);
      cut->SetPoint(npoint-1, x, y);

      fbFillWidget = TRUE;
      FillXYPointsTable();
      fbFillWidget = FALSE;
  }
}


void QFitRangeCutWidget::ExcludeCutChk_toggled( bool chk)
{
    if(!fbFillWidget && (GetComp()!=0))
      GetComp()->SetRangeCutExcluding(GetCutIndex(), chk);
}


void QFitRangeCutWidget::XYTable_valueChanged( int nrow, int ncol)
{
  if(!fbFillWidget && (GetCut()!=0)) {
     TCutG* cut = GetCut();
     bool ok;
     double zn = XYTable->item(nrow, ncol)->text().toDouble(&ok);
     if(ok) {
         if (ncol==0) cut->GetX()[nrow] = zn;
                 else cut->GetY()[nrow] = zn;
         if ((nrow==0) || (nrow==cut->GetN()-1)) {
            int nrow1 = (nrow==0) ? cut->GetN()-1 : 0;
            fbFillWidget = TRUE;
            XYTable->setItem(nrow1, ncol, new QTableWidgetItem(XYTable->item(nrow, ncol)->text()));
            if (ncol==0) cut->GetX()[nrow1] = zn;
                    else cut->GetY()[nrow1] = zn;
            fbFillWidget = FALSE;
         }
     }
  }
}
