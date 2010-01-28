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

#include "TGo4AnalysisObjectManager.h"

#include "Riostream.h"

#include "TRegexp.h"
#include "TMultiGraph.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TArrayF.h"
#include "TFolder.h"
#include "TFile.h"
#include "TList.h"
#include "TThread.h"
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
#include "TGo4DynamicListException.h"
#include "TGo4DynamicList.h"
#include "TGo4MainTree.h"
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
#include "TGo4PolyCond.h"
#include "TGo4AnalysisImp.h"

const char* TGo4AnalysisObjectManager::fgcTOPDYNAMICLIST="Go4DynamicList";
const char* TGo4AnalysisObjectManager::fgcTOPFOLDER="Go4";
const char* TGo4AnalysisObjectManager::fgcHISTFOLDER="Histograms";
const char* TGo4AnalysisObjectManager::fgcDYNFOLDER="DynamicLists";
const char* TGo4AnalysisObjectManager::fgcCONDFOLDER="Conditions";
const char* TGo4AnalysisObjectManager::fgcPARAFOLDER="Parameters";
const char* TGo4AnalysisObjectManager::fgcTREEFOLDER="Trees";
const char* TGo4AnalysisObjectManager::fgcPICTFOLDER="Pictures";
const char* TGo4AnalysisObjectManager::fgcCANVFOLDER="Canvases";
const char* TGo4AnalysisObjectManager::fgcANALYSISFOLDER="EventObjects";
const char* TGo4AnalysisObjectManager::fgcEVENTFOLDER="Events";
const char* TGo4AnalysisObjectManager::fgcSRCFOLDER="EventSources";
const char* TGo4AnalysisObjectManager::fgcSTOREFOLDER="EventStores";
const char* TGo4AnalysisObjectManager::fgcPROCFOLDER="EventProcessors";
const char* TGo4AnalysisObjectManager::fgcUSRFOLDER="UserObjects";
const char* TGo4AnalysisObjectManager::fgcTMPFOLDER="Go4-tmp";

#define fguSUBFOLDERMAXLEN 1024

TGo4AnalysisObjectManager::TGo4AnalysisObjectManager(const char* name)
:TNamed(name,"The Go4 Analysis Object Manager"),
fxGo4Dir(0),fxHistogramDir(0),fxConditionDir(0), fxParameterDir(0),
fxDynListDir(0),fxUserDir(0), fxAnalysisDir(0), fxTempFolder(0),
fxMatchList(0), fxMatchIterator(0),
fiDynListCount(0), fiDynListInterval(0),
fbCreatedinMake(kFALSE), fbSuppressLoadHistograms(kFALSE)
{
   fxDirMutex=new TMutex(kTRUE);
   fxGo4Dir=gROOT->GetRootFolder()->AddFolder(fgcTOPFOLDER,"The Go4 Object folder");
   fxGo4Dir->SetOwner(kTRUE); // top go4 dir owns subfolders
   gROOT->GetListOfBrowsables()->Add(fxGo4Dir, fxGo4Dir->GetName());
   fxHistogramDir=fxGo4Dir->AddFolder(fgcHISTFOLDER,"All Histogram objects");
   fxHistogramDir->SetOwner(kTRUE);
   fxConditionDir=fxGo4Dir->AddFolder(fgcCONDFOLDER,"All Condition objects");
   fxConditionDir->SetOwner(kTRUE);
   fxParameterDir=fxGo4Dir->AddFolder(fgcPARAFOLDER,"All Parameter objects");
   fxParameterDir->SetOwner(kTRUE);
   fxDynListDir=fxGo4Dir->AddFolder(fgcDYNFOLDER,"Dynamic List Instances");
   fxDynListDir->SetOwner(kTRUE);
   fxTreeDir=fxGo4Dir->AddFolder(fgcTREEFOLDER,"References to trees");
   fxTreeDir->SetOwner(kFALSE); // tree dir does not own objects,
   fxPictureDir=fxGo4Dir->AddFolder(fgcPICTFOLDER,"Picture objects");
   fxPictureDir->SetOwner(kTRUE);
   fxCanvasDir=fxGo4Dir->AddFolder(fgcCANVFOLDER,"All TCanvases");
   fxCanvasDir->SetOwner(kTRUE);
   fxAnalysisDir=fxGo4Dir->AddFolder(fgcANALYSISFOLDER,"Event objects of current analysis");
   fxAnalysisDir->SetOwner(kTRUE); // owns folders, but not objects in folders
   fxStoreDir=fxAnalysisDir->AddFolder(fgcSTOREFOLDER,"References to event stores");
   fxStoreDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxSourceDir=fxAnalysisDir->AddFolder(fgcSRCFOLDER,"References to event sources");
   fxSourceDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxProcessorDir=fxAnalysisDir->AddFolder(fgcPROCFOLDER,"References to event processors");
   fxProcessorDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxEventDir=fxAnalysisDir->AddFolder(fgcEVENTFOLDER,"References to event structures");
   fxEventDir->SetOwner(kFALSE); // event  classes dir does not own objects,
   fxUserDir=fxGo4Dir->AddFolder(fgcUSRFOLDER,"For User Objects");
   fxGo4Dir->SetOwner(kFALSE);
   fxTempFolder=gROOT->GetRootFolder()->AddFolder(fgcTMPFOLDER,"The Go4 temporary object folder");
   fxTempFolder->SetOwner(kFALSE);
}

TGo4AnalysisObjectManager::TGo4AnalysisObjectManager()
:fxGo4Dir(0),fxHistogramDir(0),fxConditionDir(0),
fxDynListDir(0),fxUserDir(0), fxAnalysisDir(0), fxTempFolder(0),
fiDynListCount(0), fiDynListInterval(0),
fbCreatedinMake(kFALSE), fbSuppressLoadHistograms(kFALSE)
{
   // ctor for streamer only!
}


TGo4AnalysisObjectManager::~TGo4AnalysisObjectManager()
{

   delete fxMatchIterator;
   delete fxMatchList;
   //delete fxDirMutex;
   gROOT->GetListOfBrowsables()->Remove(fxGo4Dir);
   //cout <<"Removed all folders from list of browsables" << endl;

   gROOT->GetRootFolder()->Remove(fxTempFolder);
   gROOT->GetRootFolder()->Remove(fxGo4Dir);

   fxHistogramDir->Clear();
   fxConditionDir->Clear();
   fxParameterDir->Clear();
   fxDynListDir->Clear();
   fxUserDir->Clear();
   fxTreeDir->Clear();
   fxPictureDir->Clear();
   fxCanvasDir->Clear();
   fxEventDir->Clear();
   fxProcessorDir->Clear();
   fxStoreDir->Clear();
   fxSourceDir->Clear();
   fxGo4Dir->Clear();
   fxTempFolder->Clear();
   //cout <<"Cleared all folders" << endl;
   delete fxTempFolder;
   //cout <<"deleted temporary folder" << endl;
   delete fxGo4Dir;
   //cout <<"deleted top folder" << endl;
}


Bool_t TGo4AnalysisObjectManager::AddObject(TNamed * anything, const char* subfolder, Bool_t replace)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddObject(TNamed *)",__LINE__, __FILE__));
   // unspecified objects are only allowed to put into userdir
   return (AddObjectToFolder(anything,fxUserDir,subfolder,replace));
}

Bool_t TGo4AnalysisObjectManager::RemoveObject(const char * name, Bool_t del)
{
   return RemoveObjectFromFolder(name, fxUserDir, del);
}



TNamed * TGo4AnalysisObjectManager::GetObject(const char * name, const char* folder)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetObject(char *)",__LINE__, __FILE__));
   //
   TNamed* ob=0;
   TFolder* searchfold;
   if(folder==0 || !strcmp(folder,fgcTOPFOLDER))
      searchfold=fxGo4Dir; // default: search topfolder
   else
      searchfold=FindSubFolder(fxGo4Dir, folder, kFALSE);
   if(searchfold)
   {
      ob= dynamic_cast<TNamed*> (FindObjectInFolder(searchfold, name));
      //cout << "found object in top folder :" << ob  <<endl;
      if(ob && ob->InheritsFrom(TTree::Class())) ob=0; // disable sending tree to gui!
      if(ob && ob->InheritsFrom(TFolder::Class())) ob=0; // disable sending complete folder
      if(ob && ob->InheritsFrom(TGo4EventElement::Class())) ob=0; // disable events
      if(ob && ob->InheritsFrom(TGo4EventSource::Class())) ob=0; // disable events
      if(ob && ob->InheritsFrom(TGo4EventStore::Class())) ob=0; // disable events
      if(ob && ob->InheritsFrom(TGo4EventProcessor::Class())) ob=0; // disable events
   }
   else
   {
      ob=0;
   }
   if(ob)
   {
      TGo4Analysis::Instance()->Message(0,"AnalysisObjectManager - found object %s of class %s",
            ob->GetName(), ob->ClassName());
   }
   else
   {
      TGo4Analysis::Instance()->Message(0,"!!! AnalysisObjectManager - no such object %s !!!",
            name);
   }
   return ob;
}



Bool_t TGo4AnalysisObjectManager::ClearObjects(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::ClearObject(char *)",__LINE__, __FILE__));
   Bool_t rev=kTRUE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TObject* ob = fxGo4Dir->FindObjectAny(name);
   if(ob!=0)
   {
      if(ob->InheritsFrom(TFolder::Class()))
         rev=ClearFolder(dynamic_cast<TFolder*>(ob));
      else
         rev=ClearObject(ob);
   }
   return rev;
}


