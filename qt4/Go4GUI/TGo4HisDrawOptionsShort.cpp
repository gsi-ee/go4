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

#include "TGo4HisDrawOptionsShort.h"

#include "TROOT.h"
#include "TColor.h"
#include "TAttLine.h"
#include "TAttFill.h"
#include "TAttMarker.h"

#include "TGo4MdiArea.h"
#include "TGo4ViewPanel.h"

#include <QColorDialog>


TGo4HisDrawOptionsShort::TGo4HisDrawOptionsShort(QWidget *parent, const char* name)
   : QGo4Widget(parent,name)
{
   setupUi(this);
   QObject::connect(Xlin, SIGNAL(clicked()), this, SLOT(SlotXlin()));
   QObject::connect(Xlog, SIGNAL(clicked()), this, SLOT(SlotXlog()));
   QObject::connect(Ylin, SIGNAL(clicked()), this, SLOT(SlotYlin()));
   QObject::connect(Ylog, SIGNAL(clicked()), this, SLOT(SlotYlog()));
   QObject::connect(Zlin, SIGNAL(clicked()), this, SLOT(SlotZlin()));
   QObject::connect(Zlog, SIGNAL(clicked()), this, SLOT(SlotZlog()));
   QObject::connect(D1his, SIGNAL(clicked()), this, SLOT(SlotD1his()));
   QObject::connect(D1line, SIGNAL(clicked()), this, SLOT(SlotD1line()));
   QObject::connect(D2scatter, SIGNAL(clicked()), this, SLOT(SlotD2scatter()));
   QObject::connect(D2pixel, SIGNAL(clicked()), this, SLOT(SlotD2pixel()));
   QObject::connect(D2surfcol, SIGNAL(clicked()), this, SLOT(SlotD2surfcol()));
   QObject::connect(D2lego, SIGNAL(clicked()), this, SLOT(SlotD2lego()));
   QObject::connect(D2contfill, SIGNAL(clicked()), this, SLOT(SlotD2contfill()));
   QObject::connect(D2contline, SIGNAL(clicked()), this, SLOT(SlotD2contline()));
   QObject::connect(LineColor, SIGNAL(clicked()), this, SLOT(SlotLineColor()));
   QObject::connect(FillColor, SIGNAL(clicked()), this, SLOT(SlotFillColor()));
   QObject::connect(MarkerColor, SIGNAL(clicked()), this, SLOT(SlotMarkerColor()));
}

void TGo4HisDrawOptionsShort::SlotXlin()
{
   SetDrawOpt(3, 0, "");
}

void TGo4HisDrawOptionsShort::SlotXlog()
{
    SetDrawOpt(3, 1, "");
}

void TGo4HisDrawOptionsShort::SlotYlin()
{
    SetDrawOpt(4, 0, "");
}

void TGo4HisDrawOptionsShort::SlotYlog()
{
    SetDrawOpt(4, 1, "");
}

void TGo4HisDrawOptionsShort::SlotZlin()
{
    SetDrawOpt(5, 0, "");
}

void TGo4HisDrawOptionsShort::SlotZlog()
{
    SetDrawOpt(5, 1, "");
}

void TGo4HisDrawOptionsShort::SlotD1his()
{
   SetDrawOpt(0, 0, "");
}

void TGo4HisDrawOptionsShort::SlotD1line()
{
   SetDrawOpt(0, 0, "L");
}

void TGo4HisDrawOptionsShort::SlotD2scatter()
{
    SetDrawOpt(0, 0, "HIST");
}

void TGo4HisDrawOptionsShort::SlotD2pixel()
{
    SetDrawOpt(0, 0, "COLZ");
}

void TGo4HisDrawOptionsShort::SlotD2surfcol()
{
   SetDrawOpt(0, 0, "SURF2");
}

void TGo4HisDrawOptionsShort::SlotD2lego()
{
    SetDrawOpt(0, 0, "LEGO2");
}

void TGo4HisDrawOptionsShort::SlotD2contfill()
{
    SetDrawOpt(0, 0, "CONT0Z");
}

void TGo4HisDrawOptionsShort::SlotD2contline()
{
    SetDrawOpt(0, 0, "CONT1Z");
}

void TGo4HisDrawOptionsShort::SlotLineColor()
{
   ChangeColor(0);
}

void TGo4HisDrawOptionsShort::SlotFillColor()
{
   ChangeColor(1);
}

void TGo4HisDrawOptionsShort::SlotMarkerColor()
{
   ChangeColor(2);
}

void TGo4HisDrawOptionsShort::ChangeColor(int kind)
{
   TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
   if (!panel) return;

   TPad* pad = panel->GetActivePad();
   TObject* obj = panel->GetSelectedObject(pad, 0);

   Color_t icol = 0;

   switch (kind) {
      case 0: {
        TAttLine* att = dynamic_cast<TAttLine*> (obj);
        if (!att) return;
        icol = att->GetLineColor();
        break;
      }
      case 1: {
        TAttFill* att = dynamic_cast<TAttFill*> (obj);
        if (!att) return;
        icol = att->GetFillColor();
        break;
      }
      case 2: {
        TAttMarker* att = dynamic_cast<TAttMarker*> (obj);
        if (!att) return;
        icol = att->GetMarkerColor();
        break;
      }
   }

   TColor* rcol = gROOT->GetColor(icol);

   QColor qcol;

   if (rcol)
      qcol.setRgb(rcol->GetRed(), rcol->GetGreen(), rcol->GetBlue());

   QColor c = QColorDialog::getColor(qcol);
   if (!c.isValid()) return;

   icol = TColor::GetColor(c.red(), c.green(), c.blue());

   switch (kind) {
      case 0: {
        TAttLine* att = dynamic_cast<TAttLine*> (obj);
        if (att) att->SetLineColor(icol);
        break;
      }
      case 1: {
        TAttFill* att = dynamic_cast<TAttFill*> (obj);
        if (att) att->SetFillColor(icol);
        break;
      }
      case 2: {
        TAttMarker* att = dynamic_cast<TAttMarker*> (obj);
        if (att) att->SetMarkerColor(icol);
        break;
      }
   }

   panel->MarkPadModified(pad);

   panel->ShootRepaintTimer(pad);

   panel->ActivateInGedEditor(obj);
}

void TGo4HisDrawOptionsShort::SetDrawOpt(int kind, int value, const char *dropt)
{
   TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
   if (panel) panel->ChangeDrawOption(kind, value,dropt);
}
