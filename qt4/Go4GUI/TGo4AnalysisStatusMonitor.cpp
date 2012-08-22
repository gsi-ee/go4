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

#include "TGo4AnalysisStatusMonitor.h"
#include <math.h>

#include "TGo4AnalysisClientStatus.h"
#include "TGo4BrowserProxy.h"
#include "TSystem.h"



TGo4AnalysisStatusMonitor::TGo4AnalysisStatusMonitor(QWidget *parent, const char* name)
: QGo4Widget(parent, name)
{
   setupUi(this);
   setAcceptDrops(FALSE);
   QString moviepath=":/icons/go4logorun4.gif";
   fxRunMovie= new QMovie(moviepath);
}

TGo4AnalysisStatusMonitor::~TGo4AnalysisStatusMonitor()
{
   delete fxRunMovie;
}

void TGo4AnalysisStatusMonitor::WorkWithRatemeter(TGo4Slot* slot)
{
   AddLink(slot, "Ratemeter");
}

void TGo4AnalysisStatusMonitor::linkedObjectUpdated( const char * linkname, TObject * obj )
{
   TGo4AnalysisClientStatus* status = dynamic_cast<TGo4AnalysisClientStatus*> (obj);
   if (status==0) return;
   SourceLabel->setText(status->GetCurrentSource());

   double Rate = status->GetRate();
   if(Rate>10) LCDCurrentRate->display(floor(Rate));
   else LCDCurrentRate->display(Rate);

   const char* color = "QFrame { background-color:rgb(255,0,0) }";

   if(!status->IsAnalysisRunning()) {
      Go4Pix->setWindowIcon( QIcon(":/icons/go4logo2.png"));
      fxRunMovie->stop();
   } else {
      color = "QFrame { background-color:rgb(0,255,0) }";
      Go4Pix->clear();
      Go4Pix->setMovie(fxRunMovie);
      fxRunMovie->start();
   }

   LCDCurrentRate->setStyleSheet(color);

   if(status->GetAvRate()>10)
      LCDAverageRate->display(floor(status->GetAvRate()));
   else
      LCDAverageRate->display(status->GetAvRate());
   int stime=(int)status->GetTime();
   LCDTime->display(stime);
   ulong hh=stime/3600;
   ulong mm=(stime-hh*3600)/60;
   ulong ss=(stime-hh*3600-mm*60);
   QString tooltip=QString("Time since last reset [sec] = %1:%2:%3 [hours:min:sec]").arg(hh).arg(mm).arg(ss);
   LCDTime->setToolTip(tooltip);
   QString scnt = QString("%1").arg(status->GetCurrentCount());
   if (scnt.length() > LCDProcessedEvents->numDigits())
      LCDProcessedEvents->setNumDigits(scnt.length());

   LCDProcessedEvents->display(scnt);

   DateLabel->setText(status->GetDateTime());
}

void TGo4AnalysisStatusMonitor::linkedObjectRemoved(const char * linkname)
{
   // in any case deletion of this slot means closing of widget

   if (Browser())
      Browser()->ToggleMonitoring(0);

   ShootCloseWidget();
}