Bool_t TGo4AnalysisObjectManager::DeleteObjects(const char * name)
{
   Bool_t rev=kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TObject* ob= fxGo4Dir->FindObjectAny(name);
   if(ob!=0)
   {
      if(ob->InheritsFrom(TFolder::Class()))
         rev=DeleteFolder(dynamic_cast<TFolder*>(ob));
      else
         rev=DeleteObject(ob);
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::ProtectObjects(const char* name, const Option_t* flags)
{
   Bool_t rev = kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TObject* ob= fxGo4Dir->FindObjectAny(name);
   if(ob!=0)
   {
      if(ob->InheritsFrom(TFolder::Class()))
         rev=ProtectFolder(dynamic_cast<TFolder*>(ob), flags);
      else
         rev=ProtectObject(ob, flags);
   }
   return rev;
}

TFolder * TGo4AnalysisObjectManager::CreateBranchFolder(TObjArray* branchlist,
      const char* name,
      const char* title,
      Bool_t istopbranch)
{
   TRACE((11,"TGo4AnalysisObjectManager::CreateBranchFolder(TObjArray*)",__LINE__, __FILE__));
   if (branchlist==0) return 0;

   TList* nameslist= new TList;
   TGo4ObjectStatus* state;
   Int_t cursor=0;
   TObject* entry = 0;
   TIter iter(branchlist);
   while((entry=iter()) !=0) {
      if(entry->InheritsFrom(TBranch::Class())) {
         // found subfolder, process it recursively
         TBranch* subbranch= dynamic_cast<TBranch*> (entry);
         TObjArray* subbranchlist = subbranch->GetListOfBranches();
         if(subbranchlist!=0) {
            if(subbranchlist->IsEmpty()) {
               // subbranchlist is empty, add status object to folder
               state=new TGo4BranchStatus(subbranch);
               nameslist->AddLast(state);
            } else {
               // found subbranchlist
               // test for composite event:

               TFolder* subnames = 0;

               TClass* cl = gROOT->GetClass(subbranch->GetClassName());
               if((cl!=0) && cl->InheritsFrom(TGo4CompositeEvent::Class()) && istopbranch) {
                  // search for composite event of that name in Go4 (only if top level call)
                  TGo4CompositeEvent* cevent=0;
                  TString branchname(subbranch->GetName());
                  Ssiz_t leng = branchname.Length();
                  branchname.Resize(leng-1); // strip dot from branchname
                  //cout <<"searching for composite event "<< branchname.Data() << endl;
                  cevent = dynamic_cast<TGo4CompositeEvent*> (GetEventStructure(branchname.Data()));
                  if(cevent!=0) {
                     //cout <<"found top comp event" << endl;
                     Int_t skippedentries=0;
                     // we pass complete top branchlist to method
                     subnames = CreateCompositeBranchFolder(
                           branchlist,cevent,cursor+1,
                           &skippedentries,
                           cevent->GetName(),
                           cevent->GetTitle());
                     // skip comp subelements in iterator:
                     //cout <<"top skipped:"<<skippedentries<< endl;
                     // now process subbranchlist  of top compevent,
                     // add members of this folder to existing folder subnames!
                     TFolder* temp = CreateBranchFolder(subbranchlist, "dummy","dummy");
                     subnames->GetListOfFolders()->AddAll(temp->GetListOfFolders());
                     for(Int_t t=0;t<skippedentries;++t) {
                        iter();
                        cursor++;
                     }
                  } else {
                     //cout <<"not found top comp event" << endl;
                     subnames=CreateBranchFolder(subbranchlist,
                           subbranch->GetName(),
                           subbranch->GetTitle());
                  }
               } else {
                  //cout <<"no class or not top branch" << endl;
                  subnames=CreateBranchFolder(subbranchlist,
                        subbranch->GetName(), subbranch->GetTitle());
               }
               nameslist->AddLast(subnames);
            } // if(subbranchlist->IsEmpty())
         } else {
            // no subbranchlist, should not happen...
            // add status object to folder
            state=new TGo4BranchStatus(subbranch);
            nameslist->AddLast(state);
         } // if(subbranchlist)
      }
      cursor++;
   } // while((entry=iter()) !=0)

   TFolder* fold = fxTempFolder->AddFolder(name,title,nameslist);
   fold->SetOwner(kTRUE);
   fxTempFolder->Remove(fold);
   return fold;
}

TFolder * TGo4AnalysisObjectManager::CreateCompositeBranchFolder(TObjArray* branchlist,
      TGo4CompositeEvent* compevent,
      Int_t startindex, Int_t* skip,
      const char* name, const char* title)
{
   TRACE((11,"TGo4AnalysisObjectManager::CreateCompositeBranchFolder(TObjArray*,...)",__LINE__, __FILE__));
   if (branchlist==0) return 0;

   if (compevent==0) return CreateBranchFolder(branchlist,name,title);

   Int_t lastindex=0;
   if(startindex==1)
      // top event: scan everything
      lastindex = startindex + branchlist->GetLast();
   else
      // subevent: scan only own members
      lastindex = startindex + compevent->getNElements();

   TFolder* subnames=0;
   TList* nameslist = new TList;
   TGo4ObjectStatus* state;
   TObjArray* csubevents = compevent->getListOfComposites();

   Int_t skippedentries=0;
   Int_t offset=0;
   for(Int_t i=startindex; i<lastindex;i++) {
      //cout <<"i+offset="<<i+offset << endl;
      TClass* cl=0;
      TObject* entry=branchlist->At(i+offset);
      if((entry!=0) && entry->InheritsFrom(TBranch::Class()))  {
         // found subfolder, process it recursively
         TBranch* currentbranch= dynamic_cast<TBranch*> (entry);
         TObjArray* currentbranchlist=0;
         if (currentbranch!=0) {
            currentbranchlist=currentbranch->GetListOfBranches();
            cl=gROOT->GetClass(currentbranch->GetClassName());
         }
         if((cl!=0)  && cl->InheritsFrom(TGo4CompositeEvent::Class())) {
            // subevent is also composite event, treat next n branches as subbranches:
            TGo4CompositeEvent* subevent=0;
            TString branchname(currentbranch->GetName());
            Ssiz_t leng = branchname.Length();
            branchname.Resize(leng-1); // strip dot from branchname
            //cout <<"searching for composite sub event "<< branchname.Data() << endl;
            if(csubevents!=0)
               subevent = dynamic_cast<TGo4CompositeEvent *>(csubevents->FindObject(branchname.Data()));

               if((subevent!=0) && subevent->getNElements()>0)
               {
                  //cout <<"found composite subevent "<< subevent->GetName() << endl;
                  // found subbranch, add it to folder struct

                  subnames=CreateCompositeBranchFolder(branchlist,
                        subevent,
                        i+offset+1, &skippedentries,
                        subevent->GetName(),
                        subevent->GetTitle());
                  nameslist->AddLast(subnames);
                  offset+=skippedentries;
                  //cout <<"skipped:"<<skippedentries<<", i:"<<i << endl;
                  //cout <<"offset:"<<offset<< endl;
                  // now process subbranchlist currentbranchlist of compevent,
                  // add members of this folder to existing folder subnames!
                  TFolder* temp=CreateBranchFolder(currentbranchlist,
                        "dummy",
                        "dummy");
                  subnames->GetListOfFolders()->AddAll(temp->GetListOfFolders());


               }
               else
               {
                  //cout <<"composite subevent is basic member..." << endl;
                  // subevent not in list, normal operation:
                  //state=new TGo4ObjectStatus(dynamic_cast<TNamed*> (entry) );
                  //nameslist->AddLast(state);
                  if(currentbranchlist!=0)
                  {
                     if(currentbranchlist->IsEmpty())
                     {
                        // subbranchlist is empty, add status object to folder
                        state=new TGo4BranchStatus(currentbranch);
                        nameslist->AddLast(state);
                     }
                     else
                     {
                        subnames=CreateBranchFolder(currentbranchlist,
                              currentbranch->GetName(),
                              currentbranch->GetTitle());
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
            if(currentbranchlist!=0)
            {
               if(currentbranchlist->IsEmpty())
               {
                  // subbranchlist is empty, add status object to folder
                  state=new TGo4BranchStatus(currentbranch);
                  nameslist->AddLast(state);
               }
               else
               {
                  subnames=CreateBranchFolder(currentbranchlist,
                        currentbranch->GetName(),
                        currentbranch->GetTitle());
                  nameslist->AddLast(subnames);
               }
            }
            //cout <<"added entry"<< entry->GetName() << endl;
         }
      }
      else
      {
         break;
         //            if(entry)
         //               cout <<"ERROR: no branch in branchlist, class:"<< entry->ClassName() << endl;
         //             else
         //               cout << "ZERO entry in branchlist"<< endl;

      } //if(entry->InheritsFrom(TBranch))
   } // for()

   *skip=lastindex+offset-startindex;
   TFolder* fold = fxTempFolder->AddFolder(name,title,nameslist);
   fold->SetOwner(kTRUE);
   fxTempFolder->Remove(fold);
   return fold;

}



TGo4TreeStructure * TGo4AnalysisObjectManager::CreateTreeStructure(TTree* thetree)
{
   TRACE((11,"TGo4AnalysisObjectManager::CreateTreeStructure(TTree*)",__LINE__, __FILE__));
   //
   TGo4TreeStructure* tstructure=0;
   if(thetree)
   {
      const char* treename=thetree->GetName();
      const char* treetitle=thetree->GetTitle();
      TGo4Analysis::Instance()->Message(0,"AnalysisObjectManager - creating structure of tree %s",
            treename);
      TObjArray* branchlist=thetree->GetListOfBranches();
      tstructure= new TGo4TreeStructure(treename);
      tstructure->fxTopFolder=CreateBranchFolder(branchlist,treename,treetitle, kTRUE);
   }
   else {}
   return tstructure;
}

TGo4TreeStructure * TGo4AnalysisObjectManager::CreateTreeStructure(const char* treename)
{
   TRACE((11,"TGo4AnalysisObjectManager::CreateTreeStructure(const char *)",__LINE__, __FILE__));
   TTree* thetree=0;
   if(fxTreeDir)
   {
      thetree=dynamic_cast<TTree*> (fxTreeDir->FindObjectAny(treename));
   }
   else
   {
      thetree=0;
   }
   return (CreateTreeStructure(thetree));
}


TFolder* TGo4AnalysisObjectManager::CreateMembersFolder(const char* membrfoldername, TClass* cl)
{
   if(cl==0) return 0;
   TList* nameslist= new TList;
   TIter miter(cl->GetListOfDataMembers());
   TObject* nob=0;
   // scan members of this event class:
   while((nob=miter()) !=0) {
      TDataMember* mem = dynamic_cast<TDataMember*>(nob);
      if((mem==0) || mem->IsaPointer()) continue; // suppress heap aggregates
      // later, we might scan these recursively

      TString sbuf;

      switch(mem->GetArrayDim()) {
         case 1:
            sbuf = Form("%s[%d]", mem->GetName(),mem->GetMaxIndex(0));
            break;

         case 2:
            sbuf = Form("%s[%d][%d]", mem->GetName(),mem->GetMaxIndex(0),mem->GetMaxIndex(1));
            break;

         default:
            sbuf = mem->GetName();
      }

      TGo4MemberStatus* state = new TGo4MemberStatus(sbuf.Data(), mem->GetFullTypeName());
      nameslist->AddLast(state);
   } // while

   // now process baseclasses of event:
   TIter biter(cl->GetListOfBases());
   TObject* bob=0;
   while((bob=biter()) !=0) {
      TBaseClass* baseclass = dynamic_cast<TBaseClass*>(bob);
      if(baseclass==0) continue;

      // we have a baseclass
      TClass* bclass=baseclass->GetClassPointer();
      if(bclass==0) continue;

      if(!strcmp(bclass->GetName(),"TNamed")) continue; // suppress bases above

      // recursively find out members of all baseclasses
      TFolder* subfold = CreateMembersFolder(bclass->GetName(), bclass);
      if(subfold!=0)
         nameslist->AddLast(subfold);
   } // while((bob=baseiter->Next()) !=0)

   TFolder* memberfolder = fxTempFolder->AddFolder(membrfoldername, TString("Object of class ") + cl->GetName(), nameslist);
   fxTempFolder->Remove(memberfolder);
   memberfolder->SetOwner(kTRUE);
   return memberfolder;
}

Bool_t TGo4AnalysisObjectManager::AddHistogram(TH1 * his, const char* subfolder, Bool_t replace)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddHistogram(TH1*)",__LINE__, __FILE__));
   Bool_t rev=AddObjectToFolder(his,fxHistogramDir,subfolder,replace,kTRUE);
   // for histograms: add with unique object names within histogramdir
   // to avoid errors in TTree::Draw()
   if(rev && his) his->SetDirectory(gROOT); // assign histo to the top dir, no file!
   return rev;
}

TH1* TGo4AnalysisObjectManager::MakeTH1(const char* histotype,
      const char* foldername,
      const char* histoname,
      Int_t         nbinsx,
      Axis_t        xlow,
      Axis_t        xup,
      const char* title,
      const char* xtitle,
      const char* ytitle) {

   TString htype(histotype);
   htype.ToUpper();
   if (htype.Length()==0) htype = "I";
   TClass* cl = 0;

   switch (htype[0]) {
      case 'C' : cl = TH1C::Class(); break;
      case 'D' : cl = TH1D::Class(); break;
      case 'F' : cl = TH1F::Class(); break;
      case 'I' : cl = TH1I::Class(); break;
      case 'S' : cl = TH1S::Class(); break;
      default : cl = TH1I::Class();
   }

   TH1* histo = (TH1*) TestObject(fxHistogramDir, foldername, histoname, cl);

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

TH2* TGo4AnalysisObjectManager::MakeTH2(const char* histotype,
      const char* foldername,
      const char* histoname,
      Int_t         nbinsx,
      Axis_t        xlow,
      Axis_t        xup,
      Int_t         nbinsy,
      Axis_t        ylow,
      Axis_t        yup,
      const char* title,
      const char* xtitle,
      const char* ytitle) {
   TString htype(histotype);
   htype.ToUpper();
   if (htype.Length()==0) htype = "I";

   TClass* cl = 0;

   switch (htype[0]) {
      case 'C' : cl = TH2C::Class(); break;
      case 'D' : cl = TH2D::Class(); break;
      case 'F' : cl = TH2F::Class(); break;
      case 'I' : cl = TH2I::Class(); break;
      case 'S' : cl = TH2S::Class(); break;
      default : cl = TH2I::Class();
   }

   TH2* histo = (TH2*) TestObject(fxHistogramDir, foldername, histoname, cl);

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

Bool_t TGo4AnalysisObjectManager::AddTree(TTree* tree, const char* subfolder)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddTree(TTree*, char*)",__LINE__, __FILE__));
   return (AddObjectToFolder(tree,fxTreeDir,subfolder,kFALSE));
}

Bool_t TGo4AnalysisObjectManager::RemoveTree(TTree * tree, const char* stepname)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveTree(TTree*, const char*)",__LINE__, __FILE__));
   if (tree==0) return kFALSE;

   TGo4LockGuard listguard(fxDirMutex);
   fxTreeDir->Remove(tree);
   return kTRUE;
}

TH1* TGo4AnalysisObjectManager::GetHistogram(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetHistogram(char *)",__LINE__, __FILE__));
   TH1* rev= dynamic_cast<TH1*> (FindObjectInFolder(fxHistogramDir, name));
   if(rev==0) rev= dynamic_cast<TH1*> (FindObjectInFolder(fxUserDir, name)); // also check user objects dir
   return rev;
}

TTree * TGo4AnalysisObjectManager::GetTree(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetTree(char *)",__LINE__, __FILE__));
   return dynamic_cast<TTree*> (FindObjectInFolder(fxTreeDir, name));
}

Bool_t TGo4AnalysisObjectManager::RemoveHistogram(const char * name, Bool_t del)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveHistogram(char*)",__LINE__, __FILE__));
   //
   return RemoveObjectFromFolder(name, fxHistogramDir, del);
}


Bool_t TGo4AnalysisObjectManager::AddAnalysisCondition(TGo4Condition * con, const char* subfolder)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddAnalysisCondition(TGo4Condition *)",__LINE__, __FILE__));
   return(AddObjectToFolder(con,fxConditionDir,subfolder,kTRUE));
}

Bool_t TGo4AnalysisObjectManager::SetAnalysisCondition(const char * name, TGo4Condition* con,
      Bool_t counter, TFolder* parent)
{
   TRACE((11,"TGo4AnalysisObjectManager::SetAnalysisCondition(char*, TGo4Condition*, Bool_t)",__LINE__, __FILE__));

   if(con==0) return kFALSE;

   TGo4LockGuard   listguard(fxDirMutex);
   Bool_t rev(kFALSE);
   TFolder* topfolder=0;
   if(parent)
      topfolder=parent;
   else
      topfolder=fxConditionDir;

   TObject* searchresult=topfolder->FindObjectAny(name);
   if(searchresult==0)
      searchresult=fxUserDir->FindObjectAny(name);
   // condition may be under user objects folder if not found in topfolder
   TGo4Condition* oldcon(0);
   if(searchresult && searchresult->InheritsFrom(TGo4Condition::Class())) {
      oldcon= dynamic_cast<TGo4Condition *> (searchresult);
   } else
   if (searchresult && searchresult->InheritsFrom(TFolder::Class())) {
      // this is for the case that condition has same name as its folder!
      TFolder* subf=dynamic_cast<TFolder*>(searchresult);
      searchresult=subf->FindObjectAny(name);
      oldcon= dynamic_cast<TGo4Condition *> (searchresult);
   }
   if(oldcon!=0) {
      // update existing condition of given name
      rev = oldcon->UpdateFrom(con,counter);
      //cout << "++++ Updated condition "<< name<<" from condition "<< con->GetName() << endl;
      //oldcon->PrintCondition(true);
   } else {
      // condition not yet exisiting, add external condition
      TGo4Condition* clonedcon = dynamic_cast<TGo4Condition*>(con->Clone());

      TGo4PolyCond::CleanupSpecials(); // remove reference to cloned cut

      const char* separ = strrchr(name, '/');
      if (separ!=0) {
         TString fname(name, separ-name);
         rev=AddObjectToFolder(clonedcon,topfolder,fname.Data(),kTRUE,kFALSE,kFALSE);
      } else
         rev=AddObjectToFolder(clonedcon,topfolder,0,kTRUE,kFALSE,kFALSE);

   }
   return rev;
}

TGo4Condition * TGo4AnalysisObjectManager::GetAnalysisCondition(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetAnalysisCondition(char*)",__LINE__, __FILE__));
   TGo4Condition* cond = dynamic_cast<TGo4Condition *> (FindObjectInFolder(fxConditionDir, name));
   if(cond==0) cond=dynamic_cast<TGo4Condition *> (FindObjectInFolder(fxUserDir, name));
   // some users have their conditions rather in user folder...
   return cond;
}

TGo4WinCond* TGo4AnalysisObjectManager::MakeWindowCond(const char* foldername,
      const char* conditionname,
      Double_t      xlow,
      Double_t      xup,
      const char* bindhistogram,
      Bool_t        invert) {
   TGo4WinCond* wcond = (TGo4WinCond*)
   TestObject(fxConditionDir, foldername, conditionname, TGo4WinCond::Class());

   if (wcond) return wcond;

   wcond = new TGo4WinCond(conditionname);
   wcond->SetValues(xlow, xup);
   if (bindhistogram!=0)
      wcond->SetHistogram(bindhistogram);
   if (invert) wcond->Invert(invert);
   AddAnalysisCondition(wcond, foldername);
   return wcond;
}

TGo4WinCond* TGo4AnalysisObjectManager::MakeWindowCond(const char* foldername,
      const char* conditionname,
      Double_t      xlow,
      Double_t      xup,
      Double_t      ylow,
      Double_t      yup,
      const char* bindhistogram,
      Bool_t      invert) {
   TGo4WinCond* wcond = (TGo4WinCond*)
   TestObject(fxConditionDir, foldername, conditionname, TGo4WinCond::Class());

   if (wcond) return wcond;

   wcond = new TGo4WinCond(conditionname);
   wcond->SetValues(xlow, xup, ylow, yup);
   if (bindhistogram!=0)
      wcond->SetHistogram(bindhistogram);
   if (invert) wcond->Invert(invert);
   AddAnalysisCondition(wcond, foldername);
   return wcond;
}

TGo4PolyCond* TGo4AnalysisObjectManager::MakePolyCond(const char* foldername,
      const char* conditionname,
      Int_t         size,
      Float_t      (*points)[2],
      const char* bindhistogram,
      Bool_t        invert)
{
   TGo4PolyCond* pcond = (TGo4PolyCond*)
   TestObject(fxConditionDir, foldername, conditionname, TGo4PolyCond::Class());

   if (pcond) return pcond;

   TArrayF fullx(size+1), fully(size+1);
   int numpoints = size;

   for (int i=0;i<numpoints;i++) {
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
   if (bindhistogram!=0)
      pcond->SetHistogram(bindhistogram);
   if (invert) pcond->Invert(invert);
   AddAnalysisCondition(pcond, foldername);
   return pcond;
}


Bool_t TGo4AnalysisObjectManager::RemoveAnalysisCondition(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveAnalysisCondition(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxConditionDir, kTRUE);
}

TGo4ObjectStatus * TGo4AnalysisObjectManager::CreateObjectStatus(const char * name, const char* folder)
{
   TNamed* object=GetObject(name, folder);
   return CreateObjectStatus(object);
}

TGo4ObjectStatus * TGo4AnalysisObjectManager::CreateObjectStatus(TObject* obj, Bool_t fullinfo)
{
   if(obj==0) return 0;

   if(obj->InheritsFrom(TH1::Class()))
      return new TGo4HistogramStatus(dynamic_cast<TH1*> (obj),fullinfo);

   if(obj->InheritsFrom(TGo4Parameter::Class()))
      return new TGo4ParameterStatus(dynamic_cast<TGo4Parameter*> (obj), fullinfo);

   return new TGo4ObjectStatus(obj);
}

TGo4AnalysisObjectNames * TGo4AnalysisObjectManager::CreateNamesList()
{
   TRACE((11,"TGo4AnalysisObjectManager::CreateNamesList()",__LINE__, __FILE__));
   TGo4LockGuard   listguard(fxDirMutex);
   TGo4AnalysisObjectNames* namesobject=0;
   //fxTempFolder->Clear();
   const char* name = TGo4Analysis::Instance()->GetName();
   if ((name==0) || (strlen(name)==0)) name = "Go4NamesList";
   namesobject= new TGo4AnalysisObjectNames(name);
   namesobject->fxTopFolder=CreateNamesFolder(fxGo4Dir);
   TGo4PolyCond::CleanupSpecials(); // remove references to cloned TCutG in status
   return namesobject;
}

TFolder * TGo4AnalysisObjectManager::CreateNamesFolder(TFolder * objectfolder)
{
   TRACE((11,"TGo4AnalysisObjectManager::CreateNamesFolder(TFolder*)",__LINE__, __FILE__));
   if (objectfolder==0) return 0;

   TList* nameslist= new TList;
   TIter listiter(objectfolder->GetListOfFolders());
   TObject* entry;
   while((entry=listiter()) !=0)
   {
      if(entry->InheritsFrom(TFolder::Class()))
      {
         // found subfolder, process it recursively
         //cout <<"##### parsing folder "<< entry->GetName() << endl;
         TFolder* subobj= dynamic_cast<TFolder*> (entry);
         TFolder* subnames=CreateNamesFolder(subobj);
         nameslist->AddLast(subnames);
      }
      else if (entry->InheritsFrom(TTree::Class()))
      {
         // treestructure should be ObjectStatus?
         TTree* subobj= dynamic_cast<TTree*> (entry);
         TGo4TreeStructure* treestruct=CreateTreeStructure(subobj);
         nameslist->AddLast(treestruct);
      }
      else if(entry->InheritsFrom(TGo4EventElement::Class()))
      {
         TFolder* evfolder = CreateMembersFolder(entry->GetName(), entry->IsA());
         if (evfolder!=0)
            nameslist->AddLast(evfolder);
      }
      else
      {
         TGo4ObjectStatus*state = CreateObjectStatus(entry,kFALSE); // do not use full status info for nameslist
         if(state!=0)
            nameslist->AddLast(state);
      }
   } // while

   TFolder* namesfolder=fxTempFolder->AddFolder(objectfolder->GetName(),objectfolder->GetTitle(),nameslist);
   fxTempFolder->Remove(namesfolder);
   namesfolder->SetOwner(kTRUE);

   return namesfolder;
}

TFolder * TGo4AnalysisObjectManager::GetObjectFolder()
{
   return fxGo4Dir;
}

Bool_t TGo4AnalysisObjectManager::AddEventStore(TGo4EventStore * store)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddEventStore(TGo4EventStore*)",__LINE__, __FILE__));
   return (AddObjectToFolder(store,fxStoreDir,0,kFALSE));
}
Bool_t TGo4AnalysisObjectManager::RemoveEventStore(TGo4EventStore * store)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveEventStore(TGo4EventStore*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(store)
   {
      TGo4LockGuard listguard(fxDirMutex);
      fxStoreDir->Remove(store);
      rev=kTRUE;
   }
   else
   {
      rev=kFALSE;
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::AddEventSource(TGo4EventSource * source)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddEventSource(TGo4EventSource*)",__LINE__, __FILE__));
   return (AddObjectToFolder(source,fxSourceDir,0,kFALSE));
}

Bool_t TGo4AnalysisObjectManager::RemoveEventSource(TGo4EventSource* source)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveEventSource(TGo4EventSource*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(source)
   {
      TGo4LockGuard listguard(fxDirMutex);
      fxSourceDir->Remove(source);
      rev=kTRUE;
   }
   else
   {
      rev=kFALSE;
   }
   return rev;
}
Bool_t TGo4AnalysisObjectManager::AddEventProcessor(TGo4EventProcessor * pro)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddEventProcessor(TGo4EventProcessor*)",__LINE__, __FILE__));
   return (AddObjectToFolder(pro,fxProcessorDir,0,kFALSE));
}

Bool_t TGo4AnalysisObjectManager::RemoveEventProcessor(TGo4EventProcessor * pro)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveEventProcessor(TGo4EventProcessr*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(pro)
   {
      TGo4LockGuard listguard(fxDirMutex);
      fxProcessorDir->Remove(pro);
      rev=kTRUE;
   }
   else
   {
      rev=kFALSE;
   }
   return rev;
}
Bool_t TGo4AnalysisObjectManager::AddEventStructure(TGo4EventElement * ev)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddEventStructure(TGo4EventElement*)",__LINE__, __FILE__));
   return (AddObjectToFolder(ev,fxEventDir,0,kFALSE));
}

Bool_t TGo4AnalysisObjectManager::RemoveEventStructure(TGo4EventElement * ev)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveEventStructure(TGo4EventElement*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(ev!=0) {
      TGo4LockGuard listguard(fxDirMutex);
      fxEventDir->Remove(ev);
      // remove pointers to event data from all dynamic lists:
      CleanupDynamicLists(ev);
      rev=kTRUE;
   }
   return rev;
}
TGo4EventElement * TGo4AnalysisObjectManager::GetEventStructure(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetEvenStructure(char *)",__LINE__, __FILE__));
   return dynamic_cast<TGo4EventElement*> (FindObjectInFolder(fxEventDir, name));
}

Bool_t TGo4AnalysisObjectManager::AddDynamicEntry(TGo4DynamicEntry* entry)
{
   if(entry) entry->Reset();
   return AddObjectToFolder(entry, fxDynListDir, 0, kTRUE, kFALSE, kFALSE);
}

Bool_t TGo4AnalysisObjectManager::ResetBackStores(Bool_t clearflag)
{
   TRACE((11,"TGo4AnalysisObjectManager::ResetBackStores()",__LINE__, __FILE__));
   Bool_t rev=kTRUE;
   TIter iter(fxStoreDir->GetListOfFolders());
   TObject* entry = 0;
   while((entry=iter()) !=0) {
      TGo4BackStore* bs= dynamic_cast<TGo4BackStore*>(entry);
      if(bs!=0) bs->Reset(clearflag);
   } // while
   return rev;
}

void TGo4AnalysisObjectManager::CloseAnalysis()
{
   TRACE((14,"TGo4AnalysisObjectManager::CloseAnalysis()",__LINE__, __FILE__));
   ResetCurrentDynList();
   fiDynListCount=0;
   TGo4Analysis::Instance()->Message(0,"Analysis Object Manager  --  Dynamic List was reset.");
}

void TGo4AnalysisObjectManager::SaveObjects(TFile* file)
{
   if(!file) return;
   TGo4Analysis::Instance()->Message(0,"Analysis Object Manager  --  Saving objects to file %s",file->GetName());
   fxGo4Dir->RecursiveRemove(fxTreeDir); // do not save the trees twice!
   fxGo4Dir->RecursiveRemove(fxAnalysisDir); // do not save eventclass refs
   /////// old implementation: just write top folder
   //fxGo4Dir->Write(0, TObject::kOverwrite);
   /////// end old implementation ///////////////////////////////
   ////// begin new implementation: transform folder into subdirectories of output file
   TDirectory* savdir=gDirectory;
   file->cd();
   // file->Delete("T*;*"); // remove old contents (get rid of deleted dynamic objects)
   // note: we always use RECREATE option on saving now. No need to cleanup old file!
   SaveFolder(fxGo4Dir);
   file->Write(0,TObject::kOverwrite); // write all appended objects and subdirs
   savdir->cd();
   RemoveFromDir(fxGo4Dir,file); // prevent object deletion on closing the file
   ////// end new implementation ////////////////
   fxGo4Dir->Add(fxTreeDir);
   fxGo4Dir->Add(fxAnalysisDir);
}
Bool_t TGo4AnalysisObjectManager::AddParameter(TGo4Parameter * par, const char* subfolder)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddParameter(TGo4Parameter *)",__LINE__, __FILE__));
   return (AddObjectToFolder(par,fxParameterDir,subfolder,kTRUE));
}

