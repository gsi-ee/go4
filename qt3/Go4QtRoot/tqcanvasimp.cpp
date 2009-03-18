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


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ABC describing GUI independent main window (with menubar, scrollbars //
// and a drawing area).                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "qapplication.h"
#include "tqcanvasimp.h"
#include "TClass.h"
#include "TRootCanvas.h"
#include "lockguard.h"

TQCanvasImp::TQCanvasImp(TCanvas *c, const char *name, UInt_t width, UInt_t height){

  build(c,name,10,10,width,height);
}


TQCanvasImp::TQCanvasImp(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height){

  build(c,name,x,y,width,height);

}

void TQCanvasImp::build(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height){
//Qtrootlockguard threadlock;
#if DEBUG_LEVEL
  qDebug(" canvas name %i %s %s \n",c,
          c->ClassName(), c->IsA()->GetTitle() );
#endif

      fdialbox = new QTFrame(qApp->mainWidget(),"test",Qt::WDestructiveClose);
      fQCanvas = new TQRootCanvas(fdialbox,name,c);
      fCanvas = fQCanvas->getCanvas();
      fdialbox->setCanvas(fQCanvas);
      //fdialbox->setCentralWidget( fQCanvas );
      fdialbox->move(x,y);
      fdialbox->resize(width,height);

}










