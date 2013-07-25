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

#include "QGo4RootCanvas.h"

#include "Riostream.h"
#include "TCanvas.h"

#include "qapplication.h"
#include "qstring.h"
#include "qcursor.h"
#include "qdragobject.h"

QGo4RootCanvas::QGo4RootCanvas(QWidget *parent, const char *name, TCanvas *c) :
   TQRootCanvas(parent, name, c),
   fxShowEventStatus(false)
{
   setAcceptDrops(true);
   setSizeIncrement( QSize( 100, 100 ) );
   setBaseSize(QSize(800, 500));
   Resize();
   Update();
}

QGo4RootCanvas::~QGo4RootCanvas()
{
}

void QGo4RootCanvas::setShowEventStatus(bool s)
{
   fxShowEventStatus = s;
}

bool QGo4RootCanvas::showEventStatus() const
{
   return fxShowEventStatus;
}

void QGo4RootCanvas::mouseMoveEvent(QMouseEvent *e)
{
   TQRootCanvas::mouseMoveEvent(e);
   if(fxShowEventStatus) {
      TObject* selected = fCanvas->GetSelected();
      Int_t px = fCanvas->GetEventX();
      Int_t py = fCanvas->GetEventY();
      QString buffer = "";
      if (selected!=0) {
         buffer = selected->GetName();
         buffer += "  ";
         buffer += selected->GetObjectInfo(px,py);
      } else {
         buffer = "No seleceted object x = ";
         buffer += QString::number(px);
         buffer += "  y = ";
         buffer += QString::number(py);
      }
      emit CanvasStatusEvent(buffer.latin1());
   }
}

void QGo4RootCanvas::dragEnterEvent(QDragEnterEvent* e)
{
   if (QTextDrag::canDecode(e))
      e->accept();
}

void QGo4RootCanvas::dropEvent(QDropEvent* event)
{
   TObject *object = 0;
   QPoint Pos = event->pos();
   TPad* pad = Pick(Pos.x(), Pos.y(), object);

   if (pad!=0)
     emit CanvasDropEvent(event, pad);
}

void QGo4RootCanvas::performResize()
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
   TQRootCanvas::performResize();
   emit DoCanvasResize();
   QApplication::restoreOverrideCursor();
}

void QGo4RootCanvas::leaveEvent(QEvent* e)
{
   TQRootCanvas::leaveEvent(e);
   emit CanvasLeaveEvent();
}
