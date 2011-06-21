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

#include "TGo4CompositeEvent.h"

#include "Riostream.h"

#include "TTree.h"
#include "TROOT.h"
#include "TObjArray.h"

R__EXTERN TTree *gTree;

TGo4CompositeEvent::TGo4CompositeEvent() :
   TGo4EventElement(),
   fNElements(0),
   fEventElements(0)
{
}

TGo4CompositeEvent::TGo4CompositeEvent(const char*aName, const char* aTitle, Short_t aBaseCat) :
   TGo4EventElement(aName,aTitle, aBaseCat),
   fNElements(0),
   fEventElements(0)
{
   fEventElements = new TObjArray(12);
}

TGo4CompositeEvent::~TGo4CompositeEvent()
{
   if(fEventElements!=0) {

      // SL: why it is here ????
      fEventElements->Sort();

      if(!fEventElements->IsEmpty())
         fEventElements->Delete();

      delete fEventElements;
   }
}


void TGo4CompositeEvent::makeBranch(TBranch *parent)
{
   if (fEventElements!=0)
      for(Int_t i=0; i<=fEventElements->GetLast();i++) {
         TGo4EventElement** par = (TGo4EventElement**) &((*fEventElements)[i]);
         if (par && *par) {
            TBranch *b = gTree->TTree::Branch(Form("%s.",(*par)->GetName()), (*par)->ClassName(),par,4000,99);
            (*par)->makeBranch(b);
         }
      }
   TGo4EventElement::makeBranch( parent );
}


Int_t  TGo4CompositeEvent::activateBranch(TBranch *branch, Int_t splitLevel, Int_t init)
{

   // synchronize with TTree for Composite Event
   if (branch) {
      TString cad = branch->GetName();
      TTree *tree = branch->GetTree();

      TGo4CompositeEvent *dump=this;
      tree->SetBranchStatus(cad.Data(),1);
      tree->SetBranchAddress(cad.Data(), &dump);
      cad+="*";
      tree->SetBranchStatus(cad.Data(),1);

      TGo4EventElement::activateBranch(branch, splitLevel);

      if (fDebug)
         printf("##### TGo4CompositeEvent::activateBranch called from obj:%s \n",this->GetName());

      TObjArray *br = tree->GetListOfBranches();

      Int_t i = init;

      if (fDebug)
         printf("-I-TGo4CompositeEvent::activateBranch from obj:%s bname%s Elements :%d  index:%d\n",
               this->GetName(),cad.Data(), fNElements,init);

      while (i < init+fNElements) {
         i++;
         TBranch* b = (TBranch*) br->At(i);
         if (b==0) continue;

         if (fDebug)
            printf("-I TGo4CompositeEvent::activateBranch from obj:%s (while) bname:%s cname:%s init:%d elem:%d \n",
                  this->GetName(), b->GetName(), b->GetClassName(), init, fNElements);

         TClass* cl = gROOT->GetClass(b->GetClassName());
         if (cl==0) {
            printf("-I class %s cannot be reconstructed\n", b->GetClassName());
            continue;
         }

         TGo4EventElement* par = (TGo4EventElement*) cl->New();

         if(par==0) {
            printf("-I class %s instance cannot be created\n", b->GetClassName());
            continue;

         }

         Int_t offset = par->activateBranch(b, splitLevel, i);
         if (fDebug)
            printf("-I activate from obj:%s elems:%d index:%d adding:%s\n",
                  this->GetName(), init+fNElements, i, par->GetName());

         addEventElement(par,1);
         i+=offset;
         fNElements+=offset;
         if (fDebug)
            printf("-I from obj:%s activate indexing after offset:%d index:%d elems:%d\n",
                  this->GetName(), offset,i,init+fNElements);

      } //!while
      Clear();
   }//!branch

   if (fDebug)
      printf("-I activate return value from obj:%s offset:%i \n", this->GetName(), fNElements);

   return fNElements;
}


