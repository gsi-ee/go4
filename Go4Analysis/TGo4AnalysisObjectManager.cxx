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

#include "TGo4AnalysisObjectManager.h"

#include "TRegexp.h"
#include "TMultiGraph.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TArrayF.h"
#include "TFolder.h"
#include "TFile.h"
#include "TList.h"
#include "TMutex.h"
#include "TBranch.h"

#include "TH2.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TROOT.h"
#include "TTree.h"
#include "TDataMember.h"
#include "TBaseClass.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4ObjectStatus.h"
#include "TGo4BranchStatus.h"
#include "TGo4Parameter.h"
#include "TGo4Condition.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4HistogramEntry.h"
#include "TGo4TreeHistogramEntry.h"
#include "TGo4DynamicList.h"
#include "TGo4AnalysisObjectNames.h"
#include "TGo4HistogramStatus.h"
#include "TGo4TreeStructure.h"

#include "TGo4EventStore.h"
#include "TGo4EventSource.h"
#include "TGo4EventProcessor.h"
#include "TGo4BackStore.h"

#include "TGo4Picture.h"
#include "TGo4CompositeEvent.h"
#include "TGo4ParameterStatus.h"
#include "TGo4MemberStatus.h"
#include "TGo4AnalysisImp.h"

const char *TGo4AnalysisObjectManager::fgcTOPDYNAMICLIST = "Go4DynamicList";
const char *TGo4AnalysisObjectManager::fgcTOPFOLDER      = "Go4";
const char *TGo4AnalysisObjectManager::fgcHISTFOLDER     = "Histograms";
const char *TGo4AnalysisObjectManager::fgcDYNFOLDER      = "DynamicLists";
const char *TGo4AnalysisObjectManager::fgcCONDFOLDER     = "Conditions";
const char *TGo4AnalysisObjectManager::fgcPARAFOLDER     = "Parameters";
const char *TGo4AnalysisObjectManager::fgcTREEFOLDER     = "Trees";
const char *TGo4AnalysisObjectManager::fgcPICTFOLDER     = "Pictures";
const char *TGo4AnalysisObjectManager::fgcCANVFOLDER     = "Canvases";
const char *TGo4AnalysisObjectManager::fgcANALYSISFOLDER = "EventObjects";
const char *TGo4AnalysisObjectManager::fgcEVENTFOLDER    = "Events";
const char *TGo4AnalysisObjectManager::fgcSRCFOLDER      = "EventSources";
const char *TGo4AnalysisObjectManager::fgcSTOREFOLDER    = "EventStores";
const char *TGo4AnalysisObjectManager::fgcPROCFOLDER     = "EventProcessors";
const char *TGo4AnalysisObjectManager::fgcUSRFOLDER      = "UserObjects";
const char *TGo4AnalysisObjectManager::fgcTMPFOLDER      = "Go4-tmp";

const char *TGo4AnalysisObjectManager::GetTOPDYNAMICLIST() { return fgcTOPDYNAMICLIST; }
const char *TGo4AnalysisObjectManager::GetTOPFOLDER() { return fgcTOPFOLDER; }
const char *TGo4AnalysisObjectManager::GetHISTFOLDER() { return fgcHISTFOLDER; }
const char *TGo4AnalysisObjectManager::GetDYNFOLDER() { return fgcDYNFOLDER; }
const char *TGo4AnalysisObjectManager::GetCONDFOLDER() { return fgcCONDFOLDER; }
const char *TGo4AnalysisObjectManager::GetPARAFOLDER() { return fgcPARAFOLDER; }
const char *TGo4AnalysisObjectManager::GetTREEFOLDER() { return fgcTREEFOLDER; }
const char *TGo4AnalysisObjectManager::GetPICTFOLDER() { return fgcPICTFOLDER; }
const char *TGo4AnalysisObjectManager::GetCANVFOLDER() { return fgcCANVFOLDER; }
const char *TGo4AnalysisObjectManager::GetANALYSISFOLDER() { return fgcANALYSISFOLDER; }
const char *TGo4AnalysisObjectManager::GetEVENTFOLDER() { return fgcEVENTFOLDER; }
const char *TGo4AnalysisObjectManager::GetSRCFOLDER() { return fgcSRCFOLDER; }
const char *TGo4AnalysisObjectManager::GetSTOREFOLDER() { return fgcSTOREFOLDER; }
const char *TGo4AnalysisObjectManager::GetPROCFOLDER() { return fgcPROCFOLDER; }
const char *TGo4AnalysisObjectManager::GetUSRFOLDER() { return fgcUSRFOLDER; }
const char *TGo4AnalysisObjectManager::GetTMPFOLDER() { return fgcTMPFOLDER; }



#define fguSUBFOLDERMAXLEN 1024

TGo4AnalysisObjectManager::TGo4AnalysisObjectManager(const char *name) :
   TNamed(name,"The Go4 Analysis Object Manager"),
   fxGo4Dir(nullptr),fxHistogramDir(nullptr),fxConditionDir(nullptr), fxParameterDir(nullptr),
   fxDynListDir(nullptr),fxUserDir(nullptr), fxTreeDir(nullptr), fxPictureDir(nullptr), fxCanvasDir(nullptr),
   fxStoreDir(nullptr), fxSourceDir(nullptr), fxProcessorDir(nullptr), fxEventDir(nullptr),
   fxAnalysisDir(nullptr), fxTempFolder(nullptr),
   fxMatchList(nullptr), fxMatchIterator(nullptr),
   fiDynListCount(0), fiDynListInterval(0),
   fbCreatedinMake(kFALSE), fbSuppressLoadHistograms(kFALSE), fbSortedOrder(kFALSE)
{
   fxDirMutex = new TMutex(kTRUE);
   fxGo4Dir = gROOT->GetRootFolder()->AddFolder(fgcTOPFOLDER,"The Go4 Object folder");
   fxGo4Dir->SetOwner(kTRUE); // top go4 dir owns subfolders
   gROOT->GetListOfBrowsables()->Add(fxGo4Dir, fxGo4Dir->GetName());
   fxHistogramDir = fxGo4Dir->AddFolder(fgcHISTFOLDER,"All Histogram objects");
   fxHistogramDir->SetOwner(kTRUE);
   fxConditionDir = fxGo4Dir->AddFolder(fgcCONDFOLDER,"All Condition objects");
   fxConditionDir->SetOwner(kTRUE);
   fxParameterDir = fxGo4Dir->AddFolder(fgcPARAFOLDER,"All Parameter objects");
   fxParameterDir->SetOwner(kTRUE);
   fxDynListDir = fxGo4Dir->AddFolder(fgcDYNFOLDER,"Dynamic List Instances");
   fxDynListDir->SetOwner(kTRUE);
   fxTreeDir = fxGo4Dir->AddFolder(fgcTREEFOLDER,"References to trees");
   fxTreeDir->SetOwner(kFALSE); // tree dir does not own objects,
   fxPictureDir = fxGo4Dir->AddFolder(fgcPICTFOLDER,"Picture objects");
   fxPictureDir->SetOwner(kTRUE);
   fxCanvasDir = fxGo4Dir->AddFolder(fgcCANVFOLDER,"All TCanvases");
   fxCanvasDir->SetOwner(kTRUE);
   fxAnalysisDir = fxGo4Dir->AddFolder(fgcANALYSISFOLDER,"Event objects of current analysis");
   fxAnalysisDir->SetOwner(kTRUE); // owns folders, but not objects in folders
   fxStoreDir = fxAnalysisDir->AddFolder(fgcSTOREFOLDER,"References to event stores");
   fxStoreDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxSourceDir = fxAnalysisDir->AddFolder(fgcSRCFOLDER,"References to event sources");
   fxSourceDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxProcessorDir = fxAnalysisDir->AddFolder(fgcPROCFOLDER,"References to event processors");
   fxProcessorDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxEventDir = fxAnalysisDir->AddFolder(fgcEVENTFOLDER,"References to event structures");
   fxEventDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxUserDir = fxGo4Dir->AddFolder(fgcUSRFOLDER,"For User Objects");

   // FIXME: SL, 2.01.2012  why owner flag is disabled here, causes memory leak when destroyed
   fxGo4Dir->SetOwner(kFALSE);

   fxTempFolder = gROOT->GetRootFolder()->AddFolder(fgcTMPFOLDER,"The Go4 temporary object folder");
   fxTempFolder->SetOwner(kFALSE);

   gROOT->GetListOfCleanups()->Add(this);
}

TGo4AnalysisObjectManager::TGo4AnalysisObjectManager() :
   TNamed(),
   fxGo4Dir(nullptr),fxHistogramDir(nullptr),fxConditionDir(nullptr),
   fxDynListDir(nullptr), fxUserDir(nullptr), fxTreeDir(nullptr), fxPictureDir(nullptr), fxCanvasDir(nullptr),
   fxStoreDir(nullptr), fxSourceDir(nullptr), fxProcessorDir(nullptr), fxEventDir(nullptr),
   fxAnalysisDir(nullptr), fxTempFolder(nullptr),
   fxMatchList(nullptr), fxMatchIterator(nullptr),
   fiDynListCount(0), fiDynListInterval(0),
   fbCreatedinMake(kFALSE), fbSuppressLoadHistograms(kFALSE), fbSortedOrder(kFALSE)
{
   // ctor for streamer only!

   gROOT->GetListOfCleanups()->Add(this);
}


TGo4AnalysisObjectManager::~TGo4AnalysisObjectManager()
{
   gROOT->GetListOfCleanups()->Remove(this);

   delete fxMatchIterator;
   delete fxMatchList;
   gROOT->GetListOfBrowsables()->Remove(fxGo4Dir);

   gROOT->GetRootFolder()->Remove(fxTempFolder);
   gROOT->GetRootFolder()->Remove(fxGo4Dir);

   //   disable canvas clear - does not work for some reasons
   fxCanvasDir->Clear();

   fxHistogramDir->Clear();
   fxConditionDir->Clear();
   fxParameterDir->Clear();
   fxDynListDir->Clear();
   fxUserDir->Clear();
   fxTreeDir->Clear();
   fxPictureDir->Clear();

   fxEventDir->Clear();
   fxProcessorDir->Clear();
   fxStoreDir->Clear();
   fxSourceDir->Clear();
   fxGo4Dir->Clear();
   fxTempFolder->Clear();

   delete fxTempFolder;
   delete fxGo4Dir;
   delete fxDirMutex;
}

