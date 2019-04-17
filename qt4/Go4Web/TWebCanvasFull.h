#ifndef TWEBCANVASFULL_H
#define TWEBCANVASFULL_H

#include "TWebCanvas.h"

class TWebCanvasFull : public TWebCanvas {
public:
   TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
};


#endif
