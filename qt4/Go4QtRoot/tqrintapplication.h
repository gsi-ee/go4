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

#ifndef TQRINTApplication
#define TQRINTApplication


/**
*  TQRINTApplication
*
*  This class creates Root environement that will
*  interface with the Qt windowing system eventloop and eventhandlers.
*  This class will be instantiated ones (singleton) in a main()
*  program.
*
*  @short Root application interfaced with Qt
*
* @see TApplication
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*/

#include "tqapplication.h"
#include "TRint.h"

class TQRintApplication : public TRint {
protected:

  //   TQRintApplication():TRint(){ } // used by Dictionary()
   /**
   Here we overload the LoadGraphicsLibs() function.
   This function now just instantiates a QRootGuiFactory
   object and redirect the global pointer gGuiFactory to point
   to it.
    */
   virtual void LoadGraphicsLibs();

public:
   TQRintApplication(const char *appClassName, int *argc, char **argv,
          void *options = 0, int numOptions = 0, Bool_t noLogo=kTRUE);


   virtual ~TQRintApplication();

 protected:

};



#endif
