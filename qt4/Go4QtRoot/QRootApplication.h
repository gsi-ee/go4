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

#ifndef QROOTAPPLICATION_H
#define QROOTAPPLICATION_H

/**
*  QRootApplication
*
*  This class creates Qt environment that will
*  interface with the ROOT windowing system eventloop and eventhandlers.
*  via a polling mechanism.
*
*  @short Root application interfaced with Qt
*
* @see QApplication
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*/

#include <QtGui/QApplication>

class QTimer;
class TTimer;

class QRootApplication : public QApplication {
   Q_OBJECT

   public:
      QRootApplication(int& argc, char **argv, int poll=0);
      virtual ~QRootApplication();

      void setDebugOn() { fDebug = true; }
      void setWarningOn() { fWarning = true; }

      static bool fDebug;
      static bool fWarning;

      static bool IsRootCanvasMenuEnabled();

   public slots:

     void execute();
     void quit();

   protected:
     QTimer*   timer;
     TTimer*   rtimer;

     /** Variable defines if menu methods, which creating new canvas, are enabled.
      *  Introduced for win32 version, where sometime new Canvas causes crash :(
      */
     static bool fRootCanvasMenusEnabled;
};

#endif
