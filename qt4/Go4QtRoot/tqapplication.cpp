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
// TQApplication                                                        //
//                                                                      //
// This class create the ROOT native GUI version of the ROOT            //
// application environment. This in contrast to the Win32 version.      //
// Once the native widgets work on Win32 this class can be folded into  //
// the TApplication class (since all graphic will go via TVirtualX).    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "tqapplication.h"

#include "Riostream.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TGX11.h"

#include <QtCore/qglobal.h>

#include "tqrootguifactory.h"

//______________________________________________________________________________
TQApplication::TQApplication() :
   TApplication()
{
}


//______________________________________________________________________________
TQApplication::TQApplication(const char *appClassName,
                             Int_t *argc, char **argv, void *options, Int_t numOptions)
   : TApplication(appClassName,argc,argv,options,numOptions)
{
  //qDebug(" TQApplication:creating a qtroot environment\n");
  customized=kFALSE;
  LoadGraphicsLibs();
}

//______________________________________________________________________________
TQApplication::~TQApplication()
{

   // Delete ROOT application environment.
   qDebug("~TQApplication called \n");
   if(gApplication)  gApplication->Terminate(0);
}


//______________________________________________________________________________
void TQApplication::LoadGraphicsLibs()
{
   //*** No Win32 Gui Factory implemented
   if (gROOT->IsBatch()) return;
   // loading Canvas and Setting the global GUI Factory ptr
   gROOT->LoadClass("TCanvas", "Gpad");
   // gROOT->LoadClass("TGX11", "GX11");  // implicitely loads X11 and Xpm
   // gROOT->ProcessLineFast("new TGX11(\"X11\", \"ROOT interface to X11\");");
   gGuiFactory =  new TQRootGuiFactory();
}

//______________________________________________________________________________
void TQApplication::setCustomized()
{
  customized=kTRUE;
  if (customized)
    ((TQRootGuiFactory*) gGuiFactory)->setCustomFlag(kTRUE);
}