void TGo4AnalysisObjectManager::RecursiveRemove(TObject *obj)
{
   if (obj && (obj!=this)) {
      // remove objects from canvas folder - it may happen that canvas automatically deleted
      if (fxCanvasDir) fxCanvasDir->RecursiveRemove(obj);
   }
}


Bool_t TGo4AnalysisObjectManager::AddObject(TNamed *anything, const char *subfolder, Bool_t replace)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddObject(TNamed *)",__LINE__, __FILE__));
   // unspecified objects are only allowed to put into userdir
   return AddObjectToFolder(anything,fxUserDir,subfolder,replace);
}

Bool_t TGo4AnalysisObjectManager::RemoveObject(const char *name, Bool_t del)
{
   return RemoveObjectFromFolder(name, fxUserDir, del);
}


TObject *TGo4AnalysisObjectManager::GetAsTObject(const char *name, const char *folder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetAsTObject(const char *, const char *)",__LINE__, __FILE__));
   //
   TObject *ob = nullptr;
   TFolder *searchfold = fxGo4Dir;
   if(folder && (strcmp(folder,fgcTOPFOLDER) != 0))
      searchfold = FindSubFolder(fxGo4Dir, folder, kFALSE);
   if(searchfold) {
      ob = FindObjectInFolder(searchfold, name);
      if(ob && ob->InheritsFrom(TTree::Class())) ob = nullptr; // disable sending tree to gui!
      if(ob && ob->InheritsFrom(TFolder::Class())) ob = nullptr; // disable sending complete folder
      if(ob && ob->InheritsFrom(TGo4EventElement::Class())) ob = nullptr; // disable events
      if(ob && ob->InheritsFrom(TGo4EventSource::Class())) ob = nullptr; // disable events
      if(ob && ob->InheritsFrom(TGo4EventStore::Class())) ob = nullptr; // disable events
      if(ob && ob->InheritsFrom(TGo4EventProcessor::Class())) ob = nullptr; // disable events
   }
   if(ob) {
      TGo4Analysis::Instance()->Message(0,"AnalysisObjectManager - found object %s of class %s",
            ob->GetName(), ob->ClassName());
   } else {
      TGo4Analysis::Instance()->Message(0,"!!! AnalysisObjectManager - no such object %s !!!",
            name);
   }
   return ob;
}


TNamed *TGo4AnalysisObjectManager::GetObject(const char *name, const char *folder)
{
   return dynamic_cast<TNamed*> (GetAsTObject(name, folder));
}


Bool_t TGo4AnalysisObjectManager::ClearObjects(const char *name)
{
   GO4TRACE((11, "TGo4AnalysisObjectManager::ClearObject(char *)", __LINE__, __FILE__));
   Bool_t rev = kTRUE;
   TGo4LockGuard dirguard(fxDirMutex);
   TObject *ob = fxGo4Dir->FindObjectAny(name);
   if (ob) {
      if (ob->InheritsFrom(TFolder::Class()))
         rev = ClearFolder(dynamic_cast<TFolder *>(ob));
      else
         rev = ClearObject(ob);
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::DeleteObjects(const char *name)
{
   Bool_t rev = kFALSE;
   TGo4LockGuard dirguard(fxDirMutex);
   TObject *ob = fxGo4Dir->FindObjectAny(name);
   if (ob) {
      if (ob->InheritsFrom(TFolder::Class()))
         rev = DeleteFolder(dynamic_cast<TFolder *>(ob));
      else
         rev = DeleteObject(ob);
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::ProtectObjects(const char *name, const Option_t *flags)
{
   Bool_t rev = kFALSE;
   TGo4LockGuard dirguard(fxDirMutex);
   TObject *ob = fxGo4Dir->FindObjectAny(name);
   if (ob) {
      if (ob->InheritsFrom(TFolder::Class()))
         rev = ProtectFolder(dynamic_cast<TFolder *>(ob), flags);
      else
         rev = ProtectObject(ob, flags);
   }
   return rev;
}

TFolder *TGo4AnalysisObjectManager::CreateCompositeBranchFolder(TObjArray *branchlist, TGo4CompositeEvent *compevent,
                                                                Int_t startindex, Int_t *skip, const char *name,
                                                                const char *title)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::CreateCompositeBranchFolder(TObjArray *,...)",__LINE__, __FILE__));
   if (!branchlist) return nullptr;

   if (!compevent) return CreateBranchFolder(branchlist,name,title);

   Int_t lastindex = 0;
   if(startindex==1)
      // top event: scan everything
      lastindex = startindex + branchlist->GetLast();
   else
      // subevent: scan only own members
      lastindex = startindex + compevent->getNElements();

   TFolder *subnames = nullptr;
   TList *nameslist = new TList;
   TGo4ObjectStatus *state;
   TObjArray *csubevents = compevent->getListOfComposites();

   Int_t skippedentries = 0;
   Int_t offset = 0;
   for(Int_t i=startindex; i<lastindex;i++) {
      TClass *cl = nullptr;
      TObject *entry = branchlist->At(i+offset);
      if(entry && entry->InheritsFrom(TBranch::Class()))  {
         // found subfolder, process it recursively
         TBranch *currentbranch = dynamic_cast<TBranch *> (entry);
         TObjArray *currentbranchlist = nullptr;
         if (currentbranch) {
            currentbranchlist = currentbranch->GetListOfBranches();
            cl = gROOT->GetClass(currentbranch->GetClassName());
         }
         if(cl && cl->InheritsFrom(TGo4CompositeEvent::Class())) {
            // subevent is also composite event, treat next n branches as subbranches:
            TGo4CompositeEvent *subevent = nullptr;
            TString branchname(currentbranch->GetName());
            Ssiz_t leng = branchname.Length();
            branchname.Resize(leng-1); // strip dot from branchname
            if(csubevents)
            	subevent = dynamic_cast<TGo4CompositeEvent *>(csubevents->FindObject(branchname.Data()));

            if(subevent && subevent->getNElements()>0) {
            	// found subbranch, add it to folder struct

               subnames = CreateCompositeBranchFolder(branchlist, subevent, i + offset + 1, &skippedentries,
                                                      subevent->GetName(), subevent->GetTitle());
               nameslist->AddLast(subnames);
               offset+=skippedentries;
            	// now process subbranchlist currentbranchlist of compevent,
            	// add members of this folder to existing folder subnames!
               TFolder *temp = CreateBranchFolder(currentbranchlist, "dummy", "dummy");
               subnames->GetListOfFolders()->AddAll(temp->GetListOfFolders());
            }
            else
            {
            	// subevent not in list, normal operation:
            	//state=new TGo4ObjectStatus(dynamic_cast<TNamed*> (entry) );
            	//nameslist->AddLast(state);
            	if(currentbranchlist)
            	{
            		if(currentbranchlist->IsEmpty())
            		{
            			// subbranchlist is empty, add status object to folder
                     state = new TGo4BranchStatus(currentbranch);
                     nameslist->AddLast(state);
                  }
            		else
            		{
                     subnames =
                        CreateBranchFolder(currentbranchlist, currentbranch->GetName(), currentbranch->GetTitle());
                     nameslist->AddLast(subnames);
                  }
            	} // if(currentbranchlist)
            } //if(subevent && subevent->getNElements()>0)
         }   //  if(cl  && cl->InheritsFrom(TGo4CompositeEvent))
         else
         {
            // subevent is not a composite event, normal operation:
            //                  state=new TGo4ObjectStatus(dynamic_cast<TNamed*> (entry) );
            //                  nameslist->AddLast(state);
            if(currentbranchlist)
            {
               if(currentbranchlist->IsEmpty())
               {
                  // subbranchlist is empty, add status object to folder
                  state=new TGo4BranchStatus(currentbranch);
                  nameslist->AddLast(state);
               }
               else
               {
                  subnames = CreateBranchFolder(currentbranchlist, currentbranch->GetName(), currentbranch->GetTitle());
                  nameslist->AddLast(subnames);
               }
            }
         }
      }
      else
      {
         break;
      } //if(entry->InheritsFrom(TBranch))
   } // for()

   *skip = lastindex+offset-startindex;
   TFolder *fold = fxTempFolder->AddFolder(name,title,nameslist);
   fold->SetOwner(kTRUE);
   fxTempFolder->Remove(fold);
   return fold;
}

TGo4TreeStructure * TGo4AnalysisObjectManager::CreateTreeStructure(TTree *thetree)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::CreateTreeStructure(TTree *)",__LINE__, __FILE__));
   //
   TGo4TreeStructure *tstructure = nullptr;
   if(thetree) {
      const char *treename=thetree->GetName();
      const char *treetitle=thetree->GetTitle();
      TGo4Analysis::Instance()->Message(0,"AnalysisObjectManager - creating structure of tree %s",
            treename);
      TObjArray *branchlist=thetree->GetListOfBranches();
      tstructure= new TGo4TreeStructure(treename);
      tstructure->fxTopFolder=CreateBranchFolder(branchlist,treename,treetitle, kTRUE);
   }
   return tstructure;
}

TGo4TreeStructure * TGo4AnalysisObjectManager::CreateTreeStructure(const char *treename)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::CreateTreeStructure(const char *)",__LINE__, __FILE__));
   TTree *thetree = nullptr;
   if(fxTreeDir)
      thetree = dynamic_cast<TTree *> (fxTreeDir->FindObjectAny(treename));
   return CreateTreeStructure(thetree);
}


TFolder *TGo4AnalysisObjectManager::CreateMembersFolder(TObject *obj, const char *membrfoldername, TClass *cl)
{
   if(!cl) return nullptr;
   auto nameslist = new TList;

   // now process baseclasses of event:
   TIter biter(cl->GetListOfBases());
   while(auto bob = biter()) {
      auto baseclass = dynamic_cast<TBaseClass*>(bob);
      if(!baseclass) continue;

      // we have a baseclass
      auto bclass = baseclass->GetClassPointer();
      if(!bclass) continue;

      if(!strcmp(bclass->GetName(),"TNamed")) continue; // suppress bases above

      // recursively find out members of all baseclasses
      auto subfold = CreateMembersFolder(nullptr, bclass->GetName(), bclass);
      if(subfold)
         nameslist->AddLast(subfold);
   }

   TIter miter(cl->GetListOfDataMembers());
   // scan members of this event class:
   while(auto nob = miter()) {
      auto mem = dynamic_cast<TDataMember *>(nob);
      if(!mem || mem->IsaPointer()) continue; // suppress heap aggregates
      // later, we might scan these recursively

      TString sbuf;

      switch(mem->GetArrayDim()) {
         case 1:
            sbuf.Form("%s[%d]", mem->GetName(), mem->GetMaxIndex(0));
            break;

         case 2:
            sbuf.Form("%s[%d][%d]", mem->GetName(), mem->GetMaxIndex(0), mem->GetMaxIndex(1));
            break;

         default:
            sbuf = mem->GetName();
      }

      auto state = new TGo4MemberStatus(sbuf.Data(), mem->GetFullTypeName());
      nameslist->AddLast(state);
   } // while

   // now process components of composite event
   if (obj && obj->InheritsFrom(TGo4CompositeEvent::Class())) {

      auto arr =static_cast<TGo4CompositeEvent *>(obj)->getElements();

      if (arr)
         for (Int_t n = 0; n <= arr->GetLast(); n++) {
            auto elem = static_cast<TGo4EventElement *>(arr->At(n));
            if (!elem) continue;
            auto subfold = CreateMembersFolder(elem, elem->GetName(), elem->IsA());
            if(subfold)
               nameslist->AddLast(subfold);
         }
   }

   auto memberfolder = fxTempFolder->AddFolder(membrfoldername, TString("Object of class ") + cl->GetName(), nameslist);
   fxTempFolder->Remove(memberfolder);
   memberfolder->SetOwner(kTRUE);
   return memberfolder;
}

Bool_t TGo4AnalysisObjectManager::AddHistogram(TH1 *his, const char *subfolder, Bool_t replace)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddHistogram(TH1 *)",__LINE__, __FILE__));
   Bool_t rev = AddObjectToFolder(his, fxHistogramDir, subfolder, replace, kTRUE);
   // for histograms: add with unique object names within histogramdir
   // to avoid errors in TTree::Draw()
   if(rev && his) his->SetDirectory(gROOT); // assign histo to the top dir, no file!
   return rev;
}

