// Author: Sergey Linev, GSI   7/12/2016

/*************************************************************************
 * Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TWebGuiFactory                                                       //
//                                                                      //
// This class is a factory for Web GUI components. It overrides         //
// the member functions of the abstract TGuiFactory.                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TWebGuiFactoryFull.h"

#include "TWebCanvasFull.h"

////////////////////////////////////////////////////////////////////////////////
/// TWebGuiFactory ctor.
/// Restore the right TVirtualX pointer

TWebGuiFactoryFull::TWebGuiFactoryFull() :
   TWebGuiFactory()
{
}

////////////////////////////////////////////////////////////////////////////////

TCanvasImp *TWebGuiFactoryFull::CreateCanvasImp(TCanvas *c, const char *title, UInt_t width, UInt_t height)
{
   return new TWebCanvasFull(c, title, 0, 0, width, height);
}

////////////////////////////////////////////////////////////////////////////////

TCanvasImp *TWebGuiFactoryFull::CreateCanvasImp(TCanvas *c, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height)
{
   return new TWebCanvasFull(c, title, x, y, width, height);
}
