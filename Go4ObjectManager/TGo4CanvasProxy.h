#ifndef TGO4CANVASPROXY_H
#define TGO4CANVASPROXY_H

#include "TGo4Proxy.h"

#include "TCanvas.h"

class TGo4CanvasProxy : public TGo4Proxy {
   public:
      TGo4CanvasProxy();
      TGo4CanvasProxy(TCanvas* c, Bool_t owner = kFALSE);
      virtual ~TGo4CanvasProxy();
      
      virtual Bool_t HasSublevels() const { return fCanvas!=0; }
      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);
      virtual Bool_t RemoveRegisteredObject(TObject* obj);
      
      virtual TGo4LevelIter* MakeIter()
        { return (fCanvas==0) ? 0 : ProduceIter(fCanvas); }
        
      virtual TGo4Access* MakeProxy(const char* name) 
        { return ProduceProxy(fCanvas, name); }
        
      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Bool_t IsAcceptObject(TClass* cl);
      virtual Bool_t AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner);
      virtual TObject* GetAssignedObject();
      
      static TGo4Access* ProduceProxy(TCanvas* canv, const char* name);
      static TGo4LevelIter* ProduceIter(TCanvas* canv);
      
   protected:
      TCanvas* fCanvas;          //!
      Bool_t   fOwner;           //!
   
   ClassDef(TGo4CanvasProxy, 1);
};


#endif
