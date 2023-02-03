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

#include "TApplication.h"

#include "QRootApplication.h"

#include "ExampleWidget.h"

int main(int argc, char **argv)
{

   argc = 1; // hide all additional parameters from ROOT and Qt
   TApplication app("uno", &argc, argv); // ROOT application

   cha r argv2[2];
   argv2[0] = argv[0];
   argv2[argc] = 0;

   QRootApplication myapp(argc, argv2); // Qt application

   // create instance, which should be used everywhere
   ExampleWidget* widget = new ExampleWidget();

   QObject::connect(&myapp, &QRootApplication::lastWindowClosed, &myapp, &QRootApplication::quit);

   widget->show();

   int res = myapp.exec();
   return res;
}
