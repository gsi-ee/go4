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

#ifndef TGO4CREATENEWHISTOGRAM_H
#define TGO4CREATENEWHISTOGRAM_H


#include <QDialog>
#include "ui_TGo4CreateNewHistogram.h"

class QButtonGroup;
class TH1;

class TGo4CreateNewHistogram : public QDialog, public Ui::TGo4CreateNewHistogram
 {
     Q_OBJECT

 public:
     TGo4CreateNewHistogram( QWidget* parent = 0);
    void SetAnalysisAvaliable(bool on);

   void SetLocalAvaliable(bool on);

   int GetSelectedCmd();

   TH1* MakeHistogram();

 public slots:

   virtual void CreateLocalHist();

   virtual void CreateRemoteHis();

 protected:
    int fSelectedCmd;

    QButtonGroup  *HisTypeGrp;
    QButtonGroup  *HisClassGrp;

 };
#endif
