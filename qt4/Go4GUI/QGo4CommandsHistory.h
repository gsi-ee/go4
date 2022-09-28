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

#ifndef QGO4COMMANDSHISTORY_H
#define QGO4COMMANDSHISTORY_H

#include <QComboBox>

#include <QtCore/QStringList>

#ifdef __GO4DESIGNER__
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QGo4CommandsHistory : public QComboBox {
#else
class QGo4CommandsHistory : public QComboBox {
#endif

   Q_OBJECT

   public:
      QGo4CommandsHistory(QWidget* w, const char *name = nullptr);
      virtual ~QGo4CommandsHistory();

      QStringList getHistory(int max = 50);

   Q_SIGNALS:
      void enterPressedSingal();

   protected:
      void keyPressEvent(QKeyEvent* e) override;
};

#endif
