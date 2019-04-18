#include "TWebCanvasFull.h"

TWebCanvasFull::TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height) :
   TWebCanvas(c,name,x,y,width,height)
{
   printf("CREATING FULL CANVAS\n");
}


Bool_t TWebCanvasFull::ProcessData(unsigned connid, const std::string &arg)
{
   if (TWebCanvas::ProcessData(connid, arg))
      return kTRUE;

   return kTRUE;
}
