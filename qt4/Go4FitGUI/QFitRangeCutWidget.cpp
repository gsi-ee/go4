#include "QFitRangeCutWidget.h"

#include "QFitItem.h"
#include "TGo4FitComponent.h"
#include "TCutG.h"

QFitRangeCutWidget::QFitRangeCutWidget(QWidget *parent, const char* name)
         : QFitNamedWidget(parent, name)
{ 
			setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
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

   XYTable->setNumRows(cut->GetN());

   for (int n=0;n<cut->GetN();n++) {
      Double_t x,y;
      cut->GetPoint(n, x,y);
      XYTable->setText(n, 0, QString::number(x));
      XYTable->setText(n, 1, QString::number(y));
      XYTable->verticalHeader()->setLabel(n, QString::number(n));
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
     double zn = XYTable->text(nrow, ncol).toDouble(&ok);
     if(ok) {
         if (ncol==0) cut->GetX()[nrow] = zn;
                 else cut->GetY()[nrow] = zn;
         if ((nrow==0) || (nrow==cut->GetN()-1)) {
            int nrow1 = (nrow==0) ? cut->GetN()-1 : 0;
            fbFillWidget = TRUE;
            XYTable->setText(nrow1, ncol, XYTable->text(nrow, ncol));
            if (ncol==0) cut->GetX()[nrow1] = zn;
                    else cut->GetY()[nrow1] = zn;
            fbFillWidget = FALSE;
         }
     }
  }
}
