#include "TGo4FileStore.h"

#include "Riostream.h"

#include "TFolder.h"
#include "TFile.h"
#include "TTree.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4CompositeEvent.h"
#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4Fitter.h"
#include "TGo4FileStoreParameter.h"

const char* TGo4FileStore::fgcTREESUF = "xTree";
const char* TGo4FileStore::fgcFILESUF = ".root";
const char* TGo4FileStore::fgcEVBRANCHNAME = "Go4EventBranch.";
const Int_t TGo4FileStore::fgiFILESPLITSIZE = 1000000000;


TGo4FileStore::TGo4FileStore() :
   TGo4EventStore("Go4 Default File Store"),
   fbBranchExists(kFALSE), fxEvent(0),
   fiSplit(1),
   fiBufsize(0),
   fiFillCount(0)
{
   TRACE((15,"TGo4FileStore::TGo4FileStore()", __LINE__, __FILE__));
   // public default ctor for streamer
   fxFile=0;
   fxTree=0;
   TTree::SetMaxTreeSize(fgiFILESPLITSIZE);

}

TGo4FileStore::TGo4FileStore(const char* name,
                             Int_t splitlevel,
                             Int_t compression,
                              Bool_t overwrite,
                              Int_t autosavesize,
                              Int_t bufsize) :
   TGo4EventStore(name),
   fbBranchExists(kFALSE),
   fxEvent(0),
   fiSplit(splitlevel),
   fiBufsize(bufsize),
   fiFillCount(0)
{
   TRACE((15,"TGo4FileStore::TGo4FileStore(char*,...)", __LINE__, __FILE__));
   TTree::SetMaxTreeSize(fgiFILESPLITSIZE);
   TString buffer(name);
   if(strstr(buffer.Data(),fgcFILESUF)==0)
         buffer+=fgcFILESUF;
   if(overwrite)
      {
         fxFile = new TFile(buffer.Data(), "RECREATE");
         TGo4Log::Info("TGo4FileStore: Open file %s RECREATE", buffer.Data());
      }
   else
      {
         fxFile = new TFile(buffer.Data(), "UPDATE");
         TGo4Log::Info("TGo4FileStore: Open file %s UPDATE", buffer.Data());
      }
   fxFile->SetCompressionLevel(compression);

   // strip any path information from treename:
   const char* lastname=name;
   const char* oldname=name;
   lastname=strstr(oldname,"/");
   while(lastname!=0)
      {
         oldname=lastname+1;
         lastname=strstr(oldname,"/");
      }
   buffer = oldname;
   buffer += fgcTREESUF;
   fxTree= dynamic_cast<TTree*> (fxFile->Get(buffer.Data()));
   if(fxTree)
      {
         TGo4Log::Debug(" Tree %s has been found in file %s ",buffer.Data(), fxFile->GetName());
         fiFillCount=(Int_t) (fxTree->GetEntries());
      }
   else
      {
         fxTree = new TTree(buffer.Data(), "Go4FileStore");
         fxTree->SetAutoSave(autosavesize);
         fxTree->Write();
         TGo4Log::Debug(" Tree %s has been created in file %s ",buffer.Data(), fxFile->GetName());
      }
}


TGo4FileStore::TGo4FileStore(TGo4FileStoreParameter* par) :
   TGo4EventStore("dummy"),
   fbBranchExists(kFALSE),
   fxEvent(0),
   fiSplit(1),
   fiBufsize(0),
   fiFillCount(0)
{
   TRACE((15,"TGo4FileStore::TGo4FileStore(TGo4FileStoreParameter* par)", __LINE__, __FILE__));

   if (par==0) {
      TGo4Log::Error("TGo4FileStore::TGo4FileStore(.., TGo4FileStoreParameter* is not specified");
      return;
   }

   fiSplit = par->GetSplitlevel();
   fiBufsize = par->GetBufsize();

   TTree::SetMaxTreeSize(fgiFILESPLITSIZE);

   TString buffer = par->GetName();
   SetName(buffer.Data());
   if(!buffer.Contains(fgcFILESUF)) buffer.Append(fgcFILESUF);
   if(par->IsOverwriteMode()) {
      fxFile = new TFile(buffer.Data(), "RECREATE");
      TGo4Log::Info("TGo4FileStore: Open file %s RECREATE", buffer.Data());
   } else {
      fxFile = new TFile(buffer.Data(), "UPDATE");
      TGo4Log::Info("TGo4FileStore: Open file %s UPDATE", buffer.Data());
   }
   fxFile->SetCompressionLevel(par->GetCompression());

     // strip any path information from treename (could be identical with filename!)
   const char* lastname = par->GetTitle();
   const char* oldname=lastname;
   lastname=strstr(oldname,"/");
   while(lastname!=0) {
      oldname=lastname+1;
      lastname=strstr(oldname,"/");
   }


   buffer = oldname;
   buffer += fgcTREESUF;
   fxTree = dynamic_cast<TTree*> (fxFile->Get(buffer.Data()));
   if(fxTree) {
      TGo4Log::Debug(" Tree %s has been found in file %s ", buffer.Data(), fxFile->GetName());
      fiFillCount= (Int_t) (fxTree->GetEntries());
   } else {
      fxTree = new TTree(buffer.Data(), "Go4FileStore");
      fxTree->SetAutoSave(par->GetAutosaveSize());
      fxTree->Write();
      TGo4Log::Debug(" Tree %s has been created in file %s ",buffer.Data(), fxFile->GetName());
   }
}


