#include "TWebCanvasFull.h"

#include "TWebMenuItem.h"
#include "TCanvas.h"

TWebCanvasFull::TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height) :
   TWebCanvas(c,name,x,y,width,height)
{
   printf("CREATING FULL CANVAS\n");
}


Bool_t TWebCanvasFull::ProcessData(unsigned connid, const std::string &arg)
{
   if (TWebCanvas::ProcessData(connid, arg))
      return kTRUE;

   if (arg.compare(0, 8, "GETMENU:") == 0) {

      TObject *obj = FindPrimitive(arg.c_str() + 8);
      if (!obj)
         obj = Canvas();

      TWebMenuItems items;
      items.PopulateObjectMenu(obj, obj->IsA());
      std::string buf = "MENU:";
      buf.append(arg.c_str() + 8);
      buf.append(":");
      buf.append(items.ProduceJSON().Data());

      AddToSendQueue(connid, buf);
   }


   return kTRUE;
}
