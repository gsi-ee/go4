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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qglobal.h>
#include <locale.h>
#include <QDir>
#include <QStringList>

#include "TSystem.h"
#include "RVersion.h"
#include "TEnv.h"
#include "TApplication.h"
#include "QRootApplication.h"
#include "TInterpreter.h"

#include "TGo4Log.h"
#include "TGo4Version.h"
#include "TGo4MainWindow.h"
#include "TGo4BrowserProxy.h"
#include "TGo4DabcProxy.h"
#include "TGo4BufferQueue.h"
#include "TGo4QSettings.h"
#include "TGo4AbstractInterface.h"

#include "TROOT.h"
#include "Riostream.h"


#ifndef WIN32
#include "TGX11.h"
//extern void qt_x11_set_global_double_buffer(bool);
#endif

int print_go4_version()
{
   const char* dabc_vers = TGo4DabcProxy::GetDabcVersion();
   std::cout << "   Go4 " << __GO4RELEASE__ << ",   build with ROOT " << ROOT_RELEASE;
   if (dabc_vers) std::cout << ", DABC " << dabc_vers;
   std::cout << " and Qt " << QT_VERSION_STR << std::endl;
   return 0;
}


int go4_usage() {

   print_go4_version();

   std::cout << "Usage: " << std::endl;
   std::cout << "   go4 [args]                  - start go4 GUI" << std::endl;
   std::cout << "   go4 file1.root              - load ROOT file(s) at start" << std::endl;
   std::cout << "   go4 filename[.hotstart]     - process hotstart file" << std::endl;
   std::cout << "   go4 -observer hostname port - connect with running analysis server" << std::endl;
   std::cout << "   go4 -controller hostname port" << std::endl;
   std::cout << "   go4 -admin hostname port" << std::endl;
   std::cout << "   go4 -prepare                - prepare for analysis client connection" << std::endl;
  if (TGo4DabcProxy::GetDabcVersion())
   std::cout << "   go4 dabc://server[:port]    - connect with DABC server" << std::endl;
   std::cout << "   go4 http://server[:port]    - connect with ROOT-based HTTP server" << std::endl;
   std::cout << "   go4 -debug                  - enable GUI debug output"   << std::endl;
   std::cout << "   go4 -help                   - show this help information" << std::endl;

   return 0;
}

int main(int argc, char **argv)
{
   if ((argc==2) && (!strcmp(argv[1],"?") || !strcmp(argv[1],"-h")  || !strcmp(argv[1],"-help") || !strcmp(argv[1],"--help"))) return go4_usage();

   setlocale(LC_ALL, "C");

#ifndef WIN32
   gEnv->SetValue("X11.XInitThread", 0);   // required to avoid conflicts with Qt4
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

   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      std::cerr << "Please configure your system correctly and restart go4 again" << std::endl;
      return -1;
   }

   int dologin = -1;
   const char* loghost = "localhost";
   int logport = 5000;
   const char* logpass = 0;

   bool prepare_for_client = false;

   bool traceon = false;
   QString hotstart(""), dabcnode("");
   QStringList files, httpnodes;

   for(int narg=1;narg<argc;narg++) {
      if (strlen(argv[narg])==0) continue;

      if (argv[narg][0]=='-') {

         if(!strcmp(argv[narg], "-debug")) {
            std::cout << "G-OOOO-> MainGo4GUI switched on debug output" << std::endl;
            traceon = true;
         } else
         if((strcmp(argv[narg], "-observer")==0) ||
            (strcmp(argv[narg], "-controller")==0) ||
            (strcmp(argv[narg], "-admin")==0)) {

            if(strcmp(argv[narg], "-observer")==0) dologin = 0;
            if(strcmp(argv[narg], "-controller")==0) dologin = 1;
            if(strcmp(argv[narg], "-admin")==0) dologin = 2;

            if ((narg+1<argc) && (argv[narg+1][0]!='-'))
               loghost = argv[++narg];

            if ((narg+1<argc) && (argv[narg+1][0]!='-') && (argv[narg+1][0]>='0') && (argv[narg+1][0]<='9'))
               logport = QString(argv[++narg]).toInt();

            if ((narg+1<argc) && (argv[narg+1][0]!='-')) logpass = argv[++narg];
         } else
         if (strcmp(argv[narg], "-prepare")==0) {
            prepare_for_client = true;
         }
      } else
      if (QString(argv[narg]).contains(".root")) {
         files.append(argv[narg]);
      } else
      if (QString(argv[narg]).contains("dabc://")) {
         dabcnode = argv[narg];
      } else
      if (QString(argv[narg]).contains("http://") || QString(argv[narg]).contains("https://")) {
         httpnodes.append(argv[narg]);
      } else
      if (hotstart.length()==0) {
         hotstart = argv[narg];
         if(!QString(argv[narg]).contains(".hotstart")) hotstart.append(".hotstart");
      }
   }

   argc = 1; // hide all additional parameters from ROOT and Qt

   TApplication app("uno", &argc, argv); // ROOT application

   Q_INIT_RESOURCE(go4icons);

   QRootApplication myapp(argc, argv); // Qt application

   // qt4.4 sets local settings not to "C", but to system-depended values
   ::setlocale(LC_ALL, "C");

   TGo4AbstractInterface::SetInitSharedLibs();

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
   //in qt5 such operation gives warning, but does not work :(
   QApplication::setColorSpec( QApplication::ManyColor );
