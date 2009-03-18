#include "TGo4FolderProxy.h"

#include "TROOT.h"
#include "TClass.h"
#include "TTree.h"

#include "TGo4ObjectProxy.h"
#include "TGo4DirProxy.h"
#include "TGo4TreeProxy.h"
#include "TGo4CanvasProxy.h"
#include "TGo4Slot.h"


class TGo4FolderLevelIter : public TGo4LevelIter {
   public:
      TGo4FolderLevelIter(TFolder* folder) :
         TGo4LevelIter(), 
         fIter(0), 
         fCurrent(0)
      {
         fIter = folder->GetListOfFolders()->MakeIterator(); 
      }

      virtual ~TGo4FolderLevelIter()
      {
         delete fIter;
      }

      virtual Bool_t next()
      {
         fCurrent = fIter->Next();
         return (fCurrent!=0);
      }

      virtual Bool_t isfolder()
      {
         return (dynamic_cast<TFolder*>(fCurrent)!=0) ||
                (dynamic_cast<TDirectory*>(fCurrent)!=0) ||
                (dynamic_cast<TTree*>(fCurrent)!=0) ||
                (dynamic_cast<TCanvas*>(fCurrent)!=0);
      }
      
      virtual TGo4LevelIter* subiterator()
      {
         TTree* tr = dynamic_cast<TTree*>(fCurrent);
         if (tr!=0) return TGo4TreeProxy::ProduceIter(tr);
         TDirectory* dir = dynamic_cast<TDirectory*>(fCurrent);
         if (dir!=0) return TGo4DirProxy::ProduceIter(dir, kFALSE);
         TCanvas* canv = dynamic_cast<TCanvas*>(fCurrent);
         if (canv!=0) return TGo4CanvasProxy::ProduceIter(canv);
         return new TGo4FolderLevelIter((TFolder*)fCurrent);
      }
      
      virtual const char* name()
      {
         return fCurrent->GetName();
      }
 
      virtual const char* info()
      {
         return fCurrent->ClassName();
      }
 
      virtual Int_t GetKind()
      {
         return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject;
      }
 
      virtual const char* GetClassName()
      {
         return fCurrent->ClassName();
      }
 
   protected:
      TIterator*     fIter;     //!
      TObject*       fCurrent;  //!
};

// ****************************************************************

TGo4FolderProxy::TGo4FolderProxy() : 
   TGo4Proxy(),
   fFolder(0),
   fOwner(kFALSE),
   fRootFolderName()
{
}

TGo4FolderProxy::TGo4FolderProxy(TFolder* f, Bool_t owner, const char* rootfolder) :
   TGo4Proxy(),
   fFolder(f),
   fOwner(owner),
   fRootFolderName(rootfolder)
{
}

TGo4FolderProxy::~TGo4FolderProxy()
{
   if (fOwner) delete fFolder;  
}

Int_t TGo4FolderProxy::GetObjectKind()
{
   return (fFolder!=0) ? TGo4Access::kndFolder : TGo4Access::kndNone; 
}

const char* TGo4FolderProxy::GetContainedClassName()
{
   return fFolder ? fFolder->ClassName() : 0;
}

void TGo4FolderProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   if (!onlyobjs) { 
      const char* foldername = fRootFolderName.Length()>0 ? fRootFolderName.Data() : 0;
      slot->SetPar("FolderProxy::RootFolder", foldername);
   }
}

void TGo4FolderProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char* foldername = slot->GetPar("FolderProxy::RootFolder");
   
   fFolder = LocateROOTFolder(foldername);
   if (fFolder!=0) 
      fRootFolderName = foldername;
   fOwner = kFALSE;
}

TFolder* TGo4FolderProxy::LocateROOTFolder(const char* rootfolder)
{
   TFolder* res = 0;
   if (rootfolder!=0){ 
     if (strcmp(rootfolder,"//root/")==0) 
        res = gROOT->GetRootFolder();
     else  
       res = dynamic_cast<TFolder*> (gROOT->GetRootFolder()->FindObject(rootfolder));
   }
   return res;    
}


TGo4Access* TGo4FolderProxy::ProduceProxy(TFolder* folder, const char* name)
{
   if (folder==0) return 0; 
   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(folder);
   
   TFolder* curfold = folder;
   const char* curname = name;
      
   while (curfold!=0) { 
      const char* slash = strchr(curname,'/');
      UInt_t len = (slash!=0) ? slash - curname : strlen(curname);
      TIter iter(curfold->GetListOfFolders());
      TObject* obj = 0;
      while ((obj = iter())!=0) 
         if ((strlen(obj->GetName())==len) &&
             (strncmp(obj->GetName(), curname, len)==0)) break;
      if (obj==0) return 0;
      
      if (slash==0) return new TGo4ObjectAccess(obj);
      
      curname = slash+1;
            
      TTree* tr = dynamic_cast<TTree*> (obj);
      if (tr!=0)
         return TGo4TreeProxy::ProduceProxy(tr, curname);
         
      TDirectory* dir = dynamic_cast<TDirectory*> (obj);
      if (dir!=0)
         return TGo4DirProxy::ProduceProxy(dir, kFALSE, curname);
            
      TCanvas* canv = dynamic_cast<TCanvas*> (obj);
      if (canv!=0)
         return TGo4CanvasProxy::ProduceProxy(canv, curname);
            
      curfold = dynamic_cast<TFolder*>(obj);
   }
   
   return 0;
}

TGo4LevelIter* TGo4FolderProxy::ProduceIter(TFolder* folder)
{
   return new TGo4FolderLevelIter(folder); 
}
