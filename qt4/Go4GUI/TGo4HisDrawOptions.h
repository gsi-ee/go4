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

#ifndef TGO4HISDRAWOPTIONS_H
#define TGO4HISDRAWOPTIONS_H

#include <QWidget>
#include "ui_TGo4HisDrawOptions.h"
#include "TString.h"

class TGo4ViewPanel;
class TPad;

class TGo4HisDrawOptions : public QWidget, public Ui::TGo4HisDrawOptions
 {
     Q_OBJECT

 public:

    enum { view_Histo1 = 1,
          view_Histo2 = 2,
          view_Histo2ext = 3,
          view_Graph = 4,
          view_Polar = 5 };

    TGo4HisDrawOptions( QWidget *parent = nullptr, const char *name = nullptr, Qt::WindowFlags fl = Qt::Widget );

    virtual void ChangeDrawOptionForCurrentPanel(int kind, int value);
    virtual void DecodeDrawOption(const char *drawopt,
                                  int &HisErrorStyle,
                                  int &HisCoordStyle,
                                  int &HisDrawStyle);
    virtual void CodeDrawOptions(int HisErrorStyle, int HisCoordStyle, int HisDrawStyle, TString &buf);
    virtual void UpdateView(int viewtype, bool webcanvas = false);
    virtual void ChangeColor(int kind);

 public slots:
    virtual void panelSlot( TGo4ViewPanel * panel,TPad *pad, int signalid );
    virtual void SetDrawOptions(int t);
    virtual void SetErrorBars(int t);
    virtual void SetCoordinates(int t);
    virtual void XaxisStyle(int t);
    virtual void YaxisStyle(int t);
    virtual void ZaxisStyle(int t);
    virtual void SetAutoScale(bool on);
    virtual void SetLineColor();
    virtual void SetFillColor();
    virtual void SetMarkerColor();

protected:
    bool fbSettingPanelData{false};
    int fiLastView{0};

};

#endif
