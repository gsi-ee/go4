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

#include "QGo4LineEdit.h"

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>


QGo4LineEdit::QGo4LineEdit(QWidget* parent, const char* name) :
   QLineEdit(parent)
{
   setObjectName(name ? name : "Go4LineEdit");
   setAcceptDrops( true );
}

QGo4LineEdit::~QGo4LineEdit()
{
}

void QGo4LineEdit::dragEnterEvent(QDragEnterEvent* event)
{
   if (event->mimeData()->hasText())
      event->acceptProposedAction();
}

void QGo4LineEdit::dropEvent(QDropEvent* event)
{
   setText(event->mimeData()->text());
   emit textDropped();
}
