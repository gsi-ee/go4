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

#include "TGo4MBSViewer.h"
#include <QTimer>
#include <QDateTime>
#include "Riostream.h"
#include "TSystem.h"
#include "TH1.h"
#include "TGo4Slot.h"
#include "TGo4BrowserProxy.h"
#include "TGo4QSettings.h"
#include "f_stccomm.h"

TGo4MBSViewer::TGo4MBSViewer(QWidget *parent, const char* name) :
   QGo4Widget(parent,name),
   fxRunMovie(0)
{
   setupUi(this);
   fbIsMonitoring=false;
   fbWarningState=false;
   fbRunning=false;
   fbGetSetup=false;
   fbGetSetML=false;
   fbGetSetMO=false;
   fbTrending=false;
   fbTrendingInit=true;
   fbShowMore=false;
   fbSingleRefresh=false;
   fbTrendingForward=false; // define moving direction of trend histograms
   fiTrendBins=1000;
   // now overwrite some settings:
   SetNode(go4sett->getMbsMonitorNode());
   fbTrending=go4sett->getMbsMonitorTrend();
   TrendCheck->setChecked(fbTrending);
   fiTrendBins=go4sett->getMbsMonitorBins();
   TrendBinsBox->setValue(fiTrendBins);
   fbShowMore=go4sett->getMbsMonitorMore();
   MoreBox->setChecked(fbShowMore);
   FrequencyBox->setValue(go4sett->getMbsMonitorFreq());
   fbTrendingForward=!(go4sett->getMbsMonitorBackwardsTrending());

   StateGroup = new QButtonGroup(this);
   StateGroup->setExclusive(false);
   StateGroup->addButton(StatusRadio, 0);
   StateGroup->addButton(SetupRadio, 1);
   StateGroup->addButton(SetupMLRadio, 2);
   StateGroup->addButton(SetupMORadio, 3);
   StateGroup->button(0)->setChecked(true);
   connect(StateGroup, SIGNAL(buttonClicked(int)), this, SLOT(StateGroup_clicked(int)));


   fxHistoAccessName="nosuchobject";
   fxHistokBAccessName="nosuchobject";
   fxHistoEvRatioAccessName="nosuchobject";
   fxServerLabel="NO SERVER";
   fxTimer=new QTimer(this);
   fxMovieResetTimer=new QTimer(this);
   QString moviepath=":/icons/mbslogorun.gif";
   fxRunMovie= new QMovie(moviepath);
   memset(&fxDaqStat, 0, sizeof(fxDaqStat));
   fiCalcedEventRate=0;
   fiCalcedDataRate=0;
   fiCalcedServDataRate=0;
   fiLastServDataNum=0;
   fiLastEventNum=0;
   fiLastDataNum=0;
   connect( fxTimer, SIGNAL(timeout()), this, SLOT(Refresh()) );
   connect( fxMovieResetTimer, SIGNAL(timeout()), this, SLOT(ResetRunIcon()) );
   Display();

}

TGo4MBSViewer::~TGo4MBSViewer()
{
   delete fxRunMovie;
}


void TGo4MBSViewer::StoreSettings()
{
// save  settings:
go4sett->setMbsMonitorNode(fxNode);
go4sett->setMbsMonitorTrend(fbTrending);
go4sett->setMbsMonitorBins(fiTrendBins);
go4sett->setMbsMonitorMore(fbShowMore);
go4sett->setMbsMonitorFreq(FrequencyBox->value());
go4sett->setMbsMonitorMonitorActive(fbIsMonitoring);
go4sett->setMbsMonitorBackwardsTrending(!fbTrendingForward);
}


void TGo4MBSViewer::TimerStart()
{
fbIsMonitoring=true;
fbTrendingInit=true;
fiLastEventNum=0;
fiLastDataNum=0;
fiLastServDataNum=0;
Refresh();
fxTimer->start(1000*FrequencyBox->value());
Display();
StoreSettings();
}


void TGo4MBSViewer::TimerStop()
{
fbIsMonitoring=false;
fxTimer->stop();
StartMovieReset();
Display();
StoreSettings();
}