void  TGo4CompositeEvent::synchronizeWithTree(TTree *tree, Int_t splitLevel)
{
   // synchronize with TTree for composite Event
   // FIXME !
   // should <test> the differents splitlevel?

   if (tree==0) return;

   if (fEventElements==0)
      fEventElements = new TObjArray(12);

   TGo4CompositeEvent *dump = this;

   //-- Initialize the global Branch structure
   TObjArray *br = tree->GetListOfBranches();
   TBranch* b = (TBranch*) br->At(0);
   TString cad = b->GetName();

   // SL: why it is here, absolutely same code is in the TGo4EventElement::activateBranch

   tree->SetBranchStatus(cad.Data(),1);
   tree->SetBranchAddress(cad.Data(), &dump);
   cad+="*";
   tree->SetBranchStatus(cad.Data(),1);

   // Initialize underlying  TTree

   // SL: normally at this moment one should get first entry from tree-
   //     means normal event instance plus all its fields, including number of branches
   //     But WHY fNElements+=offset later, it means reconstructed event has other number than
   //     event which was normally stored in the tree. In my mind, ABSOLUTE NONSENSE
   //     The only explanation - one wants to map hierarchical structure into flat structure of
   //     only top-levels branches in the ROOT tree. But than one should do it in different means.
   //     And one in no case could change fNElements value. If it remains unchanged one could
   //     fold/unfold complete hierarchy of composite event. In no case fEventElements should be
   //     streamed into the tree.
   TGo4EventElement::activateBranch(b,splitLevel);

   if(fDebug)
      printf("-I- TGo4CompositeEvent::synchronize(tree): Elements :%i Id:%i branch:%s \n", fNElements ,fIdentifier,cad.Data());

   //-- Rebuild the Elements array
   Int_t i=0;
   while (i<fNElements) {
      i++;
      b = (TBranch*) br->At(i);
      if(b == 0) continue;

      if (fDebug)
         printf("-I TGo4CompositeEvent::synchronize(tree) bname:%s cname:%s Elem:%i\n",
               b->GetName(), b->GetClassName(), fNElements);

      TClass* cl = gROOT->GetClass(b->GetClassName());
      if (cl==0) { printf("-I missing class\n"); continue; }

      TGo4EventElement* par = (TGo4EventElement*) cl->New();
      if (par==0) { printf("-I missing instance\n"); continue; }

      Int_t offset = par->activateBranch(b, splitLevel, i);
      if (fDebug)
         printf("-I synchronize index:%i from:%s adding:%s\n",
                  i, this->GetName(), par->GetName());
      addEventElement(par, 1);
      i+=offset;
      fNElements+=offset;

      if (fDebug)
         printf("-I synchronize indexing after offset:%i index:%i elem:%i \n",
                 offset,i,fNElements);

   } //!while
   Clear();
   //-- Resynchronize all branches with the TTree (needed once)
   //TGo4LockGuard Glob2;
   tree->GetEntry(0);
}


void TGo4CompositeEvent::Clear(Option_t *)
{
   //Clears the data in the event (i.e. clears the internal buffers...)
   TIter next(fEventElements);
   TGo4EventElement *ev;
   while ( (ev=(TGo4EventElement *)next())!=0)
      ev->Clear();
}

void TGo4CompositeEvent::clearAll(Int_t level)
{
   // Clears the data in the event and the event structure (list of subevents...)
   // FIXME
   // Levels are to be defined

   if (level==0) {
      // if (fEventElements) fEventElements->Delete();
   } else {
      TIter next(fEventElements);
      TGo4EventElement *ev;
      while ((ev=( TGo4EventElement *)next())!=0) {
         ev->clearAll(level);
      }
   }
}


