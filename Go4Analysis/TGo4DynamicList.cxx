// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4DynamicList.h"

#include <stdexcept>
#include <cstdlib>

#include "TFolder.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TBaseClass.h"
#include "TH1.h"
#include "TList.h"
#include "TROOT.h"

#include "TGo4DynamicListException.h"
#include "TGo4HistogramEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4EventElement.h"
#include "TGo4Status.h"
#include "TGo4AnalysisImp.h"

void TGo4DynamicList::ResetEntries(TFolder *folder)
{
   if (!folder) return;

   TIter iter(folder->GetListOfFolders());
   while(auto obj = iter()) {
      TGo4DynamicEntry *entry = dynamic_cast<TGo4DynamicEntry *> (obj);
      if (entry) entry->Reset();
   }
}

void TGo4DynamicList::PrintEntries(TFolder *folder)
{
   if (!folder) return;

   TIter iter(folder->GetListOfFolders());
   while(auto obj = iter()) {
      TGo4DynamicEntry *entry = dynamic_cast<TGo4DynamicEntry *> (obj);
      if (entry) entry->Print("*");
   }
}

void TGo4DynamicList::CleanupPointerInEntries(TFolder *folder, TObject *objtoremove)
{
   if (!folder) return;

   TIter iter(folder->GetListOfFolders());
   while(auto obj = iter()) {
      TGo4DynamicEntry *entry = dynamic_cast<TGo4DynamicEntry *> (obj);
      if (entry) entry->RecursiveRemove(objtoremove);
   }
}


void TGo4DynamicList::ProcessEntries(TFolder *folder, Bool_t processtrees, Int_t interval)
{
   if (!folder) return;

   TGo4DynamicEntry *errorentry = nullptr;
   TIter iter(folder->GetListOfFolders());

   try {
      while(auto obj = iter()) {
         TGo4DynamicEntry *entry = dynamic_cast<TGo4DynamicEntry *> (obj);
         if (!entry) continue;

         try {
           if (!entry->IsEnabledProcessing()) continue;

           if (!ProcessHEntry(dynamic_cast<TGo4HistogramEntry*> (entry))) {
              errorentry = entry;
              break;
           }

           if (!ProcessTEntry(dynamic_cast<TGo4TreeHistogramEntry*> (entry), processtrees, interval)) {
              errorentry = entry;
              break;
           }
         }
         catch(TGo4DynamicListException& ex) {
           ex.Handle(); // will disable the error causing entry, continue other entries
           errorentry = (TGo4DynamicEntry *) entry;
         }
      } // while
      //if(processtrees) TGo4Analysis::Instance()->ResetBackStores(kTRUE); // clear backstore reset bits
      // note: the above call is redundant in current usage, because in processtrees case the
      // backstore is immediately reset again for the next time
      // (buffer reset interval equals treedraw interval)
   }
   catch(std::exception& ex) { // treat standard library exceptions
      throw TGo4DynamicListException(errorentry,
                   TString::Format("!!!STD exception %s was raised processing dynamic entry!!!", ex.what()).Data());
   }
}

TDataMember *FindDataMember(TClass *eventclass,
                            const char *memname,
                            Long_t* totaloffset)
{
   if(!eventclass) return nullptr;
   // here we might parse the memname for dots to figure out aggregated classes

   /////////////////////////////////
   // check for array member index:
   Long_t indexoffset = 0;
   const char *ixbegin = strchr(memname,'[');
   if(ixbegin) {
      //   std::cout <<"-------------FindDataMember of Entry " << GetName() << std::endl;
      //   std::cout <<"Found index in member" << memname << std::endl;
      ixbegin++;
      const char *ixend = strchr(ixbegin,']');
      if(ixend)  {
         TString buf(ixbegin, ixend-ixbegin);
         indexoffset = buf.Atoll();
         if(indexoffset < 0) indexoffset = 0;
      }
   }
   /////////////////////////////////

   TDataMember *eventmember = eventclass->GetDataMember(memname);
   if(eventmember) {
      *totaloffset+=eventmember->GetOffset();
   } else {
      // if not found directly, check for baseclass members:
      TIter baseiter(eventclass->GetListOfBases());
      while(auto ob = baseiter()) {
         TBaseClass* baseclass = dynamic_cast<TBaseClass*>(ob);
         if(baseclass)
         {
            // we have a baseclass
            TClass *bclass = baseclass->GetClassPointer();
            // search for member in all superclasses recursively:
            eventmember = FindDataMember(bclass, memname, totaloffset);
            if (eventmember) {
               // we found member in any of the baseclasses
               *totaloffset+=baseclass->GetDelta();
               // inc total offset to this by baseclass offset
               // member offset is relative to TClass (i.e. baseclass here)
               break;
            }
         } // if(baseclass)
      } // while
   } // if (eventmember)

   // finally, we check for eventmember type to add
   //correct offset in case of array:
   if(eventmember)
   {
      const char *tname = eventmember->GetFullTypeName();
      // check if given index is inside allocated size:
      Int_t maxindex = eventmember->GetMaxIndex(0);
      if(maxindex < 0) maxindex = 1; // for non-array members maxindex is -1
      if(indexoffset<maxindex)
      {
         Int_t datasize = eventmember->GetDataType()->Size();
         *totaloffset += indexoffset*datasize;
      }
      else
      {
         throw TGo4DynamicListException(nullptr,
               TString::Format("Index %ld for array member:%s out of range %s[%d]", indexoffset, memname, tname, maxindex).Data());
      }
   }

   return eventmember;
}