TH1 *TGo4AnalysisObjectManager::MakeTH1(const char *histotype,
      const char *foldername,
      const char *histoname,
      Int_t         nbinsx,
      Axis_t        xlow,
      Axis_t        xup,
      const char *title,
      const char *xtitle,
      const char *ytitle) {

   TString htype(histotype);
   htype.ToUpper();
   if (htype.IsNull()) htype = "I";
   TClass *cl = nullptr;

   switch (htype[0]) {
      case 'C' : cl = TH1C::Class(); break;
      case 'D' : cl = TH1D::Class(); break;
      case 'F' : cl = TH1F::Class(); break;
      case 'I' : cl = TH1I::Class(); break;
      case 'S' : cl = TH1S::Class(); break;
      default : cl = TH1I::Class();
   }

   TH1 *histo = (TH1 *) TestObject(fxHistogramDir, foldername, histoname, cl);

   if (histo) return histo;

   switch(htype[0]) {
      case 'C' : histo = new TH1C(histoname, histoname, nbinsx, xlow, xup); break;
      case 'D' : histo = new TH1D(histoname, histoname, nbinsx, xlow, xup); break;
      case 'F' : histo = new TH1F(histoname, histoname, nbinsx, xlow, xup); break;
      case 'I' : histo = new TH1I(histoname, histoname, nbinsx, xlow, xup); break;
      case 'S' : histo = new TH1S(histoname, histoname, nbinsx, xlow, xup); break;
      default :  histo = new TH1I(histoname, histoname, nbinsx, xlow, xup);
   }

   if (title) histo->SetTitle(title);
   if (xtitle) histo->SetXTitle(xtitle);
   if (ytitle) histo->SetYTitle(ytitle);
   AddHistogram(histo, foldername);
   return histo;
}

TH2 *TGo4AnalysisObjectManager::MakeTH2(const char *histotype,
      const char *foldername,
      const char *histoname,
      Int_t         nbinsx,
      Axis_t        xlow,
      Axis_t        xup,
      Int_t         nbinsy,
      Axis_t        ylow,
      Axis_t        yup,
      const char *title,
      const char *xtitle,
      const char *ytitle) {
   TString htype(histotype);
   htype.ToUpper();
   if (htype.IsNull()) htype = "I";

   TClass *cl = nullptr;

   switch (htype[0]) {
      case 'C' : cl = TH2C::Class(); break;
      case 'D' : cl = TH2D::Class(); break;
      case 'F' : cl = TH2F::Class(); break;
      case 'I' : cl = TH2I::Class(); break;
      case 'S' : cl = TH2S::Class(); break;
      default : cl = TH2I::Class();
   }

   TH2 *histo = (TH2 *) TestObject(fxHistogramDir, foldername, histoname, cl);

   if (histo) return histo;

   switch(htype[0]) {
      case 'C' : histo = new TH2C(histoname, histoname, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 'D' : histo = new TH2D(histoname, histoname, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 'F' : histo = new TH2F(histoname, histoname, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 'I' : histo = new TH2I(histoname, histoname, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      case 'S' : histo = new TH2S(histoname, histoname, nbinsx, xlow, xup, nbinsy, ylow, yup); break;
      default :  histo = new TH2I(histoname, histoname, nbinsx, xlow, xup, nbinsy, ylow, yup);
   }

   if (title) histo->SetTitle(title);
   if (xtitle) histo->SetXTitle(xtitle);
   if (ytitle) histo->SetYTitle(ytitle);
   AddHistogram(histo, foldername);
   return histo;
}

TFolder *TGo4AnalysisObjectManager::CreateBranchFolder(TObjArray *branchlist,
      const char *name,
      const char *title,
      Bool_t istopbranch)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::CreateBranchFolder(TObjArray *)",__LINE__, __FILE__));
   if (!branchlist) return nullptr;

   TList *nameslist = new TList;
   TGo4ObjectStatus *state;
   Int_t cursor = 0;
   TIter iter(branchlist);
   while(auto entry = iter()) {
      if(entry->InheritsFrom(TBranch::Class())) {
         // found subfolder, process it recursively
         TBranch *subbranch = dynamic_cast<TBranch*> (entry);
         TObjArray *subbranchlist = subbranch->GetListOfBranches();
         if(subbranchlist) {
            if(subbranchlist->IsEmpty()) {
               // subbranchlist is empty, add status object to folder
               state = new TGo4BranchStatus(subbranch);
               nameslist->AddLast(state);
            } else {
               // found subbranchlist
               // test for composite event:

               TFolder *subnames = nullptr;

               TClass *cl = gROOT->GetClass(subbranch->GetClassName());
               if(cl && cl->InheritsFrom(TGo4CompositeEvent::Class()) && istopbranch) {
                  // search for composite event of that name in Go4 (only if top level call)
                  TString branchname(subbranch->GetName());
                  Ssiz_t leng = branchname.Length();
                  branchname.Resize(leng-1); // strip dot from branchname
                  auto cevent = dynamic_cast<TGo4CompositeEvent *> (GetEventStructure(branchname.Data()));
                  if(cevent) {
                     Int_t skippedentries = 0;
                     // we pass complete top branchlist to method
                     subnames = CreateCompositeBranchFolder(branchlist,cevent,cursor+1,
                                                            &skippedentries, cevent->GetName(), cevent->GetTitle());
                     // skip comp subelements in iterator:
                     // now process subbranchlist  of top compevent,
                     // add members of this folder to existing folder subnames!
                     TFolder *temp = CreateBranchFolder(subbranchlist, "dummy","dummy");
                     subnames->GetListOfFolders()->AddAll(temp->GetListOfFolders());
                     for (Int_t t = 0; t < skippedentries; ++t) {
                        iter();
                        cursor++;
                     }
                  } else {
                     subnames = CreateBranchFolder(subbranchlist, subbranch->GetName(), subbranch->GetTitle());
                  }
               } else {
                  subnames = CreateBranchFolder(subbranchlist, subbranch->GetName(), subbranch->GetTitle());
               }
               nameslist->AddLast(subnames);
            } // if(subbranchlist->IsEmpty())
         } else {
            // no subbranchlist, should not happen...
            // add status object to folder
            state = new TGo4BranchStatus(subbranch);
            nameslist->AddLast(state);
         } // if(subbranchlist)
      }
      cursor++;
   }

   TFolder *fold = fxTempFolder->AddFolder(name,title,nameslist);
   fold->SetOwner(kTRUE);
   fxTempFolder->Remove(fold);
   return fold;
}

Bool_t TGo4AnalysisObjectManager::AddTree(TTree *tree, const char *subfolder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddTree(TTree *, const char *)",__LINE__, __FILE__));
   return AddObjectToFolder(tree, fxTreeDir, subfolder, kFALSE);
}

Bool_t TGo4AnalysisObjectManager::RemoveTree(TTree *tree, const char *stepname)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveTree(TTree *, const char *)",__LINE__, __FILE__));
   if (!tree) return kFALSE;

   TGo4LockGuard listguard(fxDirMutex);
   fxTreeDir->Remove(tree);
   return kTRUE;
}

TH1 *TGo4AnalysisObjectManager::GetHistogram(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetHistogram(const char *)",__LINE__, __FILE__));
   TH1 *rev = dynamic_cast<TH1 *> (FindObjectInFolder(fxHistogramDir, name));
   if(!rev) rev = dynamic_cast<TH1 *> (FindObjectInFolder(fxUserDir, name)); // also check user objects dir
   return rev;
}

TTree *TGo4AnalysisObjectManager::GetTree(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetTree(char *)",__LINE__, __FILE__));
   return dynamic_cast<TTree *> (FindObjectInFolder(fxTreeDir, name));
}

Bool_t TGo4AnalysisObjectManager::RemoveHistogram(const char *name, Bool_t del)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveHistogram(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxHistogramDir, del);
}

Bool_t TGo4AnalysisObjectManager::AddAnalysisCondition(TGo4Condition *con, const char *subfolder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddAnalysisCondition(TGo4Condition *)",__LINE__, __FILE__));
   return(AddObjectToFolder(con,fxConditionDir,subfolder,kTRUE));
}

Bool_t TGo4AnalysisObjectManager::SetAnalysisCondition(const char *name, TGo4Condition *con,
      Bool_t counter, TFolder *parent)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::SetAnalysisCondition(char*, TGo4Condition *, Bool_t)",__LINE__, __FILE__));

   if(!con) return kFALSE;

   TGo4LockGuard   listguard(fxDirMutex);
   Bool_t rev = kFALSE;
   TFolder *topfolder = nullptr;
   if(parent)
      topfolder = parent;
   else
      topfolder = fxConditionDir;

   TObject *searchresult=topfolder->FindObjectAny(name);
   if(!searchresult)
      searchresult=fxUserDir->FindObjectAny(name);
   // condition may be under user objects folder if not found in topfolder
   TGo4Condition *oldcon = nullptr;
   if(searchresult && searchresult->InheritsFrom(TGo4Condition::Class())) {
      oldcon = dynamic_cast<TGo4Condition *> (searchresult);
   } else
   if (searchresult && searchresult->InheritsFrom(TFolder::Class())) {
      // this is for the case that condition has same name as its folder!
      auto subf = dynamic_cast<TFolder *>(searchresult);
      searchresult = subf->FindObjectAny(name);
      oldcon = dynamic_cast<TGo4Condition *> (searchresult);
   }
   if(oldcon) {
      // update existing condition of given name
      rev = oldcon->UpdateFrom(con,counter);
   } else {
      // condition not yet existing, add external condition
      TGo4Condition *clonedcon = dynamic_cast<TGo4Condition *>(con->Clone());

      TGo4PolyCond::CleanupSpecials(); // remove reference to cloned cut

      const char *separ = strrchr(name, '/');
      if (separ) {
         TString fname(name, separ-name);
         rev = AddObjectToFolder(clonedcon,topfolder,fname.Data(),kTRUE,kFALSE,kFALSE);
      } else
         rev = AddObjectToFolder(clonedcon,topfolder,nullptr,kTRUE,kFALSE,kFALSE);

   }
   return rev;
}

