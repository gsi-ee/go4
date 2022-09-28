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

#ifndef TGO4MBSVIEWER_H
#define TGO4MBSVIEWER_H

#include "QGo4Widget.h"
#include "ui_TGo4MBSViewer.h"

#include <QElapsedTimer>

extern "C" {
   #include "s_daqst.h"
   #include "s_setup.h"
   #include "s_set_ml.h"
   #include "s_set_mo.h"
}

class QButtonGroup;
class TH1;
class QMovie;

class TGo4MBSViewer : public QGo4Widget, public Ui::TGo4MBSViewer
 {
     Q_OBJECT

 public:
     TGo4MBSViewer(QWidget *parent = nullptr, const char *name = nullptr);
    virtual ~TGo4MBSViewer();

    TH1* TrendHisto(QString & refname, const QString & name, const QString & title, int value);

 public slots:

    virtual void StoreSettings();
    virtual void TimerStart();
    virtual void TimerStop();
    virtual void Display();
    virtual void Refresh();
    virtual void NodeEditEnter();
    virtual void NodeChanged(const QString &txt);
    virtual void ShowStatus();
    virtual void StateGroup_clicked(int id);
    virtual void PrintState();
    virtual void TrendSwitched(bool on);
    virtual void UpdateTrending();
    virtual void IncTrending(TH1 *histo, int value, bool forwards);
    virtual void FrequencyBox_valueChanged(int);
    virtual void MoreBox_toggled(bool on);
    virtual void TrendBinsBox_valueChanged(int i);
    virtual void RefreshButtonClick();
    virtual void SetNode(const QString &txt);
    virtual void ResetRunIcon();
    virtual void StartMovieReset();

 protected:

    bool fbSingleRefresh{false};
    int fiLastDataNum{0};
    int fiLastEventNum{0};
    int fiCalcedEventRate{0};
    s_set_mo fxSetupMO;
    bool fbRunning{false};
    bool fbTrendingForward{false};
    QString fxHistokBAccessName;
    TH1* fxRatekBTrend{nullptr};
    bool fbTrendingInit{false};
    QString fxHistoAccessName;
    bool fbTrending{false};
    bool fbGetSetML{false};
    QString fxRefTime;
    bool fbGetSetup{false};
    QString fxMessage;
    bool fbWarningState{false};
    bool fbIsMonitoring{false};
    s_daqst fxDaqStat;
    QString fxNode;
    QTimer* fxTimer{nullptr};
    s_setup fxSetup;
    s_set_ml fxSetupML;
    QMovie* fxRunMovie{nullptr};
    TH1* fxRateTrend{nullptr};
    int fiTrendBins{0};
    bool fbShowMore{false};
    QTimer* fxMovieResetTimer{nullptr};
    int fiEvRatio{0};
    QString fxHistoEvRatioAccessName;
    TH1* fxEvRatio{nullptr};
    QString fxServerLabel;
    bool fbGetSetMO{false};
    int fiCalcedDataRate{0};
    int fiLastServDataNum{0};
    int fiCalcedServDataRate{0};
    int fiDataDelta{0};
    int fiServDataDelta{0};
    QElapsedTimer fxDeltaClock;
    QButtonGroup* StateGroup{nullptr};
 };

#endif
