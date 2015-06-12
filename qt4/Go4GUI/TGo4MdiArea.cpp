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

#include "TGo4MdiArea.h"

#include "TPad.h"
#include "TROOT.h"

#include <QMdiSubWindow>

#include "TGo4ViewPanel.h"
#include "QGo4Widget.h"
#include "TGo4QSettings.h"

TGo4MdiArea* TGo4MdiArea::fInstance = 0;

TGo4MdiArea* TGo4MdiArea::Instance()
{
   return fInstance;
}

TGo4MdiArea::TGo4MdiArea(QWidget* parent) :
   QMdiArea(parent),
   fxActivePanel(0),
   fxActivePad(0),
   fxSelectedPad(0)
{
   setSizeIncrement( QSize( 100, 100 ) );
   setBaseSize( QSize( 100, 100 ) );

   connect(this,SIGNAL(subWindowActivated (QMdiSubWindow*)), this, SLOT(subWindowActivatedSlot(QMdiSubWindow*)));

   if (fInstance==0) fInstance = this;
}

TGo4MdiArea::~TGo4MdiArea()
{
   if (fInstance==this) fInstance = 0;
}


TPad* TGo4MdiArea::GetSelectedPad()
{
    return fxSelectedPad;
}

void TGo4MdiArea::SetSelectedPad(TPad* pad)
{
   fxSelectedPad = pad;
   gROOT->SetSelectedPad(pad);
   gPad = pad;
}

TGo4ViewPanel* TGo4MdiArea::GetActivePanel()
{
   return fxActivePanel;
}

QMdiSubWindow* TGo4MdiArea::AddGo4SubWindow(QWidget * widget, Qt::WindowFlags flags)
{
  QMdiSubWindow*sub=addSubWindow(widget, flags);
// JAM make window behaviour configurable in Go4 for all Qt versions.
  if(go4sett->getMoveSubwindowRubberBand())
    {
      sub->setOption(QMdiSubWindow::RubberBandResize);
      sub->setOption(QMdiSubWindow::RubberBandMove); // JAM required for qt5 performance
    }
   return sub;
}


void TGo4MdiArea::subWindowActivatedSlot(QMdiSubWindow* sub)
{
   TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (sub ? sub->widget() : 0);

   if ((panel!=0) && (fxActivePanel != panel))
     panel->SetActivePad(panel->GetActivePad());
}

void TGo4MdiArea::ResponseOnPanelEvent(int funcid, TGo4ViewPanel* panel, TPad* pad)
{
   if (panel==0) return;

   switch (funcid) {
      case QGo4Widget::panel_Activated: {
        if ((fxActivePanel!=panel) && (fxActivePanel!=0))
          fxActivePanel->SetActivePad(0);

        bool changed = (fxActivePanel != panel) || (fxActivePad != pad);
        fxActivePanel = panel;
        fxActivePad = pad;

        if (changed)
           emit panelSignal(panel, pad, QGo4Widget::panel_Activated);

        break;
      }

      case QGo4Widget::panel_Modified: {
         emit panelSignal(panel, pad, QGo4Widget::panel_Modified);
         break;
      }

      case QGo4Widget::panel_Updated: {
         emit panelSignal(panel, pad, QGo4Widget::panel_Updated);
         if ((panel==fxActivePanel) && (pad==fxActivePad))
           emit panelSignal(panel, pad, QGo4Widget::panel_ActiveUpdated);
         break;
      }

      case QGo4Widget::panel_PadDeleted: {
         if (GetSelectedPad()==pad) SetSelectedPad(0);
         if (fxActivePad==pad) fxActivePad=0;
         emit panelSignal(panel, pad, QGo4Widget::panel_PadDeleted);
         break;
      }

      case QGo4Widget::panel_Deleted: {
        if (GetSelectedPad()!=0)
          if (panel->IsPanelPad(GetSelectedPad()))
             SetSelectedPad(0);
        if (fxActivePanel==panel) {
           fxActivePanel = 0;
           fxActivePad = 0;
        }
        emit panelSignal(panel, pad, QGo4Widget::panel_Deleted);
        break;
      }
   }
}

