#ifndef TGO4ROOTBROWSERPROXY_H
#define TGO4ROOTBROWSERPROXY_H

#include "TGo4Proxy.h"

class TGo4BrowserProxy;
class TGo4Picture;
class TGo4Condition;
class TPad;
class TCanvas;

class TGo4RootBrowserProxy : public TGo4Proxy {
   public:
      TGo4RootBrowserProxy(TGo4BrowserProxy* br = 0);

      virtual ~TGo4RootBrowserProxy();

      virtual Bool_t ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param);

      void UnblockStatusOutput();

      void DrawPicture(const char* itemname, TGo4Picture* pic, TPad* pad);
      void DrawCondition(const char* itemname, TGo4Condition* con);
      void DrawItem(const char* itemname);

      void Message(const char* str1, const char* str2, Int_t blockdelay);

   protected:

      static Int_t fCanvasCounter;

      void UpdateRatemeter(TObject* obj);
      void UpdateLoginfo(TObject* obj);

      TCanvas* MakeCanvas(const char* title = 0);

      void SyncRootBrowserSlots();

      TGo4BrowserProxy*  fBrowser; //!

      Bool_t  fLockMessage;  //!

   ClassDef(TGo4RootBrowserProxy, 1);

};

#endif
