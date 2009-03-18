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

#ifndef TQROOTGUIFACTORY_H
#define TQROOTGUIFACTORY_H

/**
*
*   TQRootGuiFactory
*   As TRootGuiFactory from the ROOT library, this
*   class uses the services of the general ABC TGuiFactory
*   in order to get Qt Native GUI components instead of
*   the ROOT ones. Basically it will overrides the
*   member function
*     @li TGuiFactory::CreateCanvasImp(TCanvas *c, const char *title,
*                                      UInt_t width, UInt_t height);
*     @li TGuiFactory::CreateCanvasImp(TCanvas *c, const char *title,
*                       Int_t x, Int_t y, UInt_t width, UInt_t height)
*
*
* @short Qt Factory GUI components
*
*
* Services:
* @li Creates a specific Canvas Implementation QCanvasImp
*
* @see TGuiFactory(ROOT), TQCanvasImp, TCanvas
*
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*
*/

#include "TRootGuiFactory.h"

class TCanvasImp;

class TQRootGuiFactory : public TRootGuiFactory {
   public:
      /** TQRootGuiFactory ctor */
      TQRootGuiFactory(const char *name = "QRoot", const char *title = "Qt/ROOT GUI Factory");

      virtual ~TQRootGuiFactory();

      /**  create a Qt native Gui of TCanvasImp
        *  @param TCanvas *c (ptr to ROOT TCanvas)
        *  @param char* title (title for canvas)
        *  @param width
        *  @param height
        *  @return QCanvasImp*  */
      TCanvasImp *CreateCanvasImp(TCanvas *c, const char *title, UInt_t width, UInt_t height);

      /** create a Qt native Gui of TCanvasImp
        * @param TCanvas *c (ptr to ROOT TCanvas)
        * @param char* title (title for canvas)
        * @param x
        * @param y
        * @param width
        * @param height
        * @return TQCanvasImp*  */
      TCanvasImp *CreateCanvasImp(TCanvas *c, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height);

      void setCustomFlag(bool custom) { fCustom=custom; }

   private:
      bool fCustom;
};

#endif

