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

#include <QDir>
#include <QStringList>
#include <iostream>

#include "TSystem.h"
#include "TEnv.h"
#include "TApplication.h"
#include "TInterpreter.h"

#include "QRootApplication.h"

#include "ExampleWidget.h"

int main(int argc, char **argv)
{

   argc = 1; // hide all additional parameters from ROOT and Qt
   TApplication app("uno", &argc, argv); // ROOT application

   char* argv2[3];
   argv2[0] = argv[0];
   argv2[argc] = 0;

   QRootApplication myapp(argc, argv2); // Qt application

   // create instance, which should be used everywhere
   ExampleWidget* widget = new ExampleWidget();

   myapp.connect(&myapp, SIGNAL(lastWindowClosed()), &myapp, SLOT(quit()));

   widget->ensurePolished();
   widget->show();

   int res = myapp.exec();
   return res;
}