Bool_t TGo4AnalysisObjectManager::SetParameter(const char* name, TGo4Parameter * par, TFolder* parent)
{
   TRACE((11,"TGo4AnalysisObjectManager::SetParameter(const char*, TGo4Parameter*)",__LINE__, __FILE__));

   if (par==0) return kFALSE;

   Bool_t rev(kFALSE);
   TFolder* topfolder = parent ? parent : fxParameterDir;
   TGo4Parameter* oldpar= dynamic_cast<TGo4Parameter*> (topfolder->FindObjectAny(name));
   if(oldpar!=0) {
      // update existing parameter of given name
      rev = oldpar->UpdateFrom(par);
      //cout << "++++ Updated parameter "<< name<<" from parameter "<< par->GetName() << endl;
      //oldpar->PrintParameter();
   } else {
      // parameter not yet existing, add external parameter as a copy:
      TGo4Parameter* clonedpar=dynamic_cast<TGo4Parameter*>(par->Clone());

      const char* separ = strrchr(name, '/');
      if (separ!=0) {
         TString fname(name, separ-name);
         rev=AddObjectToFolder(clonedpar,topfolder, fname.Data(), kTRUE);
         //cout << "++++ Added new parameter "<< clonedpar->GetName()<<" to folder "<<topfolder->GetName()<<"/"<< subfolder << endl;
      } else
         rev=AddObjectToFolder(clonedpar,topfolder,0,kTRUE);
   }
   return rev;
}

