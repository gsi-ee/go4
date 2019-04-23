#ifndef ROOT_TWebCanvasFull
#define ROOT_TWebCanvasFull

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TWebCanvasFull                                                       //
//                                                                      //
// Complete implementation of TCanvasImp ABI,                           //
// including all kind of interactivity                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TWebCanvas.h"

#include "TWebPadOptions.h"

#include <vector>

class TWebCanvasFull : public TWebCanvas {
public:
   /// Function type for pad-related signals - like click or double click
   using PadSignal_t = std::function<void(TPad *)>;

   /// Function type for pad-click signals
   using PadClickedSignal_t = std::function<void(TPad *, int, int)>;

   /// Function type for signals, invoked when object is selected
   using ObjectSelectSignal_t = std::function<void(TPad *, TObject *)>;

protected:
   virtual Bool_t ProcessData(unsigned connid, const std::string &arg);

   virtual Bool_t DecodePadOptions(const char *);

   TPad *ProcessObjectOptions(TWebObjectOptions &item, TPad *pad);

   PadSignal_t fActivePadChangedSignal;     ///<! signal emitted when active pad changed in the canvas
   PadClickedSignal_t fPadClickedSignal;    ///<! signal emitted when simple mouse click performed on the pad
   PadClickedSignal_t fPadDblClickedSignal; ///<! signal emitted when simple mouse click performed on the pad
   ObjectSelectSignal_t fObjSelectSignal;   ///<! signal emitted when new object selected in the pad

public:
   TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);

   virtual Bool_t IsReadOnly() const { return kFALSE; }

   void SetActivePadChangedHandler(PadSignal_t func) { fActivePadChangedSignal = func; }

   void SetPadClickedHandler(PadClickedSignal_t func) { fPadClickedSignal = func; }

   void SetPadDblClickedHandler(PadClickedSignal_t func) { fPadDblClickedSignal = func; }

   void SetObjSelectHandler(ObjectSelectSignal_t func) { fObjSelectSignal = func; }

   ClassDef(TWebCanvasFull, 0) // Full implementation of web-based canvas
};


#endif
