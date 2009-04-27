#include "Riostream.h"
#include <stdlib.h>
#include "qdir.h"

#include "TSystem.h"
#include "TEnv.h"

#include "TApplication.h"
#include "qrootapplication.h"

#include "TGo4Log.h"
#include "TGo4Version.h"
#include "TGo4MainWindow.h"
#include "TGo4QSettings.h"

#include "TGX11.h"

extern void qt_x11_set_global_double_buffer(bool);



int main(int argc, char **argv)
{

    gEnv->SetValue("X11.XInitThread", 0);   // required to avoid conflicts with Qt4
    qt_x11_set_global_double_buffer(false); // improves qtroot canvas update
    bool traceon = false;
    bool servermode = true;
    QString hotstart = "";
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
         if (hotstart.length()==0) {
            hotstart = argv[narg];
            if(!strstr(argv[narg], ".hotstart")) hotstart.append(".hotstart");
            exit;
         }
    }
    TApplication app("uno", &argc, argv); // ROOT application

    Q_INIT_RESOURCE(go4icons);
    QRootApplication myapp( argc, argv); // Qt application
    TGo4Log::Instance(); // init logger object
    TGo4Log::SetIgnoreLevel(1); // set this to 1 to suppress detailed debug output
    // set this to 2 to get warnings and errors only
    // set this to 3 to get errors only
    TGo4Log::LogfileEnable(kFALSE); // will enable or disable logging all messages

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
    bool accountsettings=false;
    QString settingsenv=getenv("GO4SETTINGS");
    if(settingsenv.isEmpty()) {
        // try settings in $PWD/.config, use $HOME/.config, or the user defined location
       if (gSystem->AccessPathName(QDir::currentDirPath(),kWritePermission)) {
         TGo4QSettings::SetToCurrentDir(false); // can not write on current dir: use account settings
         accountsettings=true;
       } else {
         TGo4QSettings::SetToCurrentDir(true); // by default, use current dir settings
       }
    } else
    if(settingsenv.contains("ACCOUNT")) {
       TGo4QSettings::SetToCurrentDir(false); // explicit defined account settings
       accountsettings=true;
    } else {
       TGo4QSettings::SetUserPath(settingsenv); //use specified settings path
    }
    if(!accountsettings) // want to have local settings
      {
      // need to create local settings subfolder if not exisiting, otherwise write will fail:
         QString qsubdir=TGo4QSettings::GetUserPath();
         if(gSystem->AccessPathName(qsubdir))
            {
               cout <<"Creating settings subdirectory "<<qsubdir.toStdString()<<" ..."<< endl;
               if(gSystem->mkdir(qsubdir,true)!=0)
                  cout <<"Could not create "<<qsubdir.toStdString()<<" !"<< endl;
            }
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
    Go4MainGUI->setGeometry (20, 20, 1152, 864);
    myapp.setMainWidget(Go4MainGUI);
    Go4MainGUI->polish();
    Go4MainGUI->show();
    myapp.connect( &myapp, SIGNAL( lastWindowClosed() ), &myapp, SLOT( quit() ) );
    QApplication::setDoubleClickInterval(400); //ms, for Qt>=3.3 avoid very fast defaults!
    QApplication::setStartDragTime(150); // ms
    if (hotstart.length()>0)
      Go4MainGUI->HotStart(hotstart.latin1());
    int res = myapp.exec();
    delete go4sett;
    return res;
}