Bool_t TGo4AnalysisObjectManager::SetParameterStatus(const char* name, TGo4ParameterStatus* status, TFolder* parent)
{
   TRACE((11,"TGo4AnalysisObjectManager::SetParameter(char*, TGo4Parameter*)",__LINE__, __FILE__));

   if (status==0) return kFALSE;

   Bool_t rev (kFALSE);
   TFolder* topfolder = (parent!=0) ? parent : fxParameterDir;

   TGo4Parameter* oldpar= dynamic_cast<TGo4Parameter*> (topfolder->FindObjectAny(name));
   if(oldpar!=0) {
      // update existing parameter of given name
      rev = status->UpdateParameterValues(oldpar);
   } else {
      // parameter not yet exisiting, add external paramter as a copy:
      TGo4Parameter* clonedpar = status->CreateParameter();
      // find out destination folder for new parameter from name:

      const char* separ = strrchr(name, '/');
      if (separ!=0) {
         TString fname(name, separ-name);
         rev=AddObjectToFolder(clonedpar,topfolder, fname.Data(), kTRUE);
      } else {
         rev=AddObjectToFolder(clonedpar,topfolder,0,kTRUE);
      }
   }
   return rev;
}



TGo4Parameter * TGo4AnalysisObjectManager::GetParameter(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetParameter(char*)",__LINE__, __FILE__));
   TGo4Parameter* rev=dynamic_cast<TGo4Parameter *> (FindObjectInFolder(fxParameterDir,name));
   if(rev==0) rev= dynamic_cast<TGo4Parameter *> (FindObjectInFolder(fxUserDir,name));
   return rev;
}

