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

#ifndef TGO4DIVIDEPAD_H
#define TGO4DIVIDEPAD_H

#include <QWidget>
#include "ui_TGo4DividePad.h"

class TGo4DividePad : public QWidget, public Ui::TGo4DividePad
{
   Q_OBJECT

public:
   TGo4DividePad( QWidget* parent = nullptr, const char *name = nullptr, Qt::WindowFlags fl = Qt::Widget );

public slots:

   virtual void DivideCanvas();
};

#endif
