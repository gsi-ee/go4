// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4FileSource.h"

#include "Riostream.h"

#include "TKey.h"
#include "TFile.h"
#include "TTree.h"

#include "TGo4FileSourceParameter.h"
#include "TGo4CompositeEvent.h"
// get static name constants from here
#include "TGo4FileStore.h"

R__EXTERN TTree *gTree;

TGo4FileSource::TGo4FileSource(const char* name) :
   TGo4EventSource(name), fxFile(0), fxTree(0), fiMaxEvents(0), fiCurrentEvent(0),fbActivated(kFALSE),
   fxTopEvent(0)
{
   Open();
}

TGo4FileSource::TGo4FileSource(TGo4FileSourceParameter* par) :
   TGo4EventSource(par->GetName()), fxFile(0), fxTree(0), fiMaxEvents(0),
   fiCurrentEvent(0), fbActivated(kFALSE),
   fxTopEvent(0)
{
   Open();
}


TGo4FileSource::TGo4FileSource() :
   TGo4EventSource("Go4FileSource"), fxFile(0), fxTree(0), fiMaxEvents(0),
   fiCurrentEvent(0), fbActivated(kFALSE),
   fxTopEvent(0)
{
   // for streamer, do not open here!
}

Int_t TGo4FileSource::Open()
{
   TString buffer(GetName());
   if(strstr(buffer.Data(),TGo4FileStore::fgcFILESUF)==0)
      buffer+=TGo4FileStore::fgcFILESUF;
   fxFile = TFile::Open(buffer.Data(), "READ"); // in such way rfio etc is also supported!
   if(! ( fxFile && fxFile->IsOpen() )) ThrowError(66,0,"!!! ERROR: FILE not found !!!");

   TKey* kee=0;
   TIter iter(fxFile->GetListOfKeys());
   while ( ( kee=dynamic_cast<TKey*>(iter()) ) !=0 ) {
      fxTree = dynamic_cast<TTree*>(kee->ReadObj());
      if (fxTree)
         break; // we take first Tree in file, disregarding its name...
   }
   if (fxTree==0) {
      ThrowError(77,0,"!!! ERROR: Tree not found !!!");
   } else {
      SetCreateStatus(0);
      fiMaxEvents= (Int_t )fxTree->GetEntries();
   }
   TGo4Log::Info("TGo4FileSource: Open file %s", buffer.Data());

   return 0;
}

TGo4FileSource::~TGo4FileSource()
{
   delete fxFile;
}

Bool_t TGo4FileSource::BuildEvent(TGo4EventElement* dest)
{
   Bool_t rev=kTRUE;
   if(dest==0) ThrowError(0,22,"!!! ERROR BuildEvent: no destination event!!!");
   if (fxTree==0) ThrowError(0,33,"!!! ERROR BuildEvent: no Tree !!!");
   if(!fbActivated)
      {
         // Event dest should be full event as filled into the tree
         // the name of the event element may indicate the subpart
         //(tree branchname) that should be read partially only
         TString topname;
         Bool_t masterbranch=kFALSE;
         fxBranchName=dest->GetName();
         if(!fxBranchName.Contains("."))
            {
               fxBranchName+="."; // for master branch, add dot. Subbranch names with dot separators do not get final dot
               masterbranch=kTRUE;
            }
         TObjArray* blist = fxTree->GetListOfBranches();
         TBranch* topb= (TBranch*) blist->At(0);
         if(topb)
            {
               topname=topb->GetName();
               //cout <<"Activating top branch "<<topname.Data() << endl;
               fxTopEvent=dest;
               fxTree->SetBranchAddress(topname.Data(),&fxTopEvent);
               //topb->SetAddress(&fxTopEvent); // this will not set address of possible cloned tree. we use the set address of the tree
             }
         fxTree->SetBranchStatus("*",0); // note: only deactivate subleafs _after_ address of top branch is set!
         fxTree->SetBranchStatus(topname.Data(),1); // required to process any of the subbranches!
         TString wildbranch=fxBranchName;
         wildbranch+="*";
         fxTree->SetBranchStatus(wildbranch.Data(),1);
         //cout <<"Build event activates: "<<wildbranch.Data() << endl;
         wildbranch=fxBranchName;
         if(!masterbranch) wildbranch+=".";
         wildbranch+="*";
         fxTree->SetBranchStatus(wildbranch.Data(),1);
         //cout <<"Build event activates: "<<wildbranch.Data() << endl;
         fbActivated=kTRUE;
      }  //  if(!fbActivated)
   // end init section ////////////
   fxTree->GetEntry(fiCurrentEvent++);
   if(fiCurrentEvent>fiMaxEvents) ThrowEOF(0,44,"End of tree at event %d !!!",fiCurrentEvent);
   return rev;
}

Bool_t TGo4FileSource::BuildCompositeEvent(TGo4CompositeEvent* dest){
  if(dest==0) ThrowError(0,22,"!!! ERROR BuildEvent: no destination event!!!");
  if (fxTree==0) ThrowError(0,33,"!!! ERROR BuildEvent: no Tree !!!");

  // added by S.Linev 21.11.2003   This is important for Composite event activation/deactivation
  // of course better, if event do not use gTree pointer
  gTree = fxTree;

  if (!fbActivated ){
    dest->synchronizeWithTree( fxTree );
   fbActivated = kTRUE;
  }
  fxTree->GetEntry(fiCurrentEvent++);

  // !!! Added by S.Linev 20.11.2003
  if(fiCurrentEvent>fiMaxEvents) ThrowEOF(0,44,"End of tree at event %d !!!",fiCurrentEvent);

  return kTRUE;
}



