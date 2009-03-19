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
// QRintApplication                                                     //
//                                                                      //
// This class create the ROOT native GUI version of the ROOT            //
// application environment. This in contrast to the Win32 version.      //
// Once the native widgets work on Win32 this class can be folded into  //
// the TApplication class (since all graphic will go via TVirtualX).    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TApplication.h"
#include "TUnixSystem.h"
#include "qrintapplication.h"
#include "qrootguifactory.h"
#include "Riostream.h"

#include "TROOT.h"
#include "TClass.h"
#include "Getline.h"
#include "TStyle.h"
#include "TObjectTable.h"
#include "TClassTable.h"
#include "TStopwatch.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TSysEvtHandler.h"
#include "TError.h"
#include "TException.h"
#include "TInterpreter.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TFile.h"
#include "TMapFile.h"
#include "TTabCom.h"



//______________________________________________________________________________
QRintApplication::QRintApplication(const char *appClassName,
                                   Int_t *argc, char **argv, void *options, Int_t numOptions, Bool_t noLogo)
   : TRint(appClassName,argc,argv,options,numOptions,noLogo)
{
  qDebug(" creating a qrint app \n");

 LoadGraphicsLibs();


  Getlinem(kInit, GetPrompt());

   // Process shell command line input files
   if (InputFiles()) {
      TObjString *file;
      TIter next(InputFiles());
      RETRY {
         while ((file = (TObjString *)next())) {
            char cmd[256];
            if (file->String().EndsWith(".root")) {
               const char *rfile = (const char*)file->String();
               Printf("\nAttaching file %s...", rfile);
               char *base = StrDup(gSystem->BaseName(rfile));
               char *s = strchr(base, '.'); *s = 0;
               snprintf(cmd, 255, "TFile *%s = TFile::Open(\"%s\")", base, rfile);
               delete [] base;
            } else {
               Printf("\nProcessing %s...", (const char*)file->String());
               snprintf(cmd, 255, ".x %s", (const char*)file->String());
            }
            Getlinem(kCleanUp, 0);
            Gl_histadd(cmd);
            fNcmd++;
            ProcessLine(cmd);
         }
      } ENDTRY;

      if (QuitOpt())
         Terminate(0);

      ClearInputFiles();

      Getlinem(kInit, GetPrompt());
   }





}

//______________________________________________________________________________
QRintApplication::~QRintApplication()
{

   // Delete ROOT application environment.
      printf("~QRinttApplication \n");
   //  if(gApplication)  gApplication->Terminate(0);

}




void QRintApplication::LoadGraphicsLibs()
{
   //*** No Win32 Gui Factory implemented
   if (gROOT->IsBatch()) return;

   // loading Canvas and Setting the global GUI Factory ptr
   gROOT->LoadClass("TCanvas", "Gpad");
   // gROOT->LoadClass("TGX11", "GX11");  // implicitely loads X11 and Xpm
   // gROOT->ProcessLineFast("new TGX11(\"X11\", \"ROOT interface to X11\");");
   gGuiFactory =  new QRootGuiFactory();

}



