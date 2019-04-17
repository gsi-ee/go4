// Author: Sergey Linev, GSI   7/12/2016

/*************************************************************************
 * Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TWebGuiFactoryFull
#define ROOT_TWebGuiFactoryFull

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TWebGuiFactoryFull                                                   //
//                                                                      //
// This class is a proxy-factory for full-functional web canvas         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TWebGuiFactory.h"

class TWebGuiFactoryFull : public TWebGuiFactory {

public:
   TWebGuiFactoryFull();
   virtual ~TWebGuiFactoryFull() = default;

   virtual TCanvasImp *CreateCanvasImp(TCanvas *c, const char *title, UInt_t width, UInt_t height);
   virtual TCanvasImp *CreateCanvasImp(TCanvas *c, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height);

   ClassDef(TWebGuiFactoryFull,0)  //Factory for web-based ROOT GUI components
};


#endif
