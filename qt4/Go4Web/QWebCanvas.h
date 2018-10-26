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

class QWebCanvas : public QWidget {

   Q_OBJECT

public:
   QWebCanvas(QWidget *parent = 0);
   virtual ~QWebCanvas();

   TCanvas *getCanvas() { return fCanvas; }

protected:

   RootWebView *fView;  ///< qt webwidget to show

   TCanvas *fCanvas;
};

#endif
