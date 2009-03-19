#include "TGo4WorkSpace.h"

#include "TPad.h"
#include "TROOT.h"

#include "TGo4ViewPanel.h"
#include "QGo4Widget.h"
//Added by qt3to4:
#include <QtCore/QEvent>
#include <QCloseEvent>

#include <iostream>

TGo4WorkSpace* TGo4WorkSpace::fInstance = 0;

TGo4WorkSpace* TGo4WorkSpace::Instance()
{
   return fInstance;
}

TGo4WorkSpace::TGo4WorkSpace(QWidget* parent) :
   QWorkspace(parent),
   fxActivePanel(0),
   fxActivePad(0),
   fxSelectedPad(0),
   fbDisableEvents(false)
{
   setSizeIncrement( QSize( 100, 100 ) );
   setBaseSize( QSize( 100, 100 ) );

   connect(this,SIGNAL(windowActivated(QWidget*)), this, SLOT(windowActivatedSlot(QWidget*)));

   if (fInstance==0) fInstance = this;
}

void TGo4WorkSpace::closeEvent(QCloseEvent * ce)
{
    ce->accept();
    delete this;
}

TGo4WorkSpace::~TGo4WorkSpace()
{
   if (fInstance==this) fInstance=0;
}




//bool TGo4WorkSpace::eventFilter( QObject *fx , QEvent* e )
//{
////   switch (e->type()) {
////      case QEvent::Move:  // Move
////           return false;
////           break;
////      case QEvent::Resize :  //Resize
////           return false;
////           break;
////      case QEvent::Paint :  //Paint
////           return false;
////           break;
////   }
////
////   if(e->type()==QEvent::MouseButtonRelease) { // Resize on mouse release
////      if (fxActivePanel!=0)
////        fxActivePanel->CheckResizeFlags();
////   }
//   std::cout <<"TGo4WorkSpace::eventFilter gets "<<e->type(); //<< std::endl;
//   if(fbDisableEvents)
//      { 
//         std::cout <<" disabled." << std::endl;
//         return true;
//      }
//   else
//      {
//          std::cout <<" enabled." << std::endl;
//         return QWorkspace::eventFilter( fx , e );
//      }
//}



TPad* TGo4WorkSpace::GetSelectedPad()
{
    return fxSelectedPad;
}

void TGo4WorkSpace::SetSelectedPad(TPad* pad)
{
   fxSelectedPad = pad;
   gROOT->SetSelectedPad(pad);
   gPad = pad;
}

TGo4ViewPanel* TGo4WorkSpace::GetActivePanel()
{
   return fxActivePanel;
}

void TGo4WorkSpace::windowActivatedSlot(QWidget* w)
{
   TGo4ViewPanel* panel = dynamic_cast<TGo4ViewPanel*> (w);
   
   if ((panel!=0) && (fxActivePanel != panel))
     panel->SetActivePad(panel->GetActivePad());
}

void TGo4WorkSpace::ResponseOnPanelEvent(int funcid, TGo4ViewPanel* panel, TPad* pad)
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

