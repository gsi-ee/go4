#include "TGo4HistogramInfo.h"

#include "TClass.h"
#include "TH1.h"
#include "TDatime.h"

#include "TGo4BrowserProxy.h"
#include "TGo4Log.h"
#include "TGo4HistogramStatus.h"


TGo4HistogramInfo::TGo4HistogramInfo(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections
			// from ui file to the setupUI
   setCaption("Histogram Info:");
   ResetWidget();
}

void TGo4HistogramInfo::HisPrintButton_clicked()
{
   ServiceCall("PrintAnalysisHistograms");
}

void TGo4HistogramInfo::HisDrawButton_clicked()
{
   DrawItem(HisnameLbl->text().latin1());
}

void TGo4HistogramInfo::HisInfoButton_clicked()
{
   WorkWithHistogram(HisnameLbl->text());
}

bool TGo4HistogramInfo::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   return cl==0 ? false : cl->InheritsFrom(TH1::Class());
}

void TGo4HistogramInfo::DropItem(const char* itemname, TClass* cl, int kind)
{
   if (cl==0) return;

   if (cl->InheritsFrom(TH1::Class()))
      WorkWithHistogram(itemname);
}

void TGo4HistogramInfo::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   TGo4HistogramStatus* hstate =
      dynamic_cast<TGo4HistogramStatus*>(obj);
   if (hstate!=0)
      RefreshHistogramInfo(hstate);
   else
      RefreshHistogramInfo(dynamic_cast<TH1*> (obj));
}

void TGo4HistogramInfo::WorkWithHistogram(const char* itemname)
{
   ResetWidget();

   TGo4BrowserProxy* br = Browser();
   if (br==0) return;

   HisnameLbl->setText(itemname);

   if (br->IsAnalysisItem(itemname)) {
      TGo4Slot* tgtslot = AddSlot("HistStatus");
      br->RequestObjectStatus(itemname, tgtslot);
      // addlink to observe when histogram is removed
      AddLink(itemname, "HistogramLock");
   } else {
      AddLink(itemname, "Histogram");
      TH1* h1 = dynamic_cast<TH1*> (GetLinked("Histogram",2));
      RefreshHistogramInfo(h1);
  }
  setFocus();
}

void TGo4HistogramInfo::ResetWidget()
{
   QGo4Widget::ResetWidget();

   HisnameLbl->setText("");

   PropertyBox->item(HISTITLE)->setText("Title");
   PropertyBox->item(HISCLASS)->setText("Class");
   PropertyBox->item(HISENTRIES)->setText("Entries");
   PropertyBox->item(HISX)->setText("Xrange");
   PropertyBox->item(HISXSTAT)->setText("Xstats");
   PropertyBox->item(HISY)->setText("Yrange");
   PropertyBox->item(HISYSTAT)->setText("Ystats");
   PropertyBox->item(HISZ)->setText("Title");
   PropertyBox->item(HISZSTAT)->setText("Zstats");
   PropertyBox->item(HISSIZE)->setText("Size");
   TDatime now;
   PropertyBox->item(HISTIME)->setText(now.AsSQLString());
   polish();
   update();
   show();
   raise();
}

void TGo4HistogramInfo::RefreshHistogramInfo(TH1* h1)
{
  if (h1==0)
    ResetWidget();
  else {
     TGo4HistogramStatus hstate(h1, kTRUE);
     RefreshHistogramInfo(&hstate);
  }
}

void TGo4HistogramInfo::RefreshHistogramInfo(TGo4HistogramStatus* hstate)
{
   if(hstate==0) return;
   TString str;
   PropertyBox->item(HISTITLE)->setText(hstate->GetTitle());
   PropertyBox->item(HISCLASS)->setText(hstate->GetObjectClass());
   str.Form("Entries:%.5g",hstate->GetEntries());
   PropertyBox->item(HISENTRIES)->setText(str.Data());
   str.Form("X:%d [%.3g,%.3g]",hstate->GetXbins(), hstate->GetXmin(), hstate->GetXmax());
   PropertyBox->item(HISX)->setText(str.Data());
   str.Form("Xm=%.1f, Xrms=%.1f", hstate->GetXmean(), hstate->GetXrms());
   PropertyBox->item(HISXSTAT)->setText(str.Data());

   PropertyBox->item(HISY)->setText("-");
   PropertyBox->item(HISYSTAT)->setText("-");
   PropertyBox->item(HISZ)->setText("-");
   PropertyBox->item(HISZSTAT)->setText("-");

   int dime = hstate->GetDimension();
   if(dime>1) {
      str.Form("Y:%d [%.3g,%.3g]",hstate->GetYbins(), hstate->GetYmin(), hstate->GetYmax());
      PropertyBox->item(HISY)->setText(str.Data());
      str.Form("Ym=%.1f, Yrms=%.1f", hstate->GetYmean(), hstate->GetYrms());
      PropertyBox->item(HISYSTAT)->setText(str.Data());
   }
   if(dime>2) {
      str.Form("Z:%d [%.3g,%.3g]", hstate->GetZbins(), hstate->GetZmin(), hstate->GetZmax());
      PropertyBox->item(HISZ)->setText(str.Data());
      str.Form("Zm=%.1f, Zrms=%.1f", hstate->GetZmean(), hstate->GetZrms());
      PropertyBox->item(HISZSTAT)->setText(str.Data());
   }
   str.Form("size:%d b",hstate->GetObjectSize());
   PropertyBox->item(HISSIZE)->setText(str.Data());
   PropertyBox->item(HISTIME)->setText(hstate->GetTimeString());
   polish();
   update();
   show();
   raise();
   if(TGo4Log::IsAutoEnabled())
     PrintLog_clicked();
}

void TGo4HistogramInfo::PrintLog_clicked()
{
   QString textbuffer;
   textbuffer="Histogram ";
   textbuffer+=HisnameLbl->text();
   textbuffer+=" Status: \n ";
   textbuffer+="Title: ";
   textbuffer+=PropertyBox->item(HISTITLE)->text();
   textbuffer+=" Class: ";
   textbuffer+=PropertyBox->item(HISCLASS)->text();
   textbuffer+=" ";
   textbuffer+=PropertyBox->item(HISENTRIES)->text();
   textbuffer+="\n ";
   textbuffer+=PropertyBox->item(HISX)->text();
   textbuffer+=" ";
   textbuffer+=PropertyBox->item(HISXSTAT)->text();
   textbuffer+="\n ";
   textbuffer+=PropertyBox->item(HISY)->text();
   textbuffer+=" ";
   textbuffer+=PropertyBox->item(HISYSTAT)->text();
   textbuffer+="\n ";
   textbuffer+=PropertyBox->item(HISZ)->text();
   textbuffer+=" ";
   textbuffer+=PropertyBox->item(HISZSTAT)->text();
   textbuffer+="\n ";
   textbuffer+=PropertyBox->item(HISSIZE)->text();
   textbuffer+=" Status received at: ";
   textbuffer+=PropertyBox->item(HISTIME)->text();
   TGo4Log::Message(1,textbuffer.latin1());
}
