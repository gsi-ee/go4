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
#include <QGuiApplication>
#include <iostream>

#include "TSystem.h"
#include "TEnv.h"
#include "TApplication.h"
#include "TInterpreter.h"

#include "QRootApplication.h"
#include "TGo4Log.h"
#include "TGo4Version.h"
#include "TGo4MainWindow.h"
#include "TGo4BrowserProxy.h"
#include "TGo4DabcProxy.h"
#include "TGo4QSettings.h"
#include "TGo4AbstractInterface.h"

#ifndef _MSC_VER
#ifdef __GO4X11__
#include "TGX11.h"
//extern void qt_x11_set_global_double_buffer(bool);
#endif
#else
#include "TGo4BufferQueue.h"
#endif


#ifdef __GO4WEB__
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QtWebEngine>
#endif
#endif

int print_go4_version()
{
   const char *dabc_vers = TGo4DabcProxy::GetDabcVersion();
   std::cout << "   Go4 " << __GO4RELEASE__ << ",   build with ROOT " << ROOT_RELEASE;
   if (dabc_vers) std::cout << ", DABC " << dabc_vers;
   std::cout << " and Qt " << QT_VERSION_STR << std::endl;
   return 0;
}


int go4_usage() {

   print_go4_version();

   std::cout << "Usage: " << std::endl;
   std::cout << "   go4 [args]                  - start go4 GUI" << std::endl;
   std::cout << "   go4 --web                   - start go4 GUI with web graphics" << std::endl;
   std::cout << "   go4 --x11                   - start go4 GUI with X11 graphics" << std::endl;
   std::cout << "   go4 file1.root              - load ROOT file(s) at start" << std::endl;
   std::cout << "   go4 filename[.hotstart]     - process hotstart file" << std::endl;
   std::cout << "   go4 -observer hostname port - connect with running analysis server" << std::endl;
   std::cout << "   go4 -controller hostname port" << std::endl;
   std::cout << "   go4 -admin hostname port" << std::endl;
   std::cout << "   go4 -prepare                - prepare for analysis client connection" << std::endl;
   std::cout << "   go4 -usergui                - activate usergui" << std::endl;
#ifdef __GO4WEB__
   std::cout << "   go4 -web                    - activate web-based canvas" << std::endl;
#endif
   if (TGo4DabcProxy::GetDabcVersion())
       std::cout << "   go4 dabc://server[:port]    - connect with DABC server" << std::endl;
   std::cout << "   go4 http://server[:port]    - connect with ROOT-based HTTP server" << std::endl;
   std::cout << "   go4 -debug                  - enable GUI debug output" << std::endl;
   std::cout << "   go4 -gdebug [lvl]           - set ROOT gDebug value (default 1)" << std::endl;
   std::cout << "   go4 -help                   - show this help information" << std::endl;

   return 0;
}