TGo4FileStore::~TGo4FileStore()
{
   TRACE((15,"TGo4FileStore::~TGo4FileStore()", __LINE__, __FILE__));
   if(fxFile)
      {
         fxFile=fxTree->GetCurrentFile(); // for file split after 1.8 Gb!
         fxFile->cd();
         fxTree->Write(0, TObject::kOverwrite);
         delete fxFile; // closes File, fxTree is removed from memory then
      }
   else
      { }
}




void TGo4FileStore::SetAutoSave(Int_t bytesinterval)
{
   TRACE((15,"TGo4FileStore::SetAutoSave(Int_t)", __LINE__, __FILE__));
   fxTree->SetAutoSave(bytesinterval);
}
void TGo4FileStore::SetCompression(Int_t comp)
{
   TRACE((12,"TGo4FileStore::SetCompression(Int_t)", __LINE__, __FILE__));
   fxFile->SetCompressionLevel(comp);
}

Int_t TGo4FileStore::Store(TGo4EventElement* event)
{
   TRACE((12,"TGo4FileStore::Store(TGo4EventElement*)", __LINE__, __FILE__));

   Int_t rev=-1;
   TGo4EventElement* lastevent= fxEvent; // keep pointer to last event
   fxEvent=event; // address of next event into event pointer
   if(!fbBranchExists) {
      // first call of Store, create new branch
      //cout << "**********************Creating new branch!"<< endl;
      if(fxEvent) {
         TString topbranchname = Form("%s.", fxEvent->GetName());
         TBranch*  go4branch = fxTree->GetBranch(topbranchname.Data());
         if(go4branch) {
            // tree already had branch of our name, check it
            TGo4Log::Debug(" FileStore: Found existing branch %s , continue filling ", fgcEVBRANCHNAME);
            // here we might check the classname of the stored events inbranch
            go4branch->SetAddress(&fxEvent);
            fbBranchExists=kTRUE;
         } else {
            // no such branch existing, create a new one
            TBranch *topbranch=
               fxTree->Branch(topbranchname.Data(), fxEvent->ClassName(), &fxEvent, fiBufsize, fiSplit);
            TGo4Log::Debug(" FileStore: Created new branch %s ", topbranchname.Data());
            fbBranchExists=kTRUE;
            if (fxEvent->InheritsFrom(TGo4CompositeEvent::Class()))
               dynamic_cast<TGo4CompositeEvent*>  (fxEvent)->makeBranch(topbranch);
         } // if(go4branch)
         lastevent=event; // this is our first event ever...
      }
      else
      {
         // this is an error....
         rev=1;
      }
   } // if(!fbEventBranchExists)
   else
   {
      // need not to create a branch, use existing one
   }
   // check if new event is same class as previous ones
   //   if(lastevent && !strcmp( event->ClassName(), lastevent->ClassName() ) )
   //      {
   // event class is matching, we fill it into our tree
   fxTree->Fill();
   fiFillCount++;
   rev=0;
   //      }
   //   else
   //      {
   //         // new event does not match the old ones, we ignore it
   //         fxEvent=lastevent; // restore old event pointer for next cycle
   //         rev=1;
   //      }
   return rev;
}

Int_t TGo4FileStore::Store(TGo4Parameter* cali)
{
   WriteToStore(cali);
   return 0;
}

Int_t TGo4FileStore::Store(TGo4Condition* conny)
{
   WriteToStore(conny);
   return 0;
}

Int_t TGo4FileStore::Store(TGo4Fitter* fitter)
{
   WriteToStore(fitter);
   return 0;
}

Int_t TGo4FileStore::Store(TFolder* fold)
{
   WriteToStore(fold);
   return 0;
}



void TGo4FileStore::WriteToStore(TNamed* ob)
{
   if (ob==0) return;

   TDirectory* dsav=gDirectory;
   TString oldname = ob->GetName();
   ob->SetName(Form("%s_%d" , oldname, fiFillCount));
   if(fxTree) fxFile=fxTree->GetCurrentFile();
   fxFile->cd();
   ob->Write(0, TObject::kOverwrite);
   ob->SetName(oldname.Data());
   dsav->cd();
}