bool TGo4DynamicList::ProcessHEntry(TGo4HistogramEntry* hentry)
{
   if (!hentry) return true;

   if (hentry->NeedInitialisation()) {
      auto ana = TGo4Analysis::Instance();

      hentry->fxCondition = ana->GetAnalysisCondition(hentry->GetConditionName());

      for(Int_t n = 0; n < __MAXCONDIM__; n++) {
         TGo4EventElement *event = nullptr;
         TDataMember *eventmember = nullptr;
         Long_t offset = 0;

         const char *evname = hentry->GetConEventName(n);
         const char *memname = hentry->GetConVarName(n);

         if (!TString(evname).Contains(TGo4HistogramEntry::Get_fgcNOEVENT()))
            event = ana->GetEventStructure(evname);

         if(event)
            if(!TString(memname).Contains(TGo4HistogramEntry::Get_fgcNODATA()))
               eventmember = FindDataMember(event->IsA(), memname, &offset);

         hentry->InitCondPointer(n, event, eventmember, offset);
      }

      hentry->fxHistogram = ana->GetHistogram(hentry->GetHistogramName());

      for (Int_t n = 0; n < __MAXHISDIM__; n++) {
         TGo4EventElement *event = nullptr;
         TDataMember *eventmember = nullptr;
         Long_t offset = 0;

         const char *evname = hentry->GetHistEventName(n);
         const char *memname = hentry->GetHistVarName(n);

         if (!TString(evname).Contains(TGo4HistogramEntry::Get_fgcNOEVENT()))
            event = ana->GetEventStructure(evname);

         if(event)
            if(!TString(memname).Contains(TGo4HistogramEntry::Get_fgcNODATA()))
               eventmember = FindDataMember(event->IsA(), memname, &offset);

         hentry->InitHistPointer(n, event, eventmember, offset);
      }

      hentry->SetNeedInitialisation(kFALSE);
   }

   Bool_t evvalid[__MAXHISDIM__];
   for (Int_t n = 0; n < __MAXHISDIM__; n++) {
      evvalid[n] = kFALSE;
      TGo4EventElement *event = (TGo4EventElement *) hentry->fxHisEvents[n];
      if (event)
         evvalid[n] = event->IsValid();
   }

   hentry->ProcessNew(evvalid);

   return true;
}

bool TGo4DynamicList::ProcessTEntry(TGo4TreeHistogramEntry* tentry, Bool_t processtrees, Int_t interval)
{
   if (!tentry) return true;

   tentry->SetDynListInterval(interval);

   if(!processtrees) return true;

   const char *hname = tentry->GetHistogramName();

   TTree *tree = TGo4Analysis::Instance()->GetTree(tentry->GetTreeName());
   if (!tree) {
      throw TGo4DynamicListException(tentry,
               TString::Format("Tree Histogram Entry: !!! Could not find Tree %s ",tentry->GetTreeName()).Data());
   }

   TH1 *histo = TGo4Analysis::Instance()->GetHistogram(hname);
   //if(!histo) std::cout <<"ProcessTEntry do did not find histo "<<hname << std::endl;
   if (!tentry->fbNewHistogram && !histo) {
      throw TGo4DynamicListException(tentry,
               TString::Format("Tree Histogram Entry: !!! Could not find Histogram %s ",hname).Data());
   }

   if (histo)
      tentry->fbNewHistogram = kFALSE;

   tentry->ProcessTreeNew(tree, TGo4Analysis::Instance()->GetDynListInterval());

   if (tentry->fbNewHistogram) {
      histo = dynamic_cast<TH1 *>(gROOT->FindObject(hname));
      if(!histo)
         histo = dynamic_cast<TH1 *>(gROOT->FindObjectAny(hname));
      // note JA: FindObject fails with histogram created in hsimple.C on Cintserver when gDirectory was different from gRoot
      // in this case, histogram is only available in TRoot::fList via scan over root folders
      // note2: second fix is in TGo4Analysis::Process() which explicitely resets gDirectory to gROOT
      // before entering the MainCycle(); so FindObjectAny should be never necessary now
      if(histo) {
         TGo4Analysis::Instance()->AddHistogram(histo); // we put the new histogram into our histo folder!
         histo->SetBit(TGo4Status::kGo4CanDelete);
         tentry->fbNewHistogram=kFALSE;
      }
   }

   return true;
}