void TGo4CompositeEvent::addEventElement( TGo4EventElement* aElement, Int_t reading)
{
   // if Identifiers  are needed for fast retrieval of elements
   // one should use:
   // >>> fEventElements->AddAtAndExpand(aElement,aElement->getId());
   // Note: When reading from file, adding elements should not be
   // incremented

   if (getEventElement(aElement->GetName(),1)){
      printf("-E-1 <TGo4CompositeEvent::addEventElement> object:%s already in structure => not added ! \n",
            aElement->GetName());
      return;
   }

   if ( aElement->getId() < 0 ) {
      printf("-E-2 <TGo4CompositeEvent::addEventElement> object:%s with invalid Id:%i   => not added ! \n",
            aElement->GetName(), aElement->getId());
      return;
   }

   if ( getEventElement( aElement->getId() ) ){
      printf("-E-3 <TGo4CompositeEvent::addEventElement> object:%s with Id:%i already used => not added ! \n",
            aElement->GetName(), aElement->getId());
   }

   if (fDebug)
      printf("-I adding element in :%s :%p at location:%i \n",GetName(),aElement, aElement->getId());

   fEventElements->AddAtAndExpand (aElement, aElement->getId());
   if(reading == 0) fNElements++;
}


TGo4EventElement* TGo4CompositeEvent::getEventElement(Int_t idx)
{
   // Returns a pointer to the partial event with number idx.

   if ( idx > fEventElements->GetLast() ) return NULL;
   return ( ( TGo4EventElement*) fEventElements->At(idx) );
}


TGo4EventElement* TGo4CompositeEvent::getEventElement(const char* name, Int_t final)
{
   TIter next(fEventElements);
   TGo4EventElement *ev(0);
   while ((ev=( TGo4EventElement *)next())!=0) {
      if(strcmp(name,ev->GetName())==0) return ev;
      if (ev->isComposed()) {
         TGo4EventElement* inter= ((TGo4CompositeEvent*) ev)->getEventElement(name,1);
         if (inter !=0) return inter;
      }
   }
   if(final==0)
      printf("-E- TGo4CompositeEvent => Element:%s not found in Composite:%s \n",name,this->GetName());
   return NULL;
}

void TGo4CompositeEvent::deactivate()
{
   TIter next(fEventElements);
   TGo4EventElement *ev(0);

   while ((ev=( TGo4EventElement *)next())!=0)
      ev->deactivate();
}

void TGo4CompositeEvent::activate()
{
   TIter next(fEventElements);
   TGo4EventElement *ev(0);

   while ((ev=( TGo4EventElement *)next())!=0)
      ev->activate();
}


TObjArray* TGo4CompositeEvent::getListOfComposites(Bool_t toplevel)
{
   TObjArray *comp = new TObjArray(12);
   if (toplevel) comp->Add(this);

   TIter next(fEventElements);
   //-- Add top level composite
   TGo4EventElement *ev(0);
   while ((ev=( TGo4EventElement *)next())!=0) {
      if ( ev->isComposed() ) {
         comp->Add( ev );
         TObjArray* dump = ((TGo4CompositeEvent*) ev)->getListOfComposites(kFALSE);

         comp->AddAll(dump);

         // SL: memory leask, list should be removed
         delete dump;
      }
   }
   return comp;
}


TGo4EventElement& TGo4CompositeEvent::operator[]( Int_t i )
{
   return *dynamic_cast<TGo4EventElement*> ((*fEventElements)[i]);
}

void TGo4CompositeEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class TGo4CompositeEvent.
   //  char buffer[255];
   //

   // SL: definitely WRONG, one should use normal streamer, where only fNElements is stored

   if (R__b.IsReading()) {
      // printf("TGo4CompositeEvent::Streamer reading \n");
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TGo4EventElement::Streamer(R__b);
      R__b >> fEventElements;
      R__b >> fNElements;

   } else {
      //   printf("TGo4CompositeEvent::Streamer writing \n");
      TGo4EventElement::Streamer(R__b);
      R__b << fEventElements;
      R__b << fNElements;
   }
}



