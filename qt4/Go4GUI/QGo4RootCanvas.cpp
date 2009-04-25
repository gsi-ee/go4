#include "QGo4RootCanvas.h"

#include <QtCore/QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>

#include "Riostream.h"

#include "TCanvas.h"

#include "qapplication.h"
#include "qstring.h"
#include "qcursor.h"

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

void QGo4RootCanvas::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasText())
      event->acceptProposedAction();
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
   emit DoCanvasResize();
   TQRootCanvas::performResize();
   QApplication::restoreOverrideCursor();
}

void QGo4RootCanvas::leaveEvent(QEvent* e)
{
   TQRootCanvas::leaveEvent(e);
   emit CanvasLeaveEvent();
}
