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
protected:
   virtual Bool_t ProcessData(unsigned connid, const std::string &arg);

   virtual Bool_t DecodePadOptions(const char *);

   TPad *ProcessObjectOptions(TWebObjectOptions &item, TPad *pad);

public:
   TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);

   virtual Bool_t IsReadOnly() const { return kFALSE; }

   ClassDef(TWebCanvasFull, 0) // Full implementation of web-based canvas
};


#endif
