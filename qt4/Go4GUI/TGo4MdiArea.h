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

#ifndef TGO4MDIAREA_H
#define TGO4MDIAREA_H

#include <QMdiArea>

#include <QtCore/QEvent>

class TPad;
class TGo4ViewPanel;


/** @author S. Linev
  * @since 14.01.2014 */

class TGo4MdiArea : public QMdiArea {
   Q_OBJECT

   public:

      TGo4MdiArea(QWidget *parent = 0);
      virtual ~TGo4MdiArea();

      TGo4ViewPanel* GetActivePanel();

      TPad* GetSelectedPad();
      void SetSelectedPad(TPad* pad);

      void ResponseOnPanelEvent(int funcid, TGo4ViewPanel* panel, TPad* pad);

      static TGo4MdiArea* Instance();

   signals:
      void panelSignal(TGo4ViewPanel*, TPad*, int);

   protected slots:
      void subWindowActivatedSlot(QMdiSubWindow * window);

   private:
      TGo4ViewPanel*  fxActivePanel;
      TPad*           fxActivePad;
      TPad*           fxSelectedPad;

      static TGo4MdiArea* fInstance;
};

#endif
