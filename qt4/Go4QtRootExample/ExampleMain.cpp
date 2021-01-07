// $Id: MainGo4GUI.cpp 2819 2020-05-13 12:22:20Z linev $
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

#include "RVersion.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TApplication.h"
#include "TInterpreter.h"

#include "QRootApplication.h"

#include "ExampleWidget.h"


int main(int argc, char **argv)
{
   if ((argc==2) && (!strcmp(argv[1],"?") || !strcmp(argv[1],"-h")  || !strcmp(argv[1],"-help") || !strcmp(argv[1],"--help"))) return go4_usage();

   setlocale(LC_ALL, "C");

#ifndef WIN32
#ifdef __GO4X11__
   gEnv->SetValue("X11.XInitThread", 0);   // required to avoid conflicts with Qt4
#endif
   //    qt_x11_set_global_double_buffer(false); // improves qtroot canvas update
   bool iswin32 = false;
#else
   bool iswin32 = true;
   {
      // work around for client connection problem in windows
      // One should create several buffers and call WriteObjects before
      // any kind of connection will be done
      TGo4BufferQueue dummy("dummyQueue");
   }

#endif

   argc = 1; // hide all additional parameters from ROOT and Qt


   TApplication app("uno", &argc, argv); // ROOT application

   char* argv2[3];
   argv2[0] = argv[0];
#ifdef __GO4WEB__
   argv2[argc++] = (char *) "--ignore-gpu-blacklist";
#endif
   argv2[argc] = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#if QT_VERSION < QT_VERSION_CHECK(5,8,0)
   // method only available until qt5.8, then it was deprecated
   QApplication::setColorSpec( QApplication::ManyColor );
#endif
#else
    // JAM for qt4 option "many color" will cause empty ROOT canvas
   QApplication::setColorSpec( QApplication::NormalColor);
#endif

   // Q_INIT_RESOURCE(go4icons);

   QRootApplication myapp(argc, argv2); // Qt application

   // create instance, which should be used everywhere
   ExampleWidget* widget = new ExampleWidget(&myapp);

   myapp.connect(&myapp, SIGNAL(lastWindowClosed()), &myapp, SLOT(quit()));

   widget->ensurePolished();
   widget->show();

   int res = myapp.exec();
   return res;
}