TGo4Condition *TGo4AnalysisObjectManager::GetAnalysisCondition(const char *name, const char *cond_cl)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetAnalysisCondition(char*)",__LINE__, __FILE__));
   TGo4Condition *cond = dynamic_cast<TGo4Condition *> (FindObjectInFolder(fxConditionDir, name));
   // some users have their conditions rather in user folder...
   if(!cond) cond = dynamic_cast<TGo4Condition *> (FindObjectInFolder(fxUserDir, name));

   if (cond && cond_cl)
      if(!cond->InheritsFrom(cond_cl)) cond = nullptr;

   return cond;
}

TGo4WinCond *TGo4AnalysisObjectManager::MakeWindowCond(const char *foldername,
      const char *conditionname,
      Double_t      xlow,
      Double_t      xup,
      const char *bindhistogram,
      Bool_t        invert)
{
   TGo4WinCond *wcond = (TGo4WinCond*)
      TestObject(fxConditionDir, foldername, conditionname, TGo4WinCond::Class());

   if (wcond) return wcond;

   wcond = new TGo4WinCond(conditionname);
   wcond->SetValues(xlow, xup);
   if (bindhistogram)
      wcond->SetHistogram(bindhistogram);
   if (invert) wcond->Invert(invert);
   AddAnalysisCondition(wcond, foldername);
   return wcond;
}

TGo4WinCond *TGo4AnalysisObjectManager::MakeWindowCond(const char *foldername,
      const char *conditionname,
      Double_t      xlow,
      Double_t      xup,
      Double_t      ylow,
      Double_t      yup,
      const char *bindhistogram,
      Bool_t      invert)
{
   TGo4WinCond *wcond = (TGo4WinCond*)
      TestObject(fxConditionDir, foldername, conditionname, TGo4WinCond::Class());

   if (wcond) return wcond;

   wcond = new TGo4WinCond(conditionname);
   wcond->SetValues(xlow, xup, ylow, yup);
   if (bindhistogram)
      wcond->SetHistogram(bindhistogram);
   if (invert) wcond->Invert(invert);
   AddAnalysisCondition(wcond, foldername);
   return wcond;
}

TGo4PolyCond *TGo4AnalysisObjectManager::MakePolyCond(const char *foldername,
      const char *conditionname,
      Int_t         size,
      Float_t      (*points)[2],
      const char *bindhistogram,
      Bool_t        invert)
{
   TGo4PolyCond *pcond = (TGo4PolyCond*)
      TestObject(fxConditionDir, foldername, conditionname, TGo4PolyCond::Class());

   if (pcond) return pcond;

   TArrayF fullx(size+1), fully(size+1);
   int numpoints = size;

   for (int i = 0; i < numpoints; i++) {
      fullx[i] = points[i][0];
      fully[i] = points[i][1];
   }

   // connect first and last points
   if ((fullx[0]!=fullx[numpoints-1]) || (fully[0]!=fully[numpoints-1])) {
      fullx[numpoints] = fullx[0];
      fully[numpoints] = fully[0];
      numpoints++;
   }

   TCutG mycat("initialcut", numpoints, fullx.GetArray(), fully.GetArray());
   pcond = new TGo4PolyCond(conditionname);
   pcond->SetValues(&mycat);
   if (bindhistogram)
      pcond->SetHistogram(bindhistogram);
   if (invert) pcond->Invert(invert);
   AddAnalysisCondition(pcond, foldername);
   return pcond;
}


Bool_t TGo4AnalysisObjectManager::RemoveAnalysisCondition(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveAnalysisCondition(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxConditionDir, kTRUE);
}

TGo4ObjectStatus *TGo4AnalysisObjectManager::CreateObjectStatus(const char *name, const char *folder)
{
   TNamed *object = GetObject(name, folder);
   return CreateObjectStatus(object);
}

TGo4ObjectStatus *TGo4AnalysisObjectManager::CreateObjectStatus(TObject *obj, Bool_t fullinfo)
{
   if(!obj) return nullptr;

   if(obj->InheritsFrom(TH1::Class()))
      return new TGo4HistogramStatus(dynamic_cast<TH1 *> (obj),fullinfo);

   if(obj->InheritsFrom(TGo4Parameter::Class()))
      return new TGo4ParameterStatus(dynamic_cast<TGo4Parameter *> (obj), fullinfo);

   return new TGo4ObjectStatus(obj);
}

TGo4AnalysisObjectNames *TGo4AnalysisObjectManager::CreateNamesList()
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::CreateNamesList()",__LINE__, __FILE__));
   TGo4LockGuard   listguard(fxDirMutex);
   TGo4AnalysisObjectNames *namesobject = nullptr;
   //fxTempFolder->Clear();
   const char *name = TGo4Analysis::Instance()->GetName();
   if (!name || (strlen(name) == 0)) name = "Go4NamesList";
   namesobject = new TGo4AnalysisObjectNames(name);
   namesobject->fxTopFolder = CreateNamesFolder(fxGo4Dir);
   TGo4PolyCond::CleanupSpecials(); // remove references to cloned TCutG in status
   return namesobject;
}

TFolder *TGo4AnalysisObjectManager::CreateNamesFolder(TFolder *objectfolder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::CreateNamesFolder(TFolder *)",__LINE__, __FILE__));
   if (!objectfolder) return nullptr;

   auto nameslist = new TList;
   TIter listiter(objectfolder->GetListOfFolders());
   while(auto entry = listiter()) {
      if(entry->InheritsFrom(TFolder::Class())) {
         // found subfolder, process it recursively
         auto subobj= dynamic_cast<TFolder *>(entry);
         auto subnames = CreateNamesFolder(subobj);
         nameslist->AddLast(subnames);
      } else if (entry->InheritsFrom(TTree::Class())) {
         // treestructure should be ObjectStatus?
         auto subobj = dynamic_cast<TTree *> (entry);
         auto treestruct = CreateTreeStructure(subobj);
         nameslist->AddLast(treestruct);
      } else if(entry->InheritsFrom(TGo4EventElement::Class())) {
         auto evfolder = CreateMembersFolder(entry, entry->GetName(), entry->IsA());
         if (evfolder)
            nameslist->AddLast(evfolder);
      } else {
         TGo4ObjectStatus*state = CreateObjectStatus(entry,kFALSE); // do not use full status info for nameslist
         if(state)
            nameslist->AddLast(state);
      }
   } // while

   auto namesfolder = fxTempFolder->AddFolder(objectfolder->GetName(),objectfolder->GetTitle(),nameslist);
   fxTempFolder->Remove(namesfolder);
   namesfolder->SetOwner(kTRUE);

   return namesfolder;
}

TFolder *TGo4AnalysisObjectManager::GetObjectFolder()
{
   return fxGo4Dir;
}

Bool_t TGo4AnalysisObjectManager::AddEventStore(TGo4EventStore *store)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddEventStore(TGo4EventStore *)",__LINE__, __FILE__));
   return AddObjectToFolder(store,fxStoreDir,nullptr,kFALSE);
}

