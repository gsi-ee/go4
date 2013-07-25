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
 ** If you want to add, delete, or rename functions or slots, use
 ** Qt Designer to update this file, preserving your code.
 **
 ** You should not define a constructor or destructor in this file.
 ** Instead, write your code in functions called init() and destroy().
 ** These will automatically be called by the form's constructor and
 ** destructor.
 *****************************************************************************/

extern "C"
{
   INTS4 f_mbs_status(CHARS *,s_daqst *);
   INTS4 f_mbs_setup(CHARS *,s_setup *);
   INTS4 f_mbs_ml_setup(CHARS *,s_set_ml *);
   INTS4 f_mbs_mo_setup(CHARS *,s_set_mo *);
   INTS4 f_ut_seg_show (s_daqst *ps_daqst, s_setup *ps_setup, s_set_ml *ps_set_ml, s_set_mo *ps_set_mo);
}


void TGo4MBSViewer::init()
{
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


   StateGroup->setButton(0);
   fxHistoAccessName="nosuchobject";
   fxHistokBAccessName="nosuchobject";
   fxHistoEvRatioAccessName="nosuchobject";
   fxServerLabel="NO SERVER";
   fxTimer=new QTimer(this);
   fxMovieResetTimer=new QTimer(this);
   fxRunMovie = QMovie(TGo4Log::subGO4SYS("icons/mbslogorun.gif").Data());
   memset(&fxDaqStat, 0, sizeof(fxDaqStat));
   fxDaqStat.bh_acqui_running=0; // we do not want to startup with running state
   fxDaqStat.l_open_file = 0; // just set initial value
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
   setCaption("MBS Status - "+NodeEdit->text());
   if(!fbSingleRefresh)
   {
      NodeEdit->setEnabled(!fbIsMonitoring);
      MonitorButton->setEnabled(!fbIsMonitoring);
      StopButton->setEnabled(fbIsMonitoring);
      FrequencyBox->setEnabled(!fbIsMonitoring);
      //TrendCheck->setEnabled(!fbIsMonitoring);
      TrendBinsBox->setEnabled(!fbTrending);
   }
   MoreFrame->setShown(fbShowMore);
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
   }
   else{
      RunPix->setPixmap(QPixmap::fromMimeSource( "mbsbutton.png" ));

   }
   WarnPix->setShown(fbWarningState);

   // put here enable/disable setup buttons:
   SetupRadio->setEnabled(fxDaqStat.bh_setup_loaded);
   SetupMLRadio->setEnabled(fxDaqStat.bh_set_ml_loaded);
   SetupMORadio->setEnabled(fxDaqStat.bh_set_mo_loaded);
   if(fbWarningState)
      std::cerr << fxMessage << std::endl;

   polish();
   update();
   show();
}


