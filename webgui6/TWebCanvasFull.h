// Author: Sergey Linev, GSI   7/12/2016

/*************************************************************************
 * Copyright (C) 1995-2019, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TWebCanvasFull
#define ROOT_TWebCanvasFull

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TWebCanvasFull                                                       //
//                                                                      //
// Complete implementation of web-based TCanvasImp,                     //
// including all kind of interactivity                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TWebCanvas.h"

#include "TWebPadOptions.h"

class TObjLink;
class TPad;

class TWebCanvasFull : public TWebCanvas {
public:
   /// Function type for pad-related signals - like click or double click
   using PadSignal_t = std::function<void(TPad *)>;

   /// Function type for pad-click signals
   using PadClickedSignal_t = std::function<void(TPad *, int, int)>;

   /// Function type for signals, invoked when object is selected
   using ObjectSelectSignal_t = std::function<void(TPad *, TObject *)>;

protected:

   PadSignal_t fActivePadChangedSignal;     ///<! signal emitted when active pad changed in the canvas
   PadClickedSignal_t fPadClickedSignal;    ///<! signal emitted when simple mouse click performed on the pad
   PadClickedSignal_t fPadDblClickedSignal; ///<! signal emitted when simple mouse click performed on the pad
   ObjectSelectSignal_t fObjSelectSignal;   ///<! signal emitted when new object selected in the pad
   Bool_t fCanCreateObjects{kTRUE};         ///<! indicates if canvas allowed to create extra objects for interactive painting

   Bool_t ProcessData(unsigned connid, const std::string &arg) override;

   Bool_t DecodePadOptions(const char *) override;

   TPad *ProcessObjectOptions(TWebObjectOptions &item, TPad *pad);

   TObject *FindPrimitive(const std::string &id, TPad *pad = nullptr, TObjLink **padlnk = nullptr, TPad **objpad = nullptr);

   Bool_t CanCreateObject(const char *) override { return fCanCreateObjects; }

public:
   TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);

   Bool_t IsReadOnly() const override { return kFALSE; }

   void SetCanCreateObjects(Bool_t on = kTRUE) { fCanCreateObjects = on; }

   void SetActivePadChangedHandler(PadSignal_t func) { fActivePadChangedSignal = func; }

   void SetPadClickedHandler(PadClickedSignal_t func) { fPadClickedSignal = func; }

   void SetPadDblClickedHandler(PadClickedSignal_t func) { fPadDblClickedSignal = func; }

   void SetObjSelectHandler(ObjectSelectSignal_t func) { fObjSelectSignal = func; }

   ClassDefOverride(TWebCanvasFull, 0) // Web-based implementation for TCanvasImp, full interactive mode
};


#endif
