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

#include "qrootapplication.h"

#include "go4iostream.h"
#include <stdlib.h>
#include "TTimer.h"
#include "TSystem.h"
#include "TGX11.h"

#include "qobject.h"
#include "qtimer.h"

#include "TGo4LockGuard.h"

extern void qt_ignore_badwindow();
extern bool qt_badwindow();
extern bool qt_xdnd_handle_badwindow();

static  int qt_x11_errhandler( Display *dpy, XErrorEvent *err ){
  // special for modality usage: XGetWindowProperty + XQueryTree()
  if ( err->error_code == BadWindow ) {
    if ( err->request_code == 25 && qt_xdnd_handle_badwindow() )
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


bool QRootApplication::fDebug=false;
bool QRootApplication::fWarning=false;

void qMessageOutput( QtMsgType type, const char *msg )
    {
        switch ( type ) {
            case QtDebugMsg:
             if(QRootApplication::fDebug)
                fprintf( stderr, "QtRoot-Debug: \n %s\n", msg );
                break;
            case QtWarningMsg:
             if(QRootApplication::fWarning)
                fprintf( stderr, "QtRoot-Warning: \n %s\n", msg );
                break;
            case QtFatalMsg:
                fprintf( stderr, "QtRoot-Fatal: \n %s\n", msg );
                abort();         // dump core on purpose
        }
   }


QRootApplication::QRootApplication(int& argc, char **argv, int poll) :
   QApplication(argc,argv, true)
{

  // connect ROOT via Timer call back


  if (poll == 0){
    timer = new QTimer( this );
    QObject::connect( timer, SIGNAL(timeout()),
            this, SLOT(execute()) );
    timer->start( 20, FALSE );

    rtimer = new TTimer(20);
    rtimer->Start(20, kFALSE);

  }


  // install a msg-handler
  fWarning=fDebug=false;
  qInstallMsgHandler( qMessageOutput );

  // install a filter on the parent

//   QApplication::installEventFilter( this );

   // install a filter on the parent   // M.Al-Turany

  // use Qt-specific XError Handler (moved this call here from tqapplication JA)
   setErrorHandler();
}

/*
// Custom eventFilter   // M.Al-Turany
bool QRootApplication::eventFilter(QObject *o,QEvent *e){
TGo4LockGuard Global;
   //    if(e->type()== QEvent::Paint) {
   cout << "QRootApplication::eventFilter" <<endl;
   //      return QApplication::eventFilter( o, e );
      //  } else {
            // standard event processing
            return QApplication::eventFilter( o, e );
      //}

}

*/
QRootApplication::~QRootApplication()
{
}

void QRootApplication::execute()
//call the inner loop of ROOT
{
   gSystem->InnerLoop();
}

void QRootApplication::setErrorHandler()
// set a Qt-Specific error Handler
{
   XSetErrorHandler( qt_x11_errhandler );
}

void QRootApplication::quit()
{
   gSystem->Exit( 0 );
}
