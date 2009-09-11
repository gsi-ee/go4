#include <stdlib.h>

#include <qdir.h>
#include <qstringlist.h>

#include "Riostream.h"
#include "TSystem.h"

#include "tqapplication.h"
#include "qrootapplication.h"

#include "TGo4Log.h"
#include "TGo4Version.h"
#include "TGo4MainWindow.h"
#include "TGo4QSettings.h"
#include "TGo4BrowserProxy.h"

int main(int argc, char **argv)
{
   if (!TGo4Version::Instance()->CheckVersion(__GO4BUILDVERSION__)) {
      cerr << "Go4 was build with version " << __GO4BUILDVERSION__ << endl;
      cerr << "Loaded go4 libraries has version " << TGo4Version::Instance()->GetBuildVersion() << endl;
      cerr << "Please configure your system correctly and restart go4 again" << endl;
      return -1;
   }

   int dologin = -1;
   const char* loghost = "localhost";
   int logport = 5000;
   const char* logpass = 0;

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
         if((strcmp(argv[narg], "-observer")==0) ||
            (strcmp(argv[narg], "-controller")==0) ||
            (strcmp(argv[narg], "-admin")==0)) {

            if(strcmp(argv[narg], "-observer")==0) dologin = 0;
            if(strcmp(argv[narg], "-controller")==0) dologin = 1;
            if(strcmp(argv[narg], "-admin")==0) dologin = 2;

            if ((narg+1<argc) && (argv[narg+1][0]!='-'))
               loghost = argv[++narg];

            if ((narg+1<argc) && (argv[narg+1][0]!='-') && (argv[narg+1][0]>='0') && (argv[narg+1][0]<='9'))
               logport = atoi(argv[++narg]);

            if ((narg+1<argc) && (argv[narg+1][0]!='-')) logpass = argv[++narg];
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

   TQApplication app("uno",&argc,argv); // init ROOT before Qt because of XInitThreads JA

   QRootApplication myapp( argc, argv, 0 );

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
   // qt default to have the settings in $HOME/.qt/
   // NOTE: if $HOME/.qt/go4rc exists,
   // Qt would use these settings even if the TGo4QSettings use the
   // current dir option or userpath for saving the settings;
   // Therefore, the local settings will have different names go4localrc
   TGo4QSettings::SetHomeSettingsFile("/go4"); // might change the default filename in .qt here
   bool accountsettings=false;
   QString settingsenv=getenv("GO4SETTINGS");
   if(settingsenv.isEmpty()) {
      // use Qt settings in $PWD/.qt. if false, use $HOME/.qt, or the user path if set
      if (gSystem->AccessPathName(QDir::currentDirPath(),kWritePermission)) {
         TGo4QSettings::SetToCurrentDir(false);
         accountsettings=true;
      } else {
         TGo4QSettings::SetToCurrentDir(true);
      }
   } else
      if(settingsenv.contains("ACCOUNT")) {
         TGo4QSettings::SetToCurrentDir(false);
         accountsettings=true;
      } else {
         TGo4QSettings::SetUserPath(settingsenv);
      }
   /// check here if current settings exist. If not, copy from home
   QString homesettings = QDir::homeDirPath()+ "/.qt/go4rc";
   QString usersettings = TGo4QSettings::GetUserPath()+TGo4QSettings::GetSettingsName()+"rc";
   QString hometools = QDir::homeDirPath()+ "/.qt/go4toolsrc";
   QString usertools = TGo4QSettings::GetUserPath()+"/go4toolsrc";

   if(!accountsettings && gSystem->AccessPathName(usersettings)) // file not there
   {
      // first create subfolder .qt if not there, otherwise copy fails!
      QString qsubdir=TGo4QSettings::GetUserPath();
      if(gSystem->AccessPathName(qsubdir))
      {
         cout <<"Creating settings subdirectory "<<qsubdir<<" ..."<< endl;
         if(gSystem->mkdir(qsubdir,true)!=0)
            cout <<"Could not create "<<qsubdir<<" !"<< endl;

      }

      if(gSystem->CopyFile(homesettings,usersettings,kTRUE)==0)
         cout <<"Copied account settings "<<homesettings<<" to "<<usersettings << endl;
      else
         cout<<"Could not copy "<<homesettings<<" to "<<usersettings << endl;
      if(gSystem->CopyFile(hometools,usertools,kTRUE)==0)
         cout <<"Copied account settings "<<hometools<<" to "<<usertools << endl;
      else
         cout<<"Could not copy "<<hometools<<" to "<<usertools << endl;
   }
   /////// end settings setup ///////////////////////////////
   if(traceon) TGo4Log::SetIgnoreLevel(0);
   else TGo4Log::SetIgnoreLevel(1);

   if (argc<2)
      cout << "G-OOOO->  Started go4gui. "  << endl;

   cout << "   Go4 " << __GO4RELEASE__ << endl;

   // create instance, which should be used everywhere
   go4sett = new TGo4QSettings();

   TGo4MainWindow* Go4MainGUI = new TGo4MainWindow(&myapp, servermode);


   myapp.setMainWidget(Go4MainGUI);
   Go4MainGUI->polish();
   Go4MainGUI->show();
   myapp.connect( &myapp, SIGNAL( lastWindowClosed() ), &myapp, SLOT( quit() ) );
   QApplication::setDoubleClickInterval(400); //ms, for Qt>=3.3 avoid very fast defaults!
   QApplication::setStartDragTime(150); // ms

   for ( QStringList::Iterator it = files.begin(); it != files.end(); ++it )
      Go4MainGUI->Browser()->OpenFile((*it).latin1());

   if (hotstart.length()>0)
      Go4MainGUI->HotStart(hotstart.latin1());

   if (dologin>=0) {
      go4sett->setClientNode(loghost);
      go4sett->setClientPort(logport);
      go4sett->setClientDefaultPass(logpass==0);
      go4sett->setClientControllerMode(dologin);
      Go4MainGUI->ConnectServerSlot(false, logpass);
   }

   int res = myapp.exec();

   delete go4sett;

   return res;
}