int main(int argc, char **argv)
{
   if ((argc==2) && (!strcmp(argv[1],"?") || !strcmp(argv[1],"-h")  || !strcmp(argv[1],"-help") || !strcmp(argv[1],"--help"))) return go4_usage();

   setlocale(LC_ALL, "C");

#ifndef _MSC_VER
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

   if (!TGo4Version::CheckVersion(__GO4BUILDVERSION__)) {
      std::cerr << "Please configure your system correctly and restart go4 again" << std::endl;
      return -1;
   }

   int dologin = -1;
   const char *loghost = "localhost";
   int logport = 5000;
   const char *logpass = nullptr;

   bool prepare_for_client = false, traceon = false, usergui = false, usex11 = false;

#ifdef __GO4WEB__
   bool useweb = false;
#endif

#ifndef __GO4X11__
#ifdef __GO4WEB__
   useweb = true;
#else
   std::cerr << "Failure, go4 build without x11 and without web support" << std::endl;
   return -1;
#endif
#endif

   QString hotstart, dabcnode;
   QStringList files, httpnodes;

   for (int narg = 1; narg < argc; narg++) {
      if (strlen(argv[narg]) == 0)
         continue;

      QString curr(argv[narg]);

      if (argv[narg][0]=='-') {
         if ((curr == "--web") || (curr == "-web")) {
#ifdef __GO4WEB__
            useweb = true;
#endif
         } else if ((curr == "--x11") || (curr == "-x11")) {
            usex11 = true;
         } else if (curr == "-debug") {
            std::cout << "G-OOOO-> MainGo4GUI switched on debug output" << std::endl;
            traceon = true;
         } else if (curr == "-gdebug") {
            if ((narg+1 < argc) && (strlen(argv[narg+1]) > 0) && (argv[narg+1][0]!='-'))
               gDebug = TString(argv[++narg]).Atoi();
            else
               gDebug = 1;
         } else if ((curr == "-observer") || (curr == "-controller") || (curr == "-admin")) {

            if (curr == "-observer") dologin = 0;
            if (curr == "-controller") dologin = 1;
            if (curr == "-admin") dologin = 2;

            if ((narg+1<argc) && (argv[narg+1][0]!='-'))
               loghost = argv[++narg];

            if ((narg+1<argc) && (argv[narg+1][0]!='-') && (argv[narg+1][0]>='0') && (argv[narg+1][0]<='9'))
               logport = QString(argv[++narg]).toInt();

            if ((narg+1<argc) && (argv[narg+1][0]!='-')) logpass = argv[++narg];
         } else if (curr == "-prepare") {
            prepare_for_client = true;
         } else if (curr == "-usergui") {
            usergui = true;
         }
      } else if (curr.contains(".root")) {
         files.append(argv[narg]);
      } else if (curr.contains("dabc://")) {
         dabcnode = argv[narg];
      } else if (curr.contains("http://") || curr.contains("https://")) {
         httpnodes.append(argv[narg]);
      } else if (hotstart.isEmpty()) {
         hotstart = argv[narg];
         if(!curr.contains(".hotstart")) hotstart.append(".hotstart");
      }
   }

   argc = 1; // hide all additional parameters from ROOT and Qt

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

   const char *_env = gSystem->Getenv("GO4SETTINGS");

   QString settfile;
   if (_env) settfile = _env;

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
#ifdef __GO4WEB__
   if (!usex11 && !useweb) {
      if (QGuiApplication::platformName() == "wayland") {
         useweb = true;
         std::cout << "Use web graphics on Wayland by default, start go4 --x11 to force Wayland usage" << std::endl;
      }
   }

   go4sett->setWebBasedCanvas(useweb);
#else
   (void) usex11; // avoid warnings
#endif

   ////// end settings setup ///////////////////////////////

   TGo4Log::SetIgnoreLevel(traceon ? 0 : 1);

   print_go4_version();

   // first thing to set is screen scale factor for Qt>5.6
   double scalefactor = go4sett->getScreenScaleFactor();

   std::cout << "Use Screen scale factor " << scalefactor << " from settings." << std::endl;
   gSystem->Setenv("QT_SCALE_FACTOR",QString("%1").arg(scalefactor).toLatin1 ().constData ());

   TApplication app("uno", &argc, argv); // ROOT application

   char *argv2[3];
   argv2[0] = argv[0];
#ifdef __GO4WEB__
   argv2[argc++] = (char *) "--ignore-gpu-blacklist";
#endif
   argv2[argc] = nullptr;

   Q_INIT_RESOURCE(go4icons);

#ifdef __GO4WEB__
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#if QT_VERSION >= QT_VERSION_CHECK(5,13,0)
   // must be called before creating QApplication, from Qt 5.13
   if (useweb) QtWebEngine::initialize();
#endif
#endif
#endif

   QRootApplication myapp(argc, argv2); // Qt application

#ifdef __GO4WEB__
#if QT_VERSION < QT_VERSION_CHECK(5,13,0)
   // must be called after creating QApplication, until Qt 5.12
   if (useweb) QtWebEngine::initialize();
#endif
#endif

   // qt4.4 sets local settings not to "C", but to system-depended values
   ::setlocale(LC_ALL, "C");

   TGo4AbstractInterface::SetInitSharedLibs();

   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
   // set this to 2 to get warnings and errors only
   // set this to 3 to get errors only

   // SL 11.01.2018 Disable of interpreter locking  makes strange problems with other ROOT components, try to keep it enabled

   gInterpreter->SetProcessLineLock(kTRUE); // changed for root 6 problems JAM 9-10-2018

   TString go4inc = TGo4Log::GO4INCPATH();
   if (go4inc.IsNull())
      go4inc = TGo4Log::subGO4SYS("include");
   if (!go4inc.IsNull())
      gInterpreter->AddIncludePath(go4inc.Data());

   // ShowGuideLines cases crashes - it has stored gpad in static variable
   gEnv->SetValue("Canvas.ShowGuideLines", 0);

   // create instance, which should be used everywhere
   auto main = new TGo4MainWindow(&myapp);

   myapp.connect(&myapp, &QRootApplication::lastWindowClosed, &myapp, &QRootApplication::quit);

   main->ensurePolished();
   main->show();

   QApplication::setDoubleClickInterval(400); //ms, for Qt>=3.3 avoid very fast defaults!
   QApplication::setStartDragTime(150); // ms

   for (int i = 0; i < files.size(); ++i)
      main->Browser()->OpenFile(files.at(i).toLatin1().constData());

   if (dabcnode.length() > 0)
      main->Browser()->ConnectDabc(dabcnode.toLatin1().constData());

   for (int i = 0; i < httpnodes.size(); ++i)
      main->ConnectHttpServer(httpnodes.at(i).toLatin1().constData());

   if (hotstart.length() > 0)
      main->HotStart(hotstart.toLatin1().constData());

   if (usergui)
      main->UserPanelSlot();

   if (dologin >= 0) {
      go4sett->setClientNode(loghost);
      go4sett->setClientPort(logport);
      go4sett->setClientDefaultPass(!logpass);
      go4sett->setClientControllerMode(dologin);
      main->ConnectServer(false, logpass);
   } else if (prepare_for_client) {
      main->PrepareForClientConnection(false);
   }

   int res = myapp.exec();
   delete go4sett;
   return res;
}
