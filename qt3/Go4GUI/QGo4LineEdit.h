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

#ifndef QGO4LINEEDIT_H
#define QGO4LINEEDIT_H

#include "qlineedit.h"
#include "qwidgetplugin.h"

/**
 * @author Mohammad Al-Turany
 * @author S.Linev
 * @since 14.11.2001
 */

class QT_WIDGET_PLUGIN_EXPORT QGo4LineEdit : public QLineEdit {
   Q_OBJECT

   public:
      QGo4LineEdit(QWidget *parent=0, const char *name=0);
      virtual ~QGo4LineEdit();

   signals:
      void textDropped();

   protected:
      virtual void dropEvent(QDropEvent*);
      virtual void dragEnterEvent(QDragEnterEvent*);
};

#endif