void TGo4MBSViewer::Display()
{
setWindowTitle("MBS Status - "+NodeEdit->text());
if(!fbSingleRefresh)
    {
    NodeEdit->setEnabled(!fbIsMonitoring);
    MonitorButton->setEnabled(!fbIsMonitoring);
    StopButton->setEnabled(fbIsMonitoring);
    FrequencyBox->setEnabled(!fbIsMonitoring);
    //TrendCheck->setEnabled(!fbIsMonitoring);
    TrendBinsBox->setEnabled(!fbTrending);
    }
MoreFrame->setVisible(fbShowMore);
SumEvents->display((double) fxDaqStat.bl_n_events);
//RateEvents->display((double) fxDaqStat.bl_r_events);
RateEvents->display((double) fiCalcedEventRate);
SumBytes->display((double) ((unsigned int) fxDaqStat.bl_n_kbyte/1024));
//RateBytes->display((double) fxDaqStat.bl_r_kbyte);
RateBytes->display((double) fiCalcedDataRate);
DateLabel->setText(fxRefTime);
SumBytesFile->display((double) ((unsigned int) fxDaqStat.bl_n_kbyte_file/1024));

if(fxDaqStat.l_open_file)
 OutfileLabel->setText(fxDaqStat.c_file_name);
else
 OutfileLabel->setText("- file closed -") ;

ServerLabel->setText(fxServerLabel) ;
ServerPercent->display((int) fiEvRatio);
if(fbRunning && !fbWarningState){
    RunPix->clear();
    RunPix->setMovie(fxRunMovie);
    fxRunMovie->start();
}
else{
    RunPix->setWindowIcon(QIcon( ":/icons/mbsbutton.png" ));
    if(fxRunMovie) fxRunMovie->stop();
}
WarnPix->setVisible(fbWarningState);

// put here enable/disable setup buttons:
SetupRadio->setEnabled(fxDaqStat.bh_setup_loaded);
SetupMLRadio->setEnabled(fxDaqStat.bh_set_ml_loaded);
SetupMORadio->setEnabled(fxDaqStat.bh_set_mo_loaded);
if(fbWarningState)
    cerr <<fxMessage.toStdString().c_str() << endl;

ensurePolished();
update();
show();
}


void TGo4MBSViewer::Refresh()
{
// for the ratemeter and running state, we always get status block
if(fxNode.isEmpty()) {
	return;
}
int state=f_mbs_status(const_cast<char*>(fxNode.toStdString().c_str()), &fxDaqStat);
if(state!=STC__SUCCESS)
{
    fxMessage.sprintf("MBS Status refresh node %s returned error %d at ",
    		const_cast<char*>(fxNode.toStdString().c_str()),state);
    fxMessage+=QDateTime::currentDateTime().toString();
    fbWarningState=true;
}
else
    fbWarningState=false;
//fxRefTime=QDateTime::currentDateTime ().toString();
fxRefTime=fxDaqStat.c_date;
fbRunning=fxDaqStat.bh_acqui_running;
int deltat=FrequencyBox->value();
if(fbIsMonitoring)
{
    // only in monitoring mode: calculate rates ourselves, independent of mbs ratemter:
    if(fiLastEventNum && deltat)
          fiCalcedEventRate=(fxDaqStat.bl_n_events-fiLastEventNum)/deltat;
    else
        fiCalcedEventRate=0;
    fiLastEventNum=fxDaqStat.bl_n_events;

    if(fiLastDataNum && deltat)
          {
            fiDataDelta=(fxDaqStat.bl_n_kbyte-fiLastDataNum);
            fiCalcedDataRate=fiDataDelta/deltat;
          }
    else
        {
            fiDataDelta=0;
            fiCalcedDataRate=0;
        }
    fiLastDataNum=fxDaqStat.bl_n_kbyte;
//    cout <<"Data rate:"<<fiCalcedDataRate << endl;
//    cout <<"Data total:"<<fiLastDataNum << endl;
}
else
{
    // single refresh on button press: use momentary mbs rate
    fiCalcedEventRate=fxDaqStat.bl_r_events;
    fiCalcedDataRate=fxDaqStat.bl_r_kbyte;
}

if(fxDaqStat.bh_running[SYS__stream_serv])
    {
    fxServerLabel.sprintf("streamserver %d%s",
        (fxDaqStat.bl_strsrv_scale!=0 ? 100/fxDaqStat.bl_strsrv_scale : 0),"%");
    if(fbIsMonitoring)
        {
            // own rate calculation for monitoring on:
            if(fiLastServDataNum && deltat)
              {
                fiServDataDelta=(fxDaqStat.bl_n_strserv_kbytes-fiLastServDataNum);
                fiCalcedServDataRate=fiServDataDelta/deltat;
              }
            else
              {
                fiServDataDelta=0;
                fiCalcedServDataRate=0;
              }
//            cout <<"Streamserver rate:"<<fiCalcedServDataRate << endl;
//            cout <<"Streamserver data:"<<fxDaqStat.bl_n_strserv_kbytes << endl;
//            cout <<"Streamserver last data:"<<fiLastServDataNum << endl;
//            cout <<"dt:"<<deltat << endl;
            fiLastServDataNum=fxDaqStat.bl_n_strserv_kbytes;
        }
    else
        {
            // single refresh: use ratio from current mbs calculated rates
            fiCalcedServDataRate=fxDaqStat.bl_r_strserv_kbytes;
            fiDataDelta=0;
        }
    //fiEvRatio= (fiCalcedDataRate!=0 ? 100* fiCalcedServDataRate /fiCalcedDataRate : 0);
    //int curentratio=(fiCalcedDataRate!=0 ? 100* fiCalcedServDataRate /fiCalcedDataRate : 0);
    fiEvRatio= (fiDataDelta!=0 ? 100* fiServDataDelta /fiDataDelta : 0);
    //cout<<"Eventratio="<<fiEvRatio<<" , currentratio="<<curentratio<<endl;
    }
else if(fxDaqStat.bh_running[SYS__event_serv])
    {
    // calculate momentary rate for eventserver:

    fxServerLabel.sprintf("eventserver %s %d%s",
        (fxDaqStat.bh_event_serv_ready ? "R" : "S"),
        (fxDaqStat.bl_evtsrv_scale!=0 ? 100/fxDaqStat.bl_evtsrv_scale : 0),"%");
    if(fbIsMonitoring)
        {
            // own rate calculation for monitoring on:
            if(fiLastServDataNum && deltat)
              {
                fiServDataDelta=(fxDaqStat.bl_n_evserv_kbytes-fiLastServDataNum);
                fiCalcedServDataRate=fiServDataDelta/deltat;
              }
            else
              {
                fiServDataDelta=0;
                fiCalcedServDataRate=0;
              }
            fiLastServDataNum=fxDaqStat.bl_n_evserv_kbytes;
        }
    else
        {
            // single refresh: use ratio from current mbs calculated rates
            fiCalcedServDataRate=fxDaqStat.bl_r_evserv_kbytes;
            fiDataDelta=0;
        }
    //fiEvRatio= (fiCalcedDataRate!=0 ? 100* fiCalcedServDataRate /fiCalcedDataRate : 0);
    fiEvRatio= (fiDataDelta!=0 ? 100* fiServDataDelta /fiDataDelta : 0);

  }
else
    {
        fxServerLabel="NO SERVER";
        fiEvRatio=0;
    }

if(fbTrending && !fbWarningState && fbIsMonitoring)
    UpdateTrending();
StartMovieReset();
Display();
//f_ut_seg_show (&fxDaqStat,0,0,0);
}


