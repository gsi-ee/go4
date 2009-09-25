#include "TGo4TreeStore.h"

#include "TFile.h"
#include "TTree.h"

#include "TGo4Log.h"
#include "TGo4EventElement.h"
#include "TGo4EventCalibration.h"
#include "TGo4Parameter.h"
#include "TGo4TreeStoreParameter.h"
#include "TGo4MainTree.h"

const char* TGo4TreeStore::fgcFILESUF = ".root";

TGo4TreeStore::TGo4TreeStore(const char* name,
                             TGo4EventElement* event,
                             Int_t splitlevel,
                             Int_t bufsize,
                             const char* filename,
                             Int_t compression) :
   TGo4EventStore(name),
   fxBranch(0), fxEvent(event), fiSplit(splitlevel), fiBufsize(bufsize)
{
   TRACE((15,"TGo4TreeStore::TGo4TreeStore(const char*,...)", __LINE__, __FILE__));

   TString buffer = Form("%s.", name);  // for branches containing the same event structure
   fxSingletonTree = TGo4MainTree::Instance();
   if(fxEvent) {
      fcEventClass = fxEvent->ClassName();
      fxTree=fxSingletonTree->GetTree();
      fxBranch = fxTree->GetBranch(buffer.Data());
      if(fxBranch) {
         // tree already had branch of our name, check it
         TGo4Log::Debug(" TreeStore: Found existing branch %s , reset it ", name);
         fxBranch->Reset();
      } else {
         // no such branch existing, create a new one
         fxBranch = fxTree->Branch(buffer, fcEventClass.Data(), &fxEvent, fiBufsize, fiSplit);
         TGo4Log::Debug(" TreeStore: Created new branch %s ", name);
      } // if(fxBranch)
      if(filename) {
         buffer = Form("%s%s", filename, fgcFILESUF);
         fxFile = new TFile(buffer.Data(), "UPDATE");
         fxFile->SetCompressionLevel(compression);
         fxBranch->SetFile(fxFile);
         TGo4Log::Debug(" TreeStore: Set branch to file %s ", filename);
         TGo4Log::Info("TGo4TreeStore: Open file tree %s UPDATE", buffer.Data());
      } else {
         fxFile=0;
      } //  if(filename)
   }
   else
   {
      TGo4Log::Debug(" !!! TreeStore creation ERROR: no event structure !!! ");
   } // if(fxEvent)
}

TGo4TreeStore::TGo4TreeStore(TGo4TreeStoreParameter* par, TGo4EventElement* event)
: TGo4EventStore("dummy"),
   fxBranch(0), fxEvent(event), fiSplit(par->fiSplit), fiBufsize(par->fiBufsize)
{
   TRACE((15,"TGo4TreeStore::TGo4TreeStore(const char*,...)", __LINE__, __FILE__));

   SetName(par->GetName());
   TString buffer = Form("%s.", par->GetName());
   fxSingletonTree = TGo4MainTree::Instance();
   if(fxEvent) {
      fcEventClass = fxEvent->ClassName();
      fxTree=fxSingletonTree->GetTree();
      fxBranch= fxTree->GetBranch(buffer.Data());
      if(fxBranch) {
         // tree already had branch of our name, check it
         TGo4Log::Debug(" TreeStore: Found existing branch %s , reset it ", buffer.Data());
         fxBranch->Reset();
      } else {
         // no such branch existing, create a new one
         fxBranch = fxTree->Branch(buffer.Data(), fcEventClass.Data(), &fxEvent, fiBufsize, fiSplit);
         TGo4Log::Debug(" TreeStore: Created new branch %s ", buffer.Data());
      } // if(fxBranch)
      if(!par->fxBranchFile.IsNull())
      {
         buffer = Form("%s%s", par->fxBranchFile.Data(), fgcFILESUF);
         fxFile = new TFile(buffer.Data(), "UPDATE");
         fxFile->SetCompressionLevel(par->fiCompression);
         fxBranch->SetFile(fxFile);
         TGo4Log::Debug(" TreeStore: Set branch to file %s ", buffer.Data());
         TGo4Log::Info("TGo4TreeStore: Open file tree %s UPDATE", buffer.Data());
      }
      else
      {
         fxFile=0;
      } //  if(filename)

   }
   else
   {
      TGo4Log::Debug(" !!! TreeStore creation ERROR: no event structure !!! ");
   } // if(fxEvent)
}

TGo4TreeStore::TGo4TreeStore()
:TGo4EventStore("Go4 Default Tree Store"),
   fxBranch(0), fxEvent(0), fiSplit(0), fiBufsize(0)
{
   TRACE((15,"TGo4TreeStore::TGo4TreeStore()", __LINE__, __FILE__));
   // public default ctor for streamer
}


TGo4TreeStore::~TGo4TreeStore()
{
   TRACE((15,"TGo4TreeStore::~TGo4TreeStore()", __LINE__, __FILE__));
   //delete fxFile; // closes File <- done by maintree dtor
}



Int_t TGo4TreeStore::Store(TGo4EventElement* event)
{
   TRACE((12,"TGo4TreeStore::Store(TGo4EventElement*)", __LINE__, __FILE__));

   Int_t rev=-1;
   // check if new event is of the right class
   if(fcEventClass == event->ClassName()) {
      // event class is matching, we fill it into our tree
      fxEvent = event;
      fxBranch->SetAddress(&fxEvent);
      fxBranch->Fill();
      rev=0;
   } else {
      // new event does not match the old ones, we ignore it
      TGo4Log::Debug(" !!! TreeStore::Store ERROR: this event does not match !!! ");
      fxEvent->Clear();
      fxEvent->SetValid(kFALSE);
      fxBranch->SetAddress(&fxEvent);
      fxBranch->Fill(); // we fill dummy event for consistency
      rev=1;
   }
   //fxBranch->Write();
   return rev;
}


Int_t TGo4TreeStore::Store(TGo4Parameter* cali)
{
   TRACE((12,"TGo4TreeStore::Store(TGo4EventCalibration*)", __LINE__, __FILE__));
   if(cali) {
      TString oldname = cali->GetName();
      cali->SetName(Form("%s_%d", oldname.Data(), fxSingletonTree->GetCurrentIndex()));
      fxSingletonTree->fxFile->cd(); // we are main tree's friend
      cali->Write(0, TObject::kOverwrite);
      cali->SetName(oldname.Data());
   }

   return 0;
}

void TGo4TreeStore::SetCompression(Int_t comp)
{
   TRACE((12,"TGo4TreeStore::SetCompression(Int_t)", __LINE__, __FILE__));
   fxFile->SetCompressionLevel(comp);
}
