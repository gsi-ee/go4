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

#ifndef TGO4LOADEDLIBRARIES_H
#define TGO4LOADEDLIBRARIES_H

#include <QDialog>
#include "ui_TGo4LoadedLibraries.h"

class TGo4LoadedLibraries : public QDialog, public Ui::TGo4LoadedLibraries
{
    Q_OBJECT

public:
    TGo4LoadedLibraries( QWidget* parent = 0 );

public slots:
   virtual void LoadNewLibrary();
   virtual void UnloadLibrary();
   virtual void RefreshLibs();
};

#endif
