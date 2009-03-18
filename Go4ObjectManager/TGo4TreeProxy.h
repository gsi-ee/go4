#ifndef TGO4TREESLOT_H
#define TGO4TREESLOT_H

#include "TGo4Proxy.h"

class TTree;

class TGo4TreeProxy : public TGo4Proxy {
   public:
      TGo4TreeProxy();
      TGo4TreeProxy(TTree* tree, Bool_t owner = kFALSE);
      virtual ~TGo4TreeProxy();
      
      virtual Bool_t HasSublevels() const { return fTree!=0; }
      
      virtual TGo4LevelIter* MakeIter()
        { return (fTree==0) ? 0 : ProduceIter(fTree); }
        
      virtual TGo4Access* MakeProxy(const char* name) 
        { return ProduceProxy(fTree, name); }

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();
        
      static TGo4Access* ProduceProxy(TTree* tree, const char* name);
      static TGo4LevelIter* ProduceIter(TTree* tree);
   
   protected:
      TTree*   fTree;   //!
      Bool_t   fOwner;  //!
   
   ClassDef(TGo4TreeProxy, 1);
};

#endif


