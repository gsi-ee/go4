#ifndef TGO4DIRPROXY_H
#define TGO4DIRPROXY_H

#include "TGo4Proxy.h"

class TFile;
class TDirectory;

class TGo4DirProxy : public TGo4Proxy {
   public:
      TGo4DirProxy();
      TGo4DirProxy(TDirectory* dir, Bool_t readright, Bool_t owner);
      virtual ~TGo4DirProxy();

      virtual Bool_t HasSublevels() const { return fDir!=0; }

      virtual TGo4LevelIter* MakeIter()
        { return (fDir==0) ? 0 : ProduceIter(fDir, fReadRight); }

      virtual TGo4Access* MakeProxy(const char* name)
        { return ProduceProxy(fDir, fReadRight, name); }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();
      virtual const char* GetContainedObjectInfo();
      virtual Int_t GetObjectSizeInfo();

      static TGo4Access* ProduceProxy(TDirectory* dir, Bool_t readright, const char* name);
      static TGo4LevelIter* ProduceIter(TDirectory* dir, Bool_t readright);

      Bool_t UpdateObjectInFile(const char* filepath, TObject* obj);

      Bool_t IsFile() const;
      const char* GetFileName() const;

   protected:
      void SetDir(TDirectory* dir, Bool_t readright, Bool_t owner);
      void ClearDir();

      TDirectory*  fDir;          //!
      Bool_t       fOwner;        //!
      Bool_t       fReadRight;    //!

   ClassDef(TGo4DirProxy, 1);
};

#endif