Bool_t TGo4AnalysisObjectManager::RemoveParameter(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveParameter(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxParameterDir, kTRUE);
}

Bool_t TGo4AnalysisObjectManager::AddPicture(TGo4Picture * pic, const char* subfolder)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddPicture(TGo4Picture *)",__LINE__, __FILE__));
   if(pic)
   {
      //pic->LocateObjectsFrom(fxGo4Dir);
      return (AddObjectToFolder(pic,fxPictureDir,subfolder,kTRUE));
   }
   else
      return kFALSE;
}

Bool_t TGo4AnalysisObjectManager::SetPicture(const char* name, TGo4Picture * pic, TFolder* parent)
{
   TRACE((11,"TGo4AnalysisObjectManager::SetPicture(char*, TGo4Picture*)",__LINE__, __FILE__));

   if (pic==0) return kFALSE;

   Bool_t rev(kTRUE);
   TFolder* topfolder=0;
   if(parent)
      topfolder=parent;
   else
      topfolder=fxPictureDir;

   TGo4Picture* oldpic = dynamic_cast<TGo4Picture *> (topfolder->FindObjectAny(name));
   if(oldpic!=0) {
      // update existing picture of given name
      oldpic->UpdateFrom(pic);
      //            cout << "++++ Updated picture "<< name<<" from picture "<< pic->GetName() << endl;
   } else {
      // picture not yet exisiting, add external as a copy:
      TGo4Picture* clonedpic = dynamic_cast<TGo4Picture*>(pic->Clone());
      // find out destination folder for new pic from name:

      const char* separ = strrchr(name, '/');
      if (separ!=0) {
        TString fname(name, separ-name);
        rev = AddObjectToFolder(clonedpic, topfolder, fname.Data(),kTRUE);
      } else
        rev = AddObjectToFolder(clonedpic, topfolder, 0, kTRUE);
   }
   return rev;
}

TGo4Picture * TGo4AnalysisObjectManager::GetPicture(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetPicture(char*)",__LINE__, __FILE__));
   TGo4Picture* rev=dynamic_cast<TGo4Picture *> (FindObjectInFolder(fxPictureDir, name));
   if(rev==0) rev=dynamic_cast<TGo4Picture *> (FindObjectInFolder(fxUserDir, name));
   return rev;
}

Bool_t TGo4AnalysisObjectManager::RemovePicture(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemovePicture(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxPictureDir, kTRUE);
}

Bool_t TGo4AnalysisObjectManager::AddCanvas(TCanvas * can, const char* subfolder)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddCanvas(TCanvas *)",__LINE__, __FILE__));
   Bool_t rev = AddObjectToFolder(can,fxCanvasDir,subfolder,kFALSE);
   if(rev && can) gROOT->GetListOfCanvases()->Remove(can);
   // make sure that root has no other reference to our canvas
   return rev;
}

TCanvas * TGo4AnalysisObjectManager::GetCanvas(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::GetPicture(char*)",__LINE__, __FILE__));
   TCanvas* rev=dynamic_cast<TCanvas*> (FindObjectInFolder(fxCanvasDir, name));
   if(rev==0) rev=dynamic_cast<TCanvas*> (FindObjectInFolder(fxCanvasDir, name));
   return rev;
}

Bool_t TGo4AnalysisObjectManager::RemoveCanvas(const char * name)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemovePicture(char*)",__LINE__, __FILE__));
   return RemoveObjectFromFolder(name, fxCanvasDir, kTRUE);
}