Bool_t TGo4AnalysisObjectManager::RemoveEventStore(TGo4EventStore *store)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveEventStore(TGo4EventStore *)",__LINE__, __FILE__));
   Bool_t rev = kFALSE;
   if (store) {
      TGo4LockGuard listguard(fxDirMutex);
      fxStoreDir->Remove(store);
      rev = kTRUE;
   } else {
      rev = kFALSE;
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::AddEventSource(TGo4EventSource *source)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddEventSource(TGo4EventSource *)",__LINE__, __FILE__));
   return AddObjectToFolder(source,fxSourceDir,nullptr,kFALSE);
}

Bool_t TGo4AnalysisObjectManager::RemoveEventSource(TGo4EventSource *source)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveEventSource(TGo4EventSource *)",__LINE__, __FILE__));
   Bool_t rev = kFALSE;
   if (source) {
      TGo4LockGuard listguard(fxDirMutex);
      fxSourceDir->Remove(source);
      rev = kTRUE;
   }
   return rev;
}
Bool_t TGo4AnalysisObjectManager::AddEventProcessor(TGo4EventProcessor *pro)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddEventProcessor(TGo4EventProcessor *)",__LINE__, __FILE__));
   return AddObjectToFolder(pro,fxProcessorDir,nullptr,kFALSE);
}

Bool_t TGo4AnalysisObjectManager::RemoveEventProcessor(TGo4EventProcessor *pro)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveEventProcessor(TGo4EventProcessor *)",__LINE__, __FILE__));
   Bool_t rev = kFALSE;
   if (pro) {
      TGo4LockGuard listguard(fxDirMutex);
      fxProcessorDir->Remove(pro);
      rev = kTRUE;
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::AddEventStructure(TGo4EventElement *ev)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddEventStructure(TGo4EventElement *)",__LINE__, __FILE__));
   return AddObjectToFolder(ev, fxEventDir, nullptr, kFALSE);
}

Bool_t TGo4AnalysisObjectManager::RemoveEventStructure(TGo4EventElement *ev)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveEventStructure(TGo4EventElement *)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(ev) {
      TGo4LockGuard listguard(fxDirMutex);
      fxEventDir->Remove(ev);
      // remove pointers to event data from all dynamic lists:
      CleanupDynamicLists(ev);
      rev=kTRUE;
   }
   return rev;
}

TGo4EventElement *TGo4AnalysisObjectManager::GetEventStructure(const char *name) const
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetEvenStructure(const char *)",__LINE__, __FILE__));

   if (!name || (strlen(name) == 0)) return nullptr;

   TString path = name;
   TGo4EventElement *curr = nullptr;

   while (path.Length()>0) {
      Int_t pos = path.Index("/");
      if (pos == 0) { path.Remove(0, 1); continue; }

      TString sub = path;
      if (pos > 0) { sub.Resize(pos); path.Remove(0, pos+1); }
              else { path.Clear(); }

      if (!curr) {
         curr = dynamic_cast<TGo4EventElement *> (FindObjectInFolder(fxEventDir, sub.Data()));
      } else {
         TGo4EventElement *chld = curr->GetChild(sub.Data());
         // this is artefact of folder structures in go4 browser
         // event can have subfolder which corresponds to the subevent
         // but it also could have subfolder which corresponds to parent class
         // in second case just ignore all other artificial subfolders
         if (!chld && curr->InheritsFrom(sub.Data())) return curr;
         curr = chld;
      }

      if (!curr) return nullptr;
   }

   return curr;
}

Bool_t TGo4AnalysisObjectManager::AddDynamicEntry(TGo4DynamicEntry *entry)
{
   if (entry) entry->Reset();
   return AddObjectToFolder(entry, fxDynListDir, nullptr, kTRUE, kFALSE, kFALSE);
}

Bool_t TGo4AnalysisObjectManager::ResetBackStores(Bool_t clearflag)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::ResetBackStores()",__LINE__, __FILE__));
   Bool_t rev = kTRUE;
   TIter iter(fxStoreDir->GetListOfFolders());
   while(auto entry = iter()) {
      TGo4BackStore* bs= dynamic_cast<TGo4BackStore*>(entry);
      if(bs) bs->Reset(clearflag);
   }
   return rev;
}

void TGo4AnalysisObjectManager::CloseAnalysis()
{
   GO4TRACE((14,"TGo4AnalysisObjectManager::CloseAnalysis()",__LINE__, __FILE__));
   ResetCurrentDynList();
   fiDynListCount = 0;
   TGo4Analysis::Instance()->Message(0,"Analysis Object Manager  --  Dynamic List was reset.");
}

void TGo4AnalysisObjectManager::SaveObjects(TFile *file)
{
   if(!file) return;
   TGo4Analysis::Instance()->Message(0,"Analysis Object Manager  --  Saving objects to file %s",file->GetName());
   fxGo4Dir->RecursiveRemove(fxTreeDir); // do not save the trees twice!
   fxGo4Dir->RecursiveRemove(fxAnalysisDir); // do not save eventclass refs
   /////// old implementation: just write top folder
   //fxGo4Dir->Write(0, TObject::kOverwrite);
   /////// end old implementation ///////////////////////////////
   ////// begin new implementation: transform folder into subdirectories of output file
   TDirectory *savdir = gDirectory;
   file->cd();
   // file->Delete("T*;*"); // remove old contents (get rid of deleted dynamic objects)
   // note: we always use RECREATE option on saving now. No need to cleanup old file!
   SaveFolder(fxGo4Dir);
   file->Write(nullptr, TObject::kOverwrite); // write all appended objects and subdirs
   savdir->cd();
   RemoveFromDir(fxGo4Dir,file); // prevent object deletion on closing the file
   ////// end new implementation ////////////////
   fxGo4Dir->Add(fxTreeDir);
   fxGo4Dir->Add(fxAnalysisDir);
}

Bool_t TGo4AnalysisObjectManager::AddParameter(TGo4Parameter *par, const char *subfolder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddParameter(TGo4Parameter *)",__LINE__, __FILE__));
   return AddObjectToFolder(par,fxParameterDir,subfolder,kTRUE);
}

Bool_t TGo4AnalysisObjectManager::SetParameter(const char *name, TGo4Parameter *par, TFolder *parent)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::SetParameter(const char *, TGo4Parameter *)",__LINE__, __FILE__));

   if (!par) return kFALSE;

   Bool_t rev = kFALSE;
   TFolder *topfolder = parent ? parent : fxParameterDir;
   TGo4Parameter *oldpar = dynamic_cast<TGo4Parameter*> (topfolder->FindObjectAny(name));
   if(oldpar) {
      // update existing parameter of given name
      rev = oldpar->UpdateFrom(par);
      TGo4Log::Info("Updated parameter %s from parameter %s", name, par->GetName());
      //oldpar->Print();
   } else {
      // parameter not yet existing, add external parameter as a copy:
      TGo4Parameter *clonedpar=dynamic_cast<TGo4Parameter*>(par->Clone());

      const char *separ = strrchr(name, '/');
      if (separ) {
         TString fname(name, separ-name);
         rev = AddObjectToFolder(clonedpar,topfolder, fname.Data(), kTRUE);
         TGo4Log::Info("Added new parameter %s to folder %s/%s", clonedpar->GetName(), topfolder->GetName(), fname.Data());
      } else
         rev = AddObjectToFolder(clonedpar,topfolder,nullptr,kTRUE);
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::SetParameterStatus(const char *name, TGo4ParameterStatus *status, TFolder *parent)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::SetParameter(char*, TGo4Parameter*)",__LINE__, __FILE__));

   if (!status) return kFALSE;

   Bool_t rev = kFALSE;
   TFolder *topfolder = parent ? parent : fxParameterDir;

   TGo4Parameter *oldpar= dynamic_cast<TGo4Parameter*> (topfolder->FindObjectAny(name));
   if(oldpar) {
      // update existing parameter of given name
      rev = status->UpdateParameterValues(oldpar);
   } else {
      // parameter not yet existing, add external parameter as a copy:
      TGo4Parameter *clonedpar = status->CreateParameter();
      // find out destination folder for new parameter from name:

      const char *separ = strrchr(name, '/');
      if (separ) {
         TString fname(name, separ-name);
         rev = AddObjectToFolder(clonedpar,topfolder, fname.Data(), kTRUE);
      } else {
         rev = AddObjectToFolder(clonedpar,topfolder,nullptr,kTRUE);
      }
   }
   return rev;
}

TGo4Parameter *TGo4AnalysisObjectManager::GetParameter(const char *name, const char *parameter_class)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetParameter(char*)",__LINE__, __FILE__));
   TGo4Parameter *rev = dynamic_cast<TGo4Parameter *> (FindObjectInFolder(fxParameterDir, name));
   if(!rev) rev = dynamic_cast<TGo4Parameter *> (FindObjectInFolder(fxUserDir, name));
   if (rev && parameter_class && !rev->InheritsFrom(parameter_class))
      rev = nullptr;
   return rev;
}

Bool_t TGo4AnalysisObjectManager::RemoveParameter(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveParameter(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxParameterDir, kTRUE);
}

Bool_t TGo4AnalysisObjectManager::AddPicture(TGo4Picture *pic, const char *subfolder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddPicture(TGo4Picture *)",__LINE__, __FILE__));
   if(!pic) return kFALSE;

   //pic->LocateObjectsFrom(fxGo4Dir);
   return AddObjectToFolder(pic,fxPictureDir,subfolder,kTRUE);
}