void TGo4MBSViewer::NodeEditEnter()
{
    Refresh();
}



void TGo4MBSViewer::NodeChanged( const QString & txt )
{
fxNode=txt;
fxNode.trimmed();
}




void TGo4MBSViewer::ShowStatus()
{
  if(fbWarningState)
    {
          cout <<fxMessage.toStdString().c_str() << endl;
    }
  else
  {
	  cout <<"\n------------------------------------------------" << endl;
	  if(StateGroup->button(0)->isChecked())
		  f_ut_seg_show (&fxDaqStat,0,0,0);
	  if(fbGetSetup)
		  f_ut_seg_show (0,&fxSetup,0,0);
	  else if(fbGetSetML)
		  f_ut_seg_show (0,0,&fxSetupML,0);
	  else if(fbGetSetMO)
		  f_ut_seg_show (0,0,0,&fxSetupMO);
  }

}

void TGo4MBSViewer::StateGroup_clicked( int id)
{
	// only one of these can be enabled
	fbGetSetup=StateGroup->button(1)->isChecked();
	fbGetSetML=StateGroup->button(2)->isChecked();
	fbGetSetMO=StateGroup->button(3)->isChecked();
	// if status is wanted, do not print setups:
//	if(fbGetSetup=StateGroup->button(0)->isChecked()){
//        fbGetSetup=false;
//        fbGetSetML=false;
//        fbGetSetMO=false;
//	}
}


void TGo4MBSViewer::PrintStatus()
{
if(fxNode.isEmpty()) return;
if(fbGetSetup)
{
    // request setup if selected

    int state=f_mbs_setup(const_cast<char*>(fxNode.toStdString().c_str()), &fxSetup);
    if(state!=0)
        {
            fxMessage.sprintf("MBS Setup refresh returned error %d at ",state);
            fxMessage+=QDateTime::currentDateTime().toString();
            fbWarningState=true;
        }
    else
            fbWarningState=false;
}

else if(fbGetSetML)
{
    // request setup multilayer if selected
    int state=f_mbs_ml_setup(const_cast<char*>(fxNode.toStdString().c_str()), &fxSetupML);
    if(state!=0)
    {
        fxMessage.sprintf("MBS Setup ML refresh returned error %d at",state);
        fxMessage+=QDateTime::currentDateTime().toString();
        fbWarningState=true;
    }
    else
        fbWarningState=false;
}
else if(fbGetSetMO)
{
    // request setup MO if selected
    int state=f_mbs_mo_setup(const_cast<char*>(fxNode.toStdString().c_str()), &fxSetupMO);
    if(state!=0)
    {
        fxMessage.sprintf("MBS Setup MO refresh returned error %d at",state);
        fxMessage+=QDateTime::currentDateTime().toString();
        fbWarningState=true;
    }
    else
        fbWarningState=false;
}

else {}
RefreshButtonClick(); // always get new status structure and display on window
ShowStatus();
}


