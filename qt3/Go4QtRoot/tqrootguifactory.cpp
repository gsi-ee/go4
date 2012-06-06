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


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQRootGuiFactory                                                      //
//                                                                      //
// This class is a factory for Qt GUI components. It overrides          //
// the member functions of the ABS TGuiFactory.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "tqrootguifactory.h"

#include "go4iostream.h"

#include "TGuiFactory.h"
#include "TRootCanvas.h"
#include "tqcanvasimp.h"
#include "TGo4LockGuard.h"

//______________________________________________________________________________
TQRootGuiFactory::TQRootGuiFactory(const char *name, const char *title)
   : TRootGuiFactory(name, title)
{
   TGo4LockGuard threadlock(0,true);
  // TQRootGuiFactory ctor.
  // The default implementation  is not customized:
  // The  ROOT TRootCanvas class is being used
  fCustom=kFALSE;

}

//______________________________________________________________________________
TQRootGuiFactory::~TQRootGuiFactory()
{
   qDebug(" ~TQRootGuiFactory called \n");
}

//______________________________________________________________________________
TCanvasImp *TQRootGuiFactory::CreateCanvasImp(TCanvas *c, const char *title,
                                             UInt_t width, UInt_t height)
{
  // Create a ROOT native GUI version of TCanvasImp
TGo4LockGuard threadlock(0,true);
#if DEBUG_LEVEL>0
  qDebug("TQRootGuiFactory::CreateCanvasImp: \n creating a TCanvasImp with parameters:  %x name:%s \n w:%i h:%i \n",
        c, title, width, height );
#endif

  if ( fCustom ) {
    TQCanvasImp* cimp= new TQCanvasImp(c,title,width,height);
    return ( cimp );
  }else{
    return new TRootCanvas(c, title, width, height);
  }

}

//______________________________________________________________________________
TCanvasImp *TQRootGuiFactory::CreateCanvasImp(TCanvas *c, const char *title,
                                  Int_t x, Int_t y, UInt_t width, UInt_t height)
{
 // Create a ROOT native GUI version of TCanvasImp
TGo4LockGuard threadlock(0,true);
#if DEBUG_LEVEL>0
  qDebug("TQRootGuiFactory::CreateCanvasImp:\n creating a TCanvasImp with parameters:  %x name:%s  \n w:%i h:%i x:%i y:%i\n",  c, title, width, height, x, y );
#endif

  if ( fCustom ) {
    TQCanvasImp* cimp= new TQCanvasImp(c,title,x,y,width,height);
    return cimp ;
  }else{
    return new TRootCanvas(c, title, x, y, width, height);
  }

}



