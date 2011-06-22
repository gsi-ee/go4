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

#include "TGo4Log.h"

// R__EXTERN TTree *gTree;

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
}

TGo4CompositeEvent::~TGo4CompositeEvent()
{
   if(fEventElements!=0) {

      fEventElements->Delete();

      delete fEventElements;

      fEventElements = 0;
   }
}


void TGo4CompositeEvent::makeBranch(TBranch *parent)
{
   if (fEventElements!=0)
      for(Int_t i=0; i<=fEventElements->GetLast();i++) {
         TGo4EventElement** par = (TGo4EventElement**) &((*fEventElements)[i]);
         if (par && *par) {
            TBranch *b = parent->GetTree()->TTree::Branch(Form("%s.",(*par)->GetName()), (*par)->ClassName(),par,4000,99);
            (*par)->makeBranch(b);
         }
      }

   TGo4EventElement::makeBranch(parent);
}


Int_t TGo4CompositeEvent::activateBranch(TBranch *branch, Int_t init, TGo4EventElement** var_ptr)
{
   // first read event itself
   TGo4EventElement::activateBranch(branch, init, var_ptr);

   if (branch==0) return fNElements;

   // we need to call GetEntry here to get value of fNElements
   branch->GetEntry(0);

   printf("Total number of elements = %d\n", fNElements);

   if (fDebug)
      printf("##### TGo4CompositeEvent::activateBranch called from obj:%s \n",this->GetName());

   TTree* tree = branch->GetTree();
   TObjArray *br = tree->GetListOfBranches();

   Int_t i = init;
   Int_t all_branches = fNElements;

   if (fDebug)
      printf("-I-TGo4CompositeEvent::activateBranch from obj:%s bname:%s Elements:%d  index:%d\n",
            this->GetName(), branch->GetName(), fNElements, init);

   while (i < init + all_branches) {
      i++;
      TBranch* b = (TBranch*) br->At(i);
      Bool_t readentry = kFALSE;
      if (b==0) continue;

      TString sub = b->GetName();
      sub.Remove(sub.Length()-1);
      TGo4EventElement* par = getEventElement(sub.Data());

      if (fDebug)
         printf("-I TGo4CompositeEvent::activateBranch use subbranch %s\n", b->GetName());

      if (par==0) {

         TClass* cl = gROOT->GetClass(b->GetClassName());
         if (cl==0) {
            printf("-I class %s cannot be reconstructed\n", b->GetClassName());
            continue;
         }

         par = (TGo4EventElement*) cl->New();

         if (par==0) {
            printf("-I class %s instance cannot be created\n", b->GetClassName());
            continue;
         }

         readentry = !par->isComposed();
      }

      // TODO: could we put element in the elements array BEFORE we activate branch
      //  in this case activate branch will set correct address from the beginning
      Int_t offset = par->activateBranch(b, i, &par);

      if (fDebug)
         printf("-I activate from obj:%s elems:%d index:%d adding:%s\n",
               this->GetName(), init+fNElements, i, par->GetName());

      // we need to getentry only when new object was created to get its id
      if (readentry) b->GetEntry(0);

      if (fDebug)
         printf("Add branch %s event %s offset %d\n", b->GetName(), par->GetName(), offset);

      if (addEventElement(par, kTRUE)) {
         TGo4EventElement** par_ptr = (TGo4EventElement**) &((*fEventElements)[par->getId()]);
         tree->SetBranchAddress(b->GetName(), par_ptr);
         if (fDebug)
            printf("Set address to actual variable from obj array\n");
      }

      i+=offset;
      all_branches+=offset;
      if (fDebug)
         printf("-I from obj:%s activate indexing after offset:%d index:%d max:%d\n",
               this->GetName(), offset, i, init+all_branches);
   } //!while

   // FIXME: do we need clear method here ????
   // Clear();

   if (fDebug)
      printf("-I activate return value from obj:%s offset:%i \n", this->GetName(), fNElements);

   return all_branches;
}

