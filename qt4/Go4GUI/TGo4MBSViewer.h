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

#ifndef TGO4MBSVIEWER_H
#define TGO4MBSVIEWER_H

#include "QGo4Widget.h"
#include "ui_TGo4MBSViewer.h"

#include <QMovie>

#include "sys_def.h"
#include "sbs_def.h"
#include "s_daqst.h"
#include "s_setup.h"
#include "ml_def.h"
#include "s_set_ml.h"
#include "s_set_mo.h"


extern "C"
{
    INTS4 f_mbs_status(CHARS *,s_daqst *);;
    INTS4 f_mbs_setup(CHARS *,s_setup *);;
    INTS4 f_mbs_ml_setup(CHARS *,s_set_ml *);;
    INTS4 f_mbs_mo_setup(CHARS *,s_set_mo *);;
    INTS4 f_ut_seg_show (s_daqst *ps_daqst, s_setup *ps_setup, s_set_ml *ps_set_ml, s_set_mo *ps_set_mo);;
}

class QButtonGroup;
class TH1;

class TGo4MBSViewer : public QGo4Widget, public Ui::TGo4MBSViewer
 {
     Q_OBJECT

 public:
     TGo4MBSViewer(QWidget *parent = 0 , const char* name=0);
    virtual ~TGo4MBSViewer();

    TH1* TrendHisto( QString & refname ,const QString & name, const QString & title, int value);



 public slots:


   virtual void StoreSettings();

   virtual void TimerStart();

   virtual void TimerStop();

   virtual void Display();

   virtual void Refresh();

   virtual void NodeEditEnter();

   virtual void NodeChanged( const QString & txt );

   virtual void ShowStatus();

   virtual void StateGroup_clicked( int id);

   virtual void PrintStatus();

   virtual void TrendSwitched( bool on );

   virtual void UpdateTrending();

   virtual void IncTrending( TH1 * histo, int value, bool forwards );

   virtual void FrequencyBox_valueChanged( int );

   virtual void MoreBox_toggled( bool on);

   virtual void TrendBinsBox_valueChanged( int i);

   virtual void RefreshButtonClick();

   virtual void SetNode( const QString & txt );

   virtual void ResetRunIcon();

   virtual void StartMovieReset();

protected:

   bool fbSingleRefresh;
    int fiLastDataNum;
    int fiLastEventNum;
    int fiCalcedEventRate;
    s_set_mo fxSetupMO;
    bool fbRunning;
    bool fbTrendingForward;
    QString fxHistokBAccessName;
    TH1* fxRatekBTrend;
    bool fbTrendingInit;
    QString fxHistoAccessName;
    bool fbTrending;
    bool fbGetSetML;
    QString fxRefTime;
    bool fbGetSetup;
    QString fxMessage;
    bool fbWarningState;
    bool fbIsMonitoring;
    s_daqst fxDaqStat;
    QString fxNode;
    QTimer* fxTimer;
    s_setup fxSetup;
    s_set_ml fxSetupML;
    QMovie* fxRunMovie;
    TH1* fxRateTrend;
    int fiTrendBins;
    bool fbShowMore;
    QTimer* fxMovieResetTimer;
    int fiEvRatio;
    QString fxHistoEvRatioAccessName;
    TH1* fxEvRatio;
    QString fxServerLabel;
    bool fbGetSetMO;
    int fiCalcedDataRate;
    int fiLastServDataNum;
    int fiCalcedServDataRate;
    int fiDataDelta;
    int fiServDataDelta;
    QButtonGroup* StateGroup;


 };

#endif
