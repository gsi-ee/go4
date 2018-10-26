// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef QROOTCANVAS_H
#define QROOTCANVAS_H

#include "rootwebview.h"

class TCanvas;
class TPad;
class TObject;

class QWebCanvas : public QWidget {

   Q_OBJECT

public:
   QWebCanvas(QWidget *parent = 0);
   virtual ~QWebCanvas();

   /// returns canvas shown in the widget
   TCanvas *getCanvas() { return fCanvas; }

signals:

   void CanvasDropEvent(QDropEvent*, TPad*);

public slots:

   void activateGed(TObject *obj = 0);
   void activateStatusLine();

protected slots:

   void dropView(QDropEvent* event);

protected:

   virtual void resizeEvent(QResizeEvent *event);

   virtual void dropEvent(QDropEvent* event);

   RootWebView *fView;  ///< qt webwidget to show

   TCanvas *fCanvas;
};

#endif