/***
 *
 * No need for the own method - it is similar to that in TGo4EventElement

void TGo4CompositeEvent::synchronizeWithTree(TTree *tree, TGo4EventElement** var_ptr)
{
   // synchronize with TTree for composite Event
   // FIXME !
   // should <test> the differents splitlevel?

   printf("Sync with tree %p\n", tree);

   if (tree==0) return;

   // SL 22.06.2011: here was absolutely the same code as in activateBranch, let use it
   activateBranch((TBranch*) tree->GetListOfBranches()->First(), 0, var_ptr);

   // SL: do we need clear here ???
   // Clear();
   //-- Resynchronize all branches with the TTree (needed once)
   // SL: why again GetEntry(0) ???
   // tree->GetEntry(0);
}
*/

void TGo4CompositeEvent::Clear(Option_t *opt)
{
   //Clears the data in the event (i.e. clears the internal buffers...)

   TGo4EventElement::Clear(opt);

   TIter next(fEventElements);
   TGo4EventElement *ev;
   while ( (ev=(TGo4EventElement *)next())!=0)
      ev->Clear(opt);
}

Bool_t TGo4CompositeEvent::addEventElement(TGo4EventElement* aElement, Bool_t reading)
{
   // if Identifiers  are needed for fast retrieval of elements
   // one should use:
   // >>> fEventElements->AddAtAndExpand(aElement,aElement->getId());
   // Note: When reading from file, adding elements should not be
   // incremented

   // when trying to add same element second time do nothing
   if (reading && fEventElements && fEventElements->FindObject(aElement)) return kTRUE;

   if (getEventElement(aElement->GetName(),1)) {
      TGo4Log::Error("<TGo4CompositeEvent::addEventElement> object:%s already in structure => not added !",
            aElement->GetName());
      return kFALSE;
   }

   if ( aElement->getId() < 0 ) {
      TGo4Log::Error("<TGo4CompositeEvent::addEventElement> object:%s with invalid Id:%i   => not added !",
            aElement->GetName(), aElement->getId());
      return kFALSE;
   }

   if (getEventElement(aElement->getId()) != 0) {
      TGo4Log::Error("<TGo4CompositeEvent::addEventElement> object:%s with Id:%i already used => not added !",
            aElement->GetName(), aElement->getId());
      return kFALSE;
   }

   if (fDebug)
      printf("-I adding element in :%s :%p at location:%i \n",GetName(),aElement, aElement->getId());

   if (fEventElements==0) {
      Int_t size = 4;
      if (aElement->getId() >= size ) size = aElement->getId() + 1;
      fEventElements = new TObjArray(size);
   }

   fEventElements->AddAtAndExpand (aElement, aElement->getId());
   if(!reading) fNElements++;
   return kTRUE;
}


TGo4EventElement* TGo4CompositeEvent::getEventElement(Int_t idx)
{
   // Returns a pointer to the partial event with number idx.

   if ((fEventElements==0) || (idx<0) || (idx > fEventElements->GetLast())) return NULL;
   return ( TGo4EventElement*) fEventElements->At(idx);
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
      TGo4Log::Error("TGo4CompositeEvent => Element:%s not found in Composite:%s",name,this->GetName());
   return NULL;
}

void TGo4CompositeEvent::deactivate()
{
   TGo4EventElement::deactivate();

   TIter next(fEventElements);
   TGo4EventElement *ev(0);

   while ((ev=( TGo4EventElement *)next())!=0)
      ev->deactivate();
}

void TGo4CompositeEvent::activate()
{
   TGo4EventElement::activate();

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

         // SL: memory leak, list should be removed
         delete dump;
      }
   }
   return comp;
}


TGo4EventElement& TGo4CompositeEvent::operator[]( Int_t i )
{
   if ((fEventElements==0) || (i<0) || (i>fEventElements->GetLast())) {
      TGo4Log::Error("Wrong index %d in TGo4CompositeEvent::operator[]", i);
      return *this;
   }

   return * ((TGo4EventElement*) (fEventElements->At(i)));
}

/*

SL: this is wrong code, one cannot work that way

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

*/

