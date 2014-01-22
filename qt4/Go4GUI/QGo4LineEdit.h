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

#include <QLineEdit>

#include <QDropEvent>
#include <QDragEnterEvent>

/**
 * @author Mohammad Al-Turany
 * @author S.Linev
 * @since 14.11.2001
 */

#ifdef WIN32

class QGo4LineEdit : public QLineEdit {

#else

#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QGo4LineEdit : public QLineEdit {

#endif
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
