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

#ifndef QGO4COMMANDSHISTORY_H
#define QGO4COMMANDSHISTORY_H


#include "qwidgetplugin.h"
#include "qcombobox.h"

class QT_WIDGET_PLUGIN_EXPORT QGo4CommandsHistory : public QComboBox {
   Q_OBJECT

   public:
      QGo4CommandsHistory(QWidget* w, const char* name);
      virtual ~QGo4CommandsHistory();
      bool EnterPressed() const { return fiEnterPressed == 1; }
      void ResetEnterPressed() { fiEnterPressed = -1; }
      void SetEnterPressed(int val){fiEnterPressed=val;}
   protected:
      virtual void keyPressEvent(QKeyEvent* e);
      int fiEnterPressed;
};

#endif
