#include "Riostream.h"
#include <stdlib.h>
#include <QDir>
#include <QStringList>

#include "TSystem.h"
#include "TEnv.h"

#include "TApplication.h"
#include "QRootApplication.h"

#include "TGo4Log.h"
#include "TGo4Version.h"
#include "TGo4MainWindow.h"
#include "TGo4BrowserProxy.h"
#include "TGo4QSettings.h"

#ifndef WIN32
#include "TGX11.h"
//extern void qt_x11_set_global_double_buffer(bool);
#endif

int main(int argc, char **argv)
{
#ifndef WIN32
   gEnv->SetValue("X11.XInitThread", 0);   // required to avoid conflicts with Qt4
   //    qt_x11_set_global_double_buffer(false); // improves qtroot canvas update
#endif
   bool traceon = false;
   bool servermode = true;
   QString hotstart = "";
   QStringList files;

   for(int narg=1;narg<argc;narg++) {
      if (strlen(argv[narg])==0) continue;

      if (argv[narg][0]=='-') {
         if(!strcmp(argv[narg], "-debug")) {
            cout << "G-OOOO-> MainGo4GUI switched on debug output" << endl;
            traceon = true;
         } else
         if(strstr(argv[narg], "-client")) {
            cout << "G-OOOO-> MainGo4GUI is starting as client." << endl;
            servermode = false;
            if(strstr(argv[narg], "-debug")) traceon = true;
         }
      } else
      if (strstr(argv[narg],".root")!=0)
         files.append(argv[narg]);
      else
         if (hotstart.length()==0) {
            hotstart = argv[narg];
            if(!strstr(argv[narg], ".hotstart")) hotstart.append(".hotstart");
         }
   }

   TApplication app("uno", &argc, argv); // ROOT application

   Q_INIT_RESOURCE(go4icons);
   QRootApplication myapp(argc, argv); // Qt application

   TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages
   TGo4Log::Instance(); // init logger object
   TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
   // set this to 2 to get warnings and errors only
   // set this to 3 to get errors only

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
   QString settingsenv = getenv("GO4SETTINGS");

   int sett_try = 0;

   if(settingsenv.contains("ACCOUNT")) {
      sett_try++;
      // do nothing, it is default location in .config/GSI/go4.conf
   } else
   if(!settingsenv.isEmpty() && !settingsenv.contains("LOCAL")) {
      TGo4QSettings::SetSettLocation(settingsenv);
   } else
   if (!gSystem->AccessPathName(QDir::currentPath().toAscii(),kWritePermission))
       TGo4QSettings::SetSettLocation(QDir::currentPath() + "/go4.conf");

   QString settfile = TGo4QSettings::GetSettLoaction();

   while ((settfile.length() > 0) && gSystem->AccessPathName(settfile.toAscii())) {
      QString subdir = QFileInfo(settfile).absolutePath();
      if (gSystem->AccessPathName(subdir.toAscii()))
         if (gSystem->mkdir(subdir.toAscii(), kTRUE)==0)
            cout << "Create subdirectory " << subdir.toAscii().constData() << " for config file" << endl;
         else {
            cout << "Cannot create subdirectory " << subdir.toAscii().constData() << " for config file, use default locations" << endl;
            if (++sett_try>1) break;
            TGo4QSettings::SetSettLocation("");
            settfile = TGo4QSettings::GetSettLoaction();
            continue;
         }

      const char* go4sys = getenv("GO4SYS");
      QString dfltfile = "qt4/go4.conf";
      if (go4sys!=0)
         dfltfile = QString(go4sys) + "/" + dfltfile;

      if (gSystem->CopyFile(dfltfile.toAscii(), settfile.toAscii(), kFALSE)) {
         cout << "Cannot copy default config file into " << settfile.toAscii().constData() << " , use default location " << endl;
         if (++sett_try>1) break;
         TGo4QSettings::SetSettLocation("");
         settfile = TGo4QSettings::GetSettLoaction();
      } else {
         cout << "Copied default config file into " << settfile.toAscii().constData() << endl;
         break;
      }
   }

   go4sett = new TGo4QSettings();

   /////// end settings setup ///////////////////////////////
   if(traceon) TGo4Log::SetIgnoreLevel(0);
   else TGo4Log::SetIgnoreLevel(1);

   if (argc<2)
      cout << "G-OOOO->  Started go4gui. "  << endl;
   cout << "   Go4 " << __GO4RELEASE__ << endl;
   // create instance, which should be used everywhere

   TGo4MainWindow* Go4MainGUI = new TGo4MainWindow(&myapp, servermode);
   Go4MainGUI->setGeometry (20, 20, 1152, 864);

   myapp.connect(&myapp, SIGNAL(lastWindowClosed()), &myapp, SLOT(quit()));

   Go4MainGUI->ensurePolished();
   Go4MainGUI->show();
   myapp.connect( &myapp, SIGNAL( lastWindowClosed() ), &myapp, SLOT( quit() ) );
   QApplication::setDoubleClickInterval(400); //ms, for Qt>=3.3 avoid very fast defaults!
   QApplication::setStartDragTime(150); // ms

   for (int i = 0; i < files.size(); ++i)
      Go4MainGUI->Browser()->OpenFile(files.at(i).toAscii());

   if (hotstart.length()>0)
      Go4MainGUI->HotStart(hotstart.toAscii());

   int res = myapp.exec();
   delete go4sett;
   return res;
}
