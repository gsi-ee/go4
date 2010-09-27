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

void TGo4AnalysisStatusMonitor::init()
{
   setAcceptDrops(FALSE);
   fxRunMovie = QMovie(TGo4Log::subGO4SYS("images/go4logorun4.gif").Data());
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
   //if(Rate<=0)
   if(!status->IsAnalysisRunning())
    {
        LCDCurrentRate->setPaletteBackgroundColor(QColor(255,0,0));
        Go4Pix->setPixmap(QPixmap::fromMimeSource( "go4logo2.png" ));
    }
    else
     {
        LCDCurrentRate->setPaletteBackgroundColor(QColor(0,255,0));
        Go4Pix->clear();
        Go4Pix->setMovie(fxRunMovie);
     }
   if(status->GetAvRate()>10)
      LCDAverageRate->display(floor(status->GetAvRate()));
   else
      LCDAverageRate->display(status->GetAvRate());

   LCDTime->display((int)status->GetTime());

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
