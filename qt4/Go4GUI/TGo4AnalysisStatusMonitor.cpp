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

#include "TGo4AnalysisStatusMonitor.h"

#include <QStatusBar>
#include <QMovie>

#include "TGo4AnalysisClientStatus.h"
#include "TGo4Ratemeter.h"

TGo4AnalysisStatusMonitor::TGo4AnalysisStatusMonitor(QWidget *parent, const char* name) :
   QGo4Widget(parent, name)
{
   setupUi(this);
   setAcceptDrops(false);
   fxRunMovie = new QMovie(":/icons/go4logorun4.gif");

   LCDCurrentRate->setDigitCount(8);
   LCDAverageRate->setDigitCount(8);
   LCDTime->setDigitCount(8);
   LCDProcessedEvents->setDigitCount(9);
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
   TGo4Ratemeter* rate = dynamic_cast<TGo4Ratemeter*> (obj);

   // JAM2016
   //std::cout <<"linkedObjectUpdated" <<std::endl;
   QStatusBar* guistatusbar=dynamic_cast<QStatusBar*>(parentWidget());
   if(guistatusbar){
      guistatusbar->clearMessage(); // JAM2016: always wipe out old gui messages when updating ratemeter
   }

   if (!status && !rate) {
      // this is indication of lost connection to the server
      if (obj) LCDCurrentRate->setStyleSheet("QFrame { background-color:rgb(127,127,127) }");
      return;
   }

   double Rate = status ? status->GetRate() : rate->GetRate();
   if (Rate>10) LCDCurrentRate->display(floor(Rate));
           else LCDCurrentRate->display(Rate);

   double AvRate = status ? status->GetAvRate() : rate->GetAvRate();
   if(AvRate>10) LCDAverageRate->display(floor(AvRate));
            else LCDAverageRate->display(AvRate);

   int stime = status ? (int) status->GetTime() : (int) rate->GetTime();
   LCDTime->display(stime);
   ulong hh = stime/3600;
   ulong mm = (stime-hh*3600)/60;
   ulong ss = (stime-hh*3600-mm*60);
   QString tooltip = QString("Time since last reset [sec] = %1:%2:%3 [hours:min:sec]").arg(hh).arg(mm).arg(ss);
   LCDTime->setToolTip(tooltip);

   QString scnt = QString("%1").arg(status ? status->GetCurrentCount() : rate->GetCurrentCount());

   if (scnt.length() > LCDProcessedEvents->digitCount())
      LCDProcessedEvents->setDigitCount(scnt.length());

   LCDProcessedEvents->display(scnt);

   const char* color = "QFrame { background-color:rgb(255,0,0) }";

   Bool_t running = status ? status->IsAnalysisRunning() : rate->IsRunning();

   if(!running) {
      Go4Pix->setWindowIcon( QIcon(":/icons/go4logo2.png"));
      fxRunMovie->stop();
   } else {
      color = "QFrame { background-color:rgb(0,255,0) }";
      Go4Pix->clear();
      Go4Pix->setMovie(fxRunMovie);
      fxRunMovie->start();
   }

   LCDCurrentRate->setStyleSheet(color);

   SourceLabel->setText(status ? status->GetCurrentSource() : rate->GetCurrentSource());
   DateLabel->setText(status ? status->GetDateTime() : rate->GetDateTime());
}

void TGo4AnalysisStatusMonitor::linkedObjectRemoved(const char * linkname)
{
   // in any case deletion of this slot means closing of widget

   // if (Browser()) Browser()->ToggleMonitoring(0);

   ShootCloseWidget();
}
