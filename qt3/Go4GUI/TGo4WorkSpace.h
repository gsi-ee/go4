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

#ifndef TGO4WORKSPACE_H
#define TGO4WORKSPACE_H

#include "qworkspace.h"

class TPad;
class TGo4ViewPanel;


/** @author Mohammad Al-Turany
  * @author S. Linev
  * @since 02.05.2002 */

class TGo4WorkSpace : public QWorkspace {
   Q_OBJECT

   public:

      TGo4WorkSpace(QWidget *parent);
      virtual ~TGo4WorkSpace();

      TGo4ViewPanel* GetActivePanel();

      TPad* GetSelectedPad();
      void SetSelectedPad(TPad* pad);

      void ResponseOnPanelEvent(int funcid, TGo4ViewPanel* panel, TPad* pad);

      static TGo4WorkSpace* Instance();

   signals:
      void panelSignal(TGo4ViewPanel*, TPad*, int);

   protected:
      virtual bool eventFilter(QObject *fx, QEvent *e);
      virtual void closeEvent(QCloseEvent * ce);

   protected slots:
      void windowActivatedSlot(QWidget*);

   private:
      TGo4ViewPanel* fxActivePanel;
      TPad*         fxActivePad;
      TPad*         fxSelectedPad;
      static TGo4WorkSpace* fInstance;
};

#endif //TGO4WORKSPACE_H