Bool_t TGo4AnalysisObjectManager::SetPicture(const char *name, TGo4Picture *pic, TFolder *parent)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::SetPicture(char*, TGo4Picture *)",__LINE__, __FILE__));

   if (!pic) return kFALSE;

   Bool_t rev = kTRUE;
   TFolder *topfolder = parent ? parent : fxPictureDir;

   TGo4Picture *oldpic = dynamic_cast<TGo4Picture *> (topfolder->FindObjectAny(name));
   if(oldpic) {
      // update existing picture of given name
      oldpic->UpdateFrom(pic);
      //            std::cout << "++++ Updated picture "<< name<<" from picture "<< pic->GetName() << std::endl;
   } else {
      // picture not yet existing, add external as a copy:
      TGo4Picture *clonedpic = dynamic_cast<TGo4Picture *>(pic->Clone());
      // find out destination folder for new pic from name:

      const char *separ = strrchr(name, '/');
      if (separ) {
        TString fname(name, separ-name);
        rev = AddObjectToFolder(clonedpic, topfolder, fname.Data(), kTRUE);
      } else
        rev = AddObjectToFolder(clonedpic, topfolder, nullptr, kTRUE);
   }
   return rev;
}

TGo4Picture *TGo4AnalysisObjectManager::GetPicture(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetPicture(char*)",__LINE__, __FILE__));
   TGo4Picture *rev = dynamic_cast<TGo4Picture *> (FindObjectInFolder(fxPictureDir, name));
   if(!rev) rev = dynamic_cast<TGo4Picture *> (FindObjectInFolder(fxUserDir, name));
   return rev;
}

Bool_t TGo4AnalysisObjectManager::RemovePicture(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemovePicture(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxPictureDir, kTRUE);
}

Bool_t TGo4AnalysisObjectManager::AddCanvas(TCanvas *can, const char *subfolder)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddCanvas(TCanvas *)",__LINE__, __FILE__));

   Bool_t rev = AddObjectToFolder(can, fxCanvasDir, subfolder, kFALSE);

   if (rev && can) can->SetBit(kMustCleanup);

   // do not remove canvas from global list - it has effect on normal ROOT methods
//   if(rev && can) gROOT->GetListOfCanvases()->Remove(can);

   return rev;
}

TCanvas *TGo4AnalysisObjectManager::GetCanvas(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::GetPicture(const char *)",__LINE__, __FILE__));
   TCanvas *rev = dynamic_cast<TCanvas *> (FindObjectInFolder(fxCanvasDir, name));
   if(!rev) rev = dynamic_cast<TCanvas *> (FindObjectInFolder(fxCanvasDir, name));
   return rev;
}

Bool_t TGo4AnalysisObjectManager::RemoveCanvas(const char *name)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemovePicture(const char *)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxCanvasDir, kTRUE);
}

Bool_t TGo4AnalysisObjectManager::LoadObjects(TFile *obfile)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::LoadObjects(TFile *)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if (obfile) {
      TObject *ob = obfile->Get(TGo4AnalysisObjectManager::fgcTOPFOLDER);
      TFolder *top = dynamic_cast<TFolder *>(ob);
      if (top) {
         // kept for backward compatibility: read folder struct directly
         rev = LoadFolder(top, fxGo4Dir, kFALSE);
      } else {
         // new: convert directory structure of file into folders
         rev = LoadFolder(obfile, fxGo4Dir, kFALSE);
      }
      TGo4PolyCond::CleanupSpecials(); // remove references to file cuts
      ResetCurrentDynList();
   }

   return rev;
}

void TGo4AnalysisObjectManager::ResetCurrentDynList()
{
   TGo4DynamicList::ResetEntries(fxDynListDir);
}

void TGo4AnalysisObjectManager::ProcessDynamicList()
{
   fiDynListCount++;
   // normally, all entries are processed eventwise
   // except for treehistogramentries
   Bool_t processtrees = (fiDynListCount % fiDynListInterval) == 0;
   TGo4DynamicList::ProcessEntries(fxDynListDir, processtrees, GetDynListInterval());
   if(processtrees) ResetBackStores();
}

void TGo4AnalysisObjectManager::PrintDynamicList()
{
   TGo4DynamicList::PrintEntries(fxDynListDir);
}

Bool_t TGo4AnalysisObjectManager::AddDynamicHistogram(const char *name,
      const char *histo,
      const char *hevx, const char *hmemx,
      const char *hevy, const char *hmemy,
      const char *hevz, const char *hmemz,
      const char *condition,
      const char *cevx, const char *cmemx,
      const char *cevy, const char *cmemy)
{
   if(!name || !histo || !hevx || !hmemx)
      return kFALSE;
   auto entry = new TGo4HistogramEntry(name);

   entry->SetHistogramName(histo);

   if (hevx && hmemx) {
      entry->SetHisEventName(0, hevx);
      entry->SetHisVarName(0, hmemx);
   }

   if (hevy && hmemy) {
      entry->SetHisEventName(1, hevy);
      entry->SetHisVarName(1, hmemy);
   }

   if (hevz && hmemz) {
      entry->SetHisEventName(2, hevz);
      entry->SetHisVarName(2, hmemz);
   }

   if(condition && cevx && cmemx) {
      entry->SetConditionName(condition);
      entry->SetConEventName(0, cevx);
      entry->SetConVarName(0, cmemx);
      if(cevy && cmemy) {
         entry->SetConEventName(1, cevy);
         entry->SetConVarName(1, cmemy);
      }
   }

   return AddDynamicEntry(entry);
}

Bool_t TGo4AnalysisObjectManager::AddTreeHistogram(const char *hisname, const char *treename, const char *varexp, const char *cutexp)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddTreeHistogram(char*,...)",__LINE__, __FILE__));
   //
   Bool_t rev = kFALSE;
   auto tentry = new TGo4TreeHistogramEntry(hisname, treename, varexp, cutexp);
   if(AddDynamicEntry(tentry)) {
      TGo4Analysis::Instance()->Message(0,"Analysis added tree histogram %s to dynamic list", tentry->GetName());
      rev=kTRUE;
   } else {
      TGo4Analysis::Instance()->Message(0,"!!! Analysis AddTreeHistogram- ERROR: entry %s already exists !!!", tentry->GetName());
      delete tentry;
   }
   return rev;
}


Bool_t TGo4AnalysisObjectManager::RemoveDynamicEntry(const char *entryname)
{
   return RemoveObjectFromFolder(entryname, fxDynListDir, kTRUE);
}


TFolder *TGo4AnalysisObjectManager::FindSubFolder(TFolder *parent, const char *subfolder, Bool_t create)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::FindSubFolder(TFolder *, const char *, Bool_t)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   TFolder *result = nullptr;
   if (!parent) return nullptr;
   if (!subfolder) return parent;
   const char *separ = strchr(subfolder,'/'); // find end of first subfolder string
   if(separ) {
      // we have subfolder of subfolder, process recursively
      TString subname(subfolder, separ - subfolder);

      auto nextsubfolder = FindSubFolder(parent, subname.Data(), create); // get folder directly under parent
      result = FindSubFolder(nextsubfolder, separ+1,create); // search rest of path in this folder
   } else {
      // only one level of subfolder, find it directly
      TIter listiter(parent->GetListOfFolders());
      TObject *bigger_entry = nullptr;
      while (auto entry = listiter()) {
         if (entry->InheritsFrom(TFolder::Class())) {
            int cmp = strcmp(subfolder, entry->GetName());
            if (cmp == 0) {
               result = dynamic_cast<TFolder *>(entry);
               break;
            }
            if ((cmp < 0) && !bigger_entry && IsSortedOrder()) {
               bigger_entry = entry;
               // break;  // we cannot break loop while folders may exists in non-sorted order
            }
         }
      }

      if(!result && create) {
         TList *lst = nullptr;
         if (IsSortedOrder() && bigger_entry)
            lst = dynamic_cast<TList *> (parent->GetListOfFolders());
         if (lst) {
            result = new TFolder(subfolder,"UserFolder");
            lst->AddBefore(bigger_entry, result);
         } else {
            result = parent->AddFolder(subfolder,"UserFolder"); // create new subfolder if not found
         }
      }
   }
   return result;
}


Bool_t TGo4AnalysisObjectManager::AddObjectToFolder(TObject *ob,
                                                    TFolder *fold,
                                                    const char *subfolder,
                                                    Bool_t replace,
                                                    Bool_t uniquename,
                                                    Bool_t resetbits)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::AddObjectToFolder(TObject *, TFolder *, const char *, Bool_t, Bool_t, Bool_t)",__LINE__, __FILE__));

   TGo4LockGuard  dirguard(fxDirMutex);
   if(!fold) return kFALSE;

   if (!ob) {
      GO4TRACE((12,"TGo4AnalysisObjectManager::AddObjectToFolder - zero object",__LINE__, __FILE__));
      return kFALSE;
   }

   if(resetbits) {
      ob->ResetBit(TGo4Status::kGo4NoReset);
      ob->ResetBit(TGo4Status::kGo4CanDelete); // default behavior: allow reset, disable delete
   }

   TString subname;
   if (subfolder && (strlen(subfolder) != 0)) {
      subname = subfolder;
      if (subname[subname.Length()-1] == '/')
         subname.Resize(subname.Length()-1);
   }

   TObject *oldob = nullptr;
   if(uniquename) {
      // look for object of identical name anywhere in top folder
      oldob = fold->FindObjectAny(ob->GetName());
   } else {
      TString obname;

      if (subname.Length() > 0)
         obname =  subname + "/" + ob->GetName();
      else
         obname = ob->GetName();

      // only check for objects that are in given subfolder
      oldob = fold->FindObjectAny(obname.Data());
   }

   // is object already in folder?
   if(oldob) {
      //yes, shall we replace? check if pointers are identical first!
      if(replace && ob!=oldob) {
         // remove old reference before adding new one:
         fold->RecursiveRemove(oldob);
         CleanupDynamicLists(oldob);
         delete oldob;
      } else {
         return kFALSE; // do not overwrite old one
      }
   }

   TFolder *addDir = nullptr;
   if(subfolder)
      addDir = FindSubFolder(fold, subname, kTRUE);
   if(!addDir) addDir = fold;
   // if(subfolder)
   addDir->Add(ob);

   return kTRUE;
}