void TGo4MBSViewer::Refresh()
{
   // for the ratemeter and running state, we always get status block
   if(fxNode.isEmpty()) return;
   int state=f_mbs_status(const_cast<char*>(fxNode.data()), &fxDaqStat);
   if(state!=STC__SUCCESS)
   {
      fxMessage.sprintf("MBS Status refresh returned error %d at ",state);
      fxMessage+=QDateTime::currentDateTime().toString();
      fbWarningState=true;
   }
   else
      fbWarningState=false;
   //fxRefTime=QDateTime::currentDateTime ().toString();
   fxRefTime=fxDaqStat.c_date;
   fbRunning=fxDaqStat.bh_acqui_running;
   int deltat=FrequencyBox->value();
   int numperiods=1;
   int deltamilsecs=deltat*1000.0; // default
   if(fbIsMonitoring)
   {
      // only in monitoring mode: calculate rates ourselves, independent of mbs ratemter:
      // NEW: first check real time diff since last call and correct rates:
      deltamilsecs=fxDeltaClock.elapsed();
      //std::cout <<"******* found ms:"<<deltamilsecs << std::endl;
      fxDeltaClock.restart();
      int deltasecs=deltamilsecs/1000;
      if(!fbTrendingInit && (deltasecs>=deltat*2))
         //if((deltasecs>=deltat*2)) // this one was for testing JAM
      {
         std::cout <<"Warning: MBS monitor found measuring interval:"<<deltasecs<<" s ("<<deltamilsecs <<" ms) exceeding timer period "<<deltat<<" s" << std::endl;
         std::cout <<" Maybe timer was skipped?" << std::endl;
         deltat=deltasecs;
         numperiods=(deltat/ (int) FrequencyBox->value());
         std::cout <<" Correcting number of measuring periods to:"<<numperiods << std::endl;
      }
      if(fiLastEventNum && deltamilsecs)
         fiCalcedEventRate=1000.*(fxDaqStat.bl_n_events-fiLastEventNum)/deltamilsecs;
      else
         fiCalcedEventRate=0;
      fiLastEventNum=fxDaqStat.bl_n_events;

      if(fiLastDataNum && deltamilsecs)
      {
         fiDataDelta=(fxDaqStat.bl_n_kbyte-fiLastDataNum);
         fiCalcedDataRate=1000.*fiDataDelta/deltamilsecs;;
      }
      else
      {
         fiDataDelta=0;
         fiCalcedDataRate=0;
      }
      fiLastDataNum=fxDaqStat.bl_n_kbyte;
      //    std::cout <<"Data rate:"<<fiCalcedDataRate << std::endl;
      //    std::cout <<"Data total:"<<fiLastDataNum << std::endl;
   }
   else
   {
      // single refresh on button press: use momentary mbs rate
      fiCalcedEventRate = fxDaqStat.bl_r_events;
      fiCalcedDataRate = fxDaqStat.bl_r_kbyte;
   }

   if(fxDaqStat.bh_running[SYS__stream_serv])
   {
      fxServerLabel.sprintf("streamserver %d%s",
            (fxDaqStat.bl_strsrv_scale!=0 ? 100/fxDaqStat.bl_strsrv_scale : 0),"%");
      if(fbIsMonitoring)
      {
         // own rate calculation for monitoring on:
         if(fiLastServDataNum && deltamilsecs)
         {
            fiServDataDelta=(fxDaqStat.bl_n_strserv_kbytes-fiLastServDataNum);
            fiCalcedServDataRate=1000.*fiServDataDelta/deltamilsecs;
         }
         else
         {
            fiServDataDelta=0;
            fiCalcedServDataRate=0;
         }
         //            std::cout <<"Streamserver rate:"<<fiCalcedServDataRate << std::endl;
         //            std::cout <<"Streamserver data:"<<fxDaqStat.bl_n_strserv_kbytes << std::endl;
         //            std::cout <<"Streamserver last data:"<<fiLastServDataNum << std::endl;
         //            std::cout <<"dt:"<<deltat << std::endl;
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
      //std::cout<<"Eventratio="<<fiEvRatio<<" , currentratio="<<curentratio<< std::endl;
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
         if(fiLastServDataNum && deltamilsecs)
         {
            fiServDataDelta=(fxDaqStat.bl_n_evserv_kbytes-fiLastServDataNum);
            fiCalcedServDataRate=1000.0*fiServDataDelta/deltamilsecs;
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
   {
      while((numperiods--) > 0)
      {
         UpdateTrending(); // use same values for all skipped periods
         //std::cout <<"Update trending with numperiods:"<<numperiods << std::endl;
      }
   }
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
   fxNode.stripWhiteSpace();
}




void TGo4MBSViewer::ShowStatus()
{
   if(fbWarningState)
   {
      std::cout << fxMessage << std::endl;
   }
   else
   {
      std::cout <<"\n------------------------------------------------" << std::endl;
      if(fbGetSetup)
         f_ut_seg_show (0,&fxSetup,0,0);
      else if(fbGetSetML)
         f_ut_seg_show (0,0,&fxSetupML,0);
      else if(fbGetSetMO)
         f_ut_seg_show (0,0,0,&fxSetupMO);
      else
         f_ut_seg_show (&fxDaqStat,0,0,0);
   }

}

void TGo4MBSViewer::StateGroup_clicked( int id)
{
   switch(id){
      case 0:
      default:
         fbGetSetup=false;
         fbGetSetML=false;
         fbGetSetMO=false;
         break;
      case 1:
         fbGetSetup=true;
         fbGetSetML=false;
         fbGetSetMO=false;
         break;
      case 2:
         fbGetSetup=false;
         fbGetSetML=true;
         fbGetSetMO=false;
         break;
      case 3:
         fbGetSetup=false;
         fbGetSetML=false;
         fbGetSetMO=true;
         break;
   };
}


void TGo4MBSViewer::PrintStatus()
{
   if(fxNode.isEmpty()) return;
   if(fbGetSetup)
   {
      // request setup if selected

      int state=f_mbs_setup(const_cast<char*>(fxNode.data()), &fxSetup);
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
      int state=f_mbs_ml_setup(const_cast<char*>(fxNode.data()), &fxSetupML);
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
      int state = f_mbs_mo_setup(const_cast<char*>(fxNode.data()), &fxSetupMO);
      if(state!=0)
      {
         fxMessage.sprintf("MBS Setup MO refresh returned error %d at",state);
         fxMessage+=QDateTime::currentDateTime().toString();
         fbWarningState=true;
      }
      else
         fbWarningState=false;
   }
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
   if(!fbTrendingInit) histoslot = Browser()->BrowserSlot(refname);
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
      his=new TH1F(name,title,fiTrendBins,lo,up);
      TAxis* xax=his->GetXaxis();
      xax->SetTitle("s");
      xax->CenterTitle();
      //xax->SetLimits(0,lo,up);

      TGo4Slot* hisdataslot=Browser()->DataSlot(refname);
      if(hisdataslot)
      {
         hisdataslot->AssignObject(his,true);
      }
      else
      {
         refname=Browser()->SaveToMemory("Mbs", his, true);
      }
      histoslot=Browser()->BrowserSlot(refname);
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
   if(!fbIsMonitoring)
      fxMovieResetTimer->start(2000,true);

}
