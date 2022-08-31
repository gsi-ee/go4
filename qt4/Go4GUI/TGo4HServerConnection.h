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

#ifndef TGO4HSERVERCPNNECTION_H
#define TGO4HSERVERCPNNECTION_H

#include <QDialog>
#include "ui_TGo4HServerConnection.h"

class TGo4HServerConnection : public QDialog, public Ui::TGo4HServerConnection
{
   Q_OBJECT

public:
   TGo4HServerConnection( QWidget* parent = nullptr, const char *name = nullptr, bool modal = false, Qt::WindowFlags fl = Qt::Widget );

};

#endif
