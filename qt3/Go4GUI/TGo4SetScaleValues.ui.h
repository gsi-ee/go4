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

void TGo4SetScaleValues::init()
{
   connect(TGo4WorkSpace::Instance(), SIGNAL(panelSignal(TGo4ViewPanel*, TPad*, int)),
          this, SLOT(panelSlot(TGo4ViewPanel*, TPad*, int)));

   fbDoingRefresh = false;

   refreshView(true);
}

void TGo4SetScaleValues::closeEvent( QCloseEvent* ce )
{
    ce->accept();
    delete this;
}

void TGo4SetScaleValues::refreshView(bool force)
{
   if (fbDoingRefresh) return;

   fbDoingRefresh = true;

   if (force) {
      XminV->clearModified();
      XmaxV->clearModified();
      YminV->clearModified();
      YmaxV->clearModified();
      ZminV->clearModified();
      ZmaxV->clearModified();
   }

   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();

   setEnabled(panel!=0);

   if (panel!=0) {

      setCaption(QString("Scale values: ") + panel->name());

      int ndim = 0;
      double Xmin=0, Xmax=0, Ymin=0, Ymax=0, Zmin=0, Zmax=0;
      bool autoscale;

      panel->GetSelectedRange(ndim, autoscale, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);

      AutoScaleButton->setChecked(autoscale);

      if (!XminV->isModified())
         XminV->setText(QString::number(Xmin));
      if (!XmaxV->isModified())
         XmaxV->setText(QString::number(Xmax));
      if (!YminV->isModified())
         YminV->setText(QString::number(Ymin));
      if (!YmaxV->isModified())
         YmaxV->setText(QString::number(Ymax));
      if (!ZminV->isModified())
         ZminV->setText(QString::number(Zmin));
      if (!ZmaxV->isModified())
         ZmaxV->setText(QString::number(Zmax));

      // enables/disables text edit for y
      switch(ndim) {
         case 1:
            YminV->setDisabled(autoscale);
            YmaxV->setDisabled(autoscale);
            ZminV->setDisabled(true);
            ZmaxV->setDisabled(true);
            break;
         case 2:
            YminV->setDisabled(false);
            YmaxV->setDisabled(false);
            ZminV->setDisabled(autoscale);
            ZmaxV->setDisabled(autoscale);
            break;
         default:
            YminV->setDisabled(false);
            YmaxV->setDisabled(false);
            ZminV->setDisabled(false);
            ZmaxV->setDisabled(false);
            break;
      }

   } else {
      setCaption(QString("Scale values: none"));

      XminV->setText("");
      XmaxV->setText("");
      YminV->setText("");
      YmaxV->setText("");
      ZminV->setText("");
      ZmaxV->setText("");
   }

   fbDoingRefresh = false;
}


void TGo4SetScaleValues::panelSlot(TGo4ViewPanel* panel, TPad* pad, int signalid)
{
   switch (signalid) {
      case QGo4Widget::panel_Activated:
      case QGo4Widget::panel_Deleted:
         refreshView(true);
         break;
      case QGo4Widget::panel_ActiveUpdated:
         refreshView(false);
         break;
   }
}

void TGo4SetScaleValues::ApplyValues()
{
   if (fbDoingRefresh) return;

   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel==0) return;

   double Xmin = XminV->text().toDouble();
   double Xmax = XmaxV->text().toDouble();
   double Ymin = YminV->text().toDouble();
   double Ymax = YmaxV->text().toDouble();
   double Zmin = ZminV->text().toDouble();
   double Zmax = ZmaxV->text().toDouble();

   fbDoingRefresh = true;
   panel->SetSelectedRange(Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
   fbDoingRefresh = false;

   refreshView(true);
}

void TGo4SetScaleValues::AutoscaleChanged(bool on)
{
   if (fbDoingRefresh) return;
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel==0) return;

   fbDoingRefresh = true;
   panel->SetAutoScale(on, 0);
   fbDoingRefresh = false;

   refreshView(true);
}
