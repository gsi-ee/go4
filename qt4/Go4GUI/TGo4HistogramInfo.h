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

#ifndef TGO4HISTOGRAMINFO_H
#define TGO4HISTOGRAMINFO_H

#include "QGo4Widget.h"
#include "ui_TGo4HistogramInfo.h"

class TH1;
class TGo4HistogramStatus;


class TGo4HistogramInfo : public QGo4Widget, public Ui::TGo4HistogramInfo
 {
     Q_OBJECT

 public:

    enum PropertyIndex
      {
       HISTITLE,
       HISCLASS,
       HISENTRIES,
       HISX,
       HISXSTAT,
       HISY,
       HISYSTAT,
       HISZ,
       HISZSTAT,
       HISSIZE,
       HISTIME
      };


     TGo4HistogramInfo(QWidget *parent = 0, const char* name=0);

   virtual bool IsAcceptDrag(const char* itemname, TClass* cl, int kind);
    virtual void DropItem(const char* itemname, TClass* cl, int kind);
    virtual void linkedObjectUpdated(const char* linkname, TObject* obj);
    virtual void WorkWithHistogram(const char* itemname);
    virtual void ResetWidget();
    virtual void RefreshHistogramInfo(TH1* h1);
    virtual void RefreshHistogramInfo(TGo4HistogramStatus* hstate);


 public slots:

   virtual void HisInfoButton_clicked();
    virtual void HisPrintButton_clicked();
    virtual void HisDrawButton_clicked();
    virtual void PrintLog_clicked();




 };

#endif