void TGo4MBSViewer::TrendSwitched( bool on )
{
if(!fbTrending) fbTrendingInit=true;
fbTrending=on;
go4sett->setMbsMonitorTrend(fbTrending);
Display();
}


void TGo4MBSViewer::UpdateTrending()
{
TrendHisto(fxHistoAccessName,"MbsEventRate","Events/s",fiCalcedEventRate);
TrendHisto(fxHistokBAccessName,"MbsDataRate","kB/s",fiCalcedDataRate);
TrendHisto(fxHistoEvRatioAccessName,fxServerLabel.section(' ',0,0),"% Events served",fiEvRatio);
fbTrendingInit=false;
}


void TGo4MBSViewer::IncTrending( TH1 * histo, int value, bool forwards )
{
if(histo==0) return;
int bins=histo->GetNbinsX();
//bool forwards=true;
int j,dj;
if(forwards)
       dj=-1;
else
       dj=+1;
for(int i=0;i<bins;i++)
    {
    if(forwards)
        j=bins-i;
    else
        j=i;
    int oldval=histo->GetBinContent(j+dj);
    histo->SetBinContent(j,oldval);
    }
histo->SetBinContent(j+dj,value);
}


void TGo4MBSViewer::FrequencyBox_valueChanged( int )
{
    fbTrendingInit=true;
}


TH1* TGo4MBSViewer::TrendHisto( QString & refname ,const QString & name, const QString & title, int value)
{
TH1* his=0;
TGo4Slot* histoslot=0;
if(!fbTrendingInit) histoslot=Browser()->BrowserSlot(refname.toStdString().c_str());
if(histoslot==0)
    {
        Axis_t lo,up;
        if(fbTrendingForward)
            {
                lo=0;
                up=1*fiTrendBins*FrequencyBox->value();
            }
        else
            {
                lo=-1*fiTrendBins*FrequencyBox->value();
                up=0;
            }
        his=new TH1F(name.toStdString().c_str(), title.toStdString().c_str() ,fiTrendBins,lo,up);
        TAxis* xax=his->GetXaxis();
        xax->SetTitle("s");
        xax->CenterTitle();
        //xax->SetLimits(0,lo,up);

        TGo4Slot* hisdataslot=Browser()->DataSlot(refname.toStdString().c_str());
        if(hisdataslot)
            {
                hisdataslot->AssignObject(his,true);
            }
        else
            {
                refname=Browser()->SaveToMemory("Mbs", his, true);
            }
        histoslot=Browser()->BrowserSlot(refname.toStdString().c_str());
    }
else
    {
       his=dynamic_cast<TH1*>(histoslot->GetAssignedObject());
    }
IncTrending(his,value,fbTrendingForward);
if(histoslot)
    {
        histoslot->ForwardEvent(histoslot, TGo4Slot::evObjUpdated);
        Browser()->SetItemTimeDate(histoslot);
    }
return his;
}


void TGo4MBSViewer::MoreBox_toggled( bool on)
{
fbShowMore=on;
go4sett->setMbsMonitorMore(fbShowMore);
Display();
}


void TGo4MBSViewer::TrendBinsBox_valueChanged( int i)
{
fiTrendBins=i;
fbTrendingInit=true;
}


void TGo4MBSViewer::RefreshButtonClick()
{
    bool trend=fbTrending;
    bool monitoring=fbIsMonitoring;
    fbTrending=false; // avoid to put direct refresh into the trending histogram
    fbIsMonitoring=false; // avoid confuse rate calculations
    fbSingleRefresh=true; // avoid changing state of control buttons
    Refresh();
    fbSingleRefresh=false;
    fbIsMonitoring=monitoring;
    fbTrending=trend;

}


void TGo4MBSViewer::SetNode( const QString & txt )
{
fxNode=txt;
NodeEdit->setText(txt);
}


void TGo4MBSViewer::ResetRunIcon()
{
    fbRunning=false;
    Display();
}


void TGo4MBSViewer::StartMovieReset()
{
   if(!fbIsMonitoring) {
      fxMovieResetTimer->setSingleShot(true);
      fxMovieResetTimer->start(2000);
   }
}
