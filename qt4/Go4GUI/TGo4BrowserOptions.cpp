#include "TGo4BrowserOptions.h"

#include "TGo4BrowserProxy.h"
#include "TGo4AnalysisProxy.h"

TGo4BrowserOptions::TGo4BrowserOptions(QWidget *parent, const char* name)
         : QGo4Widget(parent,name)
{
	setupUi(this);
}

void TGo4BrowserOptions::StartWorking()
{
   AddLink("","Browser");
   UpdateView();
}

void TGo4BrowserOptions::UpdateView()
{
   int interval = Browser()->MonitoringPeriod()/1000;
   StartMonitorBtn->setEnabled(interval<=0);
   StopMonitorBtn->setEnabled(interval>0);
}

void TGo4BrowserOptions::linkedObjectUpdated(const char* linkname, TObject* obj)
{
   if (strcmp(linkname,"Browser")==0)
      UpdateView();
}

void TGo4BrowserOptions::RefreshBtn_clicked()
{
   TGo4AnalysisProxy* an = Browser()->FindAnalysis();
   if (an!=0) an->RefreshNamesList();
   Browser()->UpdateVisibleAnalysisObjects(false);
}

void TGo4BrowserOptions::StartMonitorBtn_clicked()
{
   int period = IntervalSpin->value();

   Browser()->ToggleMonitoring(period*1000);

   UpdateView();
}

void TGo4BrowserOptions::StopMonitorBtn_clicked()
{
   Browser()->ToggleMonitoring(0);

   UpdateView();
}

void TGo4BrowserOptions::FilterBox_activated(int indx)
{
   Browser()->SetItemsFilter(indx);
}

