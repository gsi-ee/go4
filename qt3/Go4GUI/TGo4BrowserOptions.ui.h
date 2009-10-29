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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

void TGo4BrowserOptions::init()
{
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

void TGo4BrowserOptions::ClearBtn_clicked()
{
   TGo4AnalysisProxy* an = Browser()->FindAnalysis();
   if (an) {
      an->ClearAnalysisObject("Histograms");
      an->ClearAnalysisObject("Conditions");
   }
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

