#include <math.h>


void TGo4AnalysisStatusMonitor::init()
{
   setAcceptDrops(FALSE);
   QString moviepath=gSystem->Getenv("GO4SYS");
   moviepath+="/images/go4logorun4.gif";
   fxRunMovie=QMovie(moviepath);
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

   LCDProcessedEvents->display((int)status->GetCurrentCount());

   DateLabel->setText(status->GetDateTime());
}

void TGo4AnalysisStatusMonitor::linkedObjectRemoved(const char * linkname)
{
   // in any case deletion of this slot means closing of widget

   if (Browser())
      Browser()->ToggleMonitoring(0);

   ShootCloseWidget();
}