Bool_t TGo4AnalysisObjectManager::RemoveObjectFromFolder(const char *fullname, TFolder *fold, Bool_t isDel)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::RemoveObjectFromFolder(const char *, TFolder *, Bool_t)",__LINE__, __FILE__));
   //
   if(!fold) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TObject *obj = nullptr;
   Int_t buflen = fguSUBFOLDERMAXLEN;
   char buffer[fguSUBFOLDERMAXLEN];
   if(fullname) {
      strncpy(buffer,fullname,buflen-10);
      obj = fold->FindObjectAny(buffer);
   }

   if(obj) {
      fold->RecursiveRemove(obj);
      CleanupDynamicLists(obj);
      if (isDel) delete obj;
      TGo4Analysis::Instance()->Message(0,"Analysis: removed object %s",buffer);
      return kTRUE;
   }

   TGo4Analysis::Instance()->Message(0,"!!! Analysis: removing object %s FAILED !!!",buffer);
   return kFALSE;
}

Bool_t TGo4AnalysisObjectManager::LoadFolder(TFolder *source, TFolder *destination, Bool_t replace)
{
   if (!source) return kFALSE;
   GO4TRACE((11,"TGo4AnalysisObjectManager::LoadFolder(TFolder *, TFolder *, Bool_t)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   Bool_t rev = kTRUE;
   TIter folderiter(source->GetListOfFolders());
   while (auto ob = folderiter()) {
      if (ob->InheritsFrom(TFolder::Class())) {
         TFolder *subfolder = dynamic_cast<TFolder *>(ob);
         if (subfolder) {
            TFolder *subdest = FindSubFolder(destination, subfolder->GetName(), kTRUE);
            LoadFolder(subfolder, subdest, replace); // recursively scan all subfolders
         }
      } else {
         rev = PutToFolder(ob, destination, replace);
      }
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::LoadFolder(TDirectory *source, TFolder *destination, Bool_t replace)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::LoadFolder(TDirectory *, TFolder *, Bool_t replace)",__LINE__, __FILE__));
   if(!source || !destination) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   Bool_t rev = kTRUE;
   TDirectory *savdir = gDirectory;
   source->cd(); // this is necessary to let the TKey::ReadObj work!
   source->ReadKeys();
   TIter keyiter(source->GetListOfKeys());
   while (auto keyob = keyiter()) {
      auto key = dynamic_cast<TKey *>(keyob);
      if (!key) {
         TGo4Analysis::Instance()->Message(3, "Analysis LoadFolder: ZERO key in directory %s", source->GetName());
         return kFALSE;
      }
      TObject *ob = key->ReadObj();
      if (!ob) {
         TGo4Analysis::Instance()->Message(2, "Analysis LoadFolder: Retrying to read key %s ...", key->GetName());
         ob = source->Get(key->GetName());
         if (!ob) {
            TGo4Analysis::Instance()->Message(3, "Analysis LoadFolder: Failed to read key %s !", key->GetName());
            return kFALSE;
         }
      }

      if (ob->InheritsFrom(TDirectory::Class())) {
         TDirectory *subdir = dynamic_cast<TDirectory *>(ob);
         if (subdir) {
            Bool_t inpicturefolder = kFALSE;
            if (!strcmp(subdir->GetName(), fgcPICTFOLDER))
               inpicturefolder = kTRUE;
            if (inpicturefolder)
               fbSuppressLoadHistograms = kTRUE;
            TFolder *subdest = FindSubFolder(destination, subdir->GetName(), kTRUE);
            rev = LoadFolder(subdir, subdest, replace); // recursively scan all subfolders
            if (inpicturefolder)
               fbSuppressLoadHistograms = kFALSE;
         }
      } else {
         rev = PutToFolder(ob, destination, replace);
      }
   } // while(..)
   savdir->cd();
   return rev;
}

Bool_t TGo4AnalysisObjectManager::PutToFolder(TObject *ob, TFolder *destination, Bool_t replace)
{
   if(!ob || !destination) return kFALSE;
   Bool_t rev = kTRUE;
   if (ob->InheritsFrom(TGo4DynamicEntry::Class())) {
      // new autosave file structure will save dynamic entries independently:
      TGo4DynamicEntry *dentry = dynamic_cast<TGo4DynamicEntry *> (ob->Clone()); // deep copy of source object!
      AddDynamicEntry(dentry);
   } else if(ob->InheritsFrom(TGo4Parameter::Class())) {
      // parameters never replaced, but updated
      TGo4Parameter *par = dynamic_cast<TGo4Parameter *>(ob);
      SetParameter(ob->GetName(),par,destination);
   } else if(ob->InheritsFrom(TGo4Picture::Class())) {
      // pictures never replaced, but updated
      TGo4Picture *pic =dynamic_cast<TGo4Picture *>(ob);
      SetPicture(ob->GetName(),pic,destination);
   } else if(ob->InheritsFrom(TGo4Condition::Class())) {
      // conditions not replaced, but updated
      TGo4Condition *con =dynamic_cast<TGo4Condition *>(ob);
      SetAnalysisCondition(ob->GetName(),con,kTRUE, destination);
   } else if(ob->InheritsFrom(TH1::Class())) {
      if(fbSuppressLoadHistograms) return kFALSE;
      // test: do not clone histos, but change dir from asf file to memory
      TH1 *his = dynamic_cast<TH1 *>(ob);
      if(AddObjectToFolder(his,destination,nullptr,replace,kFALSE,kFALSE)) {
         his->SetDirectory(gROOT); // set directory for histos, needed for TTree::Draw
         TGo4Analysis::Instance()->Message(0,"Analysis LoadFolder: Histogram %s was loaded.",
               his->GetName());
      }
   } else if(ob->InheritsFrom(TCanvas::Class())) {
      TObject *addob = ob->Clone(); // deep copy of source object!
      if(AddObjectToFolder(addob, destination, nullptr, replace)) {
         TGo4Analysis::Instance()->Message(0,"Analysis LoadFolder: Object %s was loaded.", addob->GetName());
      } else {
         // object already there and noreplace set: delete clone
         delete addob;
         if (gPad==addob) gPad = nullptr;
      }
   } else {
      TObject *addob = ob->Clone(); // deep copy of source object!
      if(AddObjectToFolder(addob,destination, nullptr, replace)) {
         TGo4Analysis::Instance()->Message(0,"Analysis LoadFolder: Object %s was loaded.", addob->GetName());
      } else {
         delete addob;
      }
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::SaveFolder(TFolder *source)
{
   if (!source) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   Bool_t rev = kTRUE;
   TIter folderiter(source->GetListOfFolders());
   while (auto ob = folderiter()) {
      if (ob->InheritsFrom(TFolder::Class())) {
         TFolder *subfolder = dynamic_cast<TFolder *>(ob);
         if (subfolder) {
            TDirectory *currentdir = gDirectory;
            const char *subfoldername = subfolder->GetName();
            TDirectory *subdir = dynamic_cast<TDirectory *>(currentdir->Get(subfoldername));
            if (!subdir) {
               subdir = currentdir->mkdir(subfoldername, "subdir");
            }

            if (subdir) {
               subdir->cd();
               SaveFolder(subfolder); // recursively scan all subfolders
            } else {
               TGo4Analysis::Instance()->Message(2, "Analysis SaveFolder: Could not assign subdirectory %s to folder.",
                                                 subfoldername);
            }
            currentdir->cd();
         }
      } else
         AppendToDir(ob, gDirectory);
   }
   return rev;
}

void TGo4AnalysisObjectManager::AppendToDir(TObject *ob, TDirectory *dir)
{
   if(!ob || !dir) return;
   // note: we do not check for old objects anymore, because
   // file is generally cleaned up before writing now JA
   //TObject *oldob=dir->Get(ob->GetName());
   //if(oldob)
   //   {
   ////      dir->RecursiveRemove(oldob);
   ////      delete oldob;
   //   }
   dir->Append(ob);
}

void TGo4AnalysisObjectManager::RemoveFromDir(TFolder *fold, TDirectory *dir)
{
   if(!fold || !dir) return;

   TIter iter(fold->GetListOfFolders());
   while(auto ob = iter()) {
      if(ob->InheritsFrom(TFolder::Class())) {
         TFolder *subfolder =dynamic_cast<TFolder *>(ob);
         if(subfolder)
            RemoveFromDir(subfolder,dir); // recursively scan all subfolders
      } else
         dir->RecursiveRemove(ob);
   }
}

Int_t TGo4AnalysisObjectManager::PrintFolder(TFolder *fold, Option_t *opt, const char *expression)
{
   if(!fold) return 0;

   GO4TRACE((11,"TGo4AnalysisObjectManager::PrintFolder(TFolder *, Option_t *)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   Int_t totalsize = 0;
   TROOT::IndentLevel();
   TROOT::IncreaseDirLevel();
   std::cout << "+Folder " << fold->GetName() << " content:" << std::endl;
   TIter listiter(fold->GetListOfFolders());
   while(auto ob = listiter()) {
      if(ob->InheritsFrom(TFolder::Class()))
         totalsize += PrintFolder(dynamic_cast<TFolder *>(ob),opt,expression);
      else if(IsMatching(ob->GetName(),expression)) {
         TROOT::IndentLevel();
         ob->Print(opt);
         TGo4ObjectStatus *temp = CreateObjectStatus(ob);
         if (temp)
            totalsize += temp->GetObjectSize();
         delete temp;
      }
   } // while
   TROOT::DecreaseDirLevel();
   TROOT::IndentLevel();
   std::cout <<"++++End "<<fold->GetName()<<"++++++++++" << std::endl;
   return totalsize;
}

Bool_t TGo4AnalysisObjectManager::ClearFolder(TFolder *fold)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::ClearFolder(TFolder *, Option_t *)",__LINE__, __FILE__));
   if(!fold) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TIter iter(fold->GetListOfFolders());
   while(auto ob = iter())
      if(ob->InheritsFrom(TFolder::Class()))
         ClearFolder(dynamic_cast<TFolder *>(ob));
      else
         ClearObject(ob);
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::ClearObject(TObject *ob)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::ClearObject(TObject *)",__LINE__, __FILE__));
   Bool_t rev = kFALSE;
   if (ob) {
      if (ob->TestBit(TGo4Status::kGo4NoReset))
         return kFALSE;
      rev = kTRUE;
      if (ob->InheritsFrom(TH1::Class())) {
         TH1 *his = dynamic_cast<TH1 *>(ob);
         if (his)
            his->Reset(); // histogram has no Clear implementation!
      } else if (ob->InheritsFrom(TGo4DynamicEntry::Class())) {
         TGo4DynamicEntry *entry = dynamic_cast<TGo4DynamicEntry *>(ob);
         if (entry)
            entry->Reset(); // dynamic entry has no Clear implementation!
      } else if (ob->InheritsFrom(TGo4Picture::Class())) {
         TGo4Picture *pic = dynamic_cast<TGo4Picture *>(ob);
         if (pic)
            pic->Reset(); // picture has no Clear implementation!
      } else if (ob->InheritsFrom(TGraph::Class())) {
         TGraph *gr = dynamic_cast<TGraph *>(ob);
         if (gr) {
            // Int_t pn=gr->GetN();
            gr->Set(0); // clear array of points
            // gr->Set(pn); // this should set all to 0 (JAM we don't want this for time plots!)
         }
      } else if (ob->InheritsFrom(TMultiGraph::Class())) {
         TMultiGraph *mg = dynamic_cast<TMultiGraph *>(ob);
         if (mg) {
            TIter liter(mg->GetListOfGraphs());
            while (auto gr = liter())
               ClearObject(gr);
         }
      } else if (ob->InheritsFrom(TGo4EventElement::Class())) {
         // nop, we do not allow a user clear from gui
         // on any event
         rev = kFALSE;
      } else if (ob->InheritsFrom(TTree::Class())) {
         // nop, we do not allow a user clear from gui
         // on any tree
         rev = kFALSE;
      } else {
         ob->Clear();
         // use virtual Clear of all objects
         // make sure that TNamed::Clear is overwritten in subclasses
      }
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::DeleteFolder(TFolder *fold)
{
   GO4TRACE((11, "TGo4AnalysisObjectManager::DeleteFolder(TFolder *)", __LINE__, __FILE__));
   if (!fold)
      return kFALSE;

   TGo4LockGuard dirguard(fxDirMutex);
   TIter iter(fold->GetListOfFolders());
   while (auto ob = iter())
      if (ob->InheritsFrom(TFolder::Class()))
         DeleteFolder(dynamic_cast<TFolder *>(ob));
      else
         DeleteObject(ob);
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::DeleteObject(TObject *ob)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::DeleteObject(TObject *)",__LINE__, __FILE__));
   if(!ob || !ob->TestBit(TGo4Status::kGo4CanDelete) ) return kFALSE;
   fxGo4Dir->RecursiveRemove(ob);
   CleanupDynamicLists(ob);
   delete ob;
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::ProtectFolder(TFolder *fold, const Option_t *flags)
{
   if (!fold)
      return kFALSE;
   TGo4LockGuard dirguard(fxDirMutex);
   TIter listiter(fold->GetListOfFolders());
   while (auto ob = listiter())
      if (ob->InheritsFrom(TFolder::Class()))
         ProtectFolder(dynamic_cast<TFolder *>(ob), flags);
      else
         ProtectObject(ob, flags);
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::ProtectObject(TObject *ob, const Option_t *flags)
{
   if(!ob || !flags) return kFALSE;
   TString opt = flags;
   if(opt.Contains("+D")) ob->ResetBit(TGo4Status::kGo4CanDelete);
   if(opt.Contains("-D")) ob->SetBit(TGo4Status::kGo4CanDelete);
   if(opt.Contains("+C")) ob->SetBit(TGo4Status::kGo4NoReset);
   if(opt.Contains("-C")) ob->ResetBit(TGo4Status::kGo4NoReset);
   return kTRUE;
}

void TGo4AnalysisObjectManager::CleanupDynamicLists(TObject *oldobject)
{
   GO4TRACE((11, "TGo4AnalysisObjectManager::CleanupDynamicLists(TNamed*)", __LINE__, __FILE__));
   TGo4LockGuard dirguard(fxDirMutex);
   if (!oldobject)
      return;
   // scan all dynamic lists for this object:
   if (oldobject->InheritsFrom(TH1::Class()) || oldobject->InheritsFrom(TGo4Condition::Class()) ||
       oldobject->InheritsFrom(TGo4EventElement::Class()))
      TGo4DynamicList::CleanupPointerInEntries(fxDynListDir, oldobject);
}

void TGo4AnalysisObjectManager::PrintConditions(const char *expression)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::PrintConditions(const char *)",__LINE__, __FILE__));
   Int_t totalsize = PrintFolder(fxConditionDir, "*", expression);
   std::cout << "___________________________________________________________" << std::endl;
   std::cout << "Total size of all conditions is: " << totalsize << " bytes." << std::endl;
}

void TGo4AnalysisObjectManager::PrintHistograms(const char *expression)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::PrintHistograms(const char *)",__LINE__, __FILE__));
   Int_t totalsize = PrintFolder(fxHistogramDir, "*", expression);
   std::cout << "___________________________________________________________" << std::endl;
   std::cout << "Total size of all histograms is: " << totalsize << " bytes." << std::endl;
}

void TGo4AnalysisObjectManager::PrintParameters(const char *expression)
{
   GO4TRACE((11,"TGo4AnalysisObjectManager::PrintParameters(const char *)",__LINE__, __FILE__));
   Int_t totalsize = PrintFolder(fxParameterDir, "*", expression);
   std::cout << "___________________________________________________________" << std::endl;
   std::cout << "Total size of all parameters is: " << totalsize << " bytes." << std::endl;
}

TObject *TGo4AnalysisObjectManager::NextMatchingObject(const char *expr, const char *folder, Bool_t reset)
{
   if(reset) {
      delete fxMatchIterator;
      delete fxMatchList;
      fxMatchList = CreateObjectList(expr,folder);
      fxMatchIterator = fxMatchList->MakeIterator();
   }
   return fxMatchIterator->Next();
}

TList *TGo4AnalysisObjectManager::CreateObjectList(const char *expr, const char *folder)
{
   TGo4LockGuard  dirguard(fxDirMutex);
   TFolder *searchfold;
   if(!folder || !strcmp(folder,fgcTOPFOLDER))
      searchfold = fxGo4Dir; // default: search topfolder
   else
      searchfold = FindSubFolder(fxGo4Dir, folder, kFALSE);
   return CreateObjectList(expr, searchfold);
}


TList *TGo4AnalysisObjectManager::CreateObjectList(const char *expr, TFolder *fold)
{
   auto result = new TList;
   if(fold) {
      TIter iter(fold->GetListOfFolders());
      while(auto entry = iter()) {
         if(entry->InheritsFrom(TFolder::Class())) {
            auto subfold=dynamic_cast<TFolder *>(entry);
            TList *sublist = CreateObjectList(expr,subfold);
            // add sublist contents to our result list:
            result->AddAll(sublist);
         } else
         if(entry->InheritsFrom(TTree::Class())) {
         } else  // disable sending tree to gui!
         if(entry->InheritsFrom(TGo4EventElement::Class())) {
         } else // disable events
         if(entry->InheritsFrom(TGo4EventSource::Class())) {
         } else  // disable events
         if(entry->InheritsFrom(TGo4EventStore::Class())) {
         } else // disable events
         if(entry->InheritsFrom(TGo4EventProcessor::Class())) {
         } else { // disable events
            if(IsMatching(entry->GetName(),expr))
               result->AddLast(entry);
         }
      } // while
   } // if(fold)
   return result;
}

Bool_t TGo4AnalysisObjectManager::IsMatching(const char *string, const char *expression) const
{
   if(!expression) return kTRUE;
   Bool_t ismatching = kFALSE;
   TString entrystring = string;
   TRegexp reg(expression,kTRUE);
   if(!strcmp(expression,"*"))
      ismatching = kTRUE; // take all in this folder
   //               else if (strstr(expression,string))
   //                  ismatching=kTRUE; // expression just contained in name
   else if (entrystring.Index(reg,0)!=kNPOS)
      ismatching = kTRUE; // root regular expression class
   else
      ismatching = kFALSE;
   return ismatching;
}


TObject *TGo4AnalysisObjectManager::FindObjectInFolder(TFolder *folder, const char *fullname) const
{
   GO4TRACE((12,"TGo4AnalysisObjectManager::FindObjectInFolder(TFolder *, const char *)",__LINE__, __FILE__));
   TGo4LockGuard  listguard(fxDirMutex);

   return folder ? folder->FindObjectAny(fullname) : nullptr;
}

TObject *TGo4AnalysisObjectManager::TestObject(TFolder *folder, const char *&pathname, const char *objectname, const TClass *cl)
{
   TString fullname;
   if (pathname && (strlen(pathname) == 0))
      pathname = nullptr;
   if (pathname) {
      fullname = pathname;
      fullname += "/";
   }
   fullname += objectname;

   TObject *obj = FindObjectInFolder(folder, fullname);

   if (obj && !obj->InheritsFrom(cl)) {
      RemoveObjectFromFolder(fullname, folder, kTRUE);
      obj = nullptr;
   }

   fbCreatedinMake = !obj;
   return obj;
}

Bool_t TGo4AnalysisObjectManager::FindObjectPathName(TObject *obj, TString &pathname, TFolder *fold)
{
   if (!obj) return kFALSE;

   if (!fold) fold = fxGo4Dir;

   if (fold->GetListOfFolders()->FindObject(obj) == obj) {
      pathname = "";
      return kTRUE;
   }

   TIter iter(fold->GetListOfFolders());
   while (auto sub = iter()) {
      if (!sub->InheritsFrom(TFolder::Class())) continue;
      if (FindObjectPathName(obj, pathname, (TFolder *) sub)) {
         if (pathname.IsNull())
            pathname = sub->GetName();
         else
            pathname = TString(sub->GetName()) + "/" + pathname;
         return kTRUE;
      }
   }

   return kFALSE;
}
