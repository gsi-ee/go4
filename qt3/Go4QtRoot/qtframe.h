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

#ifndef QTFRAME_H
#define QTFRAME_H

#include "qmainwindow.h"

class TQRootCanvas;


/**
*
*           Main Qt&Root Application Frame Window:
*
* Can be used internaly by ROOT itself if the user wants
* a customize Qt-based Canvas implementation as defaults
* @short Simple example using
*        QMainWindow together with an TQRootCanvas
*
*
* Services:
* Simple example
* @li Creates a Menu for file actions (save, browse, close, ect...)
* @li Creates a Toolbar with actions managed by QAction class
* @li Creates a QRootCanvas and stick it  as central Widget in QMainWindows
* @li Actions ( execute(), clear()) are defined as slots managed by this
*              QMainApplication. This slots functions acts on the ROOT Objects
*               directly ( histograms, ntuples, ect...)
*
* @see QRootCanvas, QMainWindow, QPopupMenu, QAction, QString
*
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*
*/

class QTFrame: public QMainWindow {
   Q_OBJECT

   public:
      QTFrame(QWidget * parent=0, const char * name=0, WFlags f=WType_TopLevel);
      virtual ~QTFrame();
      void setCanvas(TQRootCanvas* a);

   protected:
      virtual void closeEvent(QCloseEvent* ce);

   private:
      TQRootCanvas *aCanvas;
};

#endif