Bool_t TGo4AnalysisObjectManager::LoadObjects(TFile* obfile)
{
   TRACE((11,"TGo4AnalysisObjectManager::LoadObjects(TFile*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(obfile)
   {
      TObject* ob=obfile->Get(TGo4AnalysisObjectManager::fgcTOPFOLDER);
      TFolder* top= dynamic_cast<TFolder*>(ob);
      if(top)
      {
         // kept for backward compatibility: read folder struct directly
         //cout << "found top go4 folder "<< endl;
         rev=LoadFolder(top,fxGo4Dir,kFALSE);
      }
      else
      {
         // new: convert directory structure of file into folders
         //cout <<"LoadObjects with Dirscan..." << endl;
         rev=LoadFolder(obfile,fxGo4Dir,kFALSE);
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
   Bool_t processtrees = (fiDynListCount % fiDynListInterval)==0;
   TGo4DynamicList::ProcessEntries(fxDynListDir, processtrees, GetDynListInterval());
   if(processtrees) ResetBackStores();
}

void TGo4AnalysisObjectManager::PrintDynamicList()
{
   TGo4DynamicList::PrintEntries(fxDynListDir);
}

Bool_t TGo4AnalysisObjectManager::AddDynamicHistogram(const char* name,
      const char* histo,
      const char* hevx, const char* hmemx,
      const char* hevy, const char* hmemy,
      const char* hevz, const char* hmemz,
      const char* condition,
      const char* cevx, const char* cmemx,
      const char* cevy, const char* cmemy)
{
   if(name==0 || histo==0 || hevx==0 || hmemx==0) return kFALSE;
   TGo4HistogramEntry* entry = new TGo4HistogramEntry(name);

   entry->SetHistogramName(histo);

   if ((hevx!=0) && (hmemx!=0)) {
      entry->SetHisEventName(0, hevx);
      entry->SetHisVarName(0, hmemx);
   }

   if ((hevy!=0) && (hmemy!=0)) {
      entry->SetHisEventName(1, hevy);
      entry->SetHisVarName(1, hmemy);
   }

   if ((hevz!=0) && (hmemz!=0)) {
      entry->SetHisEventName(2, hevz);
      entry->SetHisVarName(2, hmemz);
   }

   if((condition!=0) && (cevx!=0) && (cmemx!=0)) {
      entry->SetConditionName(condition);
      entry->SetConEventName(0, cevx);
      entry->SetConVarName(0, cmemx);
      if((cevy!=0) && (cmemy!=0)) {
         entry->SetConEventName(1, cevy);
         entry->SetConVarName(1, cmemy);
      }
   }

   return AddDynamicEntry(entry);
}

Bool_t TGo4AnalysisObjectManager::AddTreeHistogram(const char* hisname, const char* treename, const char* varexp, const char* cutexp)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddTreeHistogram(char*,...)",__LINE__, __FILE__));
   //
   Bool_t rev=kFALSE;
   TGo4TreeHistogramEntry* tentry =
      new TGo4TreeHistogramEntry(hisname, treename, varexp, cutexp);
   if(AddDynamicEntry(tentry)) {
      TGo4Analysis::Instance()->Message(0,"Analysis added tree histogram %s to dynamic list", tentry->GetName());
      rev=kTRUE;
   } else {
      TGo4Analysis::Instance()->Message(0,"!!! Analysis AddTreeHistogram- ERROR: entry %s already exists !!!", tentry->GetName());
      delete tentry;
   }
   return rev;
}


Bool_t TGo4AnalysisObjectManager::RemoveDynamicEntry(const char* entryname)
{
   return RemoveObjectFromFolder(entryname, fxDynListDir, kTRUE);
}


TFolder* TGo4AnalysisObjectManager::FindSubFolder(TFolder* parent, const char* subfolder, Bool_t create)
{
   TRACE((11,"TGo4AnalysisObjectManager::FindSubFolder(TFolder*, const char*, Bool_t)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   TFolder* result=0;
   if(parent==0) return 0;
   if(subfolder==0) return parent;
   const char* separ = strchr(subfolder,'/'); // find end of first subfolder string
   if(separ!=0) {
      // we have subfolder of subfolder, process recursively
      TString subname(subfolder, separ - subfolder);

      //cout <<" fffffff Searching for subfolder "<< subname << endl;
      TFolder* nextsubfolder = FindSubFolder(parent, subname.Data(), create); // get folder directly under parent
      result = FindSubFolder(nextsubfolder, separ+1,create); // search rest of path in this folder
   } else {
      // only one level of subfolder, find it directly
      TIter listiter(parent->GetListOfFolders());
      TObject* entry;
      while((entry=listiter()) !=0)
         if(entry->InheritsFrom(TFolder::Class()))
            if(!strcmp(subfolder,entry->GetName())) {
               result= dynamic_cast<TFolder*>(entry);
               break;
            }
      if(result==0 && create)
         result=parent->AddFolder(subfolder,"UserFolder"); // create new subfolder if not found
   } // if (endname!=0)
   return result;
}


Bool_t TGo4AnalysisObjectManager::AddObjectToFolder(TObject * ob,
                                                    TFolder* fold,
                                                    const char* subfolder,
                                                    Bool_t replace,
                                                    Bool_t uniquename,
                                                    Bool_t resetbits)
{
   TRACE((11,"TGo4AnalysisObjectManager::AddObjectToFolder(TObject *, TFolder*, const char*, Bool_t)",__LINE__, __FILE__));

   TGo4LockGuard  dirguard(fxDirMutex);
   if(fold==0) return kFALSE;

   if (ob==0) {
     TRACE((12,"TGo4AnalysisObjectManager::AddObjectToFolder - zero object",__LINE__, __FILE__));
     return kFALSE;
   }

   if(resetbits) {
      ob->ResetBit(TGo4Status::kGo4NoReset);
      ob->ResetBit(TGo4Status::kGo4CanDelete); // default behavior: allow reset, disable delete
   }

   TString subname;
   if (subfolder && (strlen(subfolder)!=0)) {
      subname = subfolder;
      if (subname[subname.Length()-1] == '/')
         subname.Resize(subname.Length()-1);
   }

   TObject* oldob=0;
   if(uniquename) {
      // look for object of identical name anywhere in top folder
      oldob=fold->FindObjectAny( ob->GetName() );
   } else {
      TString obname;

      if (subname.Length()>0)
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

         cout << "Delete object " <<  oldob << "  name = " << oldob->GetName() << " isgpap = " << (oldob == gPad) << endl;

         delete oldob;
      } else
         return kFALSE; // do not overwrite old one
   }

   TFolder* addDir(0);
   if(subfolder)
      addDir = FindSubFolder(fold, subname, kTRUE);
   if(addDir==0) addDir = fold;
   // if(subfolder)
   addDir->Add(ob);

   return kTRUE;
}


Bool_t TGo4AnalysisObjectManager::RemoveObjectFromFolder(const char* fullname, TFolder* fold, Bool_t isDel)
{
   TRACE((11,"TGo4AnalysisObjectManager::RemoveObjectFromFolder(const char*, TFolder*, Bool_t)",__LINE__, __FILE__));
   //
   if(fold==0) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TNamed* obj = 0;
   Int_t buflen=fguSUBFOLDERMAXLEN;
   char buffer[fguSUBFOLDERMAXLEN];
   if(fullname) {
      strncpy(buffer,fullname,buflen-10);
      obj = dynamic_cast<TNamed*> (fold->FindObjectAny(buffer));
   }

   if(obj!=0) {
      fold->RecursiveRemove(obj);
      CleanupDynamicLists(obj);
      if (isDel) delete obj;
      TGo4Analysis::Instance()->Message(0,"Analysis: removed object %s",buffer);
      return kTRUE;
   }

   TGo4Analysis::Instance()->Message(0,"!!! Analysis: removing object %s FAILED !!!",buffer);
   return kFALSE;
}

Bool_t TGo4AnalysisObjectManager::LoadFolder(TFolder* source, TFolder* destination, Bool_t replace)
{
   if (source==0) return kFALSE;
   TRACE((11,"TGo4AnalysisObjectManager::LoadFolder(TFolder*, TFolder*, Bool_t replace)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   Bool_t rev=kTRUE;
   TIter folderiter(source->GetListOfFolders());
   TObject* ob=0;
   while(( ob=folderiter() ) !=0)
   {
      if(ob->InheritsFrom(TFolder::Class()))
      {
         TFolder* subfolder =dynamic_cast<TFolder*> (ob);
         if(subfolder)
         {
            //cout <<"LLLLLL Load folder scanning subfolder "<< subfolder->GetName() << endl;
            TFolder* subdest=FindSubFolder(destination,subfolder->GetName(), kTRUE);
            LoadFolder(subfolder,subdest,replace); // recursively scan all subfolders
         }
      }
      else
      {
         rev=PutToFolder(ob,destination,replace);
      }
   } // while(..)
   return rev;
}

Bool_t TGo4AnalysisObjectManager::LoadFolder(TDirectory* source, TFolder* destination, Bool_t replace)
{
   TRACE((11,"TGo4AnalysisObjectManager::LoadFolder(TDirectory*, TFolder*, Bool_t replace)",__LINE__, __FILE__));
   if(!source || !destination) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   Bool_t rev=kTRUE;
   TDirectory* savdir=gDirectory;
   source->cd(); // this is necessary to let the TKey::ReadObj work!
   source->ReadKeys();
   TIter keyiter(source->GetListOfKeys());
   TObject* keyob=0;
   TKey* key=0;
   while(( keyob=keyiter() ) !=0)
   {
      key=dynamic_cast<TKey*>(keyob);
      if(key==0)
      {
         TGo4Analysis::Instance()->Message(3,"Analysis LoadFolder: ZERO key in directory %s",
               source->GetName());
         return kFALSE;
      }
      //cout <<"Reading key "<<key->GetName() << endl;
      TObject* ob = key->ReadObj();
      if(ob==0)
      {
         TGo4Analysis::Instance()->Message(2,"Analysis LoadFolder: Retrying to read key %s ...",
               key->GetName());
         ob=source->Get(key->GetName());
         if(ob==0)
         {
            TGo4Analysis::Instance()->Message(3,"Analysis LoadFolder: Failed to read key %s !",
                  key->GetName());
            return kFALSE;
         }
      }

      if(ob->InheritsFrom(TDirectory::Class()))
      {
         TDirectory* subdir =dynamic_cast<TDirectory*> (ob);
         if(subdir)
         {
            //cout <<"LLLLLL Load folder dir scanning for subdir "<< subdir->GetName() << endl;
            Bool_t inpicturefolder=kFALSE;
            if(!strcmp(subdir->GetName(),fgcPICTFOLDER)) inpicturefolder=kTRUE;
            if(inpicturefolder) fbSuppressLoadHistograms=kTRUE;
            TFolder* subdest=FindSubFolder(destination,subdir->GetName(), kTRUE);
            rev=LoadFolder(subdir,subdest,replace); // recursively scan all subfolders
            if(inpicturefolder) fbSuppressLoadHistograms=kFALSE;
         }
      }
      else
      {
         rev=PutToFolder(ob,destination,replace);
      }
   } // while(..)
   savdir->cd();
   return rev;
}

Bool_t TGo4AnalysisObjectManager::PutToFolder(TObject* ob, TFolder* destination, Bool_t replace)
{
   if(!ob || !destination) return kFALSE;
   Bool_t rev=kTRUE;
   //cout <<"put to folder for "<<ob->GetName() << endl;
   if (ob->InheritsFrom(TGo4DynamicEntry::Class())) {
      // new autosave file structure will save dynamic entries independently:
      TGo4DynamicEntry* dentry =dynamic_cast<TGo4DynamicEntry*> ( ob->Clone() ); // deep copy of source object!
      AddDynamicEntry(dentry);
      //cout <<"Added dynamic entry from file: "<<dentry->GetName() << endl;
   } else
   if(ob->InheritsFrom(TGo4Parameter::Class())) {
      // parameters never replaced, but updated
      TGo4Parameter* par = dynamic_cast<TGo4Parameter*>(ob);
      SetParameter(ob->GetName(),par,destination);
   } else
   if(ob->InheritsFrom(TGo4Picture::Class())) {
      // pictures never replaced, but updated
      TGo4Picture* pic =dynamic_cast<TGo4Picture*>(ob);
      //cout <<"LLLLLoad Folder SetsPicture!!!!!" << endl;
      SetPicture(ob->GetName(),pic,destination);
   } else
   if(ob->InheritsFrom(TGo4Condition::Class())) {
      // conditions not replaced, but updated
      TGo4Condition* con =dynamic_cast<TGo4Condition*>(ob);
      SetAnalysisCondition(ob->GetName(),con,kTRUE, destination);
   } else
   if(ob->InheritsFrom(TH1::Class())) {
      if(fbSuppressLoadHistograms) return kFALSE;
      // test: do not clone histos, but change dir from asf file to memory
      TH1* his = dynamic_cast<TH1*>(ob);
      if(AddObjectToFolder(his,destination,0,replace,kFALSE,kFALSE)) {
         his->SetDirectory(gROOT); // set directory for histos, needed for TTree::Draw
         TGo4Analysis::Instance()->Message(0,"Analysis LoadFolder: Histogram %s was loaded.",
               his->GetName());
         //cout <<"Changed directory of histo "<<his->GetName() << endl;
      }
   } else
   if(ob->InheritsFrom(TCanvas::Class())) {
      TObject* addob = ob->Clone(); // deep copy of source object!
      if(AddObjectToFolder(addob, destination, 0, replace)) {
         TGo4Analysis::Instance()->Message(-1,"Analysis LoadFolder: Object %s was loaded.", addob->GetName());
      } else {
         // object already there and noreplace set: delete clone
         delete addob;
         if (gPad==addob) gPad = 0;
      }
   } else {
      cout << "Make object clone " << ob->GetName() << "  class = " << ob->ClassName() << endl;

      TObject* addob = ob->Clone(); // deep copy of source object!
      if(AddObjectToFolder(addob,destination,0,replace)) {
         TGo4Analysis::Instance()->Message(-1,"Analysis LoadFolder: Object %s was loaded.", addob->GetName());
      } else {
         delete addob;
      }
   }
   return rev;
}



Bool_t TGo4AnalysisObjectManager::SaveFolder(TFolder* source)
{
   if (source==0) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   Bool_t rev=kTRUE;
   TIter folderiter(source->GetListOfFolders());
   TObject* ob=0;
   while(( ob=folderiter() ) !=0) {
      if(ob->InheritsFrom(TFolder::Class())) {
         TFolder* subfolder =dynamic_cast<TFolder*> (ob);
         if(subfolder)
         {
            TDirectory* currentdir=gDirectory;
            const char* subfoldername=subfolder->GetName();
            TDirectory* subdir=dynamic_cast<TDirectory*>(currentdir->Get(subfoldername));
            if(subdir==0)
            {
               subdir=currentdir->mkdir(subfoldername,"subdir");
               //cout <<"SSSS Save folder created subdir "<<subfoldername << endl;
            }
            else
            {
               //cout <<"SSSS Save folder found existing subdir "<<subfoldername << endl;
            }

            if(subdir)
            {
               subdir->cd();
               //cout <<"SSSS Save folder scanning subfolder "<< subfolder->GetName() << endl;
               SaveFolder(subfolder); // recursively scan all subfolders
            }
            else
            {
               TGo4Analysis::Instance()->Message(2,"Analysis SaveFolder: Could not assign subdirectory %s to folder.",
                     subfoldername);
            }
            currentdir->cd();
         }
      } else
         AppendToDir(ob,gDirectory);
   } // while(..)
   return rev;
}

void TGo4AnalysisObjectManager::AppendToDir(TObject* ob, TDirectory* dir)
{
   if(!ob || !dir) return;
   // note: we do not check for old objects anymore, because
   // file is generally cleaned up before writing now JA
   //TObject* oldob=dir->Get(ob->GetName());
   //if(oldob)
   //   {
   ////      dir->RecursiveRemove(oldob);
   ////      delete oldob;
   //   }
   dir->Append(ob);
}

void TGo4AnalysisObjectManager::RemoveFromDir(TFolder* fold, TDirectory* dir)
{
   if((fold==0) || (dir==0)) return;

   TIter iter(fold->GetListOfFolders());
   TObject* ob = 0;
   while((ob = iter()) !=0) {
      if(ob->InheritsFrom(TFolder::Class())) {
         TFolder* subfolder =dynamic_cast<TFolder*> (ob);
         if(subfolder!=0)
            RemoveFromDir(subfolder,dir); // recursively scan all subfolders
      } else
         dir->RecursiveRemove(ob);
   }
}

Int_t TGo4AnalysisObjectManager::PrintFolder(TFolder* fold, Option_t* opt, const char* expression)
{
   if(fold==0) return 0;

   TRACE((11,"TGo4AnalysisObjectManager::PrintFolder(TFolder*, Option_t*)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   Int_t totalsize=0;
   TROOT::IndentLevel();
   TROOT::IncreaseDirLevel();
   cout <<"+Folder "<<fold->GetName()<<" content:" << endl;
   TIter listiter(fold->GetListOfFolders());
   TObject* ob=0;
   while((ob=listiter())!=0) {
      if(ob->InheritsFrom(TFolder::Class()))
         totalsize+=PrintFolder(dynamic_cast<TFolder*>(ob),opt,expression);
      else
         if(IsMatching(ob->GetName(),expression)) {
            TROOT::IndentLevel();
            ob->Print(opt);
            TGo4ObjectStatus* temp = CreateObjectStatus(ob);
            if(temp!=0) totalsize += temp->GetObjectSize();
            delete temp;
         }
   } // while
   TROOT::DecreaseDirLevel();
   TROOT::IndentLevel();
   cout <<"++++End "<<fold->GetName()<<"++++++++++" << endl;
   return totalsize;
}

Bool_t TGo4AnalysisObjectManager::ClearFolder(TFolder* fold)
{
   TRACE((11,"TGo4AnalysisObjectManager::ClearFolder(TFolder*, Option_t*)",__LINE__, __FILE__));
   if(fold==0) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TIter iter(fold->GetListOfFolders());
   TObject* ob=0;
   while((ob=iter())!=0)
      if(ob->InheritsFrom(TFolder::Class()))
         ClearFolder(dynamic_cast<TFolder*>(ob));
      else
         ClearObject(ob);
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::ClearObject(TObject* ob)
{
   TRACE((11,"TGo4AnalysisObjectManager::ClearObject(TObject*)",__LINE__, __FILE__));
   Bool_t rev=kFALSE;
   if(ob)
   {
      if(ob->TestBit(TGo4Status::kGo4NoReset)) return kFALSE;
      rev=kTRUE;
      if(ob->InheritsFrom(TH1::Class()))
      {
         TH1* his= dynamic_cast<TH1*>(ob);
         if (his) his->Reset(); // histogram has no Clear implementation!
      }
      else if(ob->InheritsFrom(TGo4DynamicEntry::Class()))
      {
         TGo4DynamicEntry* entry = dynamic_cast<TGo4DynamicEntry*>(ob);
         if (entry!=0) entry->Reset(); // dynamic entry has no Clear implementation!
      }
      else if(ob->InheritsFrom(TGo4Picture::Class()))
      {
         TGo4Picture* pic = dynamic_cast<TGo4Picture*>(ob);
         if (pic) pic->Reset(); // picture has no Clear implementation!
      }
      else if(ob->InheritsFrom(TGraph::Class()))
      {
         TGraph* gr= dynamic_cast<TGraph*>(ob);
         if (gr) {
            Int_t pn=gr->GetN();
            gr->Set(0); // clear array of points
            gr->Set(pn); // this should set all to 0
         }
      }
      else if(ob->InheritsFrom(TMultiGraph::Class()))
      {
         TMultiGraph* mg= dynamic_cast<TMultiGraph*>(ob);
         if (mg) {
            TIter liter(mg->GetListOfGraphs());
            TObject* ob=0;
            while((ob=liter())!=0)
               ClearObject(ob);
         }
      }
      else if(ob->InheritsFrom(TGo4EventElement::Class()))
      {
         // nop, we do not allow a user clear from gui
         // on any event
         rev=kFALSE;
      }
      else if(ob->InheritsFrom(TTree::Class()))
      {
         // nop, we do not allow a user clear from gui
         // on any tree
         rev=kFALSE;
      }
      else
      {
         ob->Clear();
         // use virtual Clear of all objects
         // make sure that TNamed::Clear is overwritten in subclasses
      }
   }
   return rev;
}


Bool_t TGo4AnalysisObjectManager::DeleteFolder(TFolder* fold)
{
   TRACE((11,"TGo4AnalysisObjectManager::DeleteFolder(TFolder*, Option_t*)",__LINE__, __FILE__));
   if (fold==0) return kFALSE;

   TGo4LockGuard  dirguard(fxDirMutex);
   TIter iter(fold->GetListOfFolders());
   TObject* ob=0;
   while((ob=iter())!=0)
      if(ob->InheritsFrom(TFolder::Class()))
         DeleteFolder(dynamic_cast<TFolder*>(ob));
      else
         DeleteObject(ob);
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::DeleteObject(TObject* ob)
{
   TRACE((11,"TGo4AnalysisObjectManager::DeleteObject(TObject*)",__LINE__, __FILE__));
   if(ob==0 || !ob->TestBit(TGo4Status::kGo4CanDelete) ) return kFALSE;
   fxGo4Dir->RecursiveRemove(ob);
   CleanupDynamicLists(ob);
   delete ob;
   return kTRUE;;
}



Bool_t TGo4AnalysisObjectManager::ProtectFolder(TFolder* fold, const Option_t* flags)
{
   if(fold==0) return kFALSE;
   TGo4LockGuard  dirguard(fxDirMutex);
   TIter listiter(fold->GetListOfFolders());
   TObject* ob=0;
   while((ob=listiter.Next())!=0)
      if(ob->InheritsFrom(TFolder::Class()))
         ProtectFolder(dynamic_cast<TFolder*>(ob), flags);
      else
         ProtectObject(ob, flags);
   return kTRUE;
}

Bool_t TGo4AnalysisObjectManager::ProtectObject(TObject* ob, const Option_t* flags)
{
   if((ob==0) || (flags==0)) return kFALSE;
   TString opt=flags;
   if(opt.Contains("+D")) ob->ResetBit(TGo4Status::kGo4CanDelete);
   if(opt.Contains("-D")) ob->SetBit(TGo4Status::kGo4CanDelete);
   if(opt.Contains("+C")) ob->SetBit(TGo4Status::kGo4NoReset);
   if(opt.Contains("-C")) ob->ResetBit(TGo4Status::kGo4NoReset);
   return kTRUE;
}

void TGo4AnalysisObjectManager::CleanupDynamicLists(TObject* oldobject)
{
   TRACE((11,"TGo4AnalysisObjectManager::CleanupDynamicLists(TNamed*)",__LINE__, __FILE__));
   TGo4LockGuard  dirguard(fxDirMutex);
   if (oldobject==0) return;
   // scan all dynamic lists for this object:
   if(oldobject->InheritsFrom(TH1::Class()) ||
         oldobject->InheritsFrom(TGo4Condition::Class()) ||
         oldobject->InheritsFrom(TGo4EventElement::Class()))
      TGo4DynamicList::CleanupPointerInEntries(fxDynListDir, oldobject);
}


void TGo4AnalysisObjectManager::PrintConditions(const char* expression)
{
   TRACE((11,"TGo4AnalysisObjectManager::PrintConditions(const char*)",__LINE__, __FILE__));
   Int_t totalsize = PrintFolder(fxConditionDir, "*", expression);
   cout << "___________________________________________________________" << endl;
   cout << "Total size of all conditions is: " << totalsize << " bytes." << endl;
}

void TGo4AnalysisObjectManager::PrintHistograms(const char* expression)
{
   TRACE((11,"TGo4AnalysisObjectManager::PrintHistograms(const char*)",__LINE__, __FILE__));
   Int_t totalsize = PrintFolder(fxHistogramDir, "*", expression);
   cout << "___________________________________________________________" << endl;
   cout << "Total size of all histograms is: " << totalsize << " bytes." << endl;
}

void TGo4AnalysisObjectManager::PrintParameters(const char* expression)
{
   TRACE((11,"TGo4AnalysisObjectManager::PrintParameters(const char*)",__LINE__, __FILE__));
   Int_t totalsize = PrintFolder(fxParameterDir, "*", expression);
   cout << "___________________________________________________________" << endl;
   cout << "Total size of all parameters is: " << totalsize << " bytes." << endl;
}


TObject* TGo4AnalysisObjectManager::NextMatchingObject(const char* expr,
      const char* folder,
      Bool_t reset)
{
   if(reset) {
      delete fxMatchIterator;
      delete fxMatchList;
      fxMatchList=CreateObjectList(expr,folder);
      fxMatchIterator=fxMatchList->MakeIterator();
   }
   return fxMatchIterator->Next();
}

TList* TGo4AnalysisObjectManager::CreateObjectList(const char* expr, const char* folder)
{
   TGo4LockGuard  dirguard(fxDirMutex);
   TFolder* searchfold;
   if(folder==0 || !strcmp(folder,fgcTOPFOLDER))
      searchfold=fxGo4Dir; // default: search topfolder
   else
      searchfold=FindSubFolder(fxGo4Dir, folder, kFALSE);
   return (CreateObjectList(expr, searchfold));
}


TList* TGo4AnalysisObjectManager::CreateObjectList(const char* expr, TFolder* fold)
{
   TList* result = new TList;
   if(fold!=0) {
      TIter iter(fold->GetListOfFolders());
      TObject* entry=0;
      while((entry = iter())!=0) {
         if(entry->InheritsFrom(TFolder::Class())) {
            TFolder* subfold=dynamic_cast<TFolder*>(entry);
            TList* sublist=CreateObjectList(expr,subfold);
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

Bool_t TGo4AnalysisObjectManager::IsMatching(const char* string, const char* expression)
{
   if(expression==0) return kTRUE;
   Bool_t ismatching=kFALSE;
   TString entrystring=string;
   TRegexp reg(expression,kTRUE);
   if(!strcmp(expression,"*"))
      ismatching=kTRUE; // take all in this folder
   //               else if (strstr(expression,string))
   //                  ismatching=kTRUE; // expression just contained in name
   else if (entrystring.Index(reg,0)!=kNPOS)
      ismatching=kTRUE; // root regular expression class
   else
      ismatching=kFALSE;
   return ismatching;
}


TObject* TGo4AnalysisObjectManager::FindObjectInFolder(TFolder* folder, const char* fullname)
{
   TRACE((12,"TGo4AnalysisObjectManager::FindObjectInFolder(TFolder*, const char*)",__LINE__, __FILE__));
   TGo4LockGuard  listguard(fxDirMutex);

   return folder ? folder->FindObjectAny(fullname) : 0;
}


TObject* TGo4AnalysisObjectManager::TestObject(TFolder*     folder,
      const char* &pathname,
      const char* objectname,
      const TClass* cl)
{
   TString fullname;
   if ((pathname!=0) && (strlen(pathname)==0)) pathname = 0;
   if (pathname) {
      fullname = pathname;
      fullname += "/";
   }
   fullname += objectname;

   TObject* obj = FindObjectInFolder(folder, fullname);

   if ((obj!=0) && (!obj->InheritsFrom(cl))) {
      RemoveObjectFromFolder(fullname, folder, kTRUE);
      obj = 0;
   }

   fbCreatedinMake = (obj==0);
   return obj;
}
