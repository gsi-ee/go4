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