#endif

   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
   // set this to 2 to get warnings and errors only
   // set this to 3 to get errors only


   // disable cint locking when called via process line
   // makes problem in multi-threaded environment, where CINT lock
   // also used for object streaming
   gInterpreter->SetProcessLineLock(kFALSE);

   // ShowGuideLines cases crashes - it has stored gpad in static variable
   gEnv->SetValue("Canvas.ShowGuideLines", 0);

   ///////////// Define the GO4 Settings. //////////////////////
   // has to be done here, since mainwindow components have local
   // settings access before mainwindow init is executed!
   // Default is to use current directory. Environment variable
   // GO4SETTINGS can be set to a certain go4 setup location
   // if GO4SETTINGS contains the "ACCOUNT" keyword, we use the
   // qt default to have the settings in $HOME/.config/GSI
   /////////////////JAM-  new for Qt 4.4: no search path anymore!
   // we may store window geometries to local folder (or into user path, or into account path),
   // the rest is in the account settings since we cannot specify user path anymore
   // (will be in $HOME/.config/GSI/go4.conf)

   const char* _env = gSystem->Getenv("GO4SETTINGS");

   QString settfile;
   if (_env!=0) settfile = _env;

   if(iswin32 || settfile.isEmpty() || settfile.contains("ACCOUNT")) {
      settfile = "";
      // do nothing, it is default location in .config/GSI/go4.conf
   } else {
      if (settfile.contains("LOCAL")) settfile = QDir::currentPath() + "/go4.conf";
      QString subdir = QFileInfo(settfile).absolutePath();
      // if there is no write access to directory where setting file should be placed,
      // default directory will be used
      if (gSystem->AccessPathName(subdir.toLatin1().constData(),kWritePermission))
         settfile = "";
   }

   go4sett = new TGo4QSettings(settfile);

   /////// end settings setup ///////////////////////////////
   if(traceon) TGo4Log::SetIgnoreLevel(0);
          else TGo4Log::SetIgnoreLevel(1);

   print_go4_version();

   // create instance, which should be used everywhere

   TGo4MainWindow* Go4MainGUI = new TGo4MainWindow(&myapp);

   myapp.connect(&myapp, SIGNAL(lastWindowClosed()), &myapp, SLOT(quit()));

   Go4MainGUI->ensurePolished();
   Go4MainGUI->show();

   myapp.connect( &myapp, SIGNAL( lastWindowClosed() ), &myapp, SLOT( quit() ) );
   QApplication::setDoubleClickInterval(400); //ms, for Qt>=3.3 avoid very fast defaults!
   QApplication::setStartDragTime(150); // ms

   for (int i = 0; i < files.size(); ++i)
      Go4MainGUI->Browser()->OpenFile(files.at(i).toLatin1().constData());

   if (dabcnode.length()>0)
      Go4MainGUI->Browser()->ConnectDabc(dabcnode.toLatin1().constData());

   for (int i = 0; i < httpnodes.size(); ++i)
      Go4MainGUI->ConnectHttpSlot(httpnodes.at(i).toLatin1().constData());

   if (hotstart.length()>0)
      Go4MainGUI->HotStart(hotstart.toLatin1().constData());

   if (dologin>=0) {
      go4sett->setClientNode(loghost);
      go4sett->setClientPort(logport);
      go4sett->setClientDefaultPass(logpass==0);
      go4sett->setClientControllerMode(dologin);
      Go4MainGUI->ConnectServerSlot(false, logpass);
   } else
   if (prepare_for_client) {
      Go4MainGUI->PrepareForClientConnectionSlot(false);
   }

   int res = myapp.exec();
   delete go4sett;
   return res;
}
