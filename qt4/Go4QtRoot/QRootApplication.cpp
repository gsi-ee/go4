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

//Author : Denis Bertini 01.11.2000

/**************************************************************************
* Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI           *
*                     Planckstr. 1, 64291 Darmstadt, Germany              *
*                     All rights reserved.                                *
* Contact:            http://go4.gsi.de                                   *
*                                                                         *
* This software can be used under the license agreements as stated in     *
* Go4License.txt file which is part of the distribution.                  *
***************************************************************************/

#include "QRootApplication.h"

#include <stdlib.h>

#include "Riostream.h"
#include "TTimer.h"
#include "TSystem.h"

#include <QtCore/QObject>
#include <QtCore/QTimer>

#ifndef WIN32

#include "TGX11.h"

static int qt_x11_errhandler( Display *dpy, XErrorEvent *err )
{
  // special for modality usage: XGetWindowProperty + XQueryTree()
  if ( err->error_code == BadWindow ) {
    //if ( err->request_code == 25 && qt_xdnd_handle_badwindow() )
      return 0;
  }
  //  special case for  X_SetInputFocus
  else if ( err->error_code == BadMatch
       && err->request_code == 42  ) {
    return 0;
  }
  // here XError are forwarded
  char errstr[256];
  XGetErrorText( dpy, err->error_code, errstr, 256 );
  qWarning( "X11 Error: %s %d\n  Major opcode:  %d",
             errstr, err->error_code, err->request_code );
  return 0;
}

#endif

bool QRootApplication::fDebug = false;
bool QRootApplication::fWarning = false;
bool QRootApplication::fRootCanvasMenusEnabled = true;

void qMessageOutput( QtMsgType type, const char *msg )
{
   switch ( type ) {
      case QtDebugMsg:
         if(QRootApplication::fDebug)
            std::cerr << "QtRoot-Debug: " << msg << std::endl;
         break;
      case QtWarningMsg:
         if(QRootApplication::fWarning)
            std::cerr << "QtRoot-Warning: " << msg << std::endl;
         break;
      case QtFatalMsg:
          std::cerr << "QtRoot-Fatal: " << msg << std::endl;
          gSystem->Abort();
      case QtCriticalMsg:
          std::cerr << "QtRoot-Critical: " << msg << std::endl;
          break;
      default:
          std::cerr << "QtRoot-Other: " << msg << std::endl;
          break;
   }
}


QRootApplication::QRootApplication(int& argc, char **argv, int poll)
   :QApplication(argc,argv, true)
   {

  // connect ROOT via Timer call back
  if (poll == 0){
    timer = new QTimer( this );
    QObject::connect( timer, SIGNAL(timeout()),
            this, SLOT(execute()) );
    timer->setSingleShot(false);
    timer->start(20);

    rtimer = new TTimer(20);
    rtimer->Start(20, kFALSE);

  }

  // install a msg-handler
  fWarning = fDebug = false;

  qInstallMsgHandler( qMessageOutput );

  // install a filter on the parent

//   QApplication::installEventFilter( this );

   // install a filter on the parent   // M.Al-Turany

  // use Qt-specific XError Handler (moved this call here from tqapplication JA)

  const char* env = gSystem->Getenv("ROOT_CANVAS");
  int flag = 0;
  if (env!=0) {
     if ((strcmp(env,"yes")==0) || (strcmp(env,"YES")==0)) flag = 1; else
     if ((strcmp(env,"no")==0) || (strcmp(env,"NO")==0)) flag = -1;
  }

#ifdef WIN32
  // under Windows one should explicit enable these methods
  fRootCanvasMenusEnabled = (flag == 1);
#else
   XSetErrorHandler( qt_x11_errhandler );
   // under Unix one should explicit disable these methods
   fRootCanvasMenusEnabled = (flag != -1);
#endif
}

QRootApplication::~QRootApplication()
{
}

void QRootApplication::execute()
//call the inner loop of ROOT
{
   gSystem->InnerLoop();
}

void QRootApplication::quit()
{
   std::cout <<"QRootApplication::quit()" << std::endl;
//   gSystem->Exit( 0 );
}

bool QRootApplication::IsRootCanvasMenuEnabled()
{
   return fRootCanvasMenusEnabled;
}
